# CMAKE 最低版本号
cmake_minimum_required(VERSION 3.15)

# 项目名称
project(iwealth)

# 指定标准库版本
set(CMAKE_CXX_STANDARD 11)
# 自定义第三方库目录
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
# 源代码根目录
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
# 依赖的动态库
set(DYNAMIC_LIBRARIES wxsvg wxmsw32u_xrc wxmsw32u_html wxmsw32u_core wxbase32u_xml wxbase32u_net wxbase32u pthread)
# 可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $(CMAKE_BINARY_DIR)/dist)

# 工具函数
# @Param platform 平台(Windows/Linux)
# @Param arch 架构(X86_64 X86 ARM64)
function(get_link_dirs platform arch)


endfunction()

# 全局头文件目录
include_directories(
        ${SRC_DIR}
        ${LIB_DIR}/curl/include
        ${LIB_DIR}/wxsvg/include
        ${LIB_DIR}/wxWidgets/3.2.4/mingw_x64_windows/mswu/
        ${LIB_DIR}/wxWidgets/3.2.4/include)

# 源代码，方式一
# aux_source_directory(${SRC_DIR}/console SRC_CONSOLE)
# aux_source_directory(${SRC_DIR}/formula SRC_FORMULA)
# aux_source_directory(${SRC_DIR}/net SRC_NET)
# aux_source_directory(${SRC_DIR}/search SRC_SEARCH)
# aux_source_directory(${SRC_DIR}/spider SRC_SPIDER)
# aux_source_directory(${SRC_DIR}/stock SRC_STOCK)
# aux_source_directory(${SRC_DIR}/ui SRC_UI)
# aux_source_directory(${SRC_DIR}/util SRC_UTIL)
# set(SRC_LIST ${SRC_CONSOLE} ${SRC_FORMULA} ${SRC_NET} ${SRC_SEARCH} ${SRC_SPIDER} ${SRC_STOCK} ${SRC_UI} ${SRC_UTIL})

# message(status "Sources 1= ${SRC_LIST}")

# 源代码，方式二
file(GLOB_RECURSE SRC_LIST2 ${SRC_DIR}/*.cpp)

message(status "Sources 2 = ${SRC_LIST2}")

# 链接库查找目录
link_directories(${LIB_DIR}/curl/x86_64_windows
                 ${LIB_DIR}/wxWidgets/3.2.4/mingw_x64_windows)



# 链接静态库
# link_libraries(wxsvg wxmsw32u_xrc wxmsw32u_html)
# 编译生成库
#add_library(iwealth_lib ${SRC_FILES})

# 生成可执行文件
add_executable(iwealth,${SRC_LIST})

# 预处理器定义
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWXUSINGDLL")
target_compile_definitions(iwealth WXUSINGDLL)
# 链接动态库
target_link_libraries(iwealth,${DYNAMIC_LIBRARIES})


# 设置 RPATH
set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/libs")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# 定义要拷贝的库文件
set(THIRD_PARTY_LIBS
    ${CMAKE_SOURCE_DIR}/libs/third_party_lib1.so
    ${CMAKE_SOURCE_DIR}/libs/third_party_lib2.so
)

# 创建目标目录
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/libs)

# 添加自定义命令来拷贝库文件
foreach(LIB ${THIRD_PARTY_LIBS})
    add_custom_command(
        TARGET my_executable POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${LIB} ${CMAKE_BINARY_DIR}/libs
        COMMENT "Copying ${LIB} to ${CMAKE_BINARY_DIR}/libs"
    )
endforeach()


# 递归查找所有库文件
file(GLOB_RECURSE THIRD_PARTY_LIBS "${CMAKE_SOURCE_DIR}/libs/*.so")

# 添加库搜索路径
foreach(LIB_PATH ${THIRD_PARTY_LIBS})
    get_filename_component(LIB_DIR ${LIB_PATH} DIRECTORY)
    link_directories(${LIB_DIR})
endforeach()
