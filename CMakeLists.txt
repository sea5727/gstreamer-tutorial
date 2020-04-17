cmake_minimum_required(VERSION 3.0.0)
project(rtp-streaming-server-2 VERSION 0.1.0)

include(CTest)
enable_testing()


include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    message(PKG_CONFIG_FOUND: ${PKG_CONFIG_FOUND} )
    message(PKG_CONFIG_EXECUTABLE: ${PKG_CONFIG_EXECUTABLE} )
    message(PKG_CONFIG_VERSION_STRING: ${PKG_CONFIG_VERSION_STRING} )
    message(PKG_CONFIG: ${PKG_CONFIG} ) 
    pkg_check_modules (GST gstreamer-1.0)
    pkg_check_modules (GLIB glib-2.0)
    message(GST_INCLUDE_DIRS: ${GST_INCLUDE_DIRS} )
    message(GST_LIBRARY_DIRS: ${GST_LIBRARY_DIRS} )
    message(GST_LIBRARIES: ${GST_LIBRARIES} )   
    INCLUDE_DIRECTORIES(${GST_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})
    LINK_LIBRARIES(${GST_LIBRARIES} ${GLIB_LIBRARIES})
endif()



add_executable(rtp-streaming-server-7 main7.cpp)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
