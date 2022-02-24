project('Genicam-processing', 'c', version: '0.0.2')


aravis_dep = dependency('aravis-0.8')

programs = [
  'aravis',
]

foreach e: programs
  exe = executable (e, e + '.c', dependencies: aravis_dep)
endforeach
