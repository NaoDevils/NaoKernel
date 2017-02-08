BKernel
=======
This is essentially [Aldebaran's 2.0 Kernel](https://github.com/aldebaran/linux-aldebaran/tree/release-2.0/atom) with a new camera driver and an alternative for the build in ethernet driver. Other than that nothing else was modified.

**Be aware** that you may need to update the .config file depending on the NAO OS you are using. The current file is from NAO OS 2.1.0. The config file can be found in /proc/config.bz. Make shure to rename it to .config.

Information about the camera driver
-----------------------------------
The camera driver provides some new features compared to the driver from Aldebaran. In addition to that the range of some values changed and the auto white balance is controlled by another ioctl.

The registers for the advanced auto exposure settings were adopted from [another camera manual](http://www.onsemi.com/pub_link/Collateral/AND9270-D.PDF). Check out figures 32, 38, 40 and the related sections for more information.

After building the kernel the new driver can be found in `drivers/media/video/mt9m114.ko` and has to be copied to `/lib/modules/2.6.33.9-rt31-aldebaran-rt/kernel/drivers/media/video/mt9m114.ko` on the robot.

A list of the new video controls:
<table>
    <tr>
        <td>Feature</td>
        <td>ioctl</td>
        <td>Notes</td>
    </tr>
    <tr>
        <td>enumerate all supported framesizes</td>
        <td>VIDIOC_ENUM_FRAMESIZES</td>
        <td>Enumerates all framesizes supported by the driver.</td>
    </tr>
    <tr>
        <td>enumerate all supported frameintervals</td>
        <td>VIDIOC_ENUM_FRAMEINTERVALS</td>
        <td>Enumerates all frameintervals supported by the driver.</td>
    </tr>
    <tr>
        <td>get image format</td>
        <td>VIDIOC_G_FMT</td>
        <td>Returns the currently used pixel format and the resolution of the images.</td>
    </tr>
</table>

A list of all supported mage settings:
<table>
    <tr>
        <td>Feature</td>
        <td>ioctl</td>
        <td>range</td>
        <td>Notes</td>
    </tr>
    <tr>
        <td>auto exposure</td>
        <td>V4L2_CID_EXPOSURE_AUTO</td>
        <td>min: 0 max: 3<br/>default: 1</td>
        <td>0: manual exposure mode<br/>1: auto exposure mode<br/>2: shutter priority mode (ae is disabled)<br/>3: aperture priority mode (ae is enabled)<br/>In addition to the first two modes also supported by Aldebaran, we added two other modes the chip supports. Maybe some of you can make use of them.<br/>Disabling ae will write the used exposure and contrast back, so that they can be read by the corresponding ioctls.</td>
    </tr>
    <tr>
        <td>auto white balance</td>
        <td>V4L2_CID_AUTO_WHITE_BALANCE</td>
        <td>min: 0 max: 1<br/>default: 1</td>
        <td>0: disable, 1: enable<br/>Disabling awb will write the used white balance temperature back, so that it can be read by the corresponding ioctl.</td>
    </tr>
    <tr>
        <td>backlight compensation</td>
        <td>V4L2_CID_BACKLIGHT_COMPENSATION</td>
        <td>min: 0 max: 4<br/>default: 1</td>
        <td>Only changeable while ae is enabled!</td>
    </tr>
    <tr>
        <td>brightness</td>
        <td>V4L2_CID_BRIGHTNESS</td>
        <td>min: 0 max: 255<br/>default: 55</td>
        <td>Only changeable while ae is enabled!</td>
    </tr>
    <tr>
        <td>contrast</td>
        <td>V4L2_CID_CONTRAST</td>
        <td>min: 16 max: 64<br/>default: 32</td>
        <td>Gradient of the contrast adjustment curve from 0.5 (16) to 2.0 (64)</td>
    </tr>
    <tr>
        <td>do an automatic white balance</td>
        <td>V4L2_CID_DO_WHITE_BALANCE</td>
        <td>min: 0 max: 1<br/>default: 0</td>
        <td>Does an automatic white balance. The new white balance can be read from the white balance temperature.<br/>This ioctl is a button, thus it will always return 0! **Be aware** that Aldebaran uses this ioctl for the white balance temperature.</td>
    </tr>
    <tr>
        <td>exposure time</td>
        <td>V4L2_CID_EXPOSURE</td>
        <td>min: 1 max: 1000<br/>default: 1</td>
        <td>The exposure can be changed only while ae is disabled! A value on one is 100 microseconds of exposure.</td>
    </tr>
    <tr>
        <td>exposure algorithm</td>
        <td>V4L2_CID_EXPOSURE_ALGORITHM</td>
        <td>min: 0 max: 3<br/>default: 1</td>
        <td>0: Average scene brightness<br/>1: weighted average scene brightness<br/>2: evaluated average scene brightness with frontlight detection<br/>3: evaluated average scene brightness with backlight detection</td>
    </tr>
    <tr>
        <td>gain</td>
        <td>V4L2_CID_GAIN</td>
        <td>min: 0 max: 255<br/>default: 32</td>
        <td>Only changeable while ae is disabled! A value of 32 equals to 1.0 gain.</td>
    </tr>
    <tr>
        <td>gamma correction</td>
        <td>V4L2_CID_GAMMA</td>
        <td>min: 0 max: 1000<br/>default: 220</td>
        <td>The gamma correction for the display in units multiplied by 100. A value of 220 equals to 2.2 gamma.</td>
    </tr>
    <tr>
        <td>hue</td>
        <td>V4L2_CID_HUE</td>
        <td>min: -22 max: 22<br/>default: 0</td>
        <td>Hue correction on degrees.</td>
    </tr>
    <tr>
        <td>power line frequency</td>
        <td>V4L2_CID_POWER_LINE_FREQUENCY</td>
        <td>min: 1 max: 2<br/>default: 2</td>
        <td>1: 50Hz, 2: 60Hz.<br/>The frequeny of the local power line so ae can avoid flicker.</td>
    </tr>
    <tr>
        <td>saturation</td>
        <td>V4L2_CID_SATURATION</td>
        <td>min: 0 max: 255<br/>default: 128</td>
        <td>Saturation control for the image. A value of zero results in grayscaled images while values above 128 result in over saturated images.</td>
    </tr>
    <tr>
        <td>sharpness</td>
        <td>V4L2_CID_SHARPNESS</td>
        <td>min: 0 max: 255<br/>default: 128</td>
        <td>Sharpness values are limited from 0 to +7. When seting the value tp -7 no sharpness will be applied.</td>
    </tr>
    <tr>
        <td>white balance temperature</td>
        <td>V4L2_CID_WHITE_BALANCE_TEMPERATURE</td>
        <td>min: 2700 max: 6500<br/>default: 6500</td>
        <td>The white balance temperature in kelvin. **Be aware** that Aldebaran uses another ioctl for this feature!</td>
    </tr>
    <tr>
        <td>vertical flip</td>
        <td>V4L2_CID_VFLIP</td>
        <td>min: 0 max: 1<br/>default: 0</td>
        <td></td>
    </tr>
    <tr>
        <td>horizontal flip</td>
        <td>V4L2_CID_HFLIP</td>
        <td>min: 0 max: 1<br/>default: 0</td>
        <td></td>
    </tr>
    <tr>
        <td>fade to black</td>
        <td>V4L2_MT9M114_FADE_TO_BLACK (V4L2_CID_PRIVATE_BASE)</td>
        <td>min: 0 max: 1<br/>default: 1</td>
        <td>0: disable, 1: enable<br/>When enabled the image will fade to black under low light conditions.</td>
    </tr>
    <tr>
        <td>target average luma</td>
        <td>V4L2_MT9M114_AE_TARGET_AVERAGE_LUMA (V4L2_CID_PRIVATE_BASE+1)</td>
        <td>min: 0 max: 255<br/>default: 55</td>
        <td>The brightness target to be maintained by the auto exposure for bright lighting conditions. (<a href="http://www.onsemi.com/pub_link/Collateral/AND9270-D.PDF">Figure 38</a>)</td>
    </tr>
    <tr>
        <td>target average luma dark</td>
        <td>V4L2_MT9M114_AE_TARGET_AVERAGE_LUMA_DARK (V4L2_CID_PRIVATE_BASE+2)</td>
        <td>min: 0 max: 255<br/>default: 27</td>
        <td>The brightness target to be maintained by the auto exposure for dark lighting conditions. (<a href="http://www.onsemi.com/pub_link/Collateral/AND9270-D.PDF">Figure 38</a>)</td>
    </tr>
    <tr>
        <td>target analog gain</td>
        <td>V4L2_MT9M114_AE_TARGET_GAIN (V4L2_CID_PRIVATE_BASE+3)</td>
        <td>min: 0 max: 65535<br/>default: 128</td>
        <td>The maximum analog gain before reducing the frame rate. (<a href="http://www.onsemi.com/pub_link/Collateral/AND9270-D.PDF">Figure 40</a>)</td>
    </tr>
    <tr>
        <td>min. analog gain</td>
        <td>V4L2_MT9M114_AE_MIN_VIRT_GAIN (V4L2_CID_PRIVATE_BASE+4)</td>
        <td>min: 0 max: 65535<br/>default: 32</td>
        <td>The minimum analog gain that the auto exposure is allowed to use. Increasing this value carefully results in shorter exposure times. (<a href="http://www.onsemi.com/pub_link/Collateral/AND9270-D.PDF">Figure 40</a>)</td>
    </tr>
    <tr>
        <td>max. analog gain</td>
        <td>V4L2_MT9M114_AE_MAX_VIRT_GAIN (V4L2_CID_PRIVATE_BASE+5)</td>
        <td>min: 0 max: 65535<br/>default: 256</td>
        <td>The maximum analog gain that the auto exposure is allowed to use. (<a href="http://www.onsemi.com/pub_link/Collateral/AND9270-D.PDF">Figure 40</a>)</td>
    </tr>
    <tr>
        <td>weighted brightness</td>
        <td>V4L2_MT9M114_AE_WEIGHT_TABLE_0_0 ... V4L2_MT9M114_AE_WEIGHT_TABLE_4_4 (V4L2_CID_PRIVATE_BASE+6...+30)</td>
        <td>min: 0 max: 255</td>
        <td>Requires V4L2_CID_EXPOSURE_ALGORITHM=1!<br>The weight of every 5x5 area to be used for calculating the average brightness of the current scene. (<a href="http://www.onsemi.com/pub_link/Collateral/AND9270-D.PDF">Figure 32</a>)</td>
    </tr>
</table>

Information about the alternative ethernet driver
-------------------------------------------------
Aldebran has the r8169 driver build into their kernels. [This driver does not work correctly, even system crashes are possible!](http://wiki.hetzner.de/index.php/Installation_des_r8168-Treibers/en). We added the r8168 driver which is provided by [Realtek](http://www.realtek.com.tw/downloads/downloadsView.aspx?Langid=1&PNid=13&PFid=5&Level=5&Conn=4&DownTypeID=3&GetDown=false).

Since the r8169 driver is build into the kernel it is a bit more difficult than usual to replace it. You can either replace the whole kernel, or switch to the new driver while booting.
### Replacing the kernel (in theory)
1. Deactivate the r8169 driver in the .config file by replacing the line `CONFIG_R8169=y` by `# CONFIG_R8169 is not set` (deleting this line should do the trick, too).
2. Activate the r8168 driver: In case you had to replace the .config file, you have to add either `CONFIG_R8168=y` (to build the driver into the kernel) or `CONFIG_R8168=m` (to build it as a module).
3. Build the kernel. You can use the `tarbz2-pkg` option to have the kernel and all its modules packed into a tar.bz2 archive.
4. Replace the kernel on the robot.

### Switching the driver during boot
1. Activate the r8168 driver: In case you had to replace the .config file, you have to add `CONFIG_R8168=m` to build it as a module.
2. Build the kernel.
3. Copy the driver to the Robot: The new driver can be found in `drivers/net/r8168-8.039.00/r8168.ko` and has to be copied to `/lib/modules/2.6.33.9-rt31-aldebaran-rt/kernel/drivers/net/r8168.ko`.
4. Write an init.d script that switches out the drivers. We use a script similar to this one:
```
depend() {
  need dbus
}

start() {
  # Replace buggy driver
  networkDriver=$(lspci -k -s 02:00.0 | grep "Kernel driver in use:" | awk 'NF>1{print $NF}')
  if [[ ${networkDriver} == "r8169" ]] ; then
    echo "0000:02:00.0" > /sys/bus/pci/drivers/r8169/unbind
    insmod /lib/modules/2.6.33.9-rt31-aldebaran-rt/kernel/drivers/net/r8168.ko
  fi

  ifconfig eth0 up
}
```

How to build the kernel
-----------------------
The easiest way to build the kernel is to us Aldebrans VirtualBox image provided with each NAO OS release.
After importing it into VirtualBox you can change the number of CPUs and the amount of RAM to your liking, in order to spead up the compilation. Copying data from and to the VM can be done via `scp -P 2222 nao@localhost`. Similarly ssh can be used which preserves your keybord layout: `ssh -p 2222 nao@localhost`.

Once you copied the source files to the VM run `make`. To speed up the build process you can use the `-j <number of cores>` command with the number of CPUs you gave the VM.
