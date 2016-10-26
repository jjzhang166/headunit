HeadUnit-Desktop
================

----------

I use this repository to develop and test the C code from Mike Reid's Headunit for Android Auto. My plan is to create a highly portable car head unit using QT5. By highly portable I mean that it will eventually be able to run on most Linux distros and Windows (sorry Mac users, but I don't plan to even get near a Mac anytime soon). I currently develop on Ubuntu and Debian, I have already tested it on Windows 10 and you could get it running by changing some bits and linking against all the libraries manually.

----------
How to get the code working?
----------------------------

Install QT5 and you will probably want to install QT Creator with it as well. You can get it with apt-get:

    sudo apt-get install qtcreator

This should install all the files required to get started with developing QT, but it is very likely, that you will get an outdated version of it so get it from the official site https://www.qt.io/download-open-source
After you've installed Qt add its libraries to the library load path. On Ubuntu/Debian x64 just add the following line to /etc/ld.so.conf/x86_64-linux-gnu.conf :

    /home/YOUR_USERNAME/Qt/5.7/gcc_64/lib

Install the following packages:

    libusb-1.0-0-dev libssl-dev libgstreamer1.0-dev gstreamer1.0-plugins-base-apps gstreamer1.0-plugins-bad gstreamer1.0-libav libboost-dev libgstreamer-plugins-base1.0-dev 

**Build and install QtGStreamer**

Get the latest code for QtGStreamer from https://cgit.freedesktop.org/gstreamer/qt-gstreamer configure it with CMake and install it on your system. Most of the build steps are detailed in its README file. You might want to install cmake-qt-gui to make it easier:

    sudo apt-get install cmake-qt-gui

With the cmake gui installed just open the folder where you've cloned the qt-gstreamer git, specify where you want to build it (I just used the same dir as it makes generating the Doxygen document easier) and click generate (if it throws up an error message just correct it). You will need tell cmake to use QT5:

    -DQT_VERSION=5
    -CMAKE_CXX_FLAGS=-std=c++11
   After succesfully generating the make file with cmake just open the terminal and cd into the folder you specified as build directory in and run: `make && sudo make install`

After completing the above steps you should be able to build the code with QT Creator an run it. If you have any problem, then reach out to me on XDA Developers (http://forum.xda-developers.com/member.php?u=6642908).

The code here is formatted with QT's clang-format, using the LLVM format, so please respect that :)
