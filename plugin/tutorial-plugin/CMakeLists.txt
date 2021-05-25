cmake_minimum_required(VERSION 3.0.0)
project(gst-my-plugin VERSION 1.16.1.0)

include(CTest)
enable_testing()


include(FindPkgConfig)
if(PKG_CONFIG_FOUND)
    message(PKG_CONFIG_FOUND: ${PKG_CONFIG_FOUND} )
    message(PKG_CONFIG_EXECUTABLE: ${PKG_CONFIG_EXECUTABLE} )
    message(PKG_CONFIG_VERSION_STRING: ${PKG_CONFIG_VERSION_STRING} )
    message(PKG_CONFIG: ${PKG_CONFIG} ) 
    pkg_check_modules (GST gstreamer-1.0)
    pkg_check_modules (GLIB glib-2.0)
    
    message(GST_INCLUDE_DIRS: ${GST_INCLUDE_DIRS} )
    message(GST_LIBRARY_DIRS: ${GST_LIBRARY_DIRS} )
    message(GST_LIBRARIES: ${GST_LIBRARIES} )   
    
    INCLUDE_DIRECTORIES(${NICE_INCLUDE_DIRS})
    INCLUDE_DIRECTORIES(${GST_INCLUDE_DIRS} ${GLIB_INCLUDE_DIRS})

    LINK_LIBRARIES(gio-2.0 gobject-2.0 glib-2.0)
    LINK_LIBRARIES(${GST_LIBRARIES} ${GLIB_LIBRARIES})
endif()


# add_library(gstplugin SHARED gstplugin.c)
add_library(gstmyplugin SHARED my_plugin.c)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

include(CPack)
