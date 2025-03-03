project(
  'libconops',
  'c',
  version: '0.1.0',
  default_options: [
    'warning_level=3',
    'c_std=gnu11',
    'optimization=s',
    'buildtype=release',
  ],
)

cc = meson.get_compiler('c')

conops_srcs = files(
  'src/conops.c',
)

conops_inc = include_directories('include', 'src')

c_args = [
  '-Wno-unused-parameter',
  '-Wno-unused-value',
  '-Wno-missing-field-initializers',
  '-Wno-narrowing',
  '-Wno-pointer-arith',
  '-Wshadow',
  '-Wcast-align',
  '-Wwrite-strings',
  '-Wall',
  '-Wpedantic',
]

static_library(
  'conops',
  sources: conops_srcs,
  include_directories: conops_inc,
  c_args: c_args,
  install: false,
)

shared_library(
  'conops',
  sources: conops_srcs,
  include_directories: conops_inc,
  c_args: c_args,
  version: meson.project_version(),
  install: false,
)
