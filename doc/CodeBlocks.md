### 程序编译后，运行编译好的 exe 文件，单独运行提示缺少 DLL 文件怎么办？

> 程序编译完成后，自动拷贝相关依赖 DLL 文件到编译目录

**Project > Build options > iWealth > Pre/post build steps > Post-build steps**

```cmd
cmd /c copy "$(PROJECT_DIR)..\..\lib\curl\x86_64_windows\libcurl-x64.dll"  "$(PROJECT_DIR)$(TARGET_OUTPUT_DIR)"
cmd /c copy "$(PROJECT_DIR)..\..\lib\wxWidgets\3.2.4/mingw_x64_windows\*.dll"  "$(PROJECT_DIR)$(TARGET_OUTPUT_DIR)"
```

```shell
cp  $(PROJECT_DIR)../../lib/curl/aarch64_linux/libcurl.so  $(PROJECT_DIR)$(TARGET_OUTPUT_DIR)
cp  $(PROJECT_DIR)../../lib/curl/aarch64_linux/libcurl.so.*  $(PROJECT_DIR)$(TARGET_OUTPUT_DIR)
cp  $(PROJECT_DIR)../../lib/wxWidgets/3.2.4/aarch64_linux/*.so  $(PROJECT_DIR)$(TARGET_OUTPUT_DIR)
cp  $(PROJECT_DIR)../../lib/wxWidgets/3.2.4/aarch64_linux/*.so.*  $(PROJECT_DIR)$(TARGET_OUTPUT_DIR)
```

### wxSmith 默认路径在 CodeBlocks 工程 .cbp 文件 同级目录下，如果需要把 wxSmith 目录挪到其他位置，只需要修改 cbp 工程文件对应的 wxSmith 目录路径，wxSmith 即可在 CodeBlocks 资源中正常打开进行可视化编辑

### Ubuntu 或者麒麟操作系统 CodeBlocks 默认没有安装 wxSmith 插件怎么办？

```shell
 sudo apt-get install codeblocks-contrib
```

### CodeBlocks 编译的时候会把所有添加进工程的文件都进行编译，比如这个文件本身还没开发完，是个半成品，或者这个文件是控制台程序入口文件(不是 GUI 入口文件)。

为此，你需要将他们从工程目录中移除，可以避免很多奇怪的错误，比如编译成功后，双击 exe 文件发现啥也没有显示。
此外也不要把第三方依赖库 lib 添加到当前项目中，而是要在编译器选项(compiler/linker)中添加，否则可能出现重复定义的错误
如果第三方库头文件目录中包含 cpp 源文件，也不要把这个目录添加到项目中

### Linux 编译 libcurl，并支持 https

```shell
./configure --with-ssl=/home/greatwall/cpp_libs/openssl_share_3.0.13
--prefix=/home/greatwall/cpp_libs/libcurl_share_8.7.1
```

或者

```shell
mkdir build
cd build
cmake ..
make
# 如果需要编译libcurl支持http/2协议
cmake .. -DUSE_NGHTTP2=ON -DUSE_OPENSSL_QUIC=ON
```

### 查看 CMake 源码编译所有支持的编译选项

```shell
cd build
cmake .. -LH # .. 是CMakeLists.txt文件相对路径
```
