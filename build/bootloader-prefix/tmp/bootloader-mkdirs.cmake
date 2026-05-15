# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/marionlh/esp/esp-idf-v5.3/components/bootloader/subproject"
  "/home/marionlh/Desktop/ClubRobot_infoPAMI/test_temp/PAMIs/build/bootloader"
  "/home/marionlh/Desktop/ClubRobot_infoPAMI/test_temp/PAMIs/build/bootloader-prefix"
  "/home/marionlh/Desktop/ClubRobot_infoPAMI/test_temp/PAMIs/build/bootloader-prefix/tmp"
  "/home/marionlh/Desktop/ClubRobot_infoPAMI/test_temp/PAMIs/build/bootloader-prefix/src/bootloader-stamp"
  "/home/marionlh/Desktop/ClubRobot_infoPAMI/test_temp/PAMIs/build/bootloader-prefix/src"
  "/home/marionlh/Desktop/ClubRobot_infoPAMI/test_temp/PAMIs/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/marionlh/Desktop/ClubRobot_infoPAMI/test_temp/PAMIs/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/marionlh/Desktop/ClubRobot_infoPAMI/test_temp/PAMIs/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
