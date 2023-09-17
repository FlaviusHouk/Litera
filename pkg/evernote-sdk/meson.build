evernote_sdk_sources = [  
  'errors_types.c',
  'limits_types.c',
  'note_store.c',
  'note_store_types.c',
  'types_types.c',
  'user_store.c',
  'user_store_types.c'
]

sdk_deps = [
  dependency('thrift_c_glib')
]

evernote_sdk_lib = library('evernote-sdk', sources: evernote_sdk_sources, dependencies: sdk_deps)
