# cmake needs this line
cmake_minimum_required(VERSION 3.0)

# Define project name
project(jacktripui)

find_package( Qt5 REQUIRED Core Widgets Gui Network )
add_definitions(${QT_DEFINITIONS})

include_directories(../src)
if(APPLE)
  set(GUI_TYPE MACOSX_BUNDLE)
  add_definitions(-D__MAC_OSX__ -D__MACOSX_CORE__)
  include_directories("/usr/local/include")
  find_library(COREAUDIO_LIBRARY CoreAudio)
  find_library(COREFOUNDATION_LIBRARY CoreFoundation)
  set(extra_libs ${COREAUDIO_LIBRARY} ${COREFOUNDATION_LIBRARY})
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
  add_definitions(-D__LINUX__ -D__LINUX_ALSA__)
  find_library(LIBASOUND asound)
  set(extra_libs ${LIBASOUND} -lpthread)
endif()

add_definitions(-D__RT_AUDIO__)

find_library(LIBJACK jack)

set(jacktrip_SRC
  ../src/DataProtocol.cpp
  ../src/JackTrip.cpp
  ../src/jacktrip_globals.cpp
  ../src/JackTripThread.cpp
  ../src/JackTripWorker.cpp
  ../src/LoopBack.cpp
  ../src/PacketHeader.cpp
  ../src/ProcessPlugin.cpp
  ../src/RingBuffer.cpp
  ../src/Settings.cpp
  ../src/UdpDataProtocol.cpp
  ../src/UdpMasterListener.cpp
  ../src/AudioInterface.cpp
  ../src/RtAudioInterface.cpp
  ../src/JackAudioInterface.cpp
  ../externals/includes/rtaudio-4.0.7/RtAudio.cpp
)

set(jacktrip_ui_SRC
  jacktripui_main.cpp
  JTMainWindow.cpp
)

set(jacktrip_HEADERS_MOC
  ../src/DataProtocol.h
  ../src/JackTrip.h
  ../src/JackTripWorker.h
  ../src/JackTripWorkerMessages.h
  ../src/NetKS.h
  ../src/UdpDataProtocol.h
  ../src/Settings.h
  ../src/PacketHeader.h
  ../src/ThreadPoolTest.h
  ../src/UdpMasterListener.h
)

QT5_WRAP_CPP(jacktrip_MOC ${jacktrip_HEADERS_MOC})
add_executable(jacktripui ${GUI_TYPE} ${jacktrip_SRC} ${jacktrip_ui_SRC} ${jacktrip_MOC})
target_link_libraries(jacktripui Qt5::Core Qt5::Widgets Qt5::Gui Qt5::Network ${LIBJACK} ${extra_libs})
set_target_properties(jacktripui PROPERTIES
                        MACOSX_BUNDLE ON
                        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/MacInfo.plist.in)

