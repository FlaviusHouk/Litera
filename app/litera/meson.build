litera_sources = [
    'main.c'
]

local_includes = [
    include_directories('pkg/core', is_system: false),
    include_directories('pkg/evernote-backend', is_system: false)
]

local_parts = [
   core_lib,
   evernote_lib
]

deps = [
    dependency('gtk4')
]

litera_app = executable('litera', sources: litera_sources, dependencies: deps,  include_directories: local_includes, link_with: local_parts)
