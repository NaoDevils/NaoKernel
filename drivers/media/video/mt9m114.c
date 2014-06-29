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

#include <media/v4l2-device.h>
#include <media/v4l2-chip-ident.h>
#include <asm-generic/errno.h>
#include "mt9m114_registers.h"
#include "mt9m114.h"

MODULE_AUTHOR("Alexis Tsogias <alexists@tzi.de>");
MODULE_DESCRIPTION("A V4L2 camera driver for the Aptina MT9M114 sensor.");
MODULE_LICENSE("GPL");

module_param(debug, bool, 0644);
MODULE_PARM_DESC(debug, "Debug level (0-1)");

MODULE_DEVICE_TABLE(i2c, mt9m114_id);

// read / write operations -----------------------------------------------------

static int mt9m114_read8(struct i2c_client* client, u16 reg, u8* val) {
  int ret;
  u16 swapped_reg = swab16(reg);
  struct i2c_msg msg[] = {
    {
      .addr = client->addr,
      .flags = 0,
      .len = 2,
      .buf = (u8*)(&swapped_reg),
    },
    {
      .addr = client->addr,
      .flags = I2C_M_RD,
      .len = 1,
      .buf = val,
    }
  };

  ret = i2c_transfer(client->adapter, msg, 2);
  if(ret < 0) {
    eprint("Reading register 0x%04X failed!", reg);
    return ret;
  } else {
    return 0;
  }
}

static int mt9m114_read16(struct i2c_client* client, u16 reg, u16* val) {
  int ret;
  u16 swapped_reg = swab16(reg);
  struct i2c_msg msg[] = {
    {
      .addr = client->addr,
      .flags = 0,
      .len = 2,
      .buf = (u8*)(&swapped_reg),
    },
    {
      .addr = client->addr,
      .flags = I2C_M_RD,
      .len = 2,
      .buf = (u8*)val,
    }
  };

  ret = i2c_transfer(client->adapter, msg, 2);
  if(ret < 0) {
    eprint("Reading register 0x%04X failed errno: %i!", reg, ret);
    return ret;
  } else {
    *val = swab16(*val);
    return 0;
  }
}

static int mt9m114_read32(struct i2c_client* client, u16 reg, u32* val) {
  int ret;
  u16 swapped_reg = swab16(reg);
  struct i2c_msg msgs[] = {
    {
      .addr = client->addr,
      .flags = 0,
      .len = 2,
      .buf = (u8*)(&swapped_reg),
    },
    {
      .addr = client->addr,
      .flags = I2C_M_RD,
      .len = 4,
      .buf = (u8*)val,
    }
  };

  ret = i2c_transfer(client->adapter, msgs, 2);
  if(ret < 0) {
    eprint("Reading register 0x%04X failed!", reg);
    return ret;
  } else {
    *val = swab32(*val);
    return 0;
  }
}

static int mt9m114_poll16(struct i2c_client* client, u16 reg, u16 mask, u16 expected, unsigned int delay, int timeout) {
  int ret;
  u16 val;

  do {
    ret = mt9m114_read16(client, reg, &val);

    if(ret) {
      return ret;
    } else if((val & mask) == expected) {
      return 0;
    }

    msleep(delay);
    timeout--;
  } while(timeout);

  eprint("Polling register 0x%04X with mask 0x%04X for value 0x%04X timed out!", reg, mask, expected);
  return -ETIMEDOUT;
}

static int mt9m114_write8(struct i2c_client* client, u16 reg, u8 val) {
  int ret;
  u8 buf[3];
  struct i2c_msg msg = {
    .addr = client->addr,
    .flags = 0,
    .len = 3,
    .buf = buf
  };

  *(u16*)(buf + 0) = swab16(reg);
  *(buf + 2) = val;

  ret = i2c_transfer(client->adapter, &msg, 1);
  if(ret < 0) {
    eprint("Writing %02X to register 0x%04X failed!", val, reg);
    return ret;
  } else {
    return 0;
  }
}

static int mt9m114_write16(struct i2c_client* client, u16 reg, u16 val) {
  int ret;
  u8 buf[4];
  struct i2c_msg msg = {
    .addr = client->addr,
    .flags = 0,
    .len = 4,
    .buf = buf
  };

  *(u16*)(buf + 0) = swab16(reg);
  *(u16*)(buf + 2) = swab16(val);

  ret = i2c_transfer(client->adapter, &msg, 1);
  if(ret < 0) {
    eprint("Writing %04X to register 0x%04X failed!", val, reg);
    return ret;
  } else {
    return 0;
  }
}

static int mt9m114_write32(struct i2c_client* client, u16 reg, u32 val) {
  int ret;
  u8 buf[6];
  struct i2c_msg msg = {
    .addr = client->addr,
    .flags = 0,
    .len = 6,
    .buf = buf
  };

  *(u16*)(buf + 0) = swab16(reg);
  *(u32*)(buf + 2) = swab32(val);

  ret = i2c_transfer(client->adapter, &msg, 1);
  if(ret < 0) {
    eprint("Writing %08X to register 0x%04X failed!", val, reg);
    return ret;
  } else {
    return 0;
  }
}

static int mt9m114_read_modify_write16(struct i2c_client* client, u16 reg, u16 set, u16 unset) {
  u16 val;
  int ret;

  ret = mt9m114_read16(client, reg, &val);
  if(ret) {
    return ret;
  }

  val |= set;
  val &= ~unset;

  return mt9m114_write16(client, reg, val);
}

static int mt9m114_write_array(struct i2c_client* client, const struct mt9m114_reg* regs) {
  int i = 0;
  int ret;

  while(regs[i].reg != array_end.reg) {
    switch(regs[i].size) {
      case REG_8:
        ret = mt9m114_write8(client, regs[i].reg, (u8)regs[i].val);
        break;
      case REG_16:
        ret = mt9m114_write16(client, regs[i].reg, (u16)regs[i].val);
        break;
      case REG_32:
        ret = mt9m114_write32(client, regs[i].reg, regs[i].val);
        break;
      default:
        eprint("Unsupported register size issued: %u!", regs[i].size);
        return -EINVAL;
    }
    if(ret) {
      return ret;
    }
    ++i;
  }
  return 0;
}

static int mt9m114_burst_write(struct i2c_client* client, u16 reg, const u16* array, u16 size) {
  int ret;
  static const u16 max_packet_size = 64;
  static const u16 max_buffer_size = max_packet_size * 2;
  u16 buffer_size;
  u16 buffer_index;
  u16 array_index = 0;
  u8 buf[max_buffer_size];
  struct i2c_msg msg = {
    .addr = client->addr,
    .flags = 0,
    .buf = buf
  };

  while(size) {
    // +1 for the address to write to
    if(size + 1 >= max_packet_size) {
      buffer_size = max_buffer_size;
      size -= max_packet_size - 1;
    } else {
      buffer_size = (size + 1) * 2;
      size = 0;
    }
    msg.len = buffer_size;

    *(u16*)buf = swab16(reg);
    for(buffer_index = 2; buffer_index < buffer_size; buffer_index += 2) {
      *(u16*)(buf + buffer_index) = swab16(array[array_index++]);
    }

    ret = i2c_transfer(client->adapter, &msg, 1);
    if(ret < 0) {
      eprint("Burst write failed after register 0x%04X!", reg);
      return ret;
    } else {
      reg += buffer_size - 2;
    }
  }

  return 0;
}

// misc mt9m114 methods --------------------------------------------------------

static int mt9m114_issue_command(struct i2c_client* client, MT9M114_HOST_COMMANDS command) {
  int ret;
  u16 val;
  MT9M114_ERROR_CODES errval;

  // Make sure that the FW is ready to accept a new command.
  ret = mt9m114_poll16(client, REG_COMMAND_REGISTER, (u16)command, 0x0000, 10, 100);
  // Issue the command
  // We set the 'OK' bit so we can detect if the command fails
  if(!ret) ret = mt9m114_write16(client, REG_COMMAND_REGISTER, (u16)(command | MT9M114_HOST_COMMAND_OK));
  // Wait for the FW to complete the command
  if(!ret) ret = mt9m114_poll16(client, REG_COMMAND_REGISTER, (u16)command, 0x0000, 10, 100);
  // Check the 'OK' bit to see if the command was successful
  if(!ret) ret = mt9m114_read16(client, REG_COMMAND_REGISTER, &val);
  if(!ret && !(val & MT9M114_HOST_COMMAND_OK)) {
    // For some states we can give more detailed error messages.
    switch(command) {
      case MT9M114_HOST_COMMAND_SET_STATE:
        ret = mt9m114_g_cmd_status(client, &errval);
        if(!ret) {
          if(errval == MT9M114_ERROR_CODE_EINVAL) {
            eprint("Issuing SET STATE failed with EINVAL!");
            return -EINVAL;
          } else if(errval == MT9M114_ERROR_CODE_ENOSPC) {
            eprint("Issuing SET STATE failed with ENOSPC!");
            return -ENOSPC;
          } else {
            eprint("Issuing SET STATE failed with unknown error!");
            return -EFAULT;
          }
        }
        break;
      case MT9M114_HOST_COMMAND_REFRESH:
        ret = mt9m114_g_seq_status(client, &errval);
        if(!ret) {
          if(errval == MT9M114_ERROR_CODE_EINVCROPX) {
            eprint("Issuing REFRESH failed with EINVCROPX!");
            return -EINVAL;
          } else if(errval == MT9M114_ERROR_CODE_EINVCROPY) {
            eprint("Issuing REFRESH failed with EINVCROPY!");
            return -EINVAL;
          } else if(errval == MT9M114_ERROR_CODE_EINVTC) {
            eprint("Issuing REFRESH failed with EINVTC!");
            return -EINVAL;
          } else {
            eprint("Issuing REFRESH failed with unknown error!");
            return -EFAULT;
          }
        }
        break;
      case MT9M114_HOST_COMMAND_APPLY_PATCH:
        ret = mt9m114_g_patch_status(client, &errval);
        if(!ret) {
          if(errval == MT9M114_ERROR_CODE_EBADF) {
            eprint("Issuing APPLY PATH failed with EBADF!");
            return -EBADF;
          } else {
            eprint("Issuing APPLY PATH failed with unknown error!");
            return -EFAULT;
          }
        }
        break;
      default:
        eprint("Issuing command 0x%04X failed with unknown error!", command);
        return -EFAULT;
    }
  }

  return ret;
}

static int mt9m114_s_sys_state(struct i2c_client* client, MT9M114_SYS_STATES state) {
  int ret;

  // Set the desired next state
  ret = mt9m114_write8(client, VAR_SYSMGR_NEXT_STATE, (u8)state);
  if(!ret) ret = mt9m114_issue_command(client, MT9M114_HOST_COMMAND_SET_STATE);

  return ret;
}

static int mt9m114_g_sys_state(struct i2c_client* client, MT9M114_SYS_STATES* state) {
  int ret;
  u8 val;
  ret = mt9m114_read8(client, VAR_SYSMGR_CURRENT_STATE, &val);
  *state = (MT9M114_SYS_STATES)val;
  return ret;
}

static int mt9m114_g_cmd_status(struct i2c_client* client, MT9M114_ERROR_CODES* status) {
  int ret;
  u8 val;
  ret = mt9m114_read8(client, VAR_SYSMGR_CURRENT_STATE, &val);
  *status = (MT9M114_ERROR_CODES)val;
  return ret;
}

static int mt9m114_g_seq_status(struct i2c_client* client, MT9M114_ERROR_CODES* status) {
  int ret;
  u8 val;
  ret = mt9m114_read8(client, VAR_SEQ_ERROR_CODE, &val);
  *status = (MT9M114_ERROR_CODES)val;
  return ret;
}

static int mt9m114_g_patch_status(struct i2c_client* client, MT9M114_ERROR_CODES* status) {
  int ret;
  u8 val;
  ret = mt9m114_read8(client, VAR_PATCHLDR_APPLY_STATUS, &val);
  *status = (MT9M114_ERROR_CODES)val;
  return ret;
}

static int mt9m114_g_uvc_status(struct i2c_client* client, MT9M114_ERROR_CODES* status) {
  int ret;
  u8 val;
  ret = mt9m114_read8(client, VAR_UVC_RESULT_STATUS, &val);
  *status = (MT9M114_ERROR_CODES)val;
  return ret;
}

static int mt9m114_detect(struct i2c_client* client, struct mt9m114_state* state) {
  u16 major_version;
  u16 minor_version;
  u16 release_version;
  int ret;

  ret = mt9m114_read16(client, REG_CHIP_ID, &(state->chip_id));

  if(ret) {
    eprint("Detecting chip failed @ 0x%X-%u!", client->addr, i2c_adapter_id(client->adapter));
    return ret;
  } else if(state->chip_id != MT9M114_DEFAULT_CHIP_ID) {
    eprint("Unexpected chip found @ 0x%X-%u with id: 0x%X, where it should be: 0x%X!",
            client->addr, i2c_adapter_id(client->adapter), state->chip_id, MT9M114_DEFAULT_CHIP_ID);
    return -ENODEV;
  } else {
    if(!ret) ret = mt9m114_read16(client, VAR_MON_MAJOR_VERSION, &major_version);
    if(!ret) ret = mt9m114_read16(client, VAR_MON_MONIR_VERSION, &minor_version);
    if(!ret) ret = mt9m114_read16(client, VAR_MON_RELEASE_VERSION, &release_version);
    if(!ret) ret = mt9m114_read16(client, REG_COSTOMER_REV, &state->revision);

    if(ret) {
      eprint("Reading chip version information failed @ 0x%X-%u!", client->addr, i2c_adapter_id(client->adapter));
      return ret;
    } else {
      iprint("Chip found @ 0x%X-%u with id: 0x%X, version: %u.%u.%u, release: 0x%X.",
              client->addr, i2c_adapter_id(client->adapter),
              state->chip_id, major_version, minor_version, state->revision, release_version);
      return 0;
    }
  }
}

static int mt9m114_change_config(struct i2c_client* client) {
  /*
   * change_config updates the sensor configuration using the cam variables.
   * It totally ignores what the uvc variables say.
   * E.g. if cam says the exposure should be 20 then after change_config the
   * exposure will be 20 even though uvc says it should be 42.
   *
   * This is not a problem in itself.
   * However the uvc variables are not updated.
   * After a change config command the uvc will still tell you that the exposure
   * is 42. Even though it really is 20.
   * Additionally if the user tries to reset the exposure to 42 using uvc does
   * not work because from uvc's point of view the variable has not changed.
   */
  int ret;
  
  // Set the desired next state
  ret = mt9m114_s_sys_state(client, MT9M114_SYS_STATE_ENTER_CONFIG_CHANGE);

  if(ret) {
    eprint("Change config failed!");
    return ret;
  } else {
    dprint("Change config succeeded!");
    return 0;
  }
}

static int mt9m114_refresh(struct i2c_client* client) {
  int ret;

  // Issue the refresh command
  ret = mt9m114_issue_command(client, MT9M114_HOST_COMMAND_REFRESH);

  if(ret) {
    eprint("Refresh failed!");
    return ret;
  } else {
    dprint("Refresh succeeded!");
    return 0;
  }
}

static int mt9m114_apply_patch(struct i2c_client* client, const struct mt9m114_patch* patch) {
  int ret;
  ret = mt9m114_write16(client, REG_ACCES_CTL_STAT, 0x0001);
  if(!ret) ret = mt9m114_write16(client, REG_PHYSICAL_ADDRESS_ACCESS, patch->physical_address);
  if(!ret) ret = mt9m114_burst_write(client, patch->patch_write_address, patch->patch, patch->patch_size);
  if(!ret) ret = mt9m114_write16(client, REG_LOGICAL_ADDRESS_ACCESS, 0x0000);
  if(!ret) ret = mt9m114_write16(client, VAR_PATCHLDR_LOADER_ADDRESS, patch->patch_loader_address);
  if(!ret) ret = mt9m114_write16(client, VAR_PATCHLDR_PATCH_ID, patch->patch_id);
  if(!ret) ret = mt9m114_write32(client, VAR_PATCHLDR_FIRMWARE_ID, patch->firmware_id);

  // Issue the apply patch command
  if(!ret) ret = mt9m114_issue_command(client, MT9M114_HOST_COMMAND_APPLY_PATCH);

  if(ret) {
    eprint("Applying patch %u \"%s\" failed!", patch->patch_id, patch->description);
    return ret;
  } else {
    dprint("Applying patch %u \"%s\" succeeded.", patch->patch_id, patch->description);
    return 0;
  }
}

static int mt9m114_wait_for_vertical_blanking(struct i2c_client* client) {
  int ret = mt9m114_write16(client, VAR_CMD_HANDLER_WAIT_EVENT_ID, 0x0002);
  if(!ret) ret = mt9m114_write16(client, VAR_CMD_HANDLER_NUM_EVENTS, 0x0001);
  if(!ret) ret = mt9m114_issue_command(client, MT9M114_HOST_COMMAND_WAIT_FOR_EVENT);

  if(ret) {
    eprint("Waiting for vertical blanking failed!");
    return ret;
  } else {
    return 0;
  }
}

static int mt9m114_set_streaming(struct i2c_client* client) {
  int ret;

  ret = mt9m114_s_sys_state(client, MT9M114_SYS_STATE_START_STREAMING);

  if(ret) {
    eprint("Changing state to streaming failed!");
    return ret;
  } else {
    iprint("Changed state to streaming.");
    return 0;
  }
}

static int mt9m114_set_suspend(struct i2c_client* client) {
  int ret;

  ret = mt9m114_s_sys_state(client, MT9M114_SYS_STATE_ENTER_SUSPEND);

  if(ret) {
    eprint("Changing state to suspend failed!");
    return ret;
  } else {
    iprint("Changed state to suspend.");
    return 0;
  }
}

static int mt9m114_g_auto_exposure(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u8 val8;
  int ret = mt9m114_read8(client, VAR_UVC_AE_MODE_CONTROL, &val8);
  if(val8 == 0x1) {
    *val = 0;
  } else if(val8 == 0x01 << 1) {
    *val = 1;
  } else if(val8 == 0x01 << 2) {
    *val = 2;
  } else if(val8 == 0x01 << 3) {
    *val = 3;
  } else {
    eprint("Unexpected value in UVC_AE_MODE_CONTROL: %d", val8); // This should never happen...
    return -EINVAL;
  }
  return ret;
}

static int mt9m114_g_auto_white_balance(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u8 val8;
  int ret = mt9m114_read8(client, VAR_UVC_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL, &val8);
  *val = val8;
  return ret;
}

static int mt9m114_g_backlight_compensation(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_UVC_BACKLIGHT_COMPENSATION_CONTROL, &val16);
  *val = val16;
  return ret;
}

static int mt9m114_g_brightness(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_UVC_BRIGHTNESS_CONTROL, &val16);
  *val = val16;
  return ret;
}

static int mt9m114_g_contrast(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_UVC_CONTRAST_CONTROL, &val16);
  *val = val16;
  return ret;
}

static int mt9m114_g_do_white_balance(struct v4l2_subdev* sd, s32* val) {
  *val = 0; // Do white ballance is a button. Thus, 0 is always returned.
  return 0;
}

static int mt9m114_g_exposure(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u32 val32;
  int ret = mt9m114_read32(client, VAR_UVC_EXPOSURE_TIME_ABSOLUTE_CONTROL, &val32);
  *val = (s32)val32;
  return ret;
}

static int mt9m114_g_exposure_algorithm(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_AE_RULE_ALGO, &val16);
  *val = (s32)val16;
  return ret;
}

static int mt9m114_g_gain(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_UVC_GAIN_CONTROL, &val16);
  *val = val16;
  return ret;
}

static int mt9m114_g_gamma(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_UVC_GAMMA_CONTROL, &val16);
  *val = val16;
  return ret;
}

static int mt9m114_g_hue(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_UVC_HUE_CONTROL, &val16);
  *val = (s16)(val16) / 100;
  return ret;
}

static int mt9m114_g_power_line_frequency(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u8 val8;
  int ret = mt9m114_read8(client, VAR_UVC_POWER_LINE_FREQUENCY_CONTROL, &val8);
  *val = val8;
  return ret;
}

static int mt9m114_g_saturation(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_UVC_SATURATION_CONTROL, &val16);
  *val = val16;
  return ret;
}

static int mt9m114_g_sharpness(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_UVC_SHARPNESS_CONTROL, &val16);
  *val = (s16)val16;
  return ret;
}

static int mt9m114_g_white_balance_temperature(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_UVC_WHITE_BALANCE_TEMPERATURE_CONTROL, &val16);
  *val = val16;
  return ret;
}

static int mt9m114_g_vertical_flip(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_CAM_SENSOR_CONTROL_READ_MODE, &val16);
  *val = val16 & 0x0002 ? 1 : 0;
  return ret;
}

static int mt9m114_g_horizontal_flip(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_CAM_SENSOR_CONTROL_READ_MODE, &val16);
  *val = val16 & 0x0001 ? 1 : 0;
  return ret;
}

static int mt9m114_g_fade_to_black(struct v4l2_subdev* sd, s32* val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u16 val16;
  int ret = mt9m114_read16(client, VAR_LL_MODE, &val16);
  *val = val16 & 0x0008 ? 1 : 0;
  return ret;
}

static int mt9m114_s_auto_exposure(struct v4l2_subdev* sd, s32 val) {
  // When ae is disabled (mode 0 or 2) the values of frame interval, exposure
  // and gain will be set to the ones used while ae was enabled (mode 1 or 3)!
  struct mt9m114_state* state = to_state(sd);
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  u32 chip_frameinterval;
  u32 assumed_chip_frameinterval;
  int ret;

  iprint("Setting auto exposure to %d.", val);
  ret = mt9m114_write8(client, VAR_UVC_AE_MODE_CONTROL, 0x01 << val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  if(!ret) state->ae_enabled = (bool)val;

  // Assert framerate did not change after ae change
  if(!ret && !state->ae_enabled) {
    ret = mt9m114_read32(client, VAR_UVC_FRAME_INTERVAL_CONTROL, &chip_frameinterval);
    assumed_chip_frameinterval = mt9m114_to_chip_frameinterval(state->current_frame_interval);
    if(!ret) {
      if(chip_frameinterval != assumed_chip_frameinterval) {
        wprint("frame interval changed after ae was disabled! Assumed %d, was %d", assumed_chip_frameinterval, chip_frameinterval);
      }
    }
  }
  return ret;
}

static int mt9m114_s_auto_white_balance(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  struct mt9m114_state* state = to_state(sd);
  int ret = 0;
  s32 exposure = 0;
  // It looks like auto white balance changes the exposure when changed,
  // but this changes is not applied to the uvc variable.
  if(!state->ae_enabled) {
    ret = mt9m114_g_exposure(sd, &exposure);
  }
  iprint("Setting auto white balance to %d.", val);
  if(!ret) ret = mt9m114_write8(client, VAR_UVC_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_change_config(client);
  
  if(!ret && !state->ae_enabled) {
    mt9m114_s_exposure(sd, exposure + 1);
    mt9m114_s_exposure(sd, exposure - 1);
    ret = mt9m114_s_exposure(sd, exposure);
  }
  return ret;
}

static int mt9m114_s_backlight_compensation(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  MT9M114_ERROR_CODES uvc_status;
  int ret;

  iprint("Setting backlight compensation to %d.", val);
  ret = mt9m114_write16(client, VAR_UVC_BACKLIGHT_COMPENSATION_CONTROL, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  if(!ret) ret = mt9m114_g_uvc_status(client, &uvc_status);
  if(!ret && uvc_status > MT9M114_ERROR_CODE_ENOERR) {
    eprint("Backlight compensation could not be changed!");
    if(uvc_status == MT9M114_ERROR_CODE_EACCESS) {
      eprint("This is due to auto-exposure beeing disabled.");
    }
    eprint("UVC Result Status is: %d.", uvc_status);
    ret = -uvc_status;
  }
  return ret;
}

static int mt9m114_s_brightness(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;

  iprint("Setting brightness to %d", val);
  ret = mt9m114_write16(client, VAR_UVC_BRIGHTNESS_CONTROL, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  return ret;
}

static int mt9m114_s_contrast(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  MT9M114_ERROR_CODES uvc_status;
  int ret;

  iprint("Setting contrast to %d", val);
  ret = mt9m114_write16(client, VAR_UVC_CONTRAST_CONTROL, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  if(!ret) ret = mt9m114_g_uvc_status(client, &uvc_status);
  if(!ret && uvc_status > MT9M114_ERROR_CODE_ENOERR) {
    eprint("Contrast could not be changed!");
    if(uvc_status == MT9M114_ERROR_CODE_EACCESS) {
      eprint("This is due to automatic contrast curve calculation beeing disabled.");
    }
    eprint("UVC Result Status is: %d.", uvc_status);
    ret = -uvc_status;
  }
  return ret;
}

static int mt9m114_s_do_white_balance(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;
  bool awb_enabled;

  iprint("Doing a one time auto white balance.");
  awb_enabled = !(ret = mt9m114_s_auto_white_balance(sd, 0x01));
  // Whait two frames
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(awb_enabled) ret = mt9m114_s_auto_white_balance(sd, 0x00);
  return ret;
}

static int mt9m114_s_exposure(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  MT9M114_ERROR_CODES uvc_status;
  int ret;

  iprint("Setting exposure to %d", val);
  ret = mt9m114_write32(client, VAR_UVC_EXPOSURE_TIME_ABSOLUTE_CONTROL, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  if(!ret) ret = mt9m114_g_uvc_status(client, &uvc_status);
  if(!ret && uvc_status > MT9M114_ERROR_CODE_ENOERR) {
    if(uvc_status == MT9M114_ERROR_CODE_ERANGE) {
      wprint("Requested exposure time was clamped!");
    } else {
      eprint("Exposure time could not be changed!");
      if(uvc_status == MT9M114_ERROR_CODE_EBUSY) {
        eprint("This is due to auto-exposure beeing enabled.");
      }
      eprint("UVC Result Status is: %d.", uvc_status);
    }
    ret = -uvc_status;
  }

  return ret;
}

static int mt9m114_s_exposure_algorithm(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;

  iprint("Setting exposure algorithm to %d", val);
  ret = mt9m114_write16(client, VAR_AE_RULE_ALGO, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  return ret;
}

static int mt9m114_s_gain(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  MT9M114_ERROR_CODES uvc_status;
  int ret;

  iprint("Setting gain to %d", val);
  ret = mt9m114_write16(client, VAR_UVC_GAIN_CONTROL, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  if(!ret) ret = mt9m114_g_uvc_status(client, &uvc_status);
  if(!ret && uvc_status > MT9M114_ERROR_CODE_ENOERR) {
    if(uvc_status == MT9M114_ERROR_CODE_ERANGE) {
      wprint("Requested gain was clamped!");
    } else {
      eprint("Gain could not be changed!");
      if(uvc_status == MT9M114_ERROR_CODE_EBUSY) {
        eprint("This is due to auto-exposure beeing enabled.");
      }
      eprint("UVC Result Status is: %d.", uvc_status);
    }
    ret = -uvc_status;
  }
  return ret;
}

static int mt9m114_s_gamma(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  MT9M114_ERROR_CODES uvc_status;
  int ret;

  iprint("Setting gamma to %d", val);
  ret = mt9m114_write16(client, VAR_UVC_GAMMA_CONTROL, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  if(!ret) ret = mt9m114_g_uvc_status(client, &uvc_status);
  if(!ret && uvc_status > MT9M114_ERROR_CODE_ENOERR) {
    eprint("Gamma could not be changed!");
    if(uvc_status == MT9M114_ERROR_CODE_EACCESS) {
      eprint("This is due to automatic gamma curve calculation beeing disabled.");
    }
    eprint("UVC Result Status is: %d.", uvc_status);
    ret = -uvc_status;
  }
  return ret;
}

static int mt9m114_s_hue(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;
  s32 val32 = val * 100;
  iprint("Setting hue to %d", val);
  ret = mt9m114_write16(client, VAR_UVC_HUE_CONTROL, (s16)val32);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  return ret;
}

static int mt9m114_s_power_line_frequency(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;

  iprint("Setting power line frequency to %d", val);
  ret = mt9m114_write8(client, VAR_UVC_POWER_LINE_FREQUENCY_CONTROL, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  return ret;
}

static int mt9m114_s_saturation(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;

  iprint("Setting saturation to %d", val);
  ret = mt9m114_write16(client, VAR_UVC_SATURATION_CONTROL, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  return ret;
}

static int mt9m114_s_sharpness(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;

  iprint("Setting sharpness to %d", val);
  ret = mt9m114_write16(client, VAR_UVC_SHARPNESS_CONTROL, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  return ret;
}

static int mt9m114_s_white_balance_temperature(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  MT9M114_ERROR_CODES uvc_status;
  int ret;

  iprint("Setting white balance temperature to %d", val);
  ret = mt9m114_write16(client, VAR_UVC_WHITE_BALANCE_TEMPERATURE_CONTROL, val);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_refresh(client);
  if(!ret) ret = mt9m114_g_uvc_status(client, &uvc_status);
  if(!ret && uvc_status > MT9M114_ERROR_CODE_ENOERR) {
    if(uvc_status == MT9M114_ERROR_CODE_ERANGE) {
      wprint("Requested temperature was clamped!");
    } else {
      eprint("Temperature could not be changed!");
      if(uvc_status == MT9M114_ERROR_CODE_EBUSY) {
        eprint("This is due to auto white balance beeing enabled.");
      }
      eprint("UVC Result Status is: %d.", uvc_status);
    }
    ret = -uvc_status;
  }
  return ret;
}

static int mt9m114_s_vertical_flip(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;

  iprint("Setting vertical flip to %d", val);
  if(val) {
    ret = mt9m114_read_modify_write16(client, VAR_CAM_SENSOR_CONTROL_READ_MODE, 0x0002, 0x0000);
  } else {
    ret = mt9m114_read_modify_write16(client, VAR_CAM_SENSOR_CONTROL_READ_MODE, 0x0000, 0x0002);
  }
  if(!ret) ret = mt9m114_change_config(client);
  return ret;
}

static int mt9m114_s_horizontal_flip(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;

  iprint("Setting horizontal flip to %d", val);
  if(val) {
    ret = mt9m114_read_modify_write16(client, VAR_CAM_SENSOR_CONTROL_READ_MODE, 0x0001, 0x0000);
  } else {
    ret = mt9m114_read_modify_write16(client, VAR_CAM_SENSOR_CONTROL_READ_MODE, 0x0000, 0x0001);
  }
  if(!ret) ret = mt9m114_change_config(client);
  return ret;
}

static int mt9m114_s_fade_to_black(struct v4l2_subdev* sd, s32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;

  iprint("Setting fade to black to %d", val);
  if(val) {
    ret = mt9m114_read_modify_write16(client, VAR_LL_MODE, 0x0008, 0x0000);
  } else {
    ret = mt9m114_read_modify_write16(client, VAR_LL_MODE, 0x0000, 0x0008);
  }
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  return ret;
}

static const struct mt9m114_fmt* mt9m114_find_best_fmt(u32 pixelformat) {
  int i;
  for(i = 0; i < MT9M114_NUM_FMTS; ++i) {
    if(pixelformat == mt9m114_fmts[i].pixelformat) {
      return &mt9m114_fmts[i];
    }
  }
  // Default to the first fmt.
  return &mt9m114_fmts[0];
}

static const struct mt9m114_framesize* mt9m114_find_best_framesize(u32 width, u32 height) {
  const struct mt9m114_framesize* curr_fsize;
  const struct mt9m114_framesize* best_fit = NULL;
  u32 min_dist = 0xFFFFFFFF;
  u32 dist;
  int i;
  for(i = 0; i < MT9M114_NUM_FRAMESIZES; ++i) {
    curr_fsize = &mt9m114_framesizes[i];
    if(curr_fsize->width == width && curr_fsize->height == height) {
      return curr_fsize;
    } else {
      dist = abs(width - curr_fsize->width) + abs(height - curr_fsize->height);
      if(dist < min_dist) {
        best_fit = curr_fsize;
        min_dist = dist;
      }
    }
  }
  return best_fit;
}

static int mt9m114_s_img_fmt(struct v4l2_subdev* sd, const struct mt9m114_fmt* fmt) {
  // A change config it needed for the new image format to take effect!

  struct mt9m114_state* state = to_state(sd);
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;

  dprint("");

  ret = mt9m114_write_array(client, fmt->regs);
  if(!ret) {
    state->current_fmt = fmt;
    return 0;
  } else {
    return ret;
  }
}

static int mt9m114_s_framesize(struct v4l2_subdev* sd, const struct mt9m114_framesize* fsize) {
  // A change config it needed for the new framesize to take effect!

  struct mt9m114_state* state = to_state(sd);
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;

  dprint("");

  ret = mt9m114_write_array(client, fsize->regs);
  if(!ret) {
    state->current_framesize = fsize;
    return 0;
  } else {
    return ret;
  }
}

static int mt9m114_s_frameinterval(struct v4l2_subdev* sd, const struct v4l2_fract* frame_interval) {
  // Wait for vertical blanking after this method!

  struct mt9m114_state* state = to_state(sd);
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  MT9M114_ERROR_CODES errval;
  u16 chip_framerate;
  u32 chip_frameinterval;
  int ret;

  dprint("");

  chip_framerate = mt9m114_to_chip_framerate(frame_interval);

  ret = mt9m114_write16(client, VAR_CAM_AET_MAX_FRAME_RATE, chip_framerate);
  if(!ret) ret = mt9m114_write16(client, VAR_CAM_AET_MIN_FRAME_RATE, chip_framerate);
  if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
  if(!ret) ret = mt9m114_change_config(client);
  // make UVC variables coherent
  if(!state->ae_enabled) {
    chip_frameinterval = mt9m114_to_chip_frameinterval(frame_interval);
    iprint("Setting uvc frame intercal to %d", chip_frameinterval);
    if(!ret) ret = mt9m114_write32(client, VAR_UVC_FRAME_INTERVAL_CONTROL, chip_frameinterval);
    if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
    if(!ret) ret = mt9m114_g_uvc_status(client, &errval);
    if(!ret) {
      if(errval == MT9M114_ERROR_CODE_EBUSY) {
        wprint("UVC result status is EBUSY: Auto-exposure mode is enabled although it should not be!");
      } else if(errval == MT9M114_ERROR_CODE_ERANGE) {
        wprint("UVC result status is ERANGE: Frame rate war clamped");
      }
    }
  }

  if(!ret) {
    state->current_frame_interval = frame_interval;
    return 0;
  } else {
    return ret;
  }
}

// v4l2 subdev core ops --------------------------------------------------------

static int mt9m114_g_chip_ident(struct v4l2_subdev* sd, struct v4l2_dbg_chip_ident* chip) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  struct mt9m114_state* state = to_state(sd);

  dprint("");

  return v4l2_chip_ident_i2c_client(client, chip, V4L2_IDENT_MT9M114, state->revision);
}

static int mt9m114_init(struct v4l2_subdev* sd, u32 val) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  struct mt9m114_state* state = to_state(sd);
  int ret;

  ret = mt9m114_reset(sd, 0);
  if(!ret) ret = mt9m114_write_array(client, pll_settings);
  if(!ret) ret = mt9m114_write_array(client, sensor_optimization);
  if(!ret) ret = mt9m114_write_array(client, errata_1);
  if(!ret) ret = mt9m114_write_array(client, errata_2);

  if(!ret) ret = mt9m114_change_config(client);

  // Apply patches
  if(!ret) ret = mt9m114_apply_patch(client, &black_level_correction_fix);
  if(!ret) ret = mt9m114_apply_patch(client, &adaptive_sensitivity);

  // Set status so that mt9m114_s_frameinterval does not complain
  if(!ret) {
    state->ae_enabled = true;
  }

  // Set format, framesize and framerate
  if(!ret) ret = mt9m114_s_img_fmt(sd, &mt9m114_fmts[0]); // set YCbCr as default
  if(!ret) ret = mt9m114_s_framesize(sd, &mt9m114_framesizes[0]); //1280x960
  if(!ret) ret = mt9m114_s_frameinterval(sd, &mt9m114_framesizes[0].frame_intervals[0]); // 30 fps

  // Set some additional settings
  if(!ret) ret = mt9m114_write_array(client, pga_settings);
  if(!ret) ret = mt9m114_write_array(client, awb_settings);
  if(!ret) ret = mt9m114_write_array(client, CPIPE_preference);
  if(!ret) ret = mt9m114_write_array(client, features);

  // set uvc variables -> enable ae and awb
  if(!ret) ret = mt9m114_write_array(client, uvc_settings);

  if(!ret) ret = mt9m114_change_config(client);

  if(ret) {
    eprint("Initialization of chip @ 0x%X-%u failed.", client->addr, i2c_adapter_id(client->adapter));
    return ret;
  } else {
    iprint("Initialization of chip @ 0x%X-%u succeeded.", client->addr, i2c_adapter_id(client->adapter));
    return 0;
  }
}

static int mt9m114_reset(struct v4l2_subdev* sd, u32 val) {
  // According to the documentation the chip should be in streaming mode after
  // reset is finished. That is not the case. A change config command is
  // necessary for that.
  int ret;
  struct i2c_client* client = v4l2_get_subdevdata(sd);

  ret = mt9m114_read_modify_write16(client, REG_RESET_AND_MISC_CONTROL, 0x0001, 0x0000);
  if(!ret) msleep(100);
  if(!ret) ret = mt9m114_read_modify_write16(client, REG_RESET_AND_MISC_CONTROL, 0x0000, 0x0001);
  if(!ret) msleep(100);
  // Whait for the initiallization
  if(!ret) ret = mt9m114_poll16(client, REG_COMMAND_REGISTER, (u16)MT9M114_HOST_COMMAND_SET_STATE, 0x0000, 5, 100);
  if(!ret) ret = mt9m114_write_array(client, errata_2);

  if(ret) {
    eprint("Resetting chip failed!");
    return ret;
  } else {
    iprint("Resetting chip succeeded.");
    return 0;
  }
}

static int mt9m114_queryctrl(struct v4l2_subdev* sd, struct v4l2_queryctrl* qctrl) {
  switch(qctrl->id) {
    case V4L2_CID_EXPOSURE_AUTO:
      return v4l2_ctrl_query_fill(qctrl, 0, 3, 1, 1);
    case V4L2_CID_AUTO_WHITE_BALANCE:
      return v4l2_ctrl_query_fill(qctrl, 0, 1, 1, 1);
    case V4L2_CID_BACKLIGHT_COMPENSATION:
      return v4l2_ctrl_query_fill(qctrl, 0, 4, 1, 1);
    case V4L2_CID_BRIGHTNESS:
      return v4l2_ctrl_query_fill(qctrl, 0, 255, 1, 55);
    case V4L2_CID_CONTRAST:
      return v4l2_ctrl_query_fill(qctrl, 16, 64, 1, 32);
    case V4L2_CID_DO_WHITE_BALANCE:
      return v4l2_ctrl_query_fill(qctrl, 0, 1, 1, 0);
    case V4L2_CID_EXPOSURE:
      return v4l2_ctrl_query_fill(qctrl, 1, 1000, 1, 1);
    case V4L2_CID_EXPOSURE_ALGORITHM:
      return v4l2_ctrl_query_fill(qctrl, 0, 3, 1, 1);
    case V4L2_CID_GAIN:
      return v4l2_ctrl_query_fill(qctrl, 0, 255, 1, 32);
    case V4L2_CID_GAMMA:
      return v4l2_ctrl_query_fill(qctrl, 1, 1000, 1, 220);
    case V4L2_CID_HUE:
      return v4l2_ctrl_query_fill(qctrl, -22, 22, 1, 0);
    case V4L2_CID_POWER_LINE_FREQUENCY:
      return v4l2_ctrl_query_fill(qctrl, 1, 2, 1, 2);
    case V4L2_CID_SATURATION:
      return v4l2_ctrl_query_fill(qctrl, 0, 255, 1, 128);
    case V4L2_CID_SHARPNESS:
      return v4l2_ctrl_query_fill(qctrl, -7, 7, 1, 0);
    case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
      return v4l2_ctrl_query_fill(qctrl, 2700, 6500, 1, 6500);
    case V4L2_CID_VFLIP:
      return v4l2_ctrl_query_fill(qctrl, 0, 1, 1, 0);
    case V4L2_CID_HFLIP:
      return v4l2_ctrl_query_fill(qctrl, 0, 1, 1, 0);
    case V4L2_MT9M114_FADE_TO_BLACK:
      qctrl->minimum = 0;
      qctrl->maximum = 1;
      qctrl->step = 1;
      qctrl->default_value = 1;
      qctrl->reserved[0] = qctrl->reserved[1] = 0;
      strcpy(qctrl->name, "Fade to Black");
      qctrl->type = V4L2_CTRL_TYPE_BOOLEAN;
      qctrl->flags = 0;
      return 0;
    default:
      return -EINVAL;
  }
}

static int mt9m114_g_ctrl(struct v4l2_subdev* sd, struct v4l2_control* ctrl) {
  const char* name;
  switch(ctrl->id) {
    case V4L2_CID_EXPOSURE_AUTO:
      return mt9m114_g_auto_exposure(sd, &ctrl->value);
    case V4L2_CID_AUTO_WHITE_BALANCE:
      return mt9m114_g_auto_white_balance(sd, &ctrl->value);
    case V4L2_CID_BACKLIGHT_COMPENSATION:
      return mt9m114_g_backlight_compensation(sd, &ctrl->value);
    case V4L2_CID_BRIGHTNESS:
      return mt9m114_g_brightness(sd, &ctrl->value);
    case V4L2_CID_CONTRAST:
      return mt9m114_g_contrast(sd, &ctrl->value);
    case V4L2_CID_DO_WHITE_BALANCE:
      return mt9m114_g_do_white_balance(sd, &ctrl->value);
    case V4L2_CID_EXPOSURE:
      return mt9m114_g_exposure(sd, &ctrl->value);
    case V4L2_CID_EXPOSURE_ALGORITHM:
      return mt9m114_g_exposure_algorithm(sd, &ctrl->value);
    case V4L2_CID_GAIN:
      return mt9m114_g_gain(sd, &ctrl->value);
    case V4L2_CID_GAMMA:
      return mt9m114_g_gamma(sd, &ctrl->value);
    case V4L2_CID_HUE:
      return mt9m114_g_hue(sd, &ctrl->value);
    case V4L2_CID_POWER_LINE_FREQUENCY:
      return mt9m114_g_power_line_frequency(sd, &ctrl->value);
    case V4L2_CID_SATURATION:
      return mt9m114_g_saturation(sd, &ctrl->value);
    case V4L2_CID_SHARPNESS:
      return mt9m114_g_sharpness(sd, &ctrl->value);
    case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
      return mt9m114_g_white_balance_temperature(sd, &ctrl->value);
    case V4L2_CID_VFLIP:
      return mt9m114_g_vertical_flip(sd, &ctrl->value);
    case V4L2_CID_HFLIP:
      return mt9m114_g_horizontal_flip(sd, &ctrl->value);
    case V4L2_MT9M114_FADE_TO_BLACK:
      return mt9m114_g_fade_to_black(sd, &ctrl->value);
    default:
      name = v4l2_ctrl_get_name(ctrl->id);
      if(name == NULL) {
        eprint("Unsupported v4l2_control issued with id: %u!", ctrl->id);
      } else {
        eprint("Unsupported v4l2_control issued: %s!", name);
      }
      return -EINVAL;
  }
}

static int mt9m114_s_ctrl(struct v4l2_subdev* sd, struct v4l2_control* ctrl) {
  const char* name;
  switch(ctrl->id) {
    case V4L2_CID_EXPOSURE_AUTO:
      return mt9m114_s_auto_exposure(sd, ctrl->value);
    case V4L2_CID_AUTO_WHITE_BALANCE:
      return mt9m114_s_auto_white_balance(sd, ctrl->value);
    case V4L2_CID_BACKLIGHT_COMPENSATION:
      return mt9m114_s_backlight_compensation(sd, ctrl->value);
    case V4L2_CID_BRIGHTNESS:
      return mt9m114_s_brightness(sd, ctrl->value);
    case V4L2_CID_CONTRAST:
      return mt9m114_s_contrast(sd, ctrl->value);
    case V4L2_CID_DO_WHITE_BALANCE:
      return mt9m114_s_do_white_balance(sd, ctrl->value);
    case V4L2_CID_EXPOSURE:
      return mt9m114_s_exposure(sd, ctrl->value);
    case V4L2_CID_EXPOSURE_ALGORITHM:
      return mt9m114_s_exposure_algorithm(sd, ctrl->value);
    case V4L2_CID_GAIN:
      return mt9m114_s_gain(sd, ctrl->value);
    case V4L2_CID_GAMMA:
      return mt9m114_s_gamma(sd, ctrl->value);
    case V4L2_CID_HUE:
      return mt9m114_s_hue(sd, ctrl->value);
    case V4L2_CID_POWER_LINE_FREQUENCY:
      return mt9m114_s_power_line_frequency(sd, ctrl->value);
    case V4L2_CID_SATURATION:
      return mt9m114_s_saturation(sd, ctrl->value);
    case V4L2_CID_SHARPNESS:
      return mt9m114_s_sharpness(sd, ctrl->value);
    case V4L2_CID_WHITE_BALANCE_TEMPERATURE:
      return mt9m114_s_white_balance_temperature(sd, ctrl->value);
    case V4L2_CID_VFLIP:
      return mt9m114_s_vertical_flip(sd, ctrl->value);
    case V4L2_CID_HFLIP:
      return mt9m114_s_horizontal_flip(sd, ctrl->value);
    case V4L2_MT9M114_FADE_TO_BLACK:
      return mt9m114_s_fade_to_black(sd, ctrl->value);
    default:
      name = v4l2_ctrl_get_name(ctrl->id);
      if(name == NULL) {
        eprint("Unsupported v4l2_control issued with id: %u!", ctrl->id);
      } else {
        eprint("Unsupported v4l2_control issued: %s!", name);
      }
      return -EINVAL;
  }
}

#ifdef CONFIG_VIDEO_ADV_DEBUG
static int mt9m114_g_register(struct v4l2_subdev* sd, struct v4l2_dbg_register* reg) {
  int ret;
  u32 val = 0;
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  switch(reg->size) {
    case 1:
      ret = mt9m114_read8(client, (u16)reg->reg, (u8*)(&val));
      break;
    case 2:
      ret = mt9m114_read16(client, (u16)reg->reg, (u16*)(&val));
      break;
    case 4:
      ret = mt9m114_read32(client, (u16)reg->reg, (u32*)(&val));
      break;
    default:
      eprint("Unsupported register size issued: %u!", (u16)reg->size);
      return -EINVAL;
  }
  if(ret) {
    reg->val = val;
    dprint("Register: 0x%X, size: %u, value: 0x%X.", (u16)reg->reg, (u16)reg->size, (u32)reg->val);
    return ret;
  } else {
    return 0;
  }
}

static int mt9m114_s_register(struct v4l2_subdev* sd, struct v4l2_dbg_register* reg) {
  int ret;
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  switch(reg->size) {
    case 1:
      ret = mt9m114_write8(client, (u16)reg->reg, (u8)reg->val);
      break;
    case 2:
      ret = mt9m114_write16(client, (u16)reg->reg, (u16)reg->val);
      break;
    case 4:
      ret = mt9m114_write32(client, (u16)reg->reg, (u32)reg->val);
      break;
    default:
      eprint("Unsupported register size issued: %u!", (u16)reg->size);
      return -EINVAL;
  }

  if(!ret) ret = mt9m114_refresh(client);

  if(ret) {
    dprint("Register: 0x%X, size: %u, value: 0x%X.", (u16)reg->reg, (u16)reg->size, (u32)reg->val);
    return ret;
  } else {
    return 0;
  }
}
#endif

// v4l2 subdev video ops -------------------------------------------------------

static int mt9m114_enum_fmt(struct v4l2_subdev* sd, struct v4l2_fmtdesc* fmt) {
  const struct mt9m114_fmt* curr_fmt;
  u32 i = fmt->index;

  dprint("");

  if(fmt->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || i >= MT9M114_NUM_FMTS) {
    return -EINVAL;
  }

  curr_fmt = &mt9m114_fmts[i];

  strcpy(fmt->description, curr_fmt->description);
  fmt->flags = 0;
  fmt->pixelformat = curr_fmt->pixelformat;
  memset(fmt->reserved, 0, sizeof (fmt->reserved));

  return 0;
}

static int mt9m114_enum_framesizes(struct v4l2_subdev* sd, struct v4l2_frmsizeenum* fsize) {
  const struct mt9m114_framesize* curr_fsize;
  u32 i = fsize->index;

  dprint("");

  if(fsize->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || i >= MT9M114_NUM_FRAMESIZES) {
    return -EINVAL;
  }

  curr_fsize = &mt9m114_framesizes[i];

  fsize->type = V4L2_FRMSIZE_TYPE_DISCRETE;
  fsize->discrete.width = curr_fsize->width;
  fsize->discrete.height = curr_fsize->height;
  memset(fsize->reserved, 0, sizeof (fsize->reserved));

  return 0;
}

static int mt9m114_enum_frameintervals(struct v4l2_subdev* sd, struct v4l2_frmivalenum* fival) {
  const struct mt9m114_framesize* curr_fsize = NULL;
  u32 i = fival->index;

  dprint("");

  // find selected framesize
  curr_fsize = mt9m114_find_best_framesize(fival->width, fival->height);

  if(fival->type != V4L2_BUF_TYPE_VIDEO_CAPTURE || i >= MT9M114_MAX_FRAME_INTERVALS
          || curr_fsize->width != fival->width || curr_fsize->height != fival->height // check if return framesize is the requested one
          || curr_fsize->frame_intervals[i].numerator == empty_frame_interval.numerator) {
    return -EINVAL;
  }

  fival->type = V4L2_FRMIVAL_TYPE_DISCRETE;
  fival->discrete = curr_fsize->frame_intervals[i];
  memset(fival->reserved, 0, sizeof (fival->reserved));

  return 0;
}

static int mt9m114_g_fmt(struct v4l2_subdev* sd, struct v4l2_format* fmt) {
  struct mt9m114_state* state = to_state(sd);
  struct v4l2_pix_format* pix = &fmt->fmt.pix;
  const struct mt9m114_framesize* cur_fsize = state->current_framesize;
  const struct mt9m114_fmt* cur_fmt = state->current_fmt;
  dprint("");

  if(fmt->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
    return -EINVAL;
  }

  pix->width = cur_fsize->width;
  pix->height = cur_fsize->height;
  pix->pixelformat = cur_fmt->pixelformat;
  pix->field = V4L2_FIELD_NONE;
  pix->bytesperline = cur_fmt->bytes_per_pixel * cur_fsize->width;
  pix->sizeimage = pix->bytesperline * cur_fsize->height;
  pix->colorspace = V4L2_COLORSPACE_JPEG;
  pix->priv = 0;

  return 0;
}

static int mt9m114_try_fmt(struct v4l2_subdev* sd, struct v4l2_format* fmt) {
  struct v4l2_pix_format* pix = &fmt->fmt.pix;
  const struct mt9m114_fmt* img_fmt;
  const struct mt9m114_framesize* fsize;
  dprint("");

  if(fmt->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
    return -EINVAL;
  }

  img_fmt = mt9m114_find_best_fmt(pix->pixelformat);
  fsize = mt9m114_find_best_framesize(pix->width, pix->height);

  pix->width = fsize->width;
  pix->height = fsize->height;
  pix->pixelformat = img_fmt->pixelformat;
  pix->field = V4L2_FIELD_NONE;
  pix->bytesperline = img_fmt->bytes_per_pixel * fsize->width;
  pix->sizeimage = pix->bytesperline * fsize->height;
  pix->colorspace = V4L2_COLORSPACE_JPEG;
  pix->priv = 0;

  return 0;
}

static int mt9m114_s_fmt(struct v4l2_subdev* sd, struct v4l2_format* fmt) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  struct v4l2_pix_format* pix = &fmt->fmt.pix;
  const struct mt9m114_fmt* img_fmt;
  const struct mt9m114_framesize* fsize;
  int ret;

  dprint("");

  if(fmt->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
    return -EINVAL;
  }

  img_fmt = mt9m114_find_best_fmt(pix->pixelformat);
  fsize = mt9m114_find_best_framesize(pix->width, pix->height);

  dprint("Requested format:");
  dprint("\twidth: %d", pix->width);
  dprint("\theight: %d", pix->height);

  ret = mt9m114_s_img_fmt(sd, img_fmt);
  if(!ret) ret = mt9m114_s_framesize(sd, fsize);
  if(!ret) ret = mt9m114_change_config(client);
  if(!ret) {
    pix->width = fsize->width;
    pix->height = fsize->height;
    pix->pixelformat = img_fmt->pixelformat;
    pix->field = V4L2_FIELD_NONE;
    pix->bytesperline = img_fmt->bytes_per_pixel * fsize->width;
    pix->sizeimage = pix->bytesperline * fsize->height;
    pix->colorspace = V4L2_COLORSPACE_JPEG;
    pix->priv = 0;

    iprint("Set format:");
    iprint("\twidth: %d", pix->width);
    iprint("\theight: %d", pix->height);

    return 0;
  } else {
    return ret;
  }
}

static int mt9m114_cropcap(struct v4l2_subdev* sd, struct v4l2_cropcap* cc) {
  dprint("");

  if(cc->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
    return -EINVAL;
  }

  cc->bounds.left = 0;
  cc->bounds.top = 0;
  cc->bounds.width = 1280;
  cc->bounds.height = 960;

  cc->defrect.left = 0;
  cc->defrect.top = 0;
  cc->defrect.width = 1280;
  cc->defrect.height = 960;

  cc->pixelaspect.numerator = 1;
  cc->pixelaspect.denominator = 1;

  return 0;
}

static int mt9m114_g_crop(struct v4l2_subdev* sd, struct v4l2_crop* c) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  int ret;
  u16 val;

  dprint("");

  if(c->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
    return -EINVAL;
  }

  ret = mt9m114_read16(client, VAR_CAM_CROP_WINDOW_XOFFSET, &val);
  if(!ret) c->c.left = val;
  if(!ret) ret = mt9m114_read16(client, VAR_CAM_CROP_WINDOW_YOFFSET, &val);
  if(!ret) c->c.top = val;
  if(!ret) ret = mt9m114_read16(client, VAR_CAM_CROP_WINDOW_WIDTH, &val);
  if(!ret) c->c.width = val;
  if(!ret) ret = mt9m114_read16(client, VAR_CAM_CROP_WINDOW_HEIGHT, &val);
  if(!ret) c->c.height = val;

  return ret;
}

static int mt9m114_s_crop(struct v4l2_subdev* sd, struct v4l2_crop* c) {
  dprint("");
  // Cropping is not supported due to forth bit set in VAR_CAM_OUTPUT_FORMAT
  return -EINVAL;
}

static int mt9m114_g_parm(struct v4l2_subdev* sd, struct v4l2_streamparm* params) {
  struct mt9m114_state* state = to_state(sd);
  struct v4l2_captureparm* cparams = &params->parm.capture;

  dprint("");

  if(params->type != V4L2_BUF_TYPE_VIDEO_CAPTURE) {
    return -EINVAL;
  }

  cparams->capability = V4L2_CAP_TIMEPERFRAME;
  cparams->capturemode = 0;
  cparams->timeperframe = *state->current_frame_interval;
  cparams->extendedmode = 0;
  cparams->readbuffers = 0;
  memset(cparams->reserved, 0, sizeof (cparams->reserved));
  return 0;
}

static int mt9m114_s_parm(struct v4l2_subdev* sd, struct v4l2_streamparm* params) {
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  struct mt9m114_state* state = to_state(sd);
  struct v4l2_captureparm* cparams = &params->parm.capture;
  const struct v4l2_fract* fival;
  int i;
  bool tpf_is_legal = false;
  int ret = 0;

  dprint("");

  if(params->type != V4L2_BUF_TYPE_VIDEO_CAPTURE ||
          cparams->extendedmode != 0 ||
          cparams->capturemode != 0 ||
          cparams->timeperframe.numerator == 0 ||
          cparams->timeperframe.denominator == 0) {
    return -EINVAL;
  }

  for(i = 0; i < MT9M114_MAX_FRAME_INTERVALS; ++i) {
    fival = &state->current_framesize->frame_intervals[i];
    if(fival->numerator == empty_frame_interval.numerator) {
      break;
    }

    if(fival->denominator == cparams->timeperframe.denominator &&
            fival->numerator == cparams->timeperframe.numerator) {
      tpf_is_legal = true;
      break;
    }
  }

  if(!tpf_is_legal) {
    eprint("Requested frame interval is illegal: %d / %d", cparams->timeperframe.numerator, cparams->timeperframe.denominator);
    return -EINVAL;
  } else {
    dprint("Set frame interval: %d / %d", cparams->timeperframe.numerator, cparams->timeperframe.denominator);
    ret = mt9m114_s_frameinterval(sd, &cparams->timeperframe);
    if(!ret) ret = mt9m114_wait_for_vertical_blanking(client);
    return ret;
  }
}

static int mt9m114_s_stream(struct v4l2_subdev* sd, int enable) {
  MT9M114_SYS_STATES state;
  int ret;
  struct i2c_client* client = v4l2_get_subdevdata(sd);
  
  // Save old state
  ret = mt9m114_g_sys_state(client, &state);

  // Set the desired next state (SYS_STATE_ENTER_CONFIG_CHANGE = 0x28)
  if(!ret) {
    if(enable && state == MT9M114_SYS_STATE_SUSPENDED) {
      ret = mt9m114_set_streaming(client);
    } else if(enable && state == MT9M114_SYS_STATE_STREAMING) {
      // Do nothing
    } else if(!enable && state == MT9M114_SYS_STATE_STREAMING) {
      ret = mt9m114_set_suspend(client);
    } else if(!enable && state == MT9M114_SYS_STATE_SUSPENDED) {
      // Do nothing
    } else {
      eprint("The chip is in an unsupported state 0x%X!", state);
      ret = -EFAULT;
    }
  }

  return ret;
}

// i2c driver methods ----------------------------------------------------------

static s32 mt9m114_probe(struct i2c_client* client, const struct i2c_device_id* id) {
  struct v4l2_subdev* sd;
  struct mt9m114_state* state;
  int ret;

  state = kzalloc(sizeof (struct mt9m114_state), GFP_KERNEL);
  if(state == NULL) {
    return -ENOMEM;
  }
  sd = &state->sd;
  v4l2_i2c_subdev_init(sd, client, &mt9m114_ops);

  ret = mt9m114_detect(client, state);
  if(ret) {
    v4l2_device_unregister_subdev(sd);
    kfree(state);
    return ret;
  }
  return mt9m114_init(sd, 0);
}

static s32 mt9m114_remove(struct i2c_client* client) {
  struct v4l2_subdev* sd = i2c_get_clientdata(client);
  if(sd != NULL) {
    v4l2_device_unregister_subdev(sd);
    kfree(to_state(sd));
  }
  iprint("Removed chip @ 0x%X-%u", client->addr, i2c_adapter_id(client->adapter));
  return 0;
}

// kernel methods --------------------------------------------------------------

static __init int init_mt9m114(void) {
  return i2c_add_driver(&mt9m114_driver);
}

static __exit void exit_mt9m114(void) {
  i2c_del_driver(&mt9m114_driver);
}

module_init(init_mt9m114);
module_exit(exit_mt9m114);
