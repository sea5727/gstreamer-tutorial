project('gst-my-plugin', 'c', version : '1.16.1.0', license : 'LGPL')

plugins_install_dir = join_paths(get_option('libdir'), 'gstreamer-1.0')

cc = meson.get_compiler('c')

gst_version = meson.project_version()

api_version = '1.0'

gst_dep = dependency('gstreamer-1.0',
    fallback : ['gstreamer', 'gst_dep'])

gst_base_dep = dependency('gstreamer-base-1.0',
  fallback : ['gstreamer', 'gst_base_dep'])

plugin_c_args = ['-DHAVE_CONFIG_H']

cdata = configuration_data()
cdata.set_quoted('PACKAGE_VERSION', gst_version)
cdata.set_quoted('PACKAGE', 'gst-my-plugin')
cdata.set_quoted('GST_LICENSE', 'LGPL')
cdata.set_quoted('GST_API_VERSION', api_version)
cdata.set_quoted('GST_PACKAGE_NAME', 'GStreamer template Plug-ins')
cdata.set_quoted('GST_PACKAGE_ORIGIN', 'https://gstreamer.freedesktop.org')
configure_file(output : 'config.h', configuration : cdata)

# gstaudio_dep = dependency('gstreamer-audio-1.0',
#     fallback: ['gst-plugins-base', 'audio_dep'])



plugin_sources = [
  'my_plugin.c'
]
gstpluginexample = library('gstmysourceplugin',
  plugin_sources,
  c_args: plugin_c_args,
  dependencies : [gst_dep, gst_base_dep],
  install : true,
  install_dir : plugins_install_dir,
)

