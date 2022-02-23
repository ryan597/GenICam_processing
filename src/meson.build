project('aravis-c', 'c', version: '0.0.1')


aravis_dep = dependency('aravis-0.8')

examples = [
  'aravis',
]

foreach e: examples
  exe = executable (e, e + '.c', dependencies: aravis_dep)
endforeach
