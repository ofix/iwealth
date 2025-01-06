# CMAKE 最低版本号
cmake_minimum_required(VERSION 3.15)

# 项目名称
project(iwealth)

# 开启 CMAKE 调试模式
set(CMAKE_VERBOSE_MAKEFILE ON)
# 指定标准库版本
set(CMAKE_CXX_STANDARD 11)
# 自定义第三方库目录
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
# 源代码根目录
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
# 依赖的动态库
set(SHARE_LIBRARIES
    wxsvg
    wx_gtk3u_xrc-3.2
    wx_gtk3u_html-3.2
    wx_gtk3u_qa-3.2
    wx_gtk3u_core-3.2
    wx_baseu_xml-3.2
    wx_baseu_net-3.2
    wx_baseu-3.2)
# 可执行文件输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $(CMAKE_BINARY_DIR))

# @todo 查找动态库列表文件原始路径
# @param LIB_NAMES 动态库名字列表
# @param LIB_SEARCH_DIR 库目录
# @param LIB_FILE_PATHS 第三方库文件路径
# @param LIB_LINK_DIRS 第三方库链接搜索目录
function(get_custom_libraries LIB_NAMES LIB_SEARCH_DIR LIB_FILE_PATHS LIB_LINK_DIRS)
    set(LIBRARY_PATHS "")
    set(LIBRARY_DIRS "")
    # 递归查找所有库文件
    if("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
        # 递归查找所有 .so 文件
        file(GLOB_RECURSE LIB_FILES "${LIB_SEARCH_DIR}/*.so")
        # 遍历库名字列表
        foreach(LIB_NAME IN LISTS LIB_NAMES)
            # 遍历文件列表
            foreach(LIB_FILE IN LISTS LIB_FILES)
                get_filename_component(LIB_FILE_NAME ${LIB_FILE} NAME)  # 获取带后缀的文件名
                if(${LIB_FILE_NAME} STREQUAL "lib${LIB_NAME}.so")
                    get_filename_component(LIB_DIR_NAME ${LIB_FILE} DIRECTORY) # 获取目录路径
                    list(APPEND LIBRARY_DIRS ${LIB_DIR_NAME})
                    list(APPEND LIBRARY_PATHS ${LIB_FILE})
                    break()
                endif()
            endforeach()
        endforeach()
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")

    endif()
    # 去除重复的目录
    list(REMOVE_DUPLICATES LIBRARY_DIRS)
    # 将结果传递到父作用域
    set(${LIB_FILE_PATHS} ${LIBRARY_PATHS} PARENT_SCOPE)
    set(${LIB_LINK_DIRS} ${LIBRARY_DIRS} PARENT_SCOPE)
endfunction()


# @todo 安装自定义库到指定目录
# @param FILE_PATHS 第三方库文件路径
# @param DEST_DIR 目录子目录
function(install_files FILE_PATHS DEST_DIR)
    # 创建目标目录
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${DEST_DIR})
    # 添加自定义命令来拷贝库文件
    foreach(FILE_PATH ${FILE_PATHS})
        add_custom_command(
            TARGET iwealth POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy ${FILE_PATH} ${CMAKE_BINARY_DIR}/${DEST_DIR}
            COMMENT "Copying ${FILE_PATH} to ${CMAKE_BINARY_DIR}/${DEST_DIR}"
        )
    endforeach()
endfunction()

get_custom_libraries("${SHARE_LIBRARIES}" "${LIB_DIR}" CUSTOM_LIB_FILES CUSTOM_LINK_DIRS)

# 编译头文件搜索目录
include_directories(
        ${SRC_DIR}
        ${LIB_DIR}/curl/include
        ${LIB_DIR}/wxsvg/include
        ${LIB_DIR}/wxWidgets/3.2.4/include
        ${LIB_DIR}/wxWidgets/3.2.4/aarch64_linux/wx/include/gtk3-unicode-3.2
)

# 源代码，方式一
# aux_source_directory(${SRC_DIR}/console SRC_CONSOLE)
aux_source_directory(${SRC_DIR}/formula SRC_FORMULA)
aux_source_directory(${SRC_DIR}/net SRC_NET)
aux_source_directory(${SRC_DIR}/search SRC_SEARCH)
aux_source_directory(${SRC_DIR}/spider SRC_SPIDER)
aux_source_directory(${SRC_DIR}/stock SRC_STOCK)
aux_source_directory(${SRC_DIR}/ui SRC_UI)
aux_source_directory(${SRC_DIR}/util SRC_UTIL)
set(SRC_LIST ${SRC_CONSOLE} ${SRC_FORMULA} ${SRC_NET} ${SRC_SEARCH} ${SRC_SPIDER} ${SRC_STOCK} ${SRC_UI} ${SRC_UTIL})

# message(status "Sources 1= ${SRC_LIST}")

# 源代码，方式二
file(GLOB_RECURSE SRC_LIST ${SRC_DIR}/*.cpp)

# 生成可执行文件
add_executable(iwealth ${SRC_LIST})

# 预处理器定义
target_compile_definitions(iwealth PRIVATE DIWEALTH)
target_compile_definitions(iwealth PUBLIC WXUSINGDLL)
target_compile_definitions(iwealth PUBLIC D__WXGTK3__)
target_compile_definitions(iwealth PUBLIC D__WXGTK__)
target_compile_definitions(iwealth PUBLIC D_FILE_OFFSET_BITS=64)

# 链接动态库
link_directories(${CUSTOM_LINK_DIRS})
#set_target_properties(myapp PROPERTIES LINK_FLAGS "-pthread")
target_link_libraries(iwealth PRIVATE pthread)
target_link_libraries(iwealth PRIVATE ${SHARE_LIBRARIES})

# 设置 动态库运行时搜索路径 RPATH
set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# 安装依赖的第三方动态库到指定的安装目录
install_files(${CUSTOM_LIB_FILES} "libs")
