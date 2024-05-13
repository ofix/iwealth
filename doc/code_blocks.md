### 程序编译后，运行编译好的exe文件，单独运行提示缺少DLL文件怎么办？
>程序编译完成后，自动拷贝相关依赖DLL文件到编译目录

**Project > Build options > iWealth > Pre/post build steps > Post-build steps**

```cmd
cmd /c copy "$(PROJECT_DIR)..\..\lib\curl\x86_64_windows\libcurl-x64.dll"  "$(PROJECT_DIR)$(TARGET_OUTPUT_DIR)"
cmd /c copy "$(PROJECT_DIR)..\..\lib\wxWidgets\3.2.4/mingw_x64_windows\*.dll"  "$(PROJECT_DIR)$(TARGET_OUTPUT_DIR)"
```
