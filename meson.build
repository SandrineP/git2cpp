project(
  'git2cpp',
  'cpp',
  default_options: [
    'cpp_std=c++20',
    #'werror=true',
  ],
  license: 'BSD-3-Clause',
)

cli11_dep = dependency('CLI11')
libgit2_dep = dependency('libgit2')

subdir('src')

executable(
  'git2cpp',
  src_files,
  dependencies: [
    cli11_dep,
    libgit2_dep,
  ],
)
