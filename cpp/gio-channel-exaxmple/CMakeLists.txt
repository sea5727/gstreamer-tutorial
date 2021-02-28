cmake_minimum_required(VERSION 3.0.0)
project(rtp-streaming-server-2 VERSION 0.1.0)

include(CTest)
enable_testing()


include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    message(PKG_CONFIG_FOUND: ${PKG_CONFIG_FOUND} )
    message(PKG_CONFIG_EXECUTABLE: ${PKG_CONFIG_EXECUTABLE} )
    message(PKG_CONFIG_VERSION_STRING: ${PKG_CONFIG_VERSION_STRING} )
    message(PKG_CONFIG: ${PKG_CONFIG} ) 
    pkg_check_modules (NICE nice)
    pkg_check_modules (GTHREAD gthread-2.0)
    pkg_check_modules (GST gstreamer-1.0)
    pkg_check_modules (GLIB glib-2.0)
    pkg_check_modules (GSTAUDIO gstreamer-audio-1.0)
    pkg_check_modules (GSTRTP gstreamer-rtp-1.0)
    
    message(GST_INCLUDE_DIRS: ${GST_INCLUDE_DIRS} )
    message(GST_LIBRARY_DIRS: ${GST_LIBRARY_DIRS} )
    message(GST_LIBRARIES: ${GST_LIBRARIES} )   
    INCLUDE_DIRECTORIES(${NICE_INCLUDE_DIRS} ${GTHREAD_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES(${GST_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${GSTAUDIO_INCLUDE_DIRS} ${GSTRTP_INCLUDE_DIRS})
    LINK_LIBRARIES(${NICE_LIBRARIES} ${GTHREAD_LIBRARIES})
    LINK_LIBRARIES(${GST_LIBRARIES} ${GLIB_LIBRARIES} ${GSTAUDIO_LIBRARIES} ${GSTRTP_LIBRARIES} )
endif()


add_executable(example-1 example-1.cpp)
add_executable(example-2 example-2.cpp)
add_executable(example-3 example-3.cpp)
add_executable(example-4 example-4.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
