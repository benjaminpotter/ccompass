project('ccompass', 'c')

# add_project_arguments('-pg', language: 'c')
# add_project_link_arguments('-pg', language: 'c')

headers = include_directories('include')

# TODO only compile this if pthread exists
# this breaks compilation if pthread isn't found
cli = executable(
  'ccompass',
  ['cli/main.c', 'cli/tpool.c'],
  include_directories: headers,
  link_args: '-lm'
)

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

test_hough_transform = executable(
  'test_hough_transform',
  'tests/test_hough_transform.c', 
  include_directories: headers,
  link_args: '-lm'
)

test_draw_line = executable(
  'test_draw_line',
  'tests/test_draw_line.c', 
  include_directories: headers,
  link_args: '-lm'
)

test_compute_binary_threshold = executable(
  'test_compute_binary_threshold',
  'tests/test_compute_binary_threshold.c', 
  include_directories: headers,
  link_args: '-lm'
)

test('test_compute_stokes', test_compute_stokes, args: ['../tests/images/image1.png'])
test('test_compute_aolp', test_compute_aolp, args: ['../tests/images/image1.png'])
test('test_compute_dolp', test_compute_dolp, args: ['../tests/images/image1.png'])
test('test_compute_cmap', test_compute_cmap, args: [])
test('test_transform_stokes', test_transform_stokes, args: ['../tests/images/image1.png'])
test('test_hough_transform', test_hough_transform, args: ['../tests/images/image1.png'])
test('test_draw_line', test_draw_line, args: [])
test('test_compute_binary_threshold', test_compute_binary_threshold, args: ['../tests/images/image1.png'])

