# 设置系统名称
set(CMAKE_SYSTEM_NAME Windows)

# 指定编译器路径
set(CMAKE_C_COMPILER D:/common/msys64/usr/bin/gcc.exe)
set(CMAKE_CXX_COMPILER D:/common/msys64/usr/bin/g++.exe)
set(CMAKE_MAKE_PROGRAM D:/common/msys64/usr/bin/make.exe)

# 指定工具链根目录
set(CMAKE_FIND_ROOT_PATH D:/common/msys64/mingw64/bin)

# 调整查找库和头文件的策略
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)