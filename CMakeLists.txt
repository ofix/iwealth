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
    elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
        # 递归查找所有 .dll 文件
        file(GLOB_RECURSE LIB_FILES "${LIB_SEARCH_DIR}/*.dll")
    endif()

    # 遍历库名字列表
    foreach(LIB_NAME IN LISTS LIB_NAMES)
        # 遍历文件列表
        foreach(LIB_FILE IN LISTS LIB_FILES)
            get_filename_component(LIB_FILE_NAME ${LIB_FILE} NAME)  # 获取带后缀的文件名
            if(${LIB_FILE_NAME} MATCHES "${LIB_NAME}")
                get_filename_component(LIB_DIR_NAME ${LIB_FILE} DIRECTORY) # 获取目录路径
                list(APPEND LIBRARY_DIRS ${LIB_DIR_NAME})
                list(APPEND LIBRARY_PATHS ${LIB_FILE})
                break()
            endif()
        endforeach()
    endforeach()

    # 去除重复的目录
    list(REMOVE_DUPLICATES LIBRARY_DIRS)
    # 将结果传递到父作用域
    set(${LIB_FILE_PATHS} ${LIBRARY_PATHS} PARENT_SCOPE)
    set(${LIB_LINK_DIRS} ${LIBRARY_DIRS} PARENT_SCOPE)
endfunction()

# @todo 安装自定义库到指定目录
# @param FILE_PATHS 第三方库文件路径
# @param DEST_DIR 目录子目录
function(install_files TARGET FILE_PATHS DEST_DIR)
    # 创建目标目录
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${DEST_DIR})
    # 添加自定义命令来拷贝库文件
    foreach(FILE_PATH ${FILE_PATHS})
        file(COPY ${FILE_PATH} DESTINATION ${CMAKE_BINARY_DIR}/${DEST_DIR} FOLLOW_SYMLINK_CHAIN)
    endforeach()
endfunction()

# 定义一个函数来移除包含指定关键词的项
function(list_remove LIST KEYWORD)
    # 创建一个新的列表来存储不包含关键词的项
    set(NEW_LIST "")

    # 遍历原始列表
    foreach(ITEM IN LISTS ${LIST})
        # 检查当前项是否包含关键词
        if(NOT ITEM MATCHES "${KEYWORD}")
            # 如果不包含关键词，则将其添加到新列表中
            list(APPEND NEW_LIST ${ITEM})
        endif()
    endforeach()
    # 将新列表赋值回原始列表变量
    set(${LIST} ${NEW_LIST} PARENT_SCOPE)
endfunction()

function(custom_link_directories TARGET LIB_DIRS)
    foreach(DIR ${LIB_DIRS})
        target_link_directories(${TARGET} PRIVATE "${DIR}")
    endforeach()
endfunction()

##################################################################################################
# CMAKE 最低版本号
cmake_minimum_required(VERSION 3.15)

# 项目名称
project(iwealth)

# 开启 CMAKE 调试模式
set(CMAKE_VERBOSE_MAKEFILE ON)
# 指定标准库版本
set(CMAKE_CXX_STANDARD 14)
# 自定义第三方库目录
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib)
# 源代码根目录
set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
# 设置平台
set(PLATFORM ${CMAKE_HOST_SYSTEM_PROCESSOR})
# 可执行文件输出目录
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY $(CMAKE_BINARY_DIR))

# 依赖的动态库
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(SHARE_LIBRARIES
        curl
        wxsvg
        wxmsw32u_xrc
        wxmsw32u_html
        wxmsw32u_core
        wxbase32u_xml
        wxbase32u_net
        wxbase32u)
    message("SET Window C++ Compiler")
elseif("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    set(SHARE_LIBRARIES
        curl
        wxsvg
        wx_gtk3u_xrc-3.2
        wx_gtk3u_html-3.2
        wx_gtk3u_qa-3.2
        wx_gtk3u_core-3.2
        wx_baseu_xml-3.2
        wx_baseu_net-3.2
        wx_baseu-3.2)
endif()

get_custom_libraries("${SHARE_LIBRARIES}" "${LIB_DIR}" CUSTOM_LIB_FILES CUSTOM_LINK_DIRS)

include_directories(
        ${SRC_DIR}
        ${LIB_DIR}
        ${LIB_DIR}/curl/include
        ${LIB_DIR}/wxsvg/include
        ${LIB_DIR}/wxWidgets/3.2.4/include
        ${LIB_DIR}/wxWidgets/3.2.4/aarch64_linux/wx/include/gtk3-unicode-3.2
)

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
file(GLOB_RECURSE SRC_LIST ${SRC_DIR}/*.cpp)
list_remove(SRC_LIST "console")

# 设置 动态库运行时搜索路径 RPATH
set(CMAKE_INSTALL_RPATH "$ORIGIN/libs") # $ORIGIN 表示可执行文件所在目录
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) # 在构建时也使用 RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

# 生成可执行文件
add_executable(iwealth ${SRC_LIST})

# 预处理器定义
target_compile_definitions(iwealth PRIVATE IWEALTH)
target_compile_definitions(iwealth PUBLIC WXUSINGDLL)
target_compile_definitions(iwealth PUBLIC __WXGTK3__)
target_compile_definitions(iwealth PUBLIC __WXGTK__)
target_compile_definitions(iwealth PUBLIC _FILE_OFFSET_BITS=64)


# 设置库搜索路径, 注意一定要带引号传递 CUSTOM_LINK_DIRS 列表参数
custom_link_directories(iwealth "${CUSTOM_LINK_DIRS}")

# 将依赖的第三方动态库拷贝到可执行文件所在的目录
install_files(iwealth "${CUSTOM_LIB_FILES}" "libs")
# 拷贝assets目录和拼音字典文件
file(COPY ${CMAKE_SOURCE_DIR}/src/assets DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
file(COPY ${CMAKE_SOURCE_DIR}/src/search/ChinesePinYin.dic DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)

# 链接动态库
# set_target_properties(iwealth PROPERTIES LINK_FLAGS "-Wl,-rpath,./")
#set_target_properties(myapp PROPERTIES LINK_FLAGS "-pthread")
target_link_libraries(iwealth PRIVATE pthread)
target_link_libraries(iwealth PRIVATE ${SHARE_LIBRARIES})
