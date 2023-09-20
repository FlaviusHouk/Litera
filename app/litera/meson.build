gnome = import('gnome')

gnome_res = gnome.compile_resources('resources', 'resources.xml')

litera_sources = [
    'main.c',
    'litera_app.c',
    'litera_app_main_window.c',
    'litera_login_page.c',
    gnome_res
]

local_includes = [
    include_directories('../../pkg/core', is_system: false),
    include_directories('../../pkg/evernote-backend', is_system: false)
]

local_deps = [
   core_lib,
   evernote_backend
]

deps = [
    dependency('gtk4')
]

litera_app = executable('litera', sources: litera_sources, dependencies: deps,  include_directories: local_includes, link_with: local_deps)
