project('iwealth', 'cpp',
         version : '0.1',
         default_options:[
            'default_library=shared',
            'warning_level=2',
            'debug=true',
            'c_std=c17', 
            'cpp_std=c++17'
        ])

ldflags = ['-Wl,-z,relro,-z,now']

if host_machine.cpu_family().startswith('x86')
    third_party_lib = join_paths(meson.source_root(),
      meson.current_source_dir(), 'lib/curl/x86_64')
else
    third_party_lib = join_paths(meson.source_root(),
      meson.current_source_dir(), 'lib/curl/arm64')
endif

message(third_party_lib)

inc = include_directories([
    'include',
    'include/curl',
    'include/search',
    'include/util',
    'include/net'
])

cc = meson.get_compiler('cpp')

third_party_dep = declare_dependency(
      dependencies : [
        cc.find_library('crypto', dirs: third_party_lib), 
        cc.find_library('ssl', dirs: third_party_lib),
        cc.find_library('curl', dirs: third_party_lib)
      ],
      include_directories: [
        include_directories('include/curl')
      ],
    )

compile_time = run_command('date', '+%F %H:%M:%S').stdout().strip()

cpp_args = [
    #'-Werror'
]

sources = [
    'src/application.cpp',
    'src/net/request.cpp',
    'src/search/trie.cpp',
    'src/util/console_table.cpp',
    'src/util/global.cpp']

executable('iwealth', sources,
            cpp_args:cpp_args,
            include_directories : inc,
            dependencies : [third_party_dep])