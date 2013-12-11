cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(openni2)

find_package(qibuild)

# Do not build XnLib as static but build it every time (a bit slower)
file(GLOB XnLib_SRC
  "./ThirdParty/PSCommon/XnLib/Source/Linux/*.cpp"
)
# add other sources but remove the symmetric matrix ones
set(XnLib_SRC ${XnLib_SRC}
  ./ThirdParty/PSCommon/XnLib/Source/XnDump.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnDumpFileWriter.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnErrorLogger.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnFiles.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnFPSCalculator.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnLib.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnLogAndroidWriter.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnLogConsoleWriter.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnLog.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnLogFileWriter.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnOS.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnOSMemoryProfiling.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnProfiling.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnScheduler.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnStatus.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnStrings.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnThreads.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnUSB.cpp
  ./ThirdParty/PSCommon/XnLib/Source/XnVector3D.cpp
)
set(XnLib_LIBS DL JPEG PTHREAD RT UDEV USB_1)
set(XnLib_INCLUDE_DIRS ./ThirdParty/PSCommon/XnLib/Include/)

# set the DepthUtils sources
file(GLOB DepthUtils_SRC
  "./Source/DepthUtils/*.cpp"
)
set(DepthUtils_LIBS rt)
set(DepthUtils_INCLUDE_DIRS ./Source/DepthUtils/ ./Source/DepthUtils/Include ${XnLib_INCLUDE_DIRS})

# Build OpenNI Oni Drivers
include_directories("./Source/Drivers/OniFile/")
include_directories("./Source/Drivers/OniFile/Include/")
file(GLOB openni2_oni_SRC
  "./Source/Drivers/OniFile/*.cpp"
  "./Source/Drivers/OniFile/Formats/*.cpp"
)
set(openni2_oni_SRC ${openni2_oni_SRC} ${XnLib_SRC})
qi_create_lib(OniFile ${openni2_oni_SRC} SUBFOLDER OpenNI2/Drivers)
qi_stage_lib(OniFile)

# Build OpenNI PS1080 Drivers
include_directories("./Source/Drivers/PS1080/")
include_directories("./Source/Drivers/PS1080/Include/")
include_directories("./Include/")
include_directories(${DepthUtils_INCLUDE_DIRS})
file(GLOB openni2_ps1080_SRC
  "./Source/DepthUtils/*.cpp"
  "./Source/Drivers/PS1080/Core/*.cpp"
  "./Source/Drivers/PS1080/DDK/*.cpp"
  "./Source/Drivers/PS1080/DriverImpl/*.cpp"
  "./Source/Drivers/PS1080/Formats/*.cpp"
  "./Source/Drivers/PS1080/Sensor/*.cpp"
)
set(openni2_ps1080_SRC ${openni2_ps1080_SRC} ${XnLib_SRC})
qi_create_lib(PS1080 ${openni2_ps1080_SRC} SUBFOLDER OpenNI2/Drivers)
qi_use_lib(PS1080 ${XnLib_LIBS})
qi_stage_lib(PS1080)

# Build OpenNI
include_directories("./Include")
include_directories("./Source/Drivers/OniFile/Formats/")
include_directories("./Source/Drivers/OniFile/")
include_directories(${XnLib_INCLUDE_DIRS})
file(GLOB openni2_SRC
  "./Source/Core/*.cpp"
)
set(openni2_SRC ${openni2_SRC} ${XnLib_SRC}
  "./Source/Drivers/OniFile/Formats/XnCodec.cpp"
  "./Source/Drivers/OniFile/Formats/XnStreamCompression.cpp"
)
qi_create_lib(OpenNI2 ${openni2_SRC})
qi_use_lib(OpenNI2 ${XnLib_LIBS})
qi_stage_lib(OpenNI2)

# Install the headers
#
# Install all headers on all linux platform.
# This is required for ARM build because, as ugly as it is, the
# architecture header starts by including the Linux-x86 one, then
# undefines a couple of symbols before redefining them.
# It is so sad people does not use the macro set by the compiler :'|
file(GLOB OpenNI2_HDR
  "./Include/*.h"
)
qi_install_header(${OpenNI2_HDR} SUBFOLDER openni2)
qi_install_header("./Include/Linux-x86/OniPlatformLinux-x86.h" SUBFOLDER openni2/Linux-x86)
qi_install_header("./Include/Linux-Arm/OniPlatformLinux-Arm.h" SUBFOLDER openni2/Linux-Arm)
