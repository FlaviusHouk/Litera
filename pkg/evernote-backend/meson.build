evernote_backend_sources = [
  'evernote-backend.c',
  'http-transport.c'
]

local_deps = [
  evernote_sdk_lib
]

local_includes = [
  include_directories('../core', is_system: false),
  include_directories('../evernote-sdk', is_system: false)
]

sdk_deps = [
  dependency('thrift_c_glib'),
  dependency('libsoup-2.4')
]

evernote_backend = library('evernote-backend', sources: evernote_backend_sources, link_with: local_deps, include_directories: local_includes, dependencies: sdk_deps)
