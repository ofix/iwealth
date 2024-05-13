### wxWidgets 3.2.4 麒麟 V10 系统环境下编译

```
1. unzip wxWidgets-3.2.4.zip
2. cd wxWidgets-3.2.4
3. mkdir linux_aarch64_build
4. cd linux_aarch64_build
5. cmake ..
6. make -j 8
至此，`wxWidgets-3.2.4/linux_aarch64_build/lib` 目录包含所有的动态库文件，
     `wxWidgets-3.2.4/include` 目录包含所有程序依赖头文件
```

### 查看 wxWidgets 3.2.4 动态库 cpp 编译选项

```
wxWidgets-3.2.4/linux_aarch64_build/wx-config --cppflags

-I/home/greatwall/work/wxWidgets-3.2.4/linux_aarm64_build/lib/wx/include/gtk3-unicode-3.2 -I/home/greatwall/work/wxWidgets-3.2.4/include -DWXUSINGDLL -D__WXGTK3__ -D__WXGTK__ -D_FILE_OFFSET_BITS=64
```

### 查看 wxWidgets 3.2.4 动态库 cpp 链接选项

```
wxWidgets-3.2.4/linux_aarch64_build/wx-config --libs

-L/home/greatwall/work/wxWidgets-3.2.4/linux_aarch64_build/lib -pthread -Wl,-rpath,/home/greatwall/work/wxWidgets-3.2.4/linux_aarch64_build/lib -lwx_gtk3u_xrc-3.2 -lwx_gtk3u_html-3.2 -lwx_gtk3u_qa-3.2 -lwx_gtk3u_core-3.2 -lwx_baseu_xml-3.2 -lwx_baseu_net-3.2 -lwx_baseu-3.2
```

### MinGw 编译 wxWidgets 最新版本方法

1. 去 [https://www.mingw-w64.org/downloads/](https://www.mingw-w64.org/downloads/) 下载最新版本 mingw，这里我们选择 `w64devkit`
2. 打开控制台程序，进入 `wxWidgets-3.2.4/build/msw` 目录
3. 执行命令 `mingw32-make -f makefile.gcc SHELL=CMD.exe -j2 BUILD=release SHARED=1 MONOLITHIC=1 UNICODE=1`
4. 注意以上命令必须包含 **SHELL=CMD.exe**, 否则会出现如下错误

```
process_begin: CreateProcess(NULL, -c "if not exist gcc_mswudll mkdir gcc_mswudll", ...) failed.
make (e=2): The system cannot find the file specified.
mingw32-make: [gcc_mswudll] Error 2 (ignored)
process_begin: CreateProcess(NULL, -c "if not exist ..\..\lib\gcc_dll\mswu mkdir ..\..\lib\gcc_dll\mswu", ...) failed.
make (e=2): The system cannot find the file specified.
mingw32-make: *** [..\..\lib\gcc_dll\mswu] Error 2
```

### mingw-x64 版本选择

> 选择 `x86_64-posix-sjlj`，因为 Win32 线程模型不支持 C++ 11 的 `std::thread` 抽象，而 posix 版本支持； seh 异常模型采用 windows 操作系统内置的异常捕获机制，效率比 sjlj 要高，但存在跨平台不利影响。
