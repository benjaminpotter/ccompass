project('ccompass', 'c')
add_project_arguments('-pg', language: 'c')

headers = include_directories('include')

test_compute_stokes = executable(
  'test_compute_stokes', 
  'tests/test_compute_stokes.c', 
  include_directories: headers,
  link_args: '-lm'
)

test_compute_aolp = executable(
  'test_compute_aolp', 
  'tests/test_compute_aolp.c', 
  include_directories: headers,
  link_args: '-lm'
)

test_compute_dolp = executable(
  'test_compute_dolp', 
  'tests/test_compute_dolp.c', 
  include_directories: headers,
  link_args: '-lm'
)

test_compute_cmap = executable(
  'test_compute_cmap',
  'tests/test_compute_cmap.c', 
  include_directories: headers,
  link_args: '-lm'
)

test_transform_stokes = executable(
  'test_transform_stokes',
  'tests/test_transform_stokes.c', 
  include_directories: headers,
  link_args: '-lm'
)

test('test_compute_stokes', test_compute_stokes, args: ['../tests/images/image.jpg'])
test('test_transform_stokes', test_transform_stokes, args: ['../tests/images/image.jpg'])
test('test_compute_aolp', test_compute_aolp, args: ['../tests/images/image.jpg'])
test('test_compute_dolp', test_compute_dolp, args: ['../tests/images/image.jpg'])
test('test_compute_cmap', test_compute_cmap, args: [])

