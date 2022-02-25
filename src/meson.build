project('Genicam-processing', 'cpp', version: '0.0.3')


aravis_dep = dependency('aravis-0.8')
thread_dep = dependency('threads')
openmp_dep = dependency('openmp')
x11_dep = dependency('x11')
tiff_dep = dependency('libtiff-4')

cimg_dir = include_directories('/home/ryan/Downloads/CImg')


programs = [
  'aravis',
]

foreach e: programs
  exe = executable (e, e + '.cpp', include_directories: cimg_dir, dependencies: [thread_dep, x11_dep, openmp_dep, tiff_dep, aravis_dep])
endforeach
