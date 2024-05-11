project('iwealth', 'cpp',
         version : '0.1',
         default_options:[
            'default_library=shared',
            'warning_level=2',
            'debug=true',
            'c_std=c17', 
            'cpp_std=c++17'
        ])

if host_machine.cpu_family().startswith('x86')
    third_party_lib = join_paths(meson.source_root(),
      meson.current_source_dir(), 'lib/curl/x86_64_windows')
else
    third_party_lib = join_paths(meson.source_root(),
      meson.current_source_dir(), 'lib/curl/aarch64_linux')
endif

message(third_party_lib)

inc = include_directories([
    'include',
    'include/curl',
    'include/search',
    'include/util',
    'include/net',
    'include/spider',
    'lib/nlohmann'
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
    '-I../lib/wxWidgets/3.2.4/aarch64_linux/wx/include/gtk3-unicode-3.2',
    '-I../include/wxWidgets/3.2.4/include',
    '-DWXUSINGDLL',
    '-D__WXGTK3__',
    '-D__WXGTK__',
    '-D_FILE_OFFSET_BITS=64'
]

ldflags = [
    #'-z,relro,now,',
    '-L/home/greatwall/work/iwealth/lib/wxWidgets/3.2.4/aarch64_linux',
    '-pthread',
    '-Wl,-rpath,/home/greatwall/work/iwealth/lib/wxWidgets/3.2.4/aarch64_linux',
    '-lwx_gtk3u_xrc-3.2',
    '-lwx_gtk3u_html-3.2',
    '-lwx_gtk3u_qa-3.2',
    '-lwx_gtk3u_core-3.2',
    '-lwx_baseu_xml-3.2',
    '-lwx_baseu_net-3.2',
    '-lwx_baseu-3.2']

sources = [
    # 'src/application.cpp',
    # 'src/spider/stock_list_spider_hexun.cpp',
    # 'src/spider/stock_history_spider.cpp',
    # 'src/spider/spider.cpp',
    # 'src/net/request.cpp',
    # 'src/net/concurrent_request.cpp',
    # 'src/search/trie.cpp',
    # 'src/util/console_table.cpp',
    # 'src/util/global.cpp'
    'src/ui/wxapplication.cpp'
    ]

executable('iwealth', sources,
            cpp_args:cpp_args,
            link_args : ldflags,
            include_directories : inc,
            dependencies : [third_party_dep])