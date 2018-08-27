# Install script for directory: /home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/jrtplib3" TYPE FILE FILES
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcpapppacket.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcpbyepacket.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcpcompoundpacket.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcpcompoundpacketbuilder.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcppacket.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcppacketbuilder.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcprrpacket.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcpscheduler.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcpsdesinfo.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcpsdespacket.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcpsrpacket.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtcpunknownpacket.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpaddress.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpcollisionlist.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/myRtspClient/jrtplib/src/rtpconfig.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpdebug.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpdefines.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtperrors.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtphashtable.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpinternalsourcedata.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpipv4address.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpipv4destination.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpipv6address.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpipv6destination.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpkeyhashtable.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtplibraryversion.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpmemorymanager.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpmemoryobject.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtppacket.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtppacketbuilder.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtppollthread.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtprandom.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtprandomrand48.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtprandomrands.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtprandomurandom.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtprawpacket.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpsession.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpsessionparams.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpsessionsources.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpsourcedata.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpsources.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpstructs.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtptimeutilities.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtptransmitter.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtptypes_win.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/myRtspClient/jrtplib/src/rtptypes.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpudpv4transmitter.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpudpv6transmitter.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpbyteaddress.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpexternaltransmitter.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpsecuresession.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpsocketutil.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpabortdescriptors.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtpselect.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtptcpaddress.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/rtptcptransmitter.h"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/third_party/JRTPLIB/src/extratransmitters/rtpfaketransmitter.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libjrtp.a")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE STATIC_LIBRARY FILES "/home/roy/delete/ubuntu-rtsp-ffmpeg/myRtspClient/jrtplib/src/libjrtp.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libjrtp.so.3.11.1"
      "$ENV{DESTDIR}/usr/local/lib/libjrtp.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      file(RPATH_CHECK
           FILE "${file}"
           RPATH "")
    endif()
  endforeach()
  list(APPEND CMAKE_ABSOLUTE_DESTINATION_FILES
   "/usr/local/lib/libjrtp.so.3.11.1;/usr/local/lib/libjrtp.so")
  if(CMAKE_WARN_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(WARNING "ABSOLUTE path INSTALL DESTINATION : ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
  if(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION)
    message(FATAL_ERROR "ABSOLUTE path INSTALL DESTINATION forbidden (by caller): ${CMAKE_ABSOLUTE_DESTINATION_FILES}")
  endif()
file(INSTALL DESTINATION "/usr/local/lib" TYPE SHARED_LIBRARY FILES
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/myRtspClient/jrtplib/src/libjrtp.so.3.11.1"
    "/home/roy/delete/ubuntu-rtsp-ffmpeg/myRtspClient/jrtplib/src/libjrtp.so"
    )
  foreach(file
      "$ENV{DESTDIR}/usr/local/lib/libjrtp.so.3.11.1"
      "$ENV{DESTDIR}/usr/local/lib/libjrtp.so"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/usr/bin/strip" "${file}")
      endif()
    endif()
  endforeach()
endif()

