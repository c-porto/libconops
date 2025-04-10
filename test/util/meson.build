# Tests executable
tests = [
  'cmd_queue_test',
]

foreach test_name : tests
  test_exe = executable(
    test_name,
    test_name + '.cc',
    dependencies: [gtest_main_dep, conops_dep],
  )
  test(test_name, test_exe)
endforeach
