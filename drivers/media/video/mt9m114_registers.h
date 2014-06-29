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

#ifndef MT9M114_REGISTERS_H
#define	MT9M114_REGISTERS_H

// Core Registers --------------------------------------------------------------
#define REG_Y_ADDR_START_                               0x3002
#define REG_X_ADDR_START_                               0x3004
#define REG_Y_ADDR_END_                                 0x3006
#define REG_X_ADDR_END_                                 0x3008
#define REG_FRAME_LENGTH_LINES_                         0x300A
#define REG_LINE_LENGTH_PCK_                            0x300C
#define REG_COARSE_INTEGRATION_TIME_                    0x3012
#define REG_FINE_INTEGRATION_TIME_                      0x3014
#define REG_RESET_REGISTER                              0x301A
#define REG_FLASH                                       0x3046
#define REG_FLASH_COUNT                                 0x3048
#define REG_GREEN1_GAIN                                 0x3056
#define REG_BLUE_GAIN                                   0x3058
#define REG_RED_GAIN                                    0x305A
#define REG_GREEN2_GAIN                                 0x305C
#define REG_GLOBAL_GAIN                                 0x305E
#define REG_FUSE_ID1                                    0x31F4
#define REG_FUSE_ID2                                    0x31F6
#define REG_FUSE_ID3                                    0x31F8
#define REG_FUSE_ID4                                    0x31FA
#define REG_CHAIN_CONTROL                               0x31FC
#define REG_COSTOMER_REV                                0x31FE

// SOC1 Registers --------------------------------------------------------------
#define REG_COLOR_PIPELINE_CONTROL                      0x3210

// SOC2 Registers --------------------------------------------------------------
#define REG_P_G1_P0Q0                                   0x3640
#define REG_P_G1_P0Q1                                   0x3642
#define REG_P_G1_P0Q2                                   0x3644
#define REG_P_G1_P0Q3                                   0x3646
#define REG_P_G1_P0Q4                                   0x3648
#define REG_P_R_P0Q0                                    0x364A
#define REG_P_R_P0Q1                                    0x364C
#define REG_P_R_P0Q2                                    0x364E
#define REG_P_R_P0Q3                                    0x3650
#define REG_P_R_P0Q4                                    0x3652
#define REG_P_B_P0Q0                                    0x3654
#define REG_P_B_P0Q1                                    0x3656
#define REG_P_B_P0Q2                                    0x3658
#define REG_P_B_P0Q3                                    0x365A
#define REG_P_B_P0Q4                                    0x365C
#define REG_P_G2_P0Q0                                   0x365E
#define REG_P_G2_P0Q1                                   0x3660
#define REG_P_G2_P0Q2                                   0x3662
#define REG_P_G2_P0Q3                                   0x3664
#define REG_P_G2_P0Q4                                   0x3666
#define REG_P_G1_P1Q0                                   0x3680
#define REG_P_G1_P1Q1                                   0x3682
#define REG_P_G1_P1Q2                                   0x3684
#define REG_P_G1_P1Q3                                   0x3686
#define REG_P_G1_P1Q4                                   0x3688
#define REG_P_R_P1Q0                                    0x368A
#define REG_P_R_P1Q1                                    0x368C
#define REG_P_R_P1Q2                                    0x368E
#define REG_P_R_P1Q3                                    0x3690
#define REG_P_R_P1Q4                                    0x3692
#define REG_P_B_P1Q0                                    0x3694
#define REG_P_B_P1Q1                                    0x3696
#define REG_P_B_P1Q2                                    0x3698
#define REG_P_B_P1Q3                                    0x369A
#define REG_P_B_P1Q4                                    0x369C
#define REG_P_G2_P1Q0                                   0x369E
#define REG_P_G2_P1Q1                                   0x36A0
#define REG_P_G2_P1Q2                                   0x36A2
#define REG_P_G2_P1Q3                                   0x36A4
#define REG_P_G2_P1Q4                                   0x36A6
#define REG_P_G1_P2Q0                                   0x36C0
#define REG_P_G1_P2Q1                                   0x36C2
#define REG_P_G1_P2Q2                                   0x36C4
#define REG_P_G1_P2Q3                                   0x36C6
#define REG_P_G1_P2Q4                                   0x36C8
#define REG_P_R_P2Q0                                    0x36CA
#define REG_P_R_P2Q1                                    0x36CC
#define REG_P_R_P2Q2                                    0x36CE
#define REG_P_R_P2Q3                                    0x36D0
#define REG_P_R_P2Q4                                    0x36D2
#define REG_P_B_P2Q0                                    0x36D4
#define REG_P_B_P2Q1                                    0x36D6
#define REG_P_B_P2Q2                                    0x36D8
#define REG_P_B_P2Q3                                    0x36DA
#define REG_P_B_P2Q4                                    0x36DC
#define REG_P_G2_P2Q0                                   0x36DE
#define REG_P_G2_P2Q1                                   0x36E0
#define REG_P_G2_P2Q2                                   0x36E2
#define REG_P_G2_P2Q3                                   0x36E4
#define REG_P_G2_P2Q4                                   0x36E6
#define REG_P_G1_P3Q0                                   0x3700
#define REG_P_G1_P3Q1                                   0x3702
#define REG_P_G1_P3Q2                                   0x3704
#define REG_P_G1_P3Q3                                   0x3706
#define REG_P_G1_P3Q4                                   0x3708
#define REG_P_R_P3Q0                                    0x370A
#define REG_P_R_P3Q1                                    0x370C
#define REG_P_R_P3Q2                                    0x370E
#define REG_P_R_P3Q3                                    0x3710
#define REG_P_R_P3Q4                                    0x3712
#define REG_P_B_P3Q0                                    0x3714
#define REG_P_B_P3Q1                                    0x3716
#define REG_P_B_P3Q2                                    0x3718
#define REG_P_B_P3Q3                                    0x371A
#define REG_P_B_P3Q4                                    0x371C
#define REG_P_G2_P3Q0                                   0x371E
#define REG_P_G2_P3Q1                                   0x3720
#define REG_P_G2_P3Q2                                   0x3722
#define REG_P_G2_P3Q3                                   0x3724
#define REG_P_G2_P3Q4                                   0x3726
#define REG_P_G1_P4Q0                                   0x3740
#define REG_P_G1_P4Q1                                   0x3742
#define REG_P_G1_P4Q2                                   0x3744
#define REG_P_G1_P4Q3                                   0x3746
#define REG_P_G1_P4Q4                                   0x3748
#define REG_P_R_P4Q0                                    0x374A
#define REG_P_R_P4Q1                                    0x374C
#define REG_P_R_P4Q2                                    0x374E
#define REG_P_R_P4Q3                                    0x3750
#define REG_P_R_P4Q4                                    0x3752
#define REG_P_B_P4Q0                                    0x3754
#define REG_P_B_P4Q1                                    0x3756
#define REG_P_B_P4Q2                                    0x3758
#define REG_P_B_P4Q3                                    0x375A
#define REG_P_B_P4Q4                                    0x375C
#define REG_P_G2_P4Q0                                   0x375E
#define REG_P_G2_P4Q1                                   0x3760
#define REG_P_G2_P4Q2                                   0x3762
#define REG_P_G2_P4Q3                                   0x3764
#define REG_P_G2_P4Q4                                   0x3766
#define REG_CENTER_ROW                                  0x3782
#define REG_CENTER_COLMN                                0x3784

// SYSCTL Registers ------------------------------------------------------------
#define REG_CHIP_ID                                     0x0000
#define REG_CLOCKS_CONTROL                              0x0016
#define REG_RESET_AND_MISC_CONTROL                      0x001A
#define REG_PAD_SLEW                                    0x001E
#define REG_USER_DEFINED_DEVICE_ADDRESS_ID              0x002E
#define REG_PAD_CONTROL                                 0x0032
#define REG_COMMAND_REGISTER                            0x0080

// XDMA Registers --------------------------------------------------------------
#define REG_ACCES_CTL_STAT                              0x0982
#define REG_PHYSICAL_ADDRESS_ACCESS                     0x098A
#define REG_LOGICAL_ADDRESS_ACCESS                      0x098E
#define REG_MCU_VARIABLE_DATA0                          0x0990
#define REG_MCU_VARIABLE_DATA1                          0x0992
#define REG_MCU_VARIABLE_DATA2                          0x0994
#define REG_MCU_VARIABLE_DATA3                          0x0996
#define REG_MCU_VARIABLE_DATA4                          0x0998
#define REG_MCU_VARIABLE_DATA5                          0x099A
#define REG_MCU_VARIABLE_DATA6                          0x099C
#define REG_MCU_VARIABLE_DATA7                          0x099E

// Monitor Variables -----------------------------------------------------------
#define VAR_MON_MAJOR_VERSION                           0x8000
#define VAR_MON_MONIR_VERSION                           0x8002
#define VAR_MON_RELEASE_VERSION                         0x8004
#define VAR_MON_HEARTBEAT                               0x8006

// Sequencer Variables ---------------------------------------------------------
#define VAR_SEQ_ERROR_CODE                              0x8406

// AE_Rule Variables -----------------------------------------------------------
#define VAR_AE_RULE_ALGO                                0xA404
#define VAR_AE_RULE_AVG_Y_FROM_START                    0xA406
//... Probably no need
#define VAR_AE_RULE_AE_ADAPTIVE_STRENGTH                0xA420

// AE_Track Variables ----------------------------------------------------------
#define VAR_AE_TRACK_STATUS                             0xA800
#define VAR_AE_TRACK_ALGO                               0xA804
#define VAR_AE_TRACK_TARGET_AVERAGE_LUMA                0xA807
#define VAR_AE_TRACK_GATE_PERCENTAGE                    0xA808
#define VAR_AE_TRACK_CURRENT_AVERAGE_LUMA               0xA809
#define VAR_AE_TRACK_AE_TRACKING_DAMPENING_SPEED        0xA80A
#define VAR_AE_TRACK_AE_DAMPENING_SPEED                 0xA80B
#define VAR_AE_TRACK_SKIP_FRAMES_COUNTER                0xA80D
#define VAR_AE_TRACK_CURRENT_FLICKER_LINES              0xA80E
#define VAR_AE_TRACK_FDZONE                             0xA818
#define VAR_AE_TRACK_ZONE                               0xA81B
#define VAR_AE_TRACK_FLICKER_LINES_50HZ                 0xA826
#define VAR_AE_TRACK_VIRT_EXPOSURE_LOG                  0xA828
#define VAR_AE_TRACK_MIN_VIRT_EXPOSURE_LOG_ZONE0        0xA82A
#define VAR_AE_TRACK_MAX_VIRT_EXPOSURE_LOG_ZONE0        0xA82C
#define VAR_AE_TRACK_MAX_VIRT_EXPOSURE_LOG_ZONE1        0xA82E
#define VAR_AE_TRACK_VIRT_GAIN                          0xA838

// AWB Variables ---------------------------------------------------------------
#define VAR_AWB_STATUS                                  0xAC00
#define VAR_AWB_MODE                                    0xAC02
#define VAR_AWB_R_RATIO_LOWER                           0xAC06
#define VAR_AWB_R_RATIO_UPPER                           0xAC07
#define VAR_AWB_B_RATIO_LOWER                           0xAC08
#define VAR_AWB_B_RATIO_UPPER                           0xAC09
#define VAR_AWB_R_SCENE_RATIO_LOWER                     0xAC0A
#define VAR_AWB_R_SCENE_RATIO_UPPER                     0xAC0B
#define VAR_AWB_B_SCENE_RATIO_LOWER                     0xAC0C
#define VAR_AWB_B_SCENE_RATIO_UPPER                     0xAC0D
#define VAR_AWB_R_RATIO_PRE_AWB                         0xAC0E
#define VAR_AWB_B_RATIO_PRE_AWB                         0xAC0F
#define VAR_AWB_R_GAIN                                  0xAC12
#define VAR_AWB_B_GAIN                                  0xAC14
#define VAR_AWB_PRE_AWB_RATIOS_TRACKING_SPEED           0xAC16
#define VAR_AWB_PIXEL_THRESHOLD_COUNT                   0xAC18

// Black Level Variables -------------------------------------------------------
#define VAR_BLACKLEVEL_ALGO                             0xB004
#define VAR_BLACKLEVEL_MAX_BLACK_LEVEL                  0xB00C
#define VAR_BLACKLEVEL_BLACK_LEVEL_DAMPING              0xB00D

// CCM Variables ---------------------------------------------------------------
#define VAR_CCM_ALGO                                    0xB404
#define VAR_CCM_0                                       0xB406
#define VAR_CCM_1                                       0xB408
#define VAR_CCM_2                                       0xB40A
#define VAR_CCM_3                                       0xB40C
#define VAR_CCM_4                                       0xB40E
#define VAR_CCM_5                                       0xB410
#define VAR_CCM_6                                       0xB412
#define VAR_CCM_7                                       0xB414
#define VAR_CCM_8                                       0xB416
#define VAR_CCM_LL_DELTA_CCM_0                          0xB418
#define VAR_CCM_LL_DELTA_CCM_1                          0xB41A
#define VAR_CCM_LL_DELTA_CCM_2                          0xB41C
#define VAR_CCM_LL_DELTA_CCM_3                          0xB41E
#define VAR_CCM_LL_DELTA_CCM_4                          0xB420
#define VAR_CCM_LL_DELTA_CCM_5                          0xB422
#define VAR_CCM_LL_DELTA_CCM_6                          0xB424
#define VAR_CCM_LL_DELTA_CCM_7                          0xB426
#define VAR_CCM_LL_DELTA_CCM_8                          0xB428
#define VAR_CCM_DELTA_GAIN                              0xB42A
#define VAR_CCM_DELTA_THRESH                            0xB42B

// Low Loght Variables ---------------------------------------------------------
#define VAR_LL_MODE                                     0xBC02
#define VAR_LL_ALGO                                     0xBC04
#define VAR_LL_GAMMA_SELECT                             0xBC07
#define VAR_LL_GAMMA_CONTRAST_CURVE_0                   0xBC0A
#define VAR_LL_GAMMA_CONTRAST_CURVE_1                   0xBC0B
#define VAR_LL_GAMMA_CONTRAST_CURVE_2                   0xBC0C
#define VAR_LL_GAMMA_CONTRAST_CURVE_3                   0xBC0D
#define VAR_LL_GAMMA_CONTRAST_CURVE_4                   0xBC0E
#define VAR_LL_GAMMA_CONTRAST_CURVE_5                   0xBC0F
#define VAR_LL_GAMMA_CONTRAST_CURVE_6                   0xBC10
#define VAR_LL_GAMMA_CONTRAST_CURVE_7                   0xBC11
#define VAR_LL_GAMMA_CONTRAST_CURVE_8                   0xBC12
#define VAR_LL_GAMMA_CONTRAST_CURVE_9                   0xBC13
#define VAR_LL_GAMMA_CONTRAST_CURVE_10                  0xBC14
#define VAR_LL_GAMMA_CONTRAST_CURVE_11                  0xBC15
#define VAR_LL_GAMMA_CONTRAST_CURVE_12                  0xBC16
#define VAR_LL_GAMMA_CONTRAST_CURVE_13                  0xBC17
#define VAR_LL_GAMMA_CONTRAST_CURVE_14                  0xBC18
#define VAR_LL_GAMMA_CONTRAST_CURVE_15                  0xBC19
#define VAR_LL_GAMMA_CONTRAST_CURVE_16                  0xBC1A
#define VAR_LL_GAMMA_CONTRAST_CURVE_17                  0xBC1B
#define VAR_LL_GAMMA_CONTRAST_CURVE_18                  0xBC1C
#define VAR_LL_GAMMA_NRCURVE_0                          0xBC1D
#define VAR_LL_GAMMA_NRCURVE_1                          0xBC1E
#define VAR_LL_GAMMA_NRCURVE_2                          0xBC1F
#define VAR_LL_GAMMA_NRCURVE_3                          0xBC20
#define VAR_LL_GAMMA_NRCURVE_4                          0xBC21
#define VAR_LL_GAMMA_NRCURVE_5                          0xBC22
#define VAR_LL_GAMMA_NRCURVE_6                          0xBC23
#define VAR_LL_GAMMA_NRCURVE_7                          0xBC24
#define VAR_LL_GAMMA_NRCURVE_8                          0xBC25
#define VAR_LL_GAMMA_NRCURVE_9                          0xBC26
#define VAR_LL_GAMMA_NRCURVE_10                         0xBC27
#define VAR_LL_GAMMA_NRCURVE_11                         0xBC28
#define VAR_LL_GAMMA_NRCURVE_12                         0xBC29
#define VAR_LL_GAMMA_NRCURVE_13                         0xBC2A
#define VAR_LL_GAMMA_NRCURVE_14                         0xBC2B
#define VAR_LL_GAMMA_NRCURVE_15                         0xBC2C
#define VAR_LL_GAMMA_NRCURVE_16                         0xBC2D
#define VAR_LL_GAMMA_NRCURVE_17                         0xBC2E
#define VAR_LL_GAMMA_NRCURVE_18                         0xBC2F
#define VAR_LL_BM_PRECISION_BITS                        0xBC31
#define VAR_LL_AVERAGE_LUMA_FADE_TO_BLACK               0xBC3A
#define VAR_LL_FADE_TO_BLACK_DAMPENING_SPEED            0xBC3C

// Camera Control Variables ----------------------------------------------------
#define VAR_CAM_SENSOR_CFG_Y_ADDR_START                 0xC800
#define VAR_CAM_SENSOR_CFG_X_ADDR_START                 0xC802
#define VAR_CAM_SENSOR_CFG_Y_ADDR_END                   0xC804
#define VAR_CAM_SENSOR_CFG_X_ADDR_END                   0xC806
#define VAR_CAM_SENSOR_CFG_PIXCLK                       0xC808
#define VAR_CAM_SENSOR_CFG_ROW_SPEED                    0xC80C
#define VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MIN          0xC80E
#define VAR_CAM_SENSOR_CFG_FINE_INTEG_TIME_MAX          0xC810
#define VAR_CAM_SENSOR_CFG_FRAME_LENGTH_LINES           0xC812
#define VAR_CAM_SENSOR_CFG_LINE_LENGTH_PCK              0xC814
#define VAR_CAM_SENSOR_CFG_FINE_CORRECTION              0xC816
#define VAR_CAM_SENSOR_CFG_CPIPE_LAST_ROW               0xC818
#define VAR_CAM_SENSOR_CFG_REG_0_DATA                   0xC826
#define VAR_CAM_SENSOR_CONTROL_READ_MODE                0xC834
#define VAR_CAM_SENSOR_CONTROL_ALANOG_GAIN              0xC836
#define VAR_CAM_SENSOR_CONTROL_VIRT_COLUMN_GAIN         0xC838
#define VAR_CAM_SENSOR_CONTROL_FRAME_LENGTH_LINES       0xC83A
#define VAR_CAM_SENSOR_CONTROL_COARSE_INTEGRATION_TIME  0xC83C
#define VAR_CAM_SENSOR_CONTROL_FINE_INTEGRATION_TIME    0xC83E
#define VAR_CAM_CPIPE_CONTROL_DGAIN_RED                 0xC840
#define VAR_CAM_CPIPE_CONTROL_DGAIN_GREEN1              0xC842
#define VAR_CAM_CPIPE_CONTROL_DGAIN_GREEN2              0xC844
#define VAR_CAM_CPIPE_CONTROL_DGAIN_BLUE                0xC846
#define VAR_CAM_CPIPE_CONTROL_DGAIN_SECOND              0xC848
#define VAR_CAM_CPIPE_CONTROL_SECOND_BLACK_LEVEL        0xC84B
#define VAR_CAM_MODE_SELECT                             0xC84C
#define VAR_CAM_MODE_TEST_PATTERN_SELECT                0xC84D
#define VAR_CAM_MODE_TEST_PATTERN_RED                   0xC84E
#define VAR_CAM_MODE_TEST_PATTERN_GREEN                 0xC850
#define VAR_CAM_MODE_TEST_PATTERN_BLUE                  0xC852
#define VAR_CAM_CROP_WINDOW_XOFFSET                     0xC854
#define VAR_CAM_CROP_WINDOW_YOFFSET                     0xC856
#define VAR_CAM_CROP_WINDOW_WIDTH                       0xC858
#define VAR_CAM_CROP_WINDOW_HEIGHT                      0xC85A
#define VAR_CAM_CROP_CROPMODE                           0xC85C
#define VAR_CAM_SCALE_VERTICAL_TC_MODE                  0xC85E
#define VAR_CAM_SCALE_VERTICAL_TC_PERCENTAGE            0xC860
#define VAR_CAM_SCALE_VERTICAL_TC_STRECH_FACTOR         0xC862
#define VAR_CAM_OUTPUT_WIDTH                            0xC868
#define VAR_CAM_OUTPUT_HEIGHT                           0xC86A
#define VAR_CAM_OUTPUT_FORMAT                           0xC86C
#define VAR_CAM_OUTPUT_FORMAT_YUV                       0xC86E
#define VAR_CAM_OUTPUT_Y_OFFSET                         0xC870
#define VAR_CAM_HUE_ANGLE                               0xC873
#define VAR_CAM_SFX_CONTROL                             0xC874
#define VAR_CAM_SFX_SOLARIZATION_THESH                  0xC875
#define VAR_CAM_SFX_SEPIA_CR                            0xC876
#define VAR_CAM_SFX_SPEIA_CB                            0xC877
#define VAR_CAM_AET_AEMODE                              0xC878
#define VAR_CAM_AET_SKIP_FRAMES                         0xC879
#define VAR_CAM_AET_TARGET_AVERAGE_LUMA                 0xC87A
#define VAR_CAM_AET_TARGET_AVERAGE_LUMA_DARK            0xC87B
#define VAR_CAM_AET_BLACK_CLIPPING_TARET                0xC87C
#define VAR_CAM_AET_AE_MIN_VIRT_INT_TIME_PCLK           0xC87E
#define VAR_CAM_AET_AE_MIN_VIRT_DGAIN                   0xC880
#define VAR_CAM_AET_AE_MAX_VIRT_DGAIN                   0xC882
#define VAR_CAM_AET_AE_MIN_VIRT_AGAIN                   0xC884
#define VAR_CAM_AET_AE_MAX_VIRT_AGAIN                   0xC886
#define VAR_CAM_AET_AE_VIRT_GAIN_TH_EG                  0xC888
#define VAR_CAM_AET_AE_EG_GATE_PERCENTAGE               0xC88A
#define VAR_CAM_AET_FLICKER_FREQ_HZ                     0xC88B
#define VAR_CAM_AET_MAX_FRAME_RATE                      0xC88C
#define VAR_CAM_AET_MIN_FRAME_RATE                      0xC88E
#define VAR_CAM_AET_TARGET_GAIN                         0xC890
#define VAR_CAM_AWB_CCM_L_0                             0xC892
#define VAR_CAM_AWB_CCM_L_1                             0xC894
#define VAR_CAM_AWB_CCM_L_2                             0xC896
#define VAR_CAM_AWB_CCM_L_3                             0xC898
#define VAR_CAM_AWB_CCM_L_4                             0xC89A
#define VAR_CAM_AWB_CCM_L_5                             0xC89C
#define VAR_CAM_AWB_CCM_L_6                             0xC89E
#define VAR_CAM_AWB_CCM_L_7                             0xC8A0
#define VAR_CAM_AWB_CCM_L_8                             0xC8A2
#define VAR_CAM_AWB_CCM_M_0                             0xC8A4
#define VAR_CAM_AWB_CCM_M_1                             0xC8A6
#define VAR_CAM_AWB_CCM_M_2                             0xC8A8
#define VAR_CAM_AWB_CCM_M_3                             0xC8AA
#define VAR_CAM_AWB_CCM_M_4                             0xC8AC
#define VAR_CAM_AWB_CCM_M_5                             0xC8AE
#define VAR_CAM_AWB_CCM_M_6                             0xC8B0
#define VAR_CAM_AWB_CCM_M_7                             0xC8B2
#define VAR_CAM_AWB_CCM_M_8                             0xC8B4
#define VAR_CAM_AWB_CCM_R_0                             0xC8B6
#define VAR_CAM_AWB_CCM_R_1                             0xC8B8
#define VAR_CAM_AWB_CCM_R_2                             0xC8BA
#define VAR_CAM_AWB_CCM_R_3                             0xC8BC
#define VAR_CAM_AWB_CCM_R_4                             0xC8BE
#define VAR_CAM_AWB_CCM_R_5                             0xC8C0
#define VAR_CAM_AWB_CCM_R_6                             0xC8C2
#define VAR_CAM_AWB_CCM_R_7                             0xC8C4
#define VAR_CAM_AWB_CCM_R_8                             0xC8C6
#define VAR_CAM_AWB_CCM_L_RG_GAIN                       0xC8C8
#define VAR_CAM_AWB_CCM_L_BG_GAIN                       0xC8CA
#define VAR_CAM_AWB_CCM_M_RG_GAIN                       0xC8CC
#define VAR_CAM_AWB_CCM_M_BG_GAIN                       0xC8CE
#define VAR_CAM_AWB_CCM_R_RG_GAIN                       0xC8D0
#define VAR_CAM_AWB_CCM_R_BG_GAIN                       0xC8D2
#define VAR_CAM_AWB_CCM_L_CTEMP                         0xC8D4
#define VAR_CAM_AWB_CCM_M_CTEMP                         0xC8D6
#define VAR_CAM_AWB_CCM_R_CTEMP                         0xC8D8
#define VAR_CAM_AWB_LL_CCM_0                            0xC8DA
#define VAR_CAM_AWB_LL_CCM_1                            0xC8DC
#define VAR_CAM_AWB_LL_CCM_2                            0xC8DE
#define VAR_CAM_AWB_LL_CCM_3                            0xC8E0
#define VAR_CAM_AWB_LL_CCM_4                            0xC8E2
#define VAR_CAM_AWB_LL_CCM_5                            0xC8E4
#define VAR_CAM_AWB_LL_CCM_6                            0xC8E6
#define VAR_CAM_AWB_LL_CCM_7                            0xC8E8
#define VAR_CAM_AWB_LL_CCM_8                            0xC8EA
#define VAR_CAM_AWB_COLOR_TEMPERATURE_MIN               0xC8EC
#define VAR_CAM_AWB_COLOR_TEMPERATURE_MAX               0xC8EE
#define VAR_CAM_AWB_COLOR_TEMPERATURE                   0xC8F0
#define VAR_CAM_AWB_AWB_XSCALE                          0xC8F2
#define VAR_CAM_AWB_AWB_YSCALE                          0xC8F3
#define VAR_CAM_AWB_AWB_WEIGHTS_0                       0xC8F4
#define VAR_CAM_AWB_AWB_WEIGHTS_1                       0xC8F6
#define VAR_CAM_AWB_AWB_WEIGHTS_2                       0xC8F8
#define VAR_CAM_AWB_AWB_WEIGHTS_3                       0xC8FA
#define VAR_CAM_AWB_AWB_WEIGHTS_4                       0xC8FC
#define VAR_CAM_AWB_AWB_WEIGHTS_5                       0xC8FE
#define VAR_CAM_AWB_AWB_WEIGHTS_6                       0xC900
#define VAR_CAM_AWB_AWB_WEIGHTS_7                       0xC902
#define VAR_CAM_AWB_AWB_XSHIFT_PRE_ADJ                  0xC904
#define VAR_CAM_AWB_AWB_YSHIFT_PRE_ADJ                  0xC906
#define VAR_CAM_AWB_AWBMODE                             0xC909
#define VAR_CAM_AWB_TINTS_CTEMP_THRESHOLD               0xC90A
#define VAR_CAM_AWB_K_R_L                               0xC90C
#define VAR_CAM_AWB_K_G_L                               0xC90D
#define VAR_CAM_AWB_K_B_L                               0xC90E
#define VAR_CAM_AWB_K_R_R                               0xC90F
#define VAR_CAM_AWB_K_G_R                               0xC910
#define VAR_CAM_AWB_K_B_R                               0xC911
#define VAR_CAM_STAT_AWB_CLIP_WINDOW_XSTART             0xC914
#define VAR_CAM_STAT_AWB_CLIP_WINDOW_YSTART             0xC916
#define VAR_CAM_STAT_AWB_CLIP_WINDOW_XEND               0xC918
#define VAR_CAM_STAT_AWB_CLIP_WINDOW_YEND               0xC91A
#define VAR_CAM_STAT_AE_INITIAL_WINDOW_XSTART           0xC91C
#define VAR_CAM_STAT_AE_INITIAL_WINDOW_YSTART           0xC91E
#define VAR_CAM_STAT_AE_INITIAL_WINDOW_XEND             0xC920
#define VAR_CAM_STAT_AE_INITIAL_WINDOW_YEND             0xC922
#define VAR_CAM_LL_LLMODE                               0xC924
#define VAR_CAM_LL_START_BRIGHTNESS                     0xC926
#define VAR_CAM_LL_STOP_BRIGHTNESS                      0xC928
#define VAR_CAM_LL_START_SATURATION                     0xC92A
#define VAR_CAM_LL_END_SATURATION                       0xC92B
#define VAR_CAM_LL_START_DESATURATION                   0xC92C
#define VAR_CAM_LL_END_DESATURATION                     0xC92D
#define VAR_CAM_LL_START_DEMOSAIC                       0xC92E
#define VAR_CAM_LL_START_AP_GAIN                        0xC92F
#define VAR_CAM_LL_START_AP_THRESH                      0xC930
#define VAR_CAM_LL_STOP_DEMOSAIC                        0xC931
#define VAR_CAM_LL_STOP_AP_GAIN                         0xC932
#define VAR_CAM_LL_STOP_AP_THRESH                       0xC933
#define VAR_CAM_LL_START_NR_RED                         0xC934
#define VAR_CAM_LL_START_NR_GREEN                       0xC935
#define VAR_CAM_LL_START_NR_BLUE                        0xC936
#define VAR_CAM_LL_START_NR_THRESH                      0xC937
#define VAR_CAM_LL_STOP_NR_RED                          0xC938
#define VAR_CAM_LL_STOP_NR_GREEN                        0xC939
#define VAR_CAM_LL_STOP_NR_BLUE                         0xC93A
#define VAR_CAM_LL_STOP_NR_THRESH                       0xC93B
#define VAR_CAM_LL_START_CONTRAST_BM                    0xC93C
#define VAR_CAM_LL_STOP_CONTRAST_BM                     0xC93E
#define VAR_CAM_LL_GAMMA                                0xC940
#define VAR_CAM_LL_START_CONTRAST_GRADIENT              0xC942
#define VAR_CAM_LL_STOP_CONTRAST_GRADIENT               0xC943
#define VAR_CAM_LL_START_CONTRAST_LUMA_PERCENTAGE       0xC944
#define VAR_CAM_LL_STOP_CONTRAST_LUMA_PERCENTAGE        0xC945
#define VAR_CAM_LL_START_GAIN_METRIC                    0xC946
#define VAR_CAM_LL_STOP_GAIN_METRIC                     0xC948
#define VAR_CAM_LL_START_FADE_TO_BLACK_LUMA             0xC94A
#define VAR_CAM_LL_STOP_FADE_TO_BLACK_LUMA              0xC94C
#define VAR_CAM_LL_CLUSTER_DC_TH_BM                     0xC94E
#define VAR_CAM_LL_CLUSTER_DC_GATE_PERCENTAGE           0xC950
#define VAR_CAM_LL_SUMMING_SENSITIVITY_FACTOR           0xC951
#define VAR_CAM_LL_START_TARGET_LUMA_BM                 0xC952
#define VAR_CAM_LL_STOP_TARGET_LUMA_BM                  0xC954
#define VAR_CAM_LL_INV_BRIGHTNESS_METRIC                0xC956
#define VAR_CAM_LL_GAIN_METRIC                          0xC958
#define VAR_CAM_SEQ_UV_COLOR_BOOST                      0xC95A
#define VAR_CAM_PGA_PGA_CONTROL                         0xC95E
#define VAR_CAM_PGA_L_CONFIG_COLOR_TEMP                 0xC960
#define VAR_CAM_PGA_L_CONFIG_GREEN_RED_Q14              0xC962
#define VAR_CAM_PGA_L_CONFIG_RED_Q14                    0xC964
#define VAR_CAM_PGA_L_CONFIG_GREEN_BLUE_Q14             0xC966
#define VAR_CAM_PGA_L_CONFIG_BLUE_Q14                   0xC968
#define VAR_CAM_PGA_M_CONFIG_COLOR_TEMP                 0xC96A
#define VAR_CAM_PGA_M_CONFIG_GREEN_RED_Q14              0xC96C
#define VAR_CAM_PGA_M_CONFIG_RED_Q14                    0xC96E
#define VAR_CAM_PGA_M_CONFIG_GREEN_BLUE_Q14             0xC970
#define VAR_CAM_PGA_M_CONFIG_BLUE_Q14                   0xC972
#define VAR_CAM_PGA_R_CONFIG_COLOR_TEMP                 0xC974
#define VAR_CAM_PGA_R_CONFIG_GREEN_RED_Q14              0xC976
#define VAR_CAM_PGA_R_CONFIG_RED_Q14                    0xC978
#define VAR_CAM_PGA_R_CONFIG_GREEN_BLUE_Q14             0xC97A
#define VAR_CAM_PGA_R_CONFIG_BLUE_Q14                   0xC97C
#define VAR_CAM_SYSCTL_PLL_ENABLE                       0xC97E
#define VAR_CAM_SYSCTL_PLL_DIVIDER_M_N                  0xC980
#define VAR_CAM_SYSCTL_PLL_DIVIDER_P                    0xC982
#define VAR_CAM_PORT_OUTPUT_CONTROL                     0xC984
#define VAR_CAM_PORT_PORCH                              0xC986
#define VAR_CAM_PORT_MIPI_TIMING_T_HS_ZERO              0xC988
#define VAR_CAM_PORT_MIPI_TIMING_T_HS_EXIT_HS_TRAIL     0xC98A
#define VAR_CAM_PORT_MIPI_TIMING_T_CLK_POST_CLK_PRE     0xC98C
#define VAR_CAM_PORT_MIPI_TIMING_T_CLK_TRAIL_CLK_ZERO   0xC98E
#define VAR_CAM_PORT_MIPI_TIMING_T_LPX                  0xC990
#define VAR_CAM_PORT_MIPI_TIMING_INIT_TIMING            0xC992

// UVC Control Variables -------------------------------------------------------
#define VAR_UVC_AE_MODE_CONTROL                         0xCC00
#define VAR_UVC_WHITE_BALANCE_TEMPERATURE_AUTO_CONTROL  0xCC01
#define VAR_UVC_AE_PRIORITY_CONTROL                     0xCC02
#define VAR_UVC_POWER_LINE_FREQUENCY_CONTROL            0xCC03
#define VAR_UVC_EXPOSURE_TIME_ABSOLUTE_CONTROL          0xCC04
#define VAR_UVC_BACKLIGHT_COMPENSATION_CONTROL          0xCC08
#define VAR_UVC_BRIGHTNESS_CONTROL                      0xCC0A
#define VAR_UVC_CONTRAST_CONTROL                        0xCC0C
#define VAR_UVC_GAIN_CONTROL                            0xCC0E
#define VAR_UVC_HUE_CONTROL                             0xCC10
#define VAR_UVC_SATURATION_CONTROL                      0xCC12
#define VAR_UVC_SHARPNESS_CONTROL                       0xCC14
#define VAR_UVC_GAMMA_CONTROL                           0xCC16
#define VAR_UVC_WHITE_BALANCE_TEMPERATURE_CONTROL       0xCC18
#define VAR_UVC_FRAME_INTERVAL_CONTROL                  0xCC1C
#define VAR_UVC_MANUAL_EXPOSURE_CONFIGURATION           0xCC20
#define VAR_UVC_FLICKER_AVOIDANCE_CONFIGURATION         0xCC21
#define VAR_UVC_ALGO                                    0xCC22
#define VAR_UVC_RESULT_STATUS                           0xCC24

// System Mgr Variables --------------------------------------------------------
#define VAR_SYSMGR_NEXT_STATE                           0xDC00
#define VAR_SYSMGR_CURRENT_STATE                        0xDC01
#define VAR_SYSMGR_CMD_STATUS                           0xDC02

// Patch Loader Variables ------------------------------------------------------
#define VAR_PATCHLDR_LOADER_ADDRESS                     0xE000
#define VAR_PATCHLDR_PATCH_ID                           0xE002
#define VAR_PATCHLDR_FIRMWARE_ID                        0xE004
#define VAR_PATCHLDR_APPLY_STATUS                       0xE008
#define VAR_PATCHLDR_NUM_PATCHES                        0xE009
#define VAR_PATCHLDR_PATCH_ID_0                         0xE00A
#define VAR_PATCHLDR_PATCH_ID_1                         0xE00C
#define VAR_PATCHLDR_PATCH_ID_2                         0xE00E
#define VAR_PATCHLDR_PATCH_ID_3                         0xE010
#define VAR_PATCHLDR_PATCH_ID_4                         0xE012
#define VAR_PATCHLDR_PATCH_ID_5                         0xE014
#define VAR_PATCHLDR_PATCH_ID_6                         0xE016
#define VAR_PATCHLDR_PATCH_ID_7                         0xE018

// Patch Variables -------------------------------------------------------------
#define VAR_PATCHVARS_DELTA_DK_CORRECTION_FACTOR        0xE400

// Command Handler Variables ---------------------------------------------------
#define VAR_CMD_HANDLER_WAIT_EVENT_ID                   0xFC00
#define VAR_CMD_HANDLER_NUM_EVENTS                      0xFC02

#endif	/* MT9M114_REGISTERS_H */
