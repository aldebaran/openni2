cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(openni2)

set(QI_T001CHAIN_COMPAT   ON   CACHE INTERNAL "" FORCE)
set(QI_NO_WARN_DEPRECATED TRUE CACHE INTERNAL "" FORCE)
find_package(qibuild)

# Build XnLib
include_directories("./ThirdParty/PSCommon/XnLib/Include")
file(GLOB XnLib_SRC
  "./ThirdParty/PSCommon/XnLib/Source/*.cpp"
  "./ThirdParty/PSCommon/XnLib/Source/Linux/*.cpp"
)
qi_create_lib(XnLib ${XnLib_SRC})
qi_stage_lib(XnLib)

# Build OpenNI
include_directories("./Include")
file(GLOB openni2_SRC
  "./Source/Core/*.cpp"
)
qi_create_lib(OpenNI2 ${openni2_SRC})
target_link_libraries(OpenNI2 XnLib jpeg udev usb-1.0)
qi_stage_lib(OpenNI2)

# Build OpenNI Drivers
include_directories("./Source/Drivers/OniFile/Formats")
include_directories("./Source/Drivers/OniFile/")
file(GLOB openni2_drivers_SRC
  "./Source/Drivers/OniFile/*.cpp"
  "./Source/Drivers/OniFile/Formats/*.cpp"
)
qi_create_lib(OniFile ${openni2_drivers_SRC} SUBFOLDER OpenNi2/Drivers)
target_link_libraries(OniFile OpenNI2)
qi_install_target(OniFile SUBFOLDER OpenNi2/Drivers)

# Install the headers
file(GLOB OpenNI2_HDR
  "./Include/*.hpp"
)
qi_install_header(${OpenNI2_HDR} SUBFOLDER openni2)
