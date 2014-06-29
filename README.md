BKernel
=======
This is essentially [Aldebaran's 2.0 Kernel](https://github.com/aldebaran/linux-aldebaran/tree/release-2.0/atom) with a new camera driver. Other then that nothing else was modified.

Information about the camera driver
-----------------------------------
The camera driver provides some new features compared to the driver from Aldebaran. In addition to that the range of some values changed and the auto white balance is controlled by another ioctl.

A list of the new video controls
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
        <td>min 0: max: 4<br/>default: 1</td>
        <td>Only changeable while ae is enabled!</td>
    </tr>
    <tr>
        <td>brightness</td>
        <td>V4L2_CID_BRIGHTNESS</td>
        <td>min 0: max: 255<br/>default: 55</td>
        <td>Only changeable while ae is enabled!</td>
    </tr>
    <tr>
        <td>contrast</td>
        <td>V4L2_CID_CONTRAST</td>
        <td>min 16: max: 64<br/>default: 32</td>
        <td>Gradient of the contrast adjustment curve from 0.5 (16) to 2.0 (64)</td>
    </tr>
    <tr>
        <td>*NEW* do an automatic white balance</td>
        <td>V4L2_CID_DO_WHITE_BALANCE</td>
        <td>min 0: max: 1<br/>default: 0</td>
        <td>Does an automatic white balance. The new white balance can be read from the white balance temperature.<br/>This ioctl is a button, thus it will always return 0! *Be aware* that Aldebaran uses this ioctl for the white balance temperature.</td>
    </tr>
    <tr>
        <td>exposure time</td>
        <td>V4L2_CID_EXPOSURE</td>
        <td>min 1: max: 1000<br/>default: 1</td>
        <td>The exposure can be changed only while ae is disabled! A value on one is 100 microseconds of exposure.</td>
    </tr>
    <tr>
        <td>exposure algorithm</td>
        <td>V4L2_CID_EXPOSURE_ALGORITHM</td>
        <td>min 0: max: 3<br/>default: 1</td>
        <td>0: Average scene brightness<br/>1: weighted average scene brightness<br/>2: evaluated average scene brightness with frontlight detection<br/>3: evaluated average scene brightness with backlight detection</td>
    </tr>
    <tr>
        <td>gain</td>
        <td>V4L2_CID_GAIN</td>
        <td>min 0: max: 255<br/>default: 32</td>
        <td>Only changeable while ae is disabled! A value of 32 equals to 1.0 gain.</td>
    </tr>
    <tr>
        <td>*NEW* gamma</td>
        <td>V4L2_CID_GAMMA</td>
        <td>min 0: max: 1000<br/>default: 220</td>
        <td>The gamma correction for the display in units multiplied by 100. A value of 220 equals to 2.2 gamma.</td>
    </tr>
    <tr>
        <td>hue</td>
        <td>V4L2_CID_HUE</td>
        <td>min -22: max: 22<br/>default: 0</td>
        <td>Hue correction on degrees.</td>
    </tr>
    <tr>
        <td>*NEW* power line frequency</td>
        <td>V4L2_CID_POWER_LINE_FREQUENCY</td>
        <td>min 1: max: 2<br/>default: 2</td>
        <td>1: 50Hz, 2: 60Hz.<br/>The frequeny of the local power line so ae can avoid flicker.</td>
    </tr>
    <tr>
        <td>saturation</td>
        <td>V4L2_CID_SATURATION</td>
        <td>min 0: max: 255<br/>default: 128</td>
        <td>Saturation control for the image. A value of zero results in grayscaled images while values above 128 result in over saturated images.</td>
    </tr>
    <tr>
        <td>sharpness</td>
        <td>V4L2_CID_SHARPNESS</td>
        <td>min 0: max: 255<br/>default: 128</td>
        <td>Sharpness values are limited from 0 to +7. When seting the value tp -7 no sharpness will be applied.</td>
    </tr>
    <tr>
        <td>white balance temperature</td>
        <td>V4L2_CID_WHITE_BALANCE_TEMPERATURE</td>
        <td>min 2700: max: 6500<br/>default: 6500</td>
        <td>The white balance temperature in kelvin. *Be aware* that Aldebaran uses another ioctl for this feature!</td>
    </tr>
    <tr>
        <td>vertical flip</td>
        <td>V4L2_CID_VFLIP</td>
        <td>min 0: max: 1<br/>default: 0</td>
        <td></td>
    </tr>
    <tr>
        <td>horizontal flip</td>
        <td>V4L2_CID_HFLIP</td>
        <td>min 0: max: 1<br/>default: 0</td>
        <td></td>
    </tr>
    <tr>
        <td>*NEW* fade to black</td>
        <td>V4L2_CID_PRIVATE_BASE</td>
        <td>min 0: max: 1<br/>default: 1</td>
        <td>0: disable, 1: enable<br/>When enabled the image will fade to black under low light conditions.</td>
    </tr>
</table>

How to build the kernel
------------
To build this kernel gcc 4.5.3 is needed. If you are lucky your Linux distribution has a package for that gcc version. Otherwise you have to build it yourself. For more information on that topic see [down below](README.md#some-infos-on-how-to-build-gcc-453).
The .config file was loaded from the NAOs and is included in this repository. So no further configuration is needed.

Before you build the kernel make sure gcc 4.5.3 is used by default. This can be done by adding the path to gcc's bin directory to the front of your $PATH environment variable. Then to build the kernel just type in:

    make ARCH=i386

To speed up the build process you can use the `-j <number of cores>` command with the number of cores your PC has.

Since nothing was modified on the kernel expect for the camera driver you can just copy the module to the robot and restart it.

The new driver can be found in `drivers/media/video/mt9m114.ko` and has to be copied to `/lib/modules/2.6.33.9-rt31-aldebaran-rt/kernel/drivers/media/video/mt9m114.ko`.

Some infos on how to build gcc 4.5.3
------------------------------------
A complete guide on how to build gcc can be found in the [Code Release 2013](https://github.com/bhuman/BHuman2013/blob/master/CodeRelease2013.pdf]). Although the guide was originally written for gcc 4.5.2 it is pretty much the same for gcc 4.5.3, thus you will find a short version here:
* Download gcc 4.5.3 from http://gcc.gnu.org/gcc-4.5/ and unpack it to a directory of your chioce.
* Apply the patches from https://github.com/bhuman/BHuman2013/tree/master/Util/gcc-patch .
* Download the the libraries mpfr and mpc and unpack them into the source directory (make sure, that the mpfr and mpc directories are named "mpfr" and "mpc").
* Create a build directory
* Run `<path to gcc source>/configure --prefix=/home/user/gcc-4.5.2 --enable-languages=c,c++`
* Run `make -j 4`
* Run `make install`
