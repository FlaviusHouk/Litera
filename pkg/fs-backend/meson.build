fs_backend_sources = [
  'fs-backend.c'
]

local_deps = [
  core_lib
]

local_includes = [
  include_directories('../core', is_system: false),
]

fs_backend_deps = [
  dependency('libxml-2.0')
]

fs_backend = library('fs-backend', sources: fs_backend_sources, link_with: local_deps, include_directories: local_includes, dependencies: fs_backend_deps)
