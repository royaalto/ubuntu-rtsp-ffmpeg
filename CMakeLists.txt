cmake_minimum_required(VERSION 2.8)

project(test)
add_definitions(-std=c++11)
#add_subdirectory(./third_party/JRTPLIB jrtplib)
#add_subdirectory(./myRtspClient)
find_package(OpenCV REQUIRED)
set(FFMPEG_INCLUDE_DIRS
	"/usr/local/lib"
	"/usr/local/include"
	"/usr/include"
	"/usr/include/include"
)

set(FFMPEG_LIBS
	/usr/local/lib/libavformat.a
	/usr/local/lib/libavcodec.a
    	/usr/local/lib/libavutil.a
    	/usr/local/lib/libswscale.a
    	/usr/local/lib/libswresample.a
	/usr/local/lib/libavfilter.a
)
set(TEST_INCLUDE_DIRS
	"./third_party/Base64_live555/include"
	"./third_party/md5/include"
	"./third_party/JRTPLIB/src"
	"./myRtspClient/include"
	"${CMAKE_CURRENT_BINARY_DIR}/myRtspClient/jrtplib/src"
	"/usr/include/SDL/"
    )
set(SDL_INCLUDE
	"/usr/include/SDL"
)

#_________add include___________#

include_directories(
	${TEST_INCLUDE_DIRS}
	${FFMPEG_INCLUDE_DIRS}
	${SDL_INCLUDE_DIRS}
	${SDL_IMAGE_INCLUDE_DIRS}
	${SDL_INCLUDE}
	${OpenCV_INCLUDE_DIR}
	${OPENCV_INCLUDE_DIRS}
)


#_________add lib___________#
set(rtsplib
	${CMAKE_CURRENT_BINARY_DIR}/myRtspClient/jrtplib/src/libjrtp.a
	${CMAKE_CURRENT_BINARY_DIR}/myRtspClient/libmyRtspClient.so

)


add_executable(test ${CMAKE_CURRENT_BINARY_DIR}/SDL2_visual.cpp)

#link to the lib
target_link_libraries(test
        ${rtsplib}
        ${OPENCV_LIBS}
        ${FFMPEG_LIBS}
	${SDL_LIBRARIES}
	${SDL_IMAGE_LIBRARY}
        opencv_core opencv_imgcodecs opencv_highgui opencv_imgproc opencv_calib3d opencv_videoio
        -lm
        -lz
        -llzma
        -lbz2
	-lSDL2
        -lpthread

)
