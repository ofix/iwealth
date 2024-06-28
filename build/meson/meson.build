project('iwealth', 'cpp',
         version : '0.1',
         default_options:[
            'default_library=shared',
            'warning_level=2',
            'debug=true',
            'c_std=c14',
            'cpp_std=c++14'
        ])

if host_machine.cpu_family().startswith('x86')
    third_party_lib = join_paths(meson.source_root(),
      meson.current_source_dir(), '../../lib/curl/x86_64_windows')
else
    third_party_lib = join_paths(meson.source_root(),
      meson.current_source_dir(), '../../lib/curl/aarch64_linux')
endif

message(third_party_lib)

if host_machine.cpu_family().startswith('x86')
    inc = include_directories([
        '../../include',
        '../../include/wxWidgets/3.2.4/include',
        '../../lib',
        '../../lib/wxWidgets/3.2.4/mingw_x64_windows/mswu',
    ])
else
    inc = include_directories([
        '../../include',
        '../../include/wxWidgets/3.2.4/include',
        '../../lib',
        '../../lib/wxWidgets/3.2.4/aarch64_linux/wx/include/gtk3-unicode-3.2',
    ])
endif

cc = meson.get_compiler('cpp')

third_party_dep = declare_dependency(
      dependencies : [
        # cc.find_library('crypto', dirs: third_party_lib), 
        # cc.find_library('ssl', dirs: third_party_lib),
        cc.find_library('curl', dirs: third_party_lib)
      ],
      include_directories: [
        include_directories('../../include/curl')
      ],
    )

compile_time = run_command('date', '+%F %H:%M:%S').stdout().strip()

if host_machine.cpu_family().startswith('x86')
cpp_args = [
    '-I../../../lib/wxWidgets/3.2.4/mingw_x64_windows/mswu/',
    '-I../../../include/wxWidgets/3.2.4/include',
    '-DWXUSINGDLL',
]
else
cpp_args = [
    #'-Werror'
    '-I../../../lib/wxWidgets/3.2.4/aarch64_linux/wx/include/gtk3-unicode-3.2',
    '-I../../../include/wxWidgets/3.2.4/include',
    '-DWXUSINGDLL',
    '-D__WXGTK3__',
    '-D__WXGTK__',
    '-D_FILE_OFFSET_BITS=64',
]
endif

if host_machine.cpu_family().startswith('x86')
  ldflags = [
      #'-z,relro,now,',
      '-LE:/work/iwealth/lib/wxWidgets/3.2.4/mingw_x64_windows',
      '-pthread',
      '-Wl,-rpath,E:/work/iwealth/lib/wxWidgets/3.2.4/mingw_x64_windows',
      '-lwxmsw32u_xrc',
      '-lwxmsw32u_html',
      '-lwxmsw32u_core',
      '-lwxbase32u_xml',
      '-lwxbase32u_net',
      '-lwxbase32u'
      ]
else
    ldflags = [
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
endif

gui_sources = [
    'ui/RichApplication.cpp',
    'ui/RichMainFrame.cpp',
    'ui/RichHelper.cpp',
    'ui/components/PanelStockQuote.cpp',
    'ui/components/DialogShareSearch.cpp',
    'ui/components/RichKlineCtrl.cpp',
    'ui/components/RichKlineInfoCtrl.cpp',
    'ui/components/RichVolumeBarCtrl.cpp',
    'ui/components/RichGrid.cpp',
    'ui/components/RichGridCellStringRenderer.cpp',
    'stock/StockDataStorage.cpp',
    'spider/SpiderShareBriefInfo.cpp',
    'spider/SpiderConceptListEastMoney.cpp',
    'spider/SpiderShareQuote.cpp',
    'spider/SpiderShareKline.cpp',
    'spider/SpiderShareCategory.cpp',
    'spider/Spider.cpp',
    'stock/Stock.cpp',
    'stock/ShareCategory.cpp',
    'net/Request.cpp',
    'net/ConcurrentRequest.cpp',
    'search/Trie.cpp',
    'search/ChinesePinYin.cpp',
    'util/Timer.cpp',
    'util/DateTime.cpp',
    'util/ConsoleTable.cpp',
    'util/EasyLogger.cpp',
    'util/FileTool.cpp',
    'util/Global.cpp'
]

full_gui_sources = []
foreach file : gui_sources
    full_path = join_paths('../../src', file)
    full_gui_sources += files(full_path)
endforeach

ui_cpp_args = cpp_args
ui_cpp_args += ['-DIWEALTH','-DDEBUG']
executable('iwealth', full_gui_sources,
            cpp_args:ui_cpp_args,
            link_args : ldflags,
            include_directories : inc,
            dependencies : [third_party_dep])

console_sources = [
    'console/Test.cpp',
    'stock/Stock.cpp',
    'stock/StockDataStorage.cpp',
    'spider/SpiderShareBriefInfo.cpp',
    'spider/SpiderConceptListEastMoney.cpp',
    'spider/SpiderShareQuote.cpp',
    'spider/SpiderShareKline.cpp',
    'spider/SpiderShareCategory.cpp',
    'spider/Spider.cpp',
    'stock/Stock.cpp',
    'stock/ShareCategory.cpp',
    'net/Request.cpp',
    'net/ConcurrentRequest.cpp',
    'search/Trie.cpp',
    'search/ChinesePinYin.cpp',
    'util/Timer.cpp',
    'util/DateTime.cpp',
    'util/ConsoleTable.cpp',
    'util/EasyLogger.cpp',
    'util/FileTool.cpp',
    'util/Global.cpp'
]
full_console_sources = []
foreach file : console_sources
    full_path = join_paths('../../src', file)
    full_console_sources += files(full_path)
endforeach

executable('iconsole',full_console_sources,
            cpp_args:cpp_args,
            link_args : ldflags,
            include_directories : inc,
            dependencies : [third_party_dep])



smith_sources = [
    'console/Smith.cpp',
    'util/FileTool.cpp',
    'util/Global.cpp'
]
full_smith_sources = []
foreach file : smith_sources
    full_path = join_paths('../../src', file)
    full_smith_sources += files(full_path)
endforeach

executable('smith',full_smith_sources,
            cpp_args:cpp_args,
            link_args : ldflags,
            include_directories : inc,
            dependencies : [third_party_dep])