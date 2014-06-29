/*
 * A V4L2 camera driver for the Aptina MT9M114 sensor.
 *
 * Copyright (C)  2014 B-Human
 * Written by Alexis Tsogias with inspiration from the mt9m114 driver of
 * Joseph Pinkasfeld from Aldebaran Robotics.
 *
 * Alexis Tsogias <alexists@informatik.uni-bremen.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 */

#ifndef MT9M114__H
#define	MT9M114__H

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/i2c.h>
#include <media/v4l2-subdev.h>

#include "mt9m114_registers.h"

#define MT9M114_NAME "mt9m114"
#define MT9M114_DEFAULT_CHIP_ID 0x2481

static bool debug = 0;

//Custom V4L control variables
#define V4L2_MT9M114_FADE_TO_BLACK (V4L2_CID_PRIVATE_BASE) //boolean, enable or disable fade to black feature

// Host commands
typedef enum {
  MT9M114_HOST_COMMAND_APPLY_PATCH = 0x0001,
  MT9M114_HOST_COMMAND_SET_STATE = 0x0002,
  MT9M114_HOST_COMMAND_REFRESH = 0x0004,
  MT9M114_HOST_COMMAND_WAIT_FOR_EVENT = 0x0008,
  MT9M114_HOST_COMMAND_OK = 0x8000
} MT9M114_HOST_COMMANDS;

// Curent System Manager states
typedef enum {
  MT9M114_SYS_STATE_ENTER_CONFIG_CHANGE = 0x28,
  MT9M114_SYS_STATE_STREAMING = 0x31,
  MT9M114_SYS_STATE_START_STREAMING = 0x34,
  MT9M114_SYS_STATE_ENTER_SUSPEND = 0x40,
  MT9M114_SYS_STATE_SUSPENDED = 0x41,
  MT9M114_SYS_STATE_ENTER_STANDBY = 0x50,
  MT9M114_SYS_STATE_STANDBY = 0x52,
  MT9M114_SYS_STATE_LEAVE_STANDBY = 0x54
} MT9M114_SYS_STATES;

// Error codes
typedef enum {
  MT9M114_ERROR_CODE_ENOERR = 0x00,
  MT9M114_ERROR_CODE_EBADF = 0x05,
  MT9M114_ERROR_CODE_EACCESS = 0x08,
  MT9M114_ERROR_CODE_EBUSY = 0x09,
  MT9M114_ERROR_CODE_EINVAL = 0x0C,
  MT9M114_ERROR_CODE_ENOSPC = 0x0D,
  MT9M114_ERROR_CODE_ERANGE = 0x0E,
  MT9M114_ERROR_CODE_ENOSYS = 0x0F,
  MT9M114_ERROR_CODE_EINVCROPX = 0x13,
  MT9M114_ERROR_CODE_EINVCROPY = 0x14,
  MT9M114_ERROR_CODE_EINVTC = 0x15
} MT9M114_ERROR_CODES;

enum mt9m114_reg_size {
  REG_8,
  REG_16,
  REG_32
};

// structs definitions ---------------------------------------------------------

struct mt9m114_state {
  struct v4l2_subdev sd;
  u16 chip_id;
  u16 revision;
  bool ae_enabled;
  const struct mt9m114_fmt* current_fmt;
  const struct mt9m114_framesize* current_framesize;
  const struct v4l2_fract* current_frame_interval;
};

struct mt9m114_reg {
  u16 reg;
  enum mt9m114_reg_size size;
  u32 val;
};

struct mt9m114_patch {
  char description[32];
  u16 physical_address;
  u16 patch_write_address;
  u16 patch_loader_address;
  u16 patch_id;
  u32 firmware_id;
  u16 patch_size;
  u16 patch[];
};

struct mt9m114_fmt {
  u8 description[32];
  u32 pixelformat;
  u8 bytes_per_pixel;
  struct mt9m114_reg* regs;
};

#define MT9M114_MAX_FRAME_INTERVALS 3

struct mt9m114_framesize {
  u32 width;
  u32 height;
  struct mt9m114_reg* regs;
  struct v4l2_fract frame_intervals[MT9M114_MAX_FRAME_INTERVALS];
};

// print macros -----------------------------------------------------------

#define dprint(format, arg...)\
  if(debug) { \
    printk(KERN_DEBUG "mt9m114 -debug- %s: " format, __func__, ##arg);\
  }

#define eprint(format, arg...)\
  do { \
    printk(KERN_ERR "mt9m114 -error- %s: " format, __func__, ##arg);\
  } while (0)

#define iprint(format, arg...)\
  do { \
    printk(KERN_INFO "mt9m114 -info- %s: " format, __func__, ##arg);\
  } while (0)

#define wprint(format, arg...)\
  do { \
    printk(KERN_WARNING "mt9m114 -warning- %s: " format, __func__, ##arg);\
  } while (0)

// misc methods ----------------------------------------------------------------

static inline struct mt9m114_state* to_state(struct v4l2_subdev* sd) {
  return container_of(sd, struct mt9m114_state, sd);
}

static inline u16 mt9m114_to_chip_framerate(const struct v4l2_fract* frameinterval) {
  return (u16)(((frameinterval->denominator * 256) + frameinterval->numerator - 1) / frameinterval->numerator);
}

static inline u32 mt9m114_to_chip_frameinterval(const struct v4l2_fract* frameinterval) {
  return ((frameinterval->numerator * 10000) + frameinterval->denominator - 1) / frameinterval->denominator;
}

// read / write operations -----------------------------------------------------

static int mt9m114_read8(struct i2c_client* client, u16 reg, u8* val);
static int mt9m114_read16(struct i2c_client* client, u16 reg, u16* val);
static int mt9m114_read32(struct i2c_client* client, u16 reg, u32* val);

/**
 * Poll until the value at reg & mask equals expected. Each try is delayed by delay ms.
 */
static int mt9m114_poll16(struct i2c_client* client, u16 reg, u16 mask, u16 expected, unsigned int delay, int timeout);

static int mt9m114_write8(struct i2c_client* client, u16 reg, u8 val);
static int mt9m114_write16(struct i2c_client* client, u16 reg, u16 val);
static int mt9m114_write32(struct i2c_client* client, u16 reg, u32 val);

static int mt9m114_read_modify_write16(struct i2c_client* client, u16 reg, u16 set, u16 unset);

static int mt9m114_write_array(struct i2c_client* client, const struct mt9m114_reg* regs);
static int mt9m114_burst_write(struct i2c_client* client, u16 reg, const u16* array, u16 size);

// misc mt9m114 methods --------------------------------------------------------

static int mt9m114_issue_command(struct i2c_client* client, MT9M114_HOST_COMMANDS command);
static int mt9m114_s_sys_state(struct i2c_client* client, MT9M114_SYS_STATES state);
static int mt9m114_g_sys_state(struct i2c_client* client, MT9M114_SYS_STATES* state);

static int mt9m114_g_cmd_status(struct i2c_client* client, MT9M114_ERROR_CODES* status);
static int mt9m114_g_seq_status(struct i2c_client* client, MT9M114_ERROR_CODES* status);
static int mt9m114_g_patch_status(struct i2c_client* client, MT9M114_ERROR_CODES* status);
static int mt9m114_g_uvc_status(struct i2c_client* client, MT9M114_ERROR_CODES* status);

static int mt9m114_detect(struct i2c_client* client, struct mt9m114_state* state);
static int mt9m114_change_config(struct i2c_client* client);
static int mt9m114_refresh(struct i2c_client* client);
static int mt9m114_apply_patch(struct i2c_client* client, const struct mt9m114_patch* patch);
static int mt9m114_wait_for_vertical_blanking(struct i2c_client* client);
static int mt9m114_set_streaming(struct i2c_client* client);
static int mt9m114_set_suspend(struct i2c_client* client);

static int mt9m114_g_auto_exposure(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_auto_white_balance(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_backlight_compensation(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_brightness(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_contrast(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_do_white_balance(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_exposure(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_exposure_algorithm(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_gain(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_gamma(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_hue(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_power_line_frequency(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_saturation(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_sharpness(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_white_balance_temperature(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_vertical_flip(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_horizontal_flip(struct v4l2_subdev* sd, s32* val);
static int mt9m114_g_fade_to_black(struct v4l2_subdev* sd, s32* val);

static int mt9m114_s_auto_exposure(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_auto_white_balance(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_backlight_compensation(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_brightness(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_contrast(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_do_white_balance(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_exposure(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_exposure_algorithm(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_gain(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_gamma(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_hue(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_power_line_frequency(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_saturation(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_sharpness(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_white_balance_temperature(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_vertical_flip(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_horizontal_flip(struct v4l2_subdev* sd, s32 val);
static int mt9m114_s_fade_to_black(struct v4l2_subdev* sd, s32 val);

static const struct mt9m114_fmt* mt9m114_find_best_fmt(u32 pixelformat);
static const struct mt9m114_framesize* mt9m114_find_best_framesize(u32 width, u32 height);

static int mt9m114_s_img_fmt(struct v4l2_subdev* sd, const struct mt9m114_fmt* fmt);
static int mt9m114_s_framesize(struct v4l2_subdev* sd, const struct mt9m114_framesize* fsize);
static int mt9m114_s_frameinterval(struct v4l2_subdev* sd, const struct v4l2_fract* frame_interval);

// v4l2 subdev core ops --------------------------------------------------------

static int mt9m114_g_chip_ident(struct v4l2_subdev* sd, struct v4l2_dbg_chip_ident* chip);
static int mt9m114_init(struct v4l2_subdev* sd, u32 val);
static int mt9m114_reset(struct v4l2_subdev* sd, u32 val);
static int mt9m114_queryctrl(struct v4l2_subdev* sd, struct v4l2_queryctrl* qctrl);
static int mt9m114_g_ctrl(struct v4l2_subdev* sd, struct v4l2_control* ctrl);
static int mt9m114_s_ctrl(struct v4l2_subdev* sd, struct v4l2_control* ctrl);
#ifdef CONFIG_VIDEO_ADV_DEBUG
static int mt9m114_g_register(struct v4l2_subdev* sd, struct v4l2_dbg_register* reg);
static int mt9m114_s_register(struct v4l2_subdev* sd, struct v4l2_dbg_register* reg);
#endif

// v4l2 subdev video ops -------------------------------------------------------

static int mt9m114_enum_fmt(struct v4l2_subdev* sd, struct v4l2_fmtdesc* fmt);
static int mt9m114_enum_frameintervals(struct v4l2_subdev* sd, struct v4l2_frmivalenum* fival);
static int mt9m114_enum_framesizes(struct v4l2_subdev* sd, struct v4l2_frmsizeenum* fsize);
static int mt9m114_g_fmt(struct v4l2_subdev* sd, struct v4l2_format* fmt);
static int mt9m114_try_fmt(struct v4l2_subdev* sd, struct v4l2_format* fmt);
static int mt9m114_s_fmt(struct v4l2_subdev* sd, struct v4l2_format* fmt);
static int mt9m114_cropcap(struct v4l2_subdev* sd, struct v4l2_cropcap* cc);
static int mt9m114_g_crop(struct v4l2_subdev* sd, struct v4l2_crop* c);
static int mt9m114_s_crop(struct v4l2_subdev* sd, struct v4l2_crop* c);
static int mt9m114_g_parm(struct v4l2_subdev* sd, struct v4l2_streamparm* params);
static int mt9m114_s_parm(struct v4l2_subdev* sd, struct v4l2_streamparm* params);
static int mt9m114_s_stream(struct v4l2_subdev* sd, int enable);

// i2c driver methods ----------------------------------------------------------

static int mt9m114_probe(struct i2c_client* client, const struct i2c_device_id* id);
static int mt9m114_remove(struct i2c_client* client);

// v4l2 subdev structs ---------------------------------------------------------

static const struct v4l2_subdev_core_ops mt9m114_core_ops = {
  .g_chip_ident = mt9m114_g_chip_ident,
  .init = mt9m114_init,
  .reset = mt9m114_reset,
  .queryctrl = mt9m114_queryctrl,
  .g_ctrl = mt9m114_g_ctrl,
  .s_ctrl = mt9m114_s_ctrl,
#ifdef CONFIG_VIDEO_ADV_DEBUG
  .g_register = mt9m114_g_register,
  .s_register = mt9m114_s_register
#endif
};

static const struct v4l2_subdev_video_ops mt9m114_video_ops = {
  .enum_fmt = mt9m114_enum_fmt,
  .enum_frameintervals = mt9m114_enum_frameintervals,
  .enum_framesizes = mt9m114_enum_framesizes,
  .g_fmt = mt9m114_g_fmt,
  .try_fmt = mt9m114_try_fmt,
  .s_fmt = mt9m114_s_fmt,
  .cropcap = mt9m114_cropcap,
  .g_crop = mt9m114_g_crop,
  .s_crop = mt9m114_s_crop,
  .g_parm = mt9m114_g_parm,
  .s_parm = mt9m114_s_parm,
  .s_stream = mt9m114_s_stream
};

static const struct v4l2_subdev_ops mt9m114_ops = {
  .core = &mt9m114_core_ops,
  .video = &mt9m114_video_ops
};

// i2c structs -----------------------------------------------------------------

static const struct i2c_device_id mt9m114_id[] = {
  { MT9M114_NAME, 0},
  {}
};

static struct i2c_driver mt9m114_driver = {
  .driver =
  {
    .owner = THIS_MODULE,
    .name = "mt9m114"
  },
  .probe = mt9m114_probe,
  .remove = mt9m114_remove,
  .id_table = mt9m114_id
};

// patches ---------------------------------------------------------------------

/**
 *  Patch 0202; Feature Recommended; Black level correction fix
 */
static const struct mt9m114_patch black_level_correction_fix = {
  .description = "Black level correction fix",
  .physical_address = 0x5000,
  .patch_write_address = 0xd000,
  .patch_loader_address = 0x010c,
  .patch_id = 0x0202,
  .firmware_id = 0x41030202,
  .patch_size = 150,
  .patch =
  {
    0x70cf, 0xffff, 0xc5d4, 0x903a, 0x2144, 0x0c00, 0x2186, 0x0ff3,
    0xb844, 0xb948, 0xe082, 0x20cc, 0x80e2, 0x21cc, 0x80a2, 0x21cc,
    0x80e2, 0xf404, 0xd801, 0xf003, 0xd800, 0x7ee0, 0xc0f1, 0x08ba,
    0x0600, 0xc1a1, 0x76cf, 0xffff, 0xc130, 0x6e04, 0xc040, 0x71cf,
    0xffff, 0xc790, 0x8103, 0x77cf, 0xffff, 0xc7c0, 0xe001, 0xa103,
    0xd800, 0x0c6a, 0x04e0, 0xb89e, 0x7508, 0x8e1c, 0x0809, 0x0191,
    0xd801, 0xae1d, 0xe580, 0x20ca, 0x0022, 0x20cf, 0x0522, 0x0c5c,
    0x04e2, 0x21ca, 0x0062, 0xe580, 0xd901, 0x79c0, 0xd800, 0x0be6,
    0x04e0, 0xb89e, 0x70cf, 0xffff, 0xc8d4, 0x9002, 0x0857, 0x025e,
    0xffdc, 0xe080, 0x25cc, 0x9022, 0xf225, 0x1700, 0x108a, 0x73cf,
    0xff00, 0x3174, 0x9307, 0x2a04, 0x103e, 0x9328, 0x2942, 0x7140,
    0x2a04, 0x107e, 0x9349, 0x2942, 0x7141, 0x2a04, 0x10be, 0x934a,
    0x2942, 0x714b, 0x2a04, 0x10be, 0x130c, 0x010a, 0x2942, 0x7142,
    0x2250, 0x13ca, 0x1b0c, 0x0284, 0xb307, 0xb328, 0x1b12, 0x02c4,
    0xb34a, 0xed88, 0x71cf, 0xff00, 0x3174, 0x9106, 0xb88f, 0xb106,
    0x210a, 0x8340, 0xc000, 0x21ca, 0x0062, 0x20f0, 0x0040, 0x0b02,
    0x0320, 0xd901, 0x07f1, 0x05e0, 0xc0a1, 0x78e0, 0xc0f1, 0x71cf,
    0xffff, 0xc7c0, 0xd840, 0xa900, 0x71cf, 0xffff, 0xd02c, 0xd81e,
    0x0a5a, 0x04e0, 0xda00, 0xd800, 0xc0d1, 0x7ee0
  }
};

/**
 * Patch 0302 - Feature request, Adaptive Sensitivity.
 *
 * This patch implements the new feature VGA auto binning mode. This was a
 * request to support automatic mode transition between VGA scaled and binning
 * mode (and back)
 *
 * To support this feature a new Firmware variable page has been added which
 * controls this functionality as well as hold configuration parameters for
 * the automatic binning mode of operation. This pasge needs to be configured
 * correctly as these values will be used to populate the CAM page during the
 * switch
 *
 * Main control variables
 *     AUTO_BINNING_MODE.AUTO_BINNING_MODE_ENABLE:
 *         Controls automatic binning mode (0=disabled, 1=enabled).
 *         NOTE: Requires Change-Config to apply
 *     AUTO_BINNING_STATUS.AUTO_BINNING_STATUS_ENABLE
 *         Current enable/disable state of automatic binning mode (0=disabled, 1=enabled)
 *     AUTO_BINNING_THRESHOLD_BM
 *         Switching threshold in terms of inverse brightness metric (ufixed8)
 *     AUTO_BINNING_GATE_PERCENTAGE
 *         Gate width as a percentage of threshold
 *
 * Notes:
 *     CAM_LL_SUMMING_SENSITIVITY_FACTOR
 *         This is the sensitivity gain that is achieved when sub-sampled
 *         read mode is selected, summing or average (approximately 2.0x
 *         unity=32)
 *
 *     The sensitivity factor and gate width must be tuned correctly to avoid
 *     oscillation during the switch
 */
static const struct mt9m114_patch adaptive_sensitivity = {
  .description = "Adaptive Sensitivity",
  .physical_address = 0x512c,
  .patch_write_address = 0xd12c,
  .patch_loader_address = 0x04b4,
  .patch_id = 0x0302,
  .firmware_id = 0x41030202,
  .patch_size = 514,
  .patch =
  {
    0x70cf, 0xffff, 0xc5d4, 0x903a, 0x2144, 0x0c00, 0x2186, 0x0ff3,
    0xb844, 0x262f, 0xf008, 0xb948, 0x21cc, 0x8021, 0xd801, 0xf203,
    0xd800, 0x7ee0, 0xc0f1, 0x71cf, 0xffff, 0xc610, 0x910e, 0x208c,
    0x8014, 0xf418, 0x910f, 0x208c, 0x800f, 0xf414, 0x9116, 0x208c,
    0x800a, 0xf410, 0x9117, 0x208c, 0x8807, 0xf40c, 0x9118, 0x2086,
    0x0ff3, 0xb848, 0x080d, 0x0090, 0xffea, 0xe081, 0xd801, 0xf203,
    0xd800, 0xc0d1, 0x7ee0, 0x78e0, 0xc0f1, 0x71cf, 0xffff, 0xc610,
    0x910e, 0x208c, 0x800a, 0xf418, 0x910f, 0x208c, 0x8807, 0xf414,
    0x9116, 0x208c, 0x800a, 0xf410, 0x9117, 0x208c, 0x8807, 0xf40c,
    0x9118, 0x2086, 0x0ff3, 0xb848, 0x080d, 0x0090, 0xffd9, 0xe080,
    0xd801, 0xf203, 0xd800, 0xf1df, 0x9040, 0x71cf, 0xffff, 0xc5d4,
    0xb15a, 0x9041, 0x73cf, 0xffff, 0xc7d0, 0xb140, 0x9042, 0xb141,
    0x9043, 0xb142, 0x9044, 0xb143, 0x9045, 0xb147, 0x9046, 0xb148,
    0x9047, 0xb14b, 0x9048, 0xb14c, 0x9049, 0x1958, 0x0084, 0x904a,
    0x195a, 0x0084, 0x8856, 0x1b36, 0x8082, 0x8857, 0x1b37, 0x8082,
    0x904c, 0x19a7, 0x009c, 0x881a, 0x7fe0, 0x1b54, 0x8002, 0x78e0,
    0x71cf, 0xffff, 0xc350, 0xd828, 0xa90b, 0x8100, 0x01c5, 0x0320,
    0xd900, 0x78e0, 0x220a, 0x1f80, 0xffff, 0xd4e0, 0xc0f1, 0x0811,
    0x0051, 0x2240, 0x1200, 0xffe1, 0xd801, 0xf006, 0x2240, 0x1900,
    0xffde, 0xd802, 0x1a05, 0x1002, 0xfff2, 0xf195, 0xc0f1, 0x0e7e,
    0x05c0, 0x75cf, 0xffff, 0xc84c, 0x9502, 0x77cf, 0xffff, 0xc344,
    0x2044, 0x008e, 0xb8a1, 0x0926, 0x03e0, 0xb502, 0x9502, 0x952e,
    0x7e05, 0xb5c2, 0x70cf, 0xffff, 0xc610, 0x099a, 0x04a0, 0xb026,
    0x0e02, 0x0560, 0xde00, 0x0a12, 0x0320, 0xb7c4, 0x0b36, 0x03a0,
    0x70c9, 0x9502, 0x7608, 0xb8a8, 0xb502, 0x70cf, 0x0000, 0x5536,
    0x7860, 0x2686, 0x1ffb, 0x9502, 0x78c5, 0x0631, 0x05e0, 0xb502,
    0x72cf, 0xffff, 0xc5d4, 0x923a, 0x73cf, 0xffff, 0xc7d0, 0xb020,
    0x9220, 0xb021, 0x9221, 0xb022, 0x9222, 0xb023, 0x9223, 0xb024,
    0x9227, 0xb025, 0x9228, 0xb026, 0x922b, 0xb027, 0x922c, 0xb028,
    0x1258, 0x0101, 0xb029, 0x125a, 0x0101, 0xb02a, 0x1336, 0x8081,
    0xa836, 0x1337, 0x8081, 0xa837, 0x12a7, 0x0701, 0xb02c, 0x1354,
    0x8081, 0x7fe0, 0xa83a, 0x78e0, 0xc0f1, 0x0dc2, 0x05c0, 0x7608,
    0x09bb, 0x0010, 0x75cf, 0xffff, 0xd4e0, 0x8d21, 0x8d00, 0x2153,
    0x0003, 0xb8c0, 0x8d45, 0x0b23, 0x0000, 0xea8f, 0x0915, 0x001e,
    0xff81, 0xe808, 0x2540, 0x1900, 0xffde, 0x8d00, 0xb880, 0xf004,
    0x8d00, 0xb8a0, 0xad00, 0x8d05, 0xe081, 0x20cc, 0x80a2, 0xdf00,
    0xf40a, 0x71cf, 0xffff, 0xc84c, 0x9102, 0x7708, 0xb8a6, 0x2786,
    0x1ffe, 0xb102, 0x0b42, 0x0180, 0x0e3e, 0x0180, 0x0f4a, 0x0160,
    0x70c9, 0x8d05, 0xe081, 0x20cc, 0x80a2, 0xf429, 0x76cf, 0xffff,
    0xc84c, 0x082d, 0x0051, 0x70cf, 0xffff, 0xc90c, 0x8805, 0x09b6,
    0x0360, 0xd908, 0x2099, 0x0802, 0x9634, 0xb503, 0x7902, 0x1523,
    0x1080, 0xb634, 0xe001, 0x1d23, 0x1002, 0xf00b, 0x9634, 0x9503,
    0x6038, 0xb614, 0x153f, 0x1080, 0xe001, 0x1d3f, 0x1002, 0xffa4,
    0x9602, 0x7f05, 0xd800, 0xb6e2, 0xad05, 0x0511, 0x05e0, 0xd800,
    0xc0f1, 0x0cfe, 0x05c0, 0x0a96, 0x05a0, 0x7608, 0x0c22, 0x0240,
    0xe080, 0x20ca, 0x0f82, 0x0000, 0x190b, 0x0c60, 0x05a2, 0x21ca,
    0x0022, 0x0c56, 0x0240, 0xe806, 0x0e0e, 0x0220, 0x70c9, 0xf048,
    0x0896, 0x0440, 0x0e96, 0x0400, 0x0966, 0x0380, 0x75cf, 0xffff,
    0xd4e0, 0x8d00, 0x084d, 0x001e, 0xff47, 0x080d, 0x0050, 0xff57,
    0x0841, 0x0051, 0x8d04, 0x9521, 0xe064, 0x790c, 0x702f, 0x0ce2,
    0x05e0, 0xd964, 0x72cf, 0xffff, 0xc700, 0x9235, 0x0811, 0x0043,
    0xff3d, 0x080d, 0x0051, 0xd801, 0xff77, 0xf025, 0x9501, 0x9235,
    0x0911, 0x0003, 0xff49, 0x080d, 0x0051, 0xd800, 0xff72, 0xf01b,
    0x0886, 0x03e0, 0xd801, 0x0ef6, 0x03c0, 0x0f52, 0x0340, 0x0dba,
    0x0200, 0x0af6, 0x0440, 0x0c22, 0x0400, 0x0d72, 0x0440, 0x0dc2,
    0x0200, 0x0972, 0x0440, 0x0d3a, 0x0220, 0xd820, 0x0bfa, 0x0260,
    0x70c9, 0x0451, 0x05c0, 0x78e0, 0xd900, 0xf00a, 0x70cf, 0xffff,
    0xd520, 0x7835, 0x8041, 0x8000, 0xe102, 0xa040, 0x09f1, 0x8114,
    0x71cf, 0xffff, 0xd4e0, 0x70cf, 0xffff, 0xc594, 0xb03a, 0x7fe0,
    0xd800, 0x0000, 0x0000, 0x0500, 0x0500, 0x0200, 0x0330, 0x0000,
    0x0000, 0x03cd, 0x050d, 0x01c5, 0x03b3, 0x00e0, 0x01e3, 0x0280,
    0x01e0, 0x0109, 0x0080, 0x0500, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0xffff, 0xc9b4, 0xffff, 0xd324, 0xffff, 0xca34,
    0xffff, 0xd3ec
  }
};

// chip settings ---------------------------------------------------------------

#define ARRAY_END {0xFFFF,0xFFFF,0xFFFF}

static const struct mt9m114_reg array_end = ARRAY_END;

static const struct mt9m114_reg pll_settings[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // i don't know if this in necesary
  {VAR_CAM_SYSCTL_PLL_ENABLE, REG_8, 0x01}, // enable pll
  {VAR_CAM_SYSCTL_PLL_DIVIDER_M_N, REG_16, 0x0120}, // default is 0x09a0
  {VAR_CAM_SYSCTL_PLL_DIVIDER_P, REG_16, 0x0700},
  ARRAY_END
};

/**
 * These are undocumented registes. No idea what they do...
 */
static const struct mt9m114_reg sensor_optimization[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {0x316A, REG_16, 0x8270},
  {0x316C, REG_16, 0x8270},
  {0x3ED0, REG_16, 0x3605},
  {0x3ED2, REG_16, 0x77FF},
  {0x316E, REG_16, 0xC233},
  {0x3180, REG_16, 0x87FF},
  {0x30D4, REG_16, 0x6080},
  {0xA802, REG_16, 0x0008},
  ARRAY_END
};

static const struct mt9m114_reg errata_1[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {0x3E14, REG_16, 0xFF39},
  ARRAY_END
};

static const struct mt9m114_reg errata_2[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {REG_RESET_REGISTER, REG_16, 0x0234},
  ARRAY_END
};

static const struct mt9m114_reg pga_settings[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {VAR_CAM_PGA_PGA_CONTROL, REG_16, 0x0003},
  {VAR_CAM_PGA_PGA_CONTROL, REG_16, 0x0002},
  {REG_P_G1_P0Q0, REG_16, 0x0170},
  {REG_P_G1_P0Q1, REG_16, 0x0ECB},
  {REG_P_G1_P0Q2, REG_16, 0x57D0},
  {REG_P_G1_P0Q3, REG_16, 0x830D},
  {REG_P_G1_P0Q4, REG_16, 0xF26E},
  {REG_P_R_P0Q0, REG_16, 0x7E2F},
  {REG_P_R_P0Q1, REG_16, 0x48AB},
  {REG_P_R_P0Q2, REG_16, 0x6650},
  {REG_P_R_P0Q3, REG_16, 0xCD4C},
  {REG_P_R_P0Q4, REG_16, 0xAE8E},
  {REG_P_B_P0Q0, REG_16, 0x7EAF},
  {REG_P_B_P0Q1, REG_16, 0x5ACC},
  {REG_P_B_P0Q2, REG_16, 0x1B90},
  {REG_P_B_P0Q3, REG_16, 0xFA8C},
  {REG_P_B_P0Q4, REG_16, 0x92EE},
  {REG_P_G2_P0Q0, REG_16, 0x7DCF},
  {REG_P_G2_P0Q1, REG_16, 0x4AEA},
  {REG_P_G2_P0Q2, REG_16, 0x68B0},
  {REG_P_G2_P0Q3, REG_16, 0xB02D},
  {REG_P_G2_P0Q4, REG_16, 0xA88F},
  {REG_P_G1_P1Q0, REG_16, 0x368C},
  {REG_P_G1_P1Q1, REG_16, 0x074D},
  {REG_P_G1_P1Q2, REG_16, 0x268F},
  {REG_P_G1_P1Q3, REG_16, 0x7E8A},
  {REG_P_G1_P1Q4, REG_16, 0x97B0},
  {REG_P_R_P1Q0, REG_16, 0x01EC},
  {REG_P_R_P1Q1, REG_16, 0x0B4E},
  {REG_P_R_P1Q2, REG_16, 0x124F},
  {REG_P_R_P1Q3, REG_16, 0xAFCE},
  {REG_P_R_P1Q4, REG_16, 0x8590},
  {REG_P_B_P1Q0, REG_16, 0x03CD},
  {REG_P_B_P1Q1, REG_16, 0x092D},
  {REG_P_B_P1Q2, REG_16, 0x62EB},
  {REG_P_B_P1Q3, REG_16, 0xA14E},
  {REG_P_B_P1Q4, REG_16, 0xB76F},
  {REG_P_G2_P1Q0, REG_16, 0x0D4D},
  {REG_P_G2_P1Q1, REG_16, 0xB9AB},
  {REG_P_G2_P1Q2, REG_16, 0x97CC},
  {REG_P_G2_P1Q3, REG_16, 0x57EA},
  {REG_P_G2_P1Q4, REG_16, 0x91EF},
  {REG_P_G1_P2Q0, REG_16, 0x0DF1},
  {REG_P_G1_P2Q1, REG_16, 0x8F0C},
  {REG_P_G1_P2Q2, REG_16, 0x7610},
  {REG_P_G1_P2Q3, REG_16, 0x2B6C},
  {REG_P_G1_P2Q4, REG_16, 0xA6F3},
  {REG_P_R_P2Q0, REG_16, 0x0691},
  {REG_P_R_P2Q1, REG_16, 0xF04A},
  {REG_P_R_P2Q2, REG_16, 0x33D1},
  {REG_P_R_P2Q3, REG_16, 0xADAE},
  {REG_P_R_P2Q4, REG_16, 0xC9B3},
  {REG_P_B_P2Q0, REG_16, 0x5A90},
  {REG_P_B_P2Q1, REG_16, 0x9B6D},
  {REG_P_B_P2Q2, REG_16, 0x57D0},
  {REG_P_B_P2Q3, REG_16, 0x0F2D},
  {REG_P_B_P2Q4, REG_16, 0xFD72},
  {REG_P_G2_P2Q0, REG_16, 0x0DB1},
  {REG_P_G2_P2Q1, REG_16, 0xB4EA},
  {REG_P_G2_P2Q2, REG_16, 0x68F0},
  {REG_P_G2_P2Q3, REG_16, 0x8FCD},
  {REG_P_G2_P2Q4, REG_16, 0xA253},
  {REG_P_G1_P3Q0, REG_16, 0x052D},
  {REG_P_G1_P3Q1, REG_16, 0xEC8D},
  {REG_P_G1_P3Q2, REG_16, 0xB771},
  {REG_P_G1_P3Q3, REG_16, 0x338F},
  {REG_P_G1_P3Q4, REG_16, 0x6292},
  {REG_P_R_P3Q0, REG_16, 0x280D},
  {REG_P_R_P3Q1, REG_16, 0x9030},
  {REG_P_R_P3Q2, REG_16, 0xB571},
  {REG_P_R_P3Q3, REG_16, 0x4531},
  {REG_P_R_P3Q4, REG_16, 0x4B52},
  {REG_P_B_P3Q0, REG_16, 0x290D},
  {REG_P_B_P3Q1, REG_16, 0xD02F},
  {REG_P_B_P3Q2, REG_16, 0x9611},
  {REG_P_B_P3Q3, REG_16, 0x22B1},
  {REG_P_B_P3Q4, REG_16, 0x68D2},
  {REG_P_G2_P3Q0, REG_16, 0x6D4D},
  {REG_P_G2_P3Q1, REG_16, 0x9A2D},
  {REG_P_G2_P3Q2, REG_16, 0x8611},
  {REG_P_G2_P3Q3, REG_16, 0x1470},
  {REG_P_G2_P3Q4, REG_16, 0x51F2},
  {REG_P_G1_P4Q0, REG_16, 0x89EB},
  {REG_P_G1_P4Q1, REG_16, 0xC28E},
  {REG_P_G1_P4Q2, REG_16, 0xF254},
  {REG_P_G1_P4Q3, REG_16, 0x5AD2},
  {REG_P_G1_P4Q4, REG_16, 0x6CB6},
  {REG_P_R_P4Q0, REG_16, 0x1FAF},
  {REG_P_R_P4Q1, REG_16, 0xC78D},
  {REG_P_R_P4Q2, REG_16, 0x8375},
  {REG_P_R_P4Q3, REG_16, 0x6152},
  {REG_P_R_P4Q4, REG_16, 0x7256},
  {REG_P_B_P4Q0, REG_16, 0x140D},
  {REG_P_B_P4Q1, REG_16, 0x172D},
  {REG_P_B_P4Q2, REG_16, 0xCCD4},
  {REG_P_B_P4Q3, REG_16, 0x3432},
  {REG_P_B_P4Q4, REG_16, 0x46F6},
  {REG_P_G2_P4Q0, REG_16, 0x92AE},
  {REG_P_G2_P4Q1, REG_16, 0x9A8E},
  {REG_P_G2_P4Q2, REG_16, 0xEF74},
  {REG_P_G2_P4Q3, REG_16, 0x5C32},
  {REG_P_G2_P4Q4, REG_16, 0x6656},
  {REG_CENTER_ROW, REG_16, 0x01E0},
  {REG_CENTER_COLMN, REG_16, 0x02A0},
  {VAR_CAM_PGA_L_CONFIG_COLOR_TEMP, REG_16, 0x0AF0},
  {VAR_CAM_PGA_L_CONFIG_GREEN_RED_Q14, REG_16, 0x79AD},
  {VAR_CAM_PGA_L_CONFIG_RED_Q14, REG_16, 0x57B0},
  {VAR_CAM_PGA_L_CONFIG_GREEN_BLUE_Q14, REG_16, 0x78D8},
  {VAR_CAM_PGA_L_CONFIG_BLUE_Q14, REG_16, 0x7460},
  {VAR_CAM_PGA_M_CONFIG_COLOR_TEMP, REG_16, 0x0FA0},
  {VAR_CAM_PGA_M_CONFIG_GREEN_RED_Q14, REG_16, 0x8177},
  {VAR_CAM_PGA_M_CONFIG_RED_Q14, REG_16, 0x815C},
  {VAR_CAM_PGA_M_CONFIG_GREEN_BLUE_Q14, REG_16, 0x8111},
  {VAR_CAM_PGA_M_CONFIG_BLUE_Q14, REG_16, 0x8057},
  {VAR_CAM_PGA_R_CONFIG_COLOR_TEMP, REG_16, 0x1964},
  {VAR_CAM_PGA_R_CONFIG_GREEN_RED_Q14, REG_16, 0x7C2A},
  {VAR_CAM_PGA_R_CONFIG_RED_Q14, REG_16, 0x669C},
  {VAR_CAM_PGA_R_CONFIG_GREEN_BLUE_Q14, REG_16, 0x7E3F},
  {VAR_CAM_PGA_R_CONFIG_BLUE_Q14, REG_16, 0x73D2},
  {VAR_CAM_PGA_PGA_CONTROL, REG_16, 0x0003},
  ARRAY_END
};

static const struct mt9m114_reg awb_settings[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {VAR_CAM_AWB_CCM_L_0, REG_16, 0x0267},
  {VAR_CAM_AWB_CCM_L_1, REG_16, 0xFF1A},
  {VAR_CAM_AWB_CCM_L_2, REG_16, 0xFFB3},
  {VAR_CAM_AWB_CCM_L_3, REG_16, 0xFF80},
  {VAR_CAM_AWB_CCM_L_4, REG_16, 0x0166},
  {VAR_CAM_AWB_CCM_L_5, REG_16, 0x0003},
  {VAR_CAM_AWB_CCM_L_6, REG_16, 0xFF9A},
  {VAR_CAM_AWB_CCM_L_7, REG_16, 0xFEB4},
  {VAR_CAM_AWB_CCM_L_8, REG_16, 0x024D},
  {VAR_CAM_AWB_CCM_M_0, REG_16, 0x01BF},
  {VAR_CAM_AWB_CCM_M_1, REG_16, 0xFF01},
  {VAR_CAM_AWB_CCM_M_2, REG_16, 0xFFF3},
  {VAR_CAM_AWB_CCM_M_3, REG_16, 0xFF75},
  {VAR_CAM_AWB_CCM_M_4, REG_16, 0x0198},
  {VAR_CAM_AWB_CCM_M_5, REG_16, 0xFFFD},
  {VAR_CAM_AWB_CCM_M_6, REG_16, 0xFF9A},
  {VAR_CAM_AWB_CCM_M_7, REG_16, 0xFEE7},
  {VAR_CAM_AWB_CCM_M_8, REG_16, 0x02A8},
  {VAR_CAM_AWB_CCM_R_0, REG_16, 0x01D9},
  {VAR_CAM_AWB_CCM_R_1, REG_16, 0xFF26},
  {VAR_CAM_AWB_CCM_R_2, REG_16, 0xFFF3},
  {VAR_CAM_AWB_CCM_R_3, REG_16, 0xFFB3},
  {VAR_CAM_AWB_CCM_R_4, REG_16, 0x0132},
  {VAR_CAM_AWB_CCM_R_5, REG_16, 0xFFE8},
  {VAR_CAM_AWB_CCM_R_6, REG_16, 0xFFDA},
  {VAR_CAM_AWB_CCM_R_7, REG_16, 0xFECD},
  {VAR_CAM_AWB_CCM_R_8, REG_16, 0x02C2},
  {VAR_CAM_AWB_CCM_L_RG_GAIN, REG_16, 0x0075},
  {VAR_CAM_AWB_CCM_L_BG_GAIN, REG_16, 0x011C},
  {VAR_CAM_AWB_CCM_M_RG_GAIN, REG_16, 0x009A},
  {VAR_CAM_AWB_CCM_M_BG_GAIN, REG_16, 0x0105},
  {VAR_CAM_AWB_CCM_R_RG_GAIN, REG_16, 0x00A4},
  {VAR_CAM_AWB_CCM_R_BG_GAIN, REG_16, 0x00AC},
  {VAR_CAM_AWB_CCM_L_CTEMP, REG_16, 0x0A8C},
  {VAR_CAM_AWB_CCM_M_CTEMP, REG_16, 0x0F0A},
  {VAR_CAM_AWB_CCM_R_CTEMP, REG_16, 0x1964},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XEND, REG_16, 0x04FF},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YEND, REG_16, 0x02CF},
  {VAR_CAM_AWB_AWB_XSHIFT_PRE_ADJ, REG_16, 0x0033},
  {VAR_CAM_AWB_AWB_YSHIFT_PRE_ADJ, REG_16, 0x0040},
  {VAR_CAM_AWB_AWB_XSCALE, REG_8, 0x03},
  {VAR_CAM_AWB_AWB_YSCALE, REG_8, 0x02},
  {VAR_CAM_AWB_AWB_YSHIFT_PRE_ADJ, REG_16, 0x003C},
  {VAR_CAM_AWB_AWB_WEIGHTS_0, REG_16, 0x0000},
  {VAR_CAM_AWB_AWB_WEIGHTS_1, REG_16, 0x0000},
  {VAR_CAM_AWB_AWB_WEIGHTS_2, REG_16, 0x0000},
  {VAR_CAM_AWB_AWB_WEIGHTS_3, REG_16, 0xE724},
  {VAR_CAM_AWB_AWB_WEIGHTS_4, REG_16, 0x1583},
  {VAR_CAM_AWB_AWB_WEIGHTS_5, REG_16, 0x2045},
  {VAR_CAM_AWB_AWB_WEIGHTS_6, REG_16, 0x03FF},
  {VAR_CAM_AWB_AWB_WEIGHTS_7, REG_16, 0x007C},
  {VAR_CAM_AWB_K_R_L, REG_8, 0x80},
  {VAR_CAM_AWB_K_G_L, REG_8, 0x80},
  {VAR_CAM_AWB_K_B_L, REG_8, 0x80},
  {VAR_CAM_AWB_K_R_R, REG_8, 0x88},
  {VAR_CAM_AWB_K_G_R, REG_8, 0x80},
  {VAR_CAM_AWB_K_B_R, REG_8, 0x80},
  ARRAY_END
};

static const struct mt9m114_reg CPIPE_preference[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {VAR_CAM_LL_START_BRIGHTNESS, REG_16, 0x0020},
  {VAR_CAM_LL_STOP_BRIGHTNESS, REG_16, 0x009A},
  {VAR_CAM_LL_START_GAIN_METRIC, REG_16, 0x0070},
  {VAR_CAM_LL_STOP_GAIN_METRIC, REG_16, 0x00F3},
  {VAR_CAM_LL_START_TARGET_LUMA_BM, REG_16, 0x0020},
  {VAR_CAM_LL_STOP_TARGET_LUMA_BM, REG_16, 0x009A},
  {VAR_CAM_LL_START_SATURATION, REG_8, 0x80},
  {VAR_CAM_LL_END_SATURATION, REG_8, 0x4B},
  {VAR_CAM_LL_START_DESATURATION, REG_8, 0x00},
  {VAR_CAM_LL_END_DESATURATION, REG_8, 0xFF},
  {VAR_CAM_LL_START_DEMOSAIC, REG_8, 0x1E},
  {VAR_CAM_LL_START_AP_GAIN, REG_8, 0x02},
  {VAR_CAM_LL_START_AP_THRESH, REG_8, 0x06},
  {VAR_CAM_LL_STOP_DEMOSAIC, REG_8, 0x3C},
  {VAR_CAM_LL_STOP_AP_GAIN, REG_8, 0x01},
  {VAR_CAM_LL_STOP_AP_THRESH, REG_8, 0x0C},
  {VAR_CAM_LL_START_NR_RED, REG_8, 0x3C},
  {VAR_CAM_LL_START_NR_GREEN, REG_8, 0x3C},
  {VAR_CAM_LL_START_NR_BLUE, REG_8, 0x3C},
  {VAR_CAM_LL_START_NR_THRESH, REG_8, 0x0F},
  {VAR_CAM_LL_STOP_NR_RED, REG_8, 0x64},
  {VAR_CAM_LL_STOP_NR_GREEN, REG_8, 0x64},
  {VAR_CAM_LL_STOP_NR_BLUE, REG_8, 0x64},
  {VAR_CAM_LL_STOP_NR_THRESH, REG_8, 0x32},
  {VAR_CAM_LL_START_CONTRAST_BM, REG_16, 0x0020},
  {VAR_CAM_LL_STOP_CONTRAST_BM, REG_16, 0x009A},
  {VAR_CAM_LL_GAMMA, REG_16, 0x00DC},
  {VAR_CAM_LL_START_CONTRAST_GRADIENT, REG_8, 0x38},
  {VAR_CAM_LL_STOP_CONTRAST_GRADIENT, REG_8, 0x30},
  {VAR_CAM_LL_START_CONTRAST_LUMA_PERCENTAGE, REG_8, 0x50},
  {VAR_CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE, REG_8, 0x19},
  {VAR_CAM_LL_START_FADE_TO_BLACK_LUMA, REG_16, 0x0230},
  {VAR_CAM_LL_STOP_FADE_TO_BLACK_LUMA, REG_16, 0x0010},
  {VAR_CAM_LL_CLUSTER_DC_TH_BM, REG_16, 0x0800},
  {VAR_CAM_LL_CLUSTER_DC_GATE_PERCENTAGE, REG_8, 0x05},
  {VAR_CAM_LL_SUMMING_SENSITIVITY_FACTOR, REG_8, 0x40},
  {VAR_CAM_AET_TARGET_AVERAGE_LUMA_DARK, REG_8, 0x1B},
  {VAR_CAM_AET_AEMODE, REG_8, 0x0E},
  {VAR_CAM_AET_TARGET_GAIN, REG_16, 0x0080},
  {VAR_CAM_AET_AE_MAX_VIRT_AGAIN, REG_16, 0x0100},
  {0xC81C, REG_16, 0x01F8}, // CAM_SENSOR_CFG_MAX_ANALOG_GAIN
  {VAR_CAM_AET_BLACK_CLIPPING_TARET, REG_16, 0x005A},
  {VAR_CCM_DELTA_GAIN, REG_8, 0x05},
  {VAR_AE_TRACK_AE_TRACKING_DAMPENING_SPEED, REG_8, 0x20},
  ARRAY_END
};

/**
 * The stardard settings for the UVC variables. Auto exposure and auto white
 * balance are enabled thus only variables that do not reject changes while ae
 * and awb are enabled are set.
 */
static const struct mt9m114_reg uvc_settings[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {VAR_UVC_AE_MODE_CONTROL, REG_8, 0x02},                         // Enable auto exposure
  {VAR_UVC_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL, REG_8, 0x01},  // Enable aoto white balance
  {VAR_UVC_AE_PRIORITY_CONTROL, REG_8, 0x00},
  {VAR_UVC_POWER_LINE_FREQUENCY_CONTROL, REG_8, 0x02},            // Set the power line frequency to 60 Hz
  {VAR_UVC_BACKLIGHT_COMPENSATION_CONTROL, REG_16, 0x0001},       // Set backlight compensation to first mode
  {VAR_UVC_BRIGHTNESS_CONTROL, REG_16, 0x0037},
  {VAR_UVC_CONTRAST_CONTROL, REG_16, 0x0020},
  {VAR_UVC_HUE_CONTROL, REG_16, 0x0000},
  {VAR_UVC_SATURATION_CONTROL, REG_16, 0x0080},
  {VAR_UVC_SHARPNESS_CONTROL, REG_16, 0x0000},
  {VAR_UVC_GAMMA_CONTROL, REG_16, 0x00DC},
  {VAR_UVC_MANUAL_EXPOSURE_CONFIGURATION, REG_8, 0x00},
  {VAR_UVC_FLICKER_AVOIDANCE_CONFIGURATION, REG_8, 0x00},
  {VAR_UVC_ALGO, REG_16, 0x0007},
  ARRAY_END
};

static struct mt9m114_reg features[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {VAR_CAM_PORT_OUTPUT_CONTROL, REG_16, 0x8040},
  {REG_PAD_SLEW, REG_16, 0x0777},
  ARRAY_END
};

// available formats, frameintervals and framesizes ----------------------------

static struct mt9m114_reg mt9m114_fmt_yuv422[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {VAR_CAM_OUTPUT_FORMAT, REG_16, 0x000A},
  {VAR_CAM_OUTPUT_Y_OFFSET, REG_8, 0x10},
  {VAR_CAM_OUTPUT_FORMAT_YUV, REG_16, 0x1A},
  ARRAY_END
};

static struct mt9m114_reg mt9m114_1280x960[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {VAR_CAM_SENSOR_CFG_Y_ADDR_START, REG_16, 0x0004},
  {VAR_CAM_SENSOR_CFG_X_ADDR_START, REG_16, 0x0004},
  {VAR_CAM_SENSOR_CFG_Y_ADDR_END, REG_16, 0x03CB}, // 971
  {VAR_CAM_SENSOR_CFG_X_ADDR_END, REG_16, 0x050B}, // 1291
  {VAR_CAM_SENSOR_CFG_PIXCLK, REG_32, 0x2DC6C00}, // 48000000
  {VAR_CAM_SENSOR_CFG_ROW_SPEED, REG_16, 0x0001},
  {VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MIN, REG_16, 0x00DB}, // 219
  {VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MAX, REG_16, 0x05C8}, // 1480
  {VAR_CAM_SENSOR_CFG_FRAME_LENGTH_LINES, REG_16, 0x03EF}, // 1007
  {VAR_CAM_SENSOR_CFG_LINE_LENGTH_PCK, REG_16, 0x064B}, // 1611
  {VAR_CAM_SENSOR_CFG_FINE_CORRECTION, REG_16, 0x0060}, // 96
  {VAR_CAM_SENSOR_CFG_CPIPE_LAST_ROW, REG_16, 0x03C3}, // 963
  {VAR_CAM_SENSOR_CFG_REG_0_DATA, REG_16, 0x0020},
  {VAR_CAM_CROP_WINDOW_XOFFSET, REG_16, 0x0000},
  {VAR_CAM_CROP_WINDOW_YOFFSET, REG_16, 0x0000},
  {VAR_CAM_CROP_WINDOW_WIDTH, REG_16, 0x0500}, // 1280
  {VAR_CAM_CROP_WINDOW_HEIGHT, REG_16, 0x03C0}, // 960
  {VAR_CAM_CROP_CROPMODE, REG_8, 0x03},
  {VAR_CAM_OUTPUT_WIDTH, REG_16, 0x0500}, // 1280
  {VAR_CAM_OUTPUT_HEIGHT, REG_16, 0x03C0}, // 960
  {VAR_CAM_AET_AEMODE, REG_8, 0x00},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XEND, REG_16, 0x04FF}, // 1259
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YEND, REG_16, 0x03BF}, // 959
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_XSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_YSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_XEND, REG_16, 0x00FF}, // 255
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_YEND, REG_16, 0x00BF}, // 191
  ARRAY_END
};

static struct mt9m114_reg mt9m114_1280x720[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {VAR_CAM_SENSOR_CFG_Y_ADDR_START, REG_16, 0x007C}, // 124
  {VAR_CAM_SENSOR_CFG_X_ADDR_START, REG_16, 0x0004},
  {VAR_CAM_SENSOR_CFG_Y_ADDR_END, REG_16, 0x0353}, // 851
  {VAR_CAM_SENSOR_CFG_X_ADDR_END, REG_16, 0x050B}, // 1291
  {VAR_CAM_SENSOR_CFG_PIXCLK, REG_32, 0x2DC6C00}, // 48000000
  {VAR_CAM_SENSOR_CFG_ROW_SPEED, REG_16, 0x0001},
  {VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MIN, REG_16, 0x00DB}, // 219
  {VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MAX, REG_16, 0x0616}, // 1558
  {VAR_CAM_SENSOR_CFG_FRAME_LENGTH_LINES, REG_16, 0x030A}, // 778
  {VAR_CAM_SENSOR_CFG_LINE_LENGTH_PCK, REG_16, 0x0699}, // 1689
  {VAR_CAM_SENSOR_CFG_FINE_CORRECTION, REG_16, 0x0060}, // 96
  {VAR_CAM_SENSOR_CFG_CPIPE_LAST_ROW, REG_16, 0x02D3}, // 723
  {VAR_CAM_SENSOR_CFG_REG_0_DATA, REG_16, 0x0020},
  {VAR_CAM_CROP_WINDOW_XOFFSET, REG_16, 0x0000},
  {VAR_CAM_CROP_WINDOW_YOFFSET, REG_16, 0x0000},
  {VAR_CAM_CROP_WINDOW_WIDTH, REG_16, 0x0500}, // 1280
  {VAR_CAM_CROP_WINDOW_HEIGHT, REG_16, 0x02D0}, // 720
  {VAR_CAM_CROP_CROPMODE, REG_8, 0x03},
  {VAR_CAM_OUTPUT_WIDTH, REG_16, 0x0500}, // 1280
  {VAR_CAM_OUTPUT_HEIGHT, REG_16, 0x02D0}, // 720
  {VAR_CAM_AET_AEMODE, REG_8, 0x00},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XEND, REG_16, 0x04FF}, // 1259
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YEND, REG_16, 0x02CF}, // 719
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_XSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_YSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_XEND, REG_16, 0x00FF}, // 255
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_YEND, REG_16, 0x008F}, // 143
  ARRAY_END
};

static struct mt9m114_reg mt9m114_640x480[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {VAR_CAM_SENSOR_CFG_Y_ADDR_START, REG_16, 0x0004},
  {VAR_CAM_SENSOR_CFG_X_ADDR_START, REG_16, 0x0004},
  {VAR_CAM_SENSOR_CFG_Y_ADDR_END, REG_16, 0x03CB}, // 971
  {VAR_CAM_SENSOR_CFG_X_ADDR_END, REG_16, 0x050B}, // 1291
  {VAR_CAM_SENSOR_CFG_PIXCLK, REG_32, 0x2DC6C00}, // 48000000
  {VAR_CAM_SENSOR_CFG_ROW_SPEED, REG_16, 0x0001},
  {VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MIN, REG_16, 0x00DB}, // 219
  {VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MAX, REG_16, 0x05B4}, // 1460
  {VAR_CAM_SENSOR_CFG_FRAME_LENGTH_LINES, REG_16, 0x03EE}, // 1006
  {VAR_CAM_SENSOR_CFG_LINE_LENGTH_PCK, REG_16, 0x0637}, // 1591
  {VAR_CAM_SENSOR_CFG_FINE_CORRECTION, REG_16, 0x0060}, // 96
  {VAR_CAM_SENSOR_CFG_CPIPE_LAST_ROW, REG_16, 0x03C3}, // 963
  {VAR_CAM_SENSOR_CFG_REG_0_DATA, REG_16, 0x0020},
  {VAR_CAM_CROP_WINDOW_XOFFSET, REG_16, 0x0000},
  {VAR_CAM_CROP_WINDOW_YOFFSET, REG_16, 0x0000},
  {VAR_CAM_CROP_WINDOW_WIDTH, REG_16, 0x0500}, // 1280
  {VAR_CAM_CROP_WINDOW_HEIGHT, REG_16, 0x03C0}, // 960
  {VAR_CAM_CROP_CROPMODE, REG_8, 0x03},
  {VAR_CAM_OUTPUT_WIDTH, REG_16, 0x0280}, // 640
  {VAR_CAM_OUTPUT_HEIGHT, REG_16, 0x01E0}, // 480
  {VAR_CAM_AET_AEMODE, REG_8, 0x00},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XEND, REG_16, 0x027F}, // 639
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YEND, REG_16, 0x01DF}, // 479
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_XSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_YSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_XEND, REG_16, 0x007F}, // 127
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_YEND, REG_16, 0x005F}, // 95
  ARRAY_END
};

static struct mt9m114_reg mt9m114_320x240[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {VAR_CAM_SENSOR_CFG_Y_ADDR_START, REG_16, 0x0004},
  {VAR_CAM_SENSOR_CFG_X_ADDR_START, REG_16, 0x0004},
  {VAR_CAM_SENSOR_CFG_Y_ADDR_END, REG_16, 0x03CB}, // 971
  {VAR_CAM_SENSOR_CFG_X_ADDR_END, REG_16, 0x050B}, // 1291
  {VAR_CAM_SENSOR_CFG_PIXCLK, REG_32, 0x2DC6C00}, // 48000000
  {VAR_CAM_SENSOR_CFG_ROW_SPEED, REG_16, 0x0001},
  {VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MIN, REG_16, 0x00DB}, // 219
  {VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MAX, REG_16, 0x05B4}, // 1460
  {VAR_CAM_SENSOR_CFG_FRAME_LENGTH_LINES, REG_16, 0x03EE}, // 1006
  {VAR_CAM_SENSOR_CFG_LINE_LENGTH_PCK, REG_16, 0x0637}, // 1591
  {VAR_CAM_SENSOR_CFG_FINE_CORRECTION, REG_16, 0x0060}, // 96
  {VAR_CAM_SENSOR_CFG_CPIPE_LAST_ROW, REG_16, 0x03C3}, // 963
  {VAR_CAM_SENSOR_CFG_REG_0_DATA, REG_16, 0x0020},
  {VAR_CAM_CROP_WINDOW_XOFFSET, REG_16, 0x0000},
  {VAR_CAM_CROP_WINDOW_YOFFSET, REG_16, 0x0000},
  {VAR_CAM_CROP_WINDOW_WIDTH, REG_16, 0x0500}, // 1280
  {VAR_CAM_CROP_WINDOW_HEIGHT, REG_16, 0x03C0}, // 960
  {VAR_CAM_CROP_CROPMODE, REG_8, 0x03},
  {VAR_CAM_OUTPUT_WIDTH, REG_16, 0x0140}, // 320
  {VAR_CAM_OUTPUT_HEIGHT, REG_16, 0x00F0}, // 240
  {VAR_CAM_AET_AEMODE, REG_8, 0x00},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XEND, REG_16, 0x013F}, // 319
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YEND, REG_16, 0x00EF}, // 239
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_XSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_YSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_XEND, REG_16, 0x003F}, // 63
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_YEND, REG_16, 0x002F}, // 47
  ARRAY_END
};

static struct mt9m114_reg mt9m114_160x120[] = {
  {REG_LOGICAL_ADDRESS_ACCESS, REG_16, 0x0000}, // Enable logical adress access. No idea if this is necessary.
  {VAR_CAM_SENSOR_CFG_Y_ADDR_START, REG_16, 0x0004},
  {VAR_CAM_SENSOR_CFG_X_ADDR_START, REG_16, 0x0004},
  {VAR_CAM_SENSOR_CFG_Y_ADDR_END, REG_16, 0x03CB}, // 971
  {VAR_CAM_SENSOR_CFG_X_ADDR_END, REG_16, 0x050B}, // 1291
  {VAR_CAM_SENSOR_CFG_PIXCLK, REG_32, 0x2DC6C00}, // 48000000
  {VAR_CAM_SENSOR_CFG_ROW_SPEED, REG_16, 0x0001},
  {VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MIN, REG_16, 0x00DB}, // 219
  {VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MAX, REG_16, 0x05B4}, // 1460
  {VAR_CAM_SENSOR_CFG_FRAME_LENGTH_LINES, REG_16, 0x03EE}, // 1006
  {VAR_CAM_SENSOR_CFG_LINE_LENGTH_PCK, REG_16, 0x0637}, // 1591
  {VAR_CAM_SENSOR_CFG_FINE_CORRECTION, REG_16, 0x0060}, // 96
  {VAR_CAM_SENSOR_CFG_CPIPE_LAST_ROW, REG_16, 0x03C3}, // 963
  {VAR_CAM_SENSOR_CFG_REG_0_DATA, REG_16, 0x0020},
  {VAR_CAM_CROP_WINDOW_XOFFSET, REG_16, 0x0000},
  {VAR_CAM_CROP_WINDOW_YOFFSET, REG_16, 0x0000},
  {VAR_CAM_CROP_WINDOW_WIDTH, REG_16, 0x0500}, // 1280
  {VAR_CAM_CROP_WINDOW_HEIGHT, REG_16, 0x03C0}, // 960
  {VAR_CAM_CROP_CROPMODE, REG_8, 0x03},
  {VAR_CAM_OUTPUT_WIDTH, REG_16, 0x00A0}, // 160
  {VAR_CAM_OUTPUT_HEIGHT, REG_16, 0x0078}, // 120
  {VAR_CAM_AET_AEMODE, REG_8, 0x00},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_XEND, REG_16, 0x009F}, // 159
  {VAR_CAM_STAT_AWB_CLIP_WINDOW_YEND, REG_16, 0x0077}, // 119
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_XSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_YSTART, REG_16, 0x0000},
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_XEND, REG_16, 0x001F}, // 31
  {VAR_CAM_STAT_AE_INITIAL_WINDOW_YEND, REG_16, 0x0017}, // 23
  ARRAY_END
};

/**
 * The mt9m114 supports YCbCr, 565RGB, 555RGB, 444RGB and raw Bayer patterns.
 * All but YCbCr have a very inconvenient output format so they will not be
 * supported by this driver.
 */
static const struct mt9m114_fmt mt9m114_fmts[] = {
  {
    .description = "YUYV 4:2:2",
    .pixelformat = V4L2_PIX_FMT_YUYV,
    .bytes_per_pixel = 2,
    .regs = mt9m114_fmt_yuv422
  }
};

#define EMPTY_FRAME_INTERVAL {0xFFFF,0xFFFF}

static const struct v4l2_fract empty_frame_interval = EMPTY_FRAME_INTERVAL;

static const struct mt9m114_framesize mt9m114_framesizes[] = {
  {
    .width = 1280,
    .height = 960,
    .regs = mt9m114_1280x960,
    .frame_intervals =
    {
      {1, 30},
      EMPTY_FRAME_INTERVAL,
      EMPTY_FRAME_INTERVAL
    }
  },
  {
    .width = 1280,
    .height = 720,
    .regs = mt9m114_1280x720,
    .frame_intervals =
    {
      {1, 30},
      {3, 110},
      EMPTY_FRAME_INTERVAL
    }
  },
  {
    .width = 640,
    .height = 480,
    .regs = mt9m114_640x480,
    .frame_intervals =
    {
      {1, 30},
      EMPTY_FRAME_INTERVAL,
      EMPTY_FRAME_INTERVAL
    }
  },
  {
    .width = 320,
    .height = 240,
    .regs = mt9m114_320x240,
    .frame_intervals =
    {
      {1, 30},
      EMPTY_FRAME_INTERVAL,
      EMPTY_FRAME_INTERVAL
    }
  },
  {
    .width = 160,
    .height = 120,
    .regs = mt9m114_160x120,
    .frame_intervals =
    {
      {1, 30},
      EMPTY_FRAME_INTERVAL,
      EMPTY_FRAME_INTERVAL
    }
  }
};

#define MT9M114_NUM_FMTS ARRAY_SIZE(mt9m114_fmts)
#define MT9M114_NUM_FRAMESIZES ARRAY_SIZE(mt9m114_framesizes)

#endif	/* MT9M114__H */
