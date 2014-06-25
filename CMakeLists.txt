cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(openni2)

# OpenNI2 dependency list:
# - dl (C-library)
# - pthread (C-library)
# - rt (C-library)
# - jpeg
# - udev
# - libusb-1

# Dependency checks

# C-library features:
# dl
find_path(DL_INCLUDE dlfcn.h)
mark_as_advanced(DL_INCLUDE)
find_library(DL_LIBRARY dl)
mark_as_advanced(DL_LIBRARY)
set(DL_INCLUDE_DIRS "${DL_INCLUDE}" CACHE INTERNAL "" FORCE)
set(DL_LIBRARIES "${DL_LIBRARY}" CACHE INTERNAL "" FORCE)
# pthread
find_path(PTHREAD_INCLUDE pthread.h)
mark_as_advanced(PTHREAD_INCLUDE)
if(NOT ANDROID)
  find_package(Threads QUIET)
  if(CMAKE_THREAD_LIBS_INIT)
    set(PTHREAD_LIBRARY ${CMAKE_THREAD_LIBS_INIT} CACHE INTERNAL "" FORCE)
  else()
    find_library(PTHREAD_LIBRARY pthread pthreadVCE2)
    mark_as_advanced(PTHREAD_LIBRARY)
  endif()
  set(PTHREAD_LIBRARIES "${PTHREAD_LIBRARY}" CACHE INTERNAL "" FORCE)
endif()
set(PTHREAD_INCLUDE_DIRS "${PTHREAD_INCLUDE}" CACHE INTERNAL "" FORCE)
# rt
find_path(RT_INCLUDE time.h)
mark_as_advanced(RT_INCLUDE)
find_library(RT_LIBRARY rt)
mark_as_advanced(RT_LIBRARY)
set(RT_INCLUDE_DIRS "${RT_INCLUDE}" CACHE INTERNAL "" FORCE)
set(RT_LIBRARIES "${RT_LIBRARY}" CACHE INTERNAL "" FORCE)

# jpeg
find_package(JPEG)


find_package(PkgConfig)

# udev
pkg_check_modules(UDEV libudev REQUIRED)

# libusb-1
pkg_check_modules(USB_1 libusb-1.0 REQUIRED)

# Set some variables that will be useful tfor the compiler and the linker
set(XnLib_LIBS
  ${DL_LIBRARIES}
  ${JPEG_LIBRARIES}
  ${PTHREAD_LIBRARIES}
  ${RT_LIBRARIES}
  ${UDEV_LIBRARIES}
  ${USB_1_LIBRARIES}
)
set(XnLib_HDRS
  ${DL_INCLUDE_DIRS}
  ${JPEG_INCLUDE_DIRS}
  ${PTHREAD_INCLUDE_DIRS}
  ${RT_INCLUDE_DIRS}
  ${UDEV_INCLUDE_DIRS}
  ${USB_1_INCLUDE_DIRS}
)
include_directories(${XnLib_HDRS})

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
add_library(OniFile ${openni2_oni_SRC})
install(TARGETS OniFile LIBRARY DESTINATION lib/OpenNI2/Drivers ARCHIVE DESTINATION lib)

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
add_library(PS1080 ${openni2_ps1080_SRC})
target_link_libraries(PS1080 ${XnLib_LIBS})
install(TARGETS PS1080 LIBRARY DESTINATION lib/OpenNI2/Drivers ARCHIVE DESTINATION lib)

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

file(GLOB OpenNI2_HDR
  "./Include/*.h"
)

file(GLOB OpenNI2_driver_HDR
  "./Include/Driver/*.h"
)

add_library(OpenNI2 ${openni2_SRC} ${OpenNI2_HDR} ${OpenNI2_driver_HDR})
target_link_libraries(OpenNI2 ${XnLib_LIBS})
install(TARGETS OpenNI2 LIBRARY DESTINATION lib ARCHIVE DESTINATION lib)

# Install the headers
#
# Install all headers on all linux platform.
# This is required for ARM build because, as ugly as it is, the
# architecture header starts by including the Linux-x86 one, then
# undefines a couple of symbols before redefining them.
# It is so sad people does not use the macro set by the compiler :'|
install(FILES ${OpenNI2_HDR} DESTINATION include/openni2)
install(FILES "./Include/Linux-x86/OniPlatformLinux-x86.h" DESTINATION include/openni2/Linux-x86)
install(FILES "./Include/Linux-Arm/OniPlatformLinux-Arm.h" DESTINATION include/openni2/Linux-Arm)

configure_file(openni2-config.cmake.in "${PROJECT_BINARY_DIR}/InstallFiles/openni2-config.cmake" @ONLY)
install(FILES "${PROJECT_BINARY_DIR}/InstallFiles/openni2-config.cmake" DESTINATION share/openni2)

add_subdirectory("Samples")
