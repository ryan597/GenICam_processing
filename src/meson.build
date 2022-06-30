project('Genicam-processing', 'cpp', version: '0.2.0', 
  default_options : ['cpp_std=c++20', 'optimization=3'])


aravis_dep = dependency('aravis-0.8')
thread_dep = dependency('threads')
x11_dep = dependency('x11')
openmp_dep = dependency('openmp')
tiff_dep = dependency('libtiff-4')
opencv_dep = dependency('opencv4')

cimg_dir = include_directories('/home/ryan/Downloads/CImg')


programs = [
  'main',
  'convert'
]

foreach e: programs
  exe = executable (e, e + '.cpp', include_directories: cimg_dir, dependencies: [thread_dep, x11_dep, openmp_dep, tiff_dep, aravis_dep, opencv_dep])
endforeach
