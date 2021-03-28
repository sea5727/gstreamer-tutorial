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
    pkg_check_modules (GST gstreamer-1.0)
    pkg_check_modules (GLIB glib-2.0)
    pkg_check_modules (GSTAUDIO gstreamer-audio-1.0)
    pkg_check_modules (GSTRTP gstreamer-rtp-1.0)
    
    message(GST_INCLUDE_DIRS: ${GST_INCLUDE_DIRS} )
    message(GST_LIBRARY_DIRS: ${GST_LIBRARY_DIRS} )
    message(GST_LIBRARIES: ${GST_LIBRARIES} )   
    message(NICE_INCLUDE_DIRS: ${NICE_INCLUDE_DIRS} )
    message(NICE_LIBRARIES: ${NICE_LIBRARIES} )
    
    INCLUDE_DIRECTORIES(${NICE_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES(${GST_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS} ${GSTAUDIO_INCLUDE_DIRS} ${GSTRTP_INCLUDE_DIRS})

    #nice
    # LINK_LIBRARIES(/usr/local/lib64/libnice.so)
    # LINK_LIBRARIES(/home/ysh8361/workspace/gstreamer/gstreamer-tutorial/cpp/nice-example/libnice.so.10.11.0)
    LINK_LIBRARIES(gio-2.0 gobject-2.0 glib-2.0)
    LINK_LIBRARIES(${NICE_LIBRARIES})
    LINK_LIBRARIES(${GST_LIBRARIES} ${GLIB_LIBRARIES} ${GSTAUDIO_LIBRARIES} ${GSTRTP_LIBRARIES} )
endif()


# add_library(gstplugin SHARED gstplugin.c)
add_library(gstrtpchecker SHARED gstrtpchecker.c)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
