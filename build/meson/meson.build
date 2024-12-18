project('iwealth', 'cpp',
         version : '0.3',
         default_options:[
            'default_library=shared',
            'warning_level=2',
            'debug=true',
            'c_std=c14',
            'cpp_std=c++14'
        ])

x86_64_platform = host_machine.cpu_family().startswith('x86_64')
windows_os = host_machine.system().startswith('windows')

if x86_64_platform
    if windows_os
        third_party_lib = join_paths(meson.source_root(),
            meson.current_source_dir(), '../../lib/curl/x86_64_windows')
    else
        third_party_lib = join_paths(meson.source_root(),
            meson.current_source_dir(), '../../lib/curl/x86_64_linux')
    endif
else
    third_party_lib = join_paths(meson.source_root(),
      meson.current_source_dir(), '../../lib/curl/aarch64_linux')
endif

message(third_party_lib)

if x86_64_platform
    if windows_os
        inc = include_directories([
            '../../src',
            '../../lib',
            '../../lib/wxWidgets/3.2.4/include',
            '../../lib/wxWidgets/3.2.4/mingw_x64_windows/mswu',
        ])
    else 
        inc = include_directories([
            '../../src',
            '../../lib',
            '../../lib/wxWidgets/3.2.4/include',
            '../../lib/wxWidgets/3.2.4/x86_64_linux/wx/include/gtk3-unicode-3.2',
        ])
    endif
else
    inc = include_directories([
        '../../src',
        '../../lib',
        '../../lib/wxWidgets/3.2.4/include',
        '../../lib/wxWidgets/3.2.4/aarch64_linux/wx/include/gtk3-unicode-3.2',
    ])
endif

cc = meson.get_compiler('cpp')

if x86_64_platform and not(windows_os)
third_party_dep = declare_dependency(
      dependencies : [
        cc.find_library('crypto', dirs: third_party_lib), 
        cc.find_library('ssl', dirs: third_party_lib),
        cc.find_library('curl', dirs: third_party_lib)
      ],
      include_directories: [
        include_directories('../../lib/curl/include')
      ],
    )
else
third_party_dep = declare_dependency(
      dependencies : [
        # cc.find_library('crypto', dirs: third_party_lib), 
        # cc.find_library('ssl', dirs: third_party_lib),
        cc.find_library('curl', dirs: third_party_lib)
      ],
      include_directories: [
        include_directories('../../lib/curl/include')
      ],
    )
endif



compile_time = run_command('date', '+%F %H:%M:%S').stdout().strip()

if x86_64_platform
    if windows_os
        cpp_args = [
            '-I../../../lib/wxWidgets/3.2.4/mingw_x64_windows/mswu/',
            '-I../../../lib/wxWidgets/3.2.4/include',
            '-DWXUSINGDLL',
        ]
    else
        cpp_args = [
            #'-Werror'
            '-I../../../lib/wxWidgets/3.2.4/x86_64_linux/wx/include/gtk3-unicode-3.2',
            '-I../../../lib/wxWidgets/3.2.4/include',
            '-DWXUSINGDLL',
            '-D__WXGTK3__',
            '-D__WXGTK__',
            '-D_FILE_OFFSET_BITS=64',
        ]
    endif
else
    cpp_args = [
        #'-Werror'
        '-I../../../lib/wxWidgets/3.2.4/aarch64_linux/wx/include/gtk3-unicode-3.2',
        '-I../../../lib/wxWidgets/3.2.4/include',
        '-DWXUSINGDLL',
        '-D__WXGTK3__',
        '-D__WXGTK__',
        '-D_FILE_OFFSET_BITS=64',
    ]
endif

if x86_64_platform
    if windows_os
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
            '-L/home/ofix/work/iwealth/lib/wxWidgets/3.2.4/x86_64_linux',
            '-pthread',
            '-Wl,-rpath,/home/ofix/work/iwealth/lib/wxWidgets/3.2.4/x86_64_linux',
            '-lwx_gtk3u_xrc-3.2',
            '-lwx_gtk3u_html-3.2',
            '-lwx_gtk3u_qa-3.2',
            '-lwx_gtk3u_core-3.2',
            '-lwx_baseu_xml-3.2',
            '-lwx_baseu_net-3.2',
            '-lwx_baseu-3.2']
    endif
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
    'ui/RichPanelStockQuote.cpp',
    'ui/RichPanelKline.cpp',
    'ui/components/RichPanel.cpp',
    'ui/RichDialogShareSearch.cpp',
    'ui/components/RichKlineCtrl.cpp',
    'ui/components/RichPngKline.cpp',
    'ui/RichDialogKlineInfo.cpp',
    'ui/components/RichFrame.cpp',
    'ui/components/RichDialog.cpp',
    'ui/indicators/RichVolumeIndicatorCtrl.cpp',
    'ui/indicators/RichAmountIndicatorCtrl.cpp',
    'ui/indicators/RichTurnoverRateIndicatorCtrl.cpp',
    'ui/indicators/RichIndicatorCtrl.cpp',
    'ui/components/RichGrid.cpp',
    'ui/components/RichGridCellStringRenderer.cpp',
    'ui/components/RichGridColumnHeaderProvider.cpp',
    'ui/components/RichGridColumnHeaderRenderer.cpp',
    'ui/components/ShareListGridCellStringRenderer.cpp',
    'ui/components/ShareListGridTable.cpp',
    'ui/components/RichGridTable.cpp',
    'ui/components/RichRadioCtrl.cpp',
    'ui/components/RichTopBar.cpp',
    'ui/components/RichTopMenu.cpp',
    'ui/components/RichTopMenuItem.cpp',
    'ui/events/RichRadioEvent.cpp',
    'ui/events/RichShareSearchEvent.cpp',
    'ui/RichHelper.cpp',
    'formula/FormulaEma.cpp',
    'formula/FormulaBase.cpp',
    'stock/StockDataStorage.cpp',
    'stock/StockShareKline.cpp',
    'stock/StockCompositeIndex.cpp',
    'spider/SpiderShareBriefInfo.cpp',
    'spider/SpiderConceptListEastMoney.cpp',
    'spider/SpiderShareQuote.cpp',
    'spider/SpiderCompositeIndex.cpp',
    'spider/SpiderShareKline.cpp',
    'spider/SpiderShareKlineProviderBaidu.cpp',
    'spider/SpiderShareKlineProviderEastMoney.cpp',
    'spider/SpiderShareKlineProvider.cpp',
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
    'util/Global.cpp',
    'util/RichResult.cpp',
    'util/RichResult.cpp'
]

full_gui_sources = []
foreach file : gui_sources
    full_path = join_paths('../../src', file)
    full_gui_sources += files(full_path)
endforeach

ui_cpp_args = cpp_args
ui_cpp_args += ['-DIWEALTH',
                #'-DDEBUG'
               ]

executable('iwealth', full_gui_sources,
            cpp_args:ui_cpp_args,
            link_args : ldflags,
            include_directories : inc,
            dependencies : [third_party_dep])


dir_assets = join_paths(meson.source_root(),'build/assets')
# 拷贝图片目录到build目录
install_subdir('../../src/assets',
                install_dir:dir_assets,
                install_mode:'rw-r--r--',
                strip_directory:true)
# 拷贝文件
if x86_64_platform
    CP=find_program('cp')
else
    CP=find_program('cp')
endif
cp_pinyin_dic = custom_target('cp--pinyin-dic',
                        input : [join_paths(meson.source_root(),'../../src/search/ChinesePinYin.dic')],
                        output : ['ChinesePinYin.dic'],
                        command : [CP, '-rf','@INPUT@', '@OUTPUT@'],
                        install: false,
                        build_by_default: true)

console_sources = [
    'console/Test.cpp',
    'stock/Stock.cpp',
    'stock/StockDataStorage.cpp',
    'stock/StockShareKline.cpp',
    'spider/SpiderShareBriefInfo.cpp',
    'spider/SpiderConceptListEastMoney.cpp',
    'spider/SpiderShareQuote.cpp',
    'spider/SpiderShareKline.cpp',
    'spider/SpiderShareKlineProviderBaidu.cpp',
    'spider/SpiderShareKlineProviderEastMoney.cpp',
    'spider/SpiderShareKlineProvider.cpp',
    'spider/SpiderShareCategory.cpp',
    'spider/Spider.cpp',
    'stock/StockMonitorShare.cpp',
    'stock/UserFavoriteShare.cpp',
    'stock/Stock.cpp',
    'stock/ShareCategory.cpp',
    'net/Request.cpp',
    'net/ConcurrentRequest.cpp',
    'search/Trie.cpp',
    'search/ChinesePinYin.cpp',
    'ui/RichHelper.cpp',
    'util/Timer.cpp',
    'util/DateTime.cpp',
    'util/ConsoleTable.cpp',
    'util/EasyLogger.cpp',
    'util/FileTool.cpp',
    'util/Global.cpp',
    'util/RichResult.cpp'
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