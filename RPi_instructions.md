#Setting up the PI #
*Follow these steps to setup the necessary packages and make the necessary changes to Raspbian in order to run the code in this repo. These steps were reproduced on a Raspberry PI 3 using the official LCD screen.*

Grab the latest Raspbian Jessie Lite image from : https://www.raspberrypi.org/downloads/raspbian/ . Unzip it and write it to an SD Card and extend the root partition of the SD card to fill out the empty space. (either with gparted, resize2fs or when Raspbian booted with raspi-config)

Setup networking and SSH to work out-of-the box
-----------------------------------------------
Mount the SD card/USB drive on a PC which has an OS that supports ext4 filesystems. The commands here are for Debian, Ubuntu ...etc

Setup the WiFi by editing the the /etc/wpa_supplicant/wpa_supplicant.conf file in the raspbian image:

	sudo nano /MOUNTING_POINT/etc/wpa_supplicant/wpa_supplicant.conf

and add the following to it:

	network={
	     ssid="Your Wifi's ESSID"
	     psk="Your wifi's password"
	}

If you are using the official LCD screen with the case then your screen will be upside down, so add the following to the /boot/config.txt

	#Rotate screen
	display_rotate=2

Enable SSH access to the PI by creating a file in the /MOUNTING_POINT/boot dir named ssh

	echo "" > /MOUNTING_POINT/boot/ssh

Setup boot from USB
-----------------------------------------------

>*These steps are optional, but booting from USB makes it easier to access the Pi's filesystem if something goes wrong*

>To enable USB boot mode follow the steps from here: https://github.com/raspberrypi/documentation/blob/master/hardware/raspberrypi/bootmodes/msd.md

>If you don't want to copy Raspbian from the SD card to the USB drive (it is slow :/ ) then follow the steps from above (write the image to the USB drive then setup networking and SSH to work out-of-the box). Get rpi-update and do an offline update of the Raspbian kernel on your Linux machine. Install rpi-update on a Linux PC with the following:
>
	sudo curl -L --output /usr/bin/rpi-update https://raw.githubusercontent.com/Hexxeh/rpi-update/master/rpi-update && sudo chmod +x /usr/bin/rpi-update

>Then run it with the following arguments:
>
	sudo ROOT_PATH=/MOUNTING_POINT/root BOOT_PATH=/MOUNTING_POING/boot BRANCH=next rpi-update
>*Please note that this only need to be done once for each Pi*

Update Raspbian to Stretch
--------------------------
*To have Qt5.7 you either need to cross-compile it or update Raspbian from Jessie to Stretch and install it from the Raspbian package repository. Here I'll explain how to get Raspbian Stretch running on your Pi. If you'd rather cross-compile Qt5.7 then follow the steps from the [Qt Wiki](https://wiki.qt.io/RaspberryPi2EGLFS), but please beware that it takes a hours (even on a decent PC with 100% CPU utilisation) to compile qtbase plus all the needed modules.*

When done unmount your USB drive / SD card insert it into the PI then boot it up. Login into the PI using SSH (either by using the ssh command or using PuTTy). The default username is pi the password is raspberry

To update to stretch add the following line top the /etc/apt/sources.list file:

	deb http://mirrordirector.raspbian.org/raspbian/ stretch main contrib non-free rpi

Run the update

	sudo apt-get update && sudo apt-get dist-upgrade

Then run rpi-update with the following arguments:

	sudo BRANCH=next rpi-update

Remove libnettle4:

	sudo apt-get remove libnettle4

Autoremove all the uneeded packages:

	sudo apt autoremove

Get the driver for the onboard WLAN adapter from the RPi-Distro repo

	sudo wget -P /lib/firmware/brcm/ https://raw.githubusercontent.com/RPi-Distro/firmware-nonfree/master/brcm80211/brcm/brcmfmac43430-sdio.bin
	sudo wget -P /lib/firmware/brcm/ https://raw.githubusercontent.com/RPi-Distro/firmware-nonfree/master/brcm80211/brcm/brcmfmac43430-sdio.txt

> If you use a screen that's connected to the Pi's HDMI port then follow the steps below. Any screen connected to the DSI port will not work as of now. For details see:https://github.com/anholt/linux/issues/8
> 
> First change QT's default platform plugin to EGLFS by running the following command:
>
	echo "export QT_QPA_PLATFORM=eglfs" >> ~/.profile
> 
> If you want to use the LCD then set QT's default platform to linuxfb:
>
	echo "export QT_QPA_PLATFORM=linuxfb" >> ~/.profile
>
> To enable the OpenGL driver run `sudo raspi-config` then go to Advanced -> Options -> GL Driver then confirm that you want to enable the GL driver, press okay and click on finish in the raspi-config menu. When prompted restart the Pi.

Reboot the PI

	sudo reboot

Install the prerequisites for building and running headunit
---------------------------------------------------------------

Install the neccesarry build tools:

	sudo apt-get install build-essential qt5-default qml-module-qtquick2 cmake

Install all the requirements:

	sudo apt-get install libusb-1.0-0-dev libssl-dev=1.0.1t-1+deb8u5 openssl=1.0.1t-1+deb8u5 libglib2.0-dev libgstreamer1.0-dev gstreamer1.0-plugins-base-apps gstreamer1.0-plugins-bad gstreamer1.0-libav gstreamer1.0-alsa libboost-dev libgstreamer-plugins-base1.0-dev qtdeclarative5-dev qtmultimedia5-dev

Install Git:

	sudo apt-get install git

Change device permission for all devices that are made by known Android vendors and are connected through USB, so libusb can access them. Download and copy the `51-android.rules` file from https://github.com/snowdream/51-android to /etc/udev/rules.dev/:

	sudo wget -P /etc/udev/rules.dev/ https://raw.githubusercontent.com/snowdream/51-android/master/51-android.rules

Build and install QtGstreamer
-----------------------------
Clone QtGstreamer

	git clone git://anongit.freedesktop.org/gstreamer/qt-gstreamer
	cd qt-gstreamer

Create build dir:

	mkdir build && cd build

cmake QtGstreamer

	cmake .. -DCMAKE_INSTALL_PREFIX=/usr/lib/arm-linux-gnueabihf -DQT_VERSION=5 -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-std=c++11

Make and install QtGstreamer

	make && sudo make install

Check if QtGstreamer is installed properly.

	gst-inspect-1.0 | grep qt5videosink

If the output is something similar, the it is installed properly:

	qt5videosink:  qt5videosink: Qt video sink
	qt5videosink:  qt5glvideosink: Qt GL video sink
	qt5videosink:  qwidget5videosink: QWidget video sink
	qt5videosink:  qtquick2videosink: QtQuick2 video sink

After successfully installing QtGstreamer you need to add it to Raspbian's system library path, edit the `/etc/ld.so.conf.d/arm-linux-gnueabihf.conf` file as root and add the following line to the bottom of it. When done editing just run `sudo ldconfig`

	/usr/lib/arm-linux-gnueabihf/lib

Build headunit
--------------
Clone this repo

	git clone -b aa-qt5-test https://github.com/viktorgino/headunit.git && cd headunit

And compile it

	qmake && make
If the program compiles without error then run it with `./Headunit-Desktop`

