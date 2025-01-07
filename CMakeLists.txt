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

# @todo 拷贝文件列表到指定目录
# @param FILE_PATHS 第三方库文件路径
# @param DEST_DIR 目录子目录
function(copy_files TARGET FILE_PATHS DEST_DIR)
    # 创建目标目录
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${DEST_DIR})
    # 添加自定义命令来拷贝库文件
    foreach(FILE_PATH ${FILE_PATHS})
        file(COPY ${FILE_PATH} DESTINATION ${CMAKE_BINARY_DIR}/${DEST_DIR} FOLLOW_SYMLINK_CHAIN)
        message("-- Copying file ${FILE_PATH} --> ${CMAKE_BINARY_DIR}/${DEST_DIR}")
    endforeach()
endfunction()

# @todo cmake安装到指定文件目录
# @param SRC_PATH 原始路径
# @param DST_PATH 目标路径
function(create_symlink SRC_PATH DST_PATH)
    install(CODE "execute_process(COMMAND ${CMAKE_COMMAND} -E create_symlink ${SRC_PATH} ${DST_PATH})")
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
project(iwealth VERSION 0.5)

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
# 设置 动态库运行时搜索路径 RPATH
set(CMAKE_INSTALL_RPATH "$ORIGIN/libs") # $ORIGIN 表示可执行文件所在目录
# 等价于下面这条语句,注意 $ORIGIN 需要用单引号（'）括起来，以防止 Shell 解释它。
# set_target_properties(iwealth PROPERTIES LINK_FLAGS "-Wl,-rpath,'$ORIGIN/libs'")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE) # 在构建时也使用 RPATH
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# 设置工程默认安装路径
set(CMAKE_INSTALL_PREFIX "/usr/local/iwealth")
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

# iWealth 源代码
file(GLOB_RECURSE WEALTH_SRC_LIST ${SRC_DIR}/*.cpp)
list_remove(WEALTH_SRC_LIST "console")

# iConsole 源代码
set(CONSOLE_SRC_LIST
    ${SRC_DIR}/console/Test.cpp
    ${SRC_DIR}/stock/Stock.cpp
    ${SRC_DIR}/stock/StockDataStorage.cpp
    ${SRC_DIR}/stock/StockShareKline.cpp
    ${SRC_DIR}/spider/SpiderShareBriefInfo.cpp
    ${SRC_DIR}/spider/SpiderConceptListEastMoney.cpp
    ${SRC_DIR}/spider/SpiderShareQuote.cpp
    ${SRC_DIR}/spider/SpiderShareKline.cpp
    ${SRC_DIR}/spider/SpiderShareKlineProviderBaidu.cpp
    ${SRC_DIR}/spider/SpiderShareKlineProviderEastMoney.cpp
    ${SRC_DIR}/spider/SpiderShareKlineProvider.cpp
    ${SRC_DIR}/spider/SpiderShareCategory.cpp
    ${SRC_DIR}/spider/Spider.cpp
    ${SRC_DIR}/stock/StockMonitorShare.cpp
    ${SRC_DIR}/stock/UserFavoriteShare.cpp
    ${SRC_DIR}/stock/Stock.cpp
    ${SRC_DIR}/stock/ShareCategory.cpp
    ${SRC_DIR}/net/Request.cpp
    ${SRC_DIR}/net/ConcurrentRequest.cpp
    ${SRC_DIR}/search/Trie.cpp
    ${SRC_DIR}/search/ChinesePinYin.cpp
    ${SRC_DIR}/ui/RichHelper.cpp
    ${SRC_DIR}/util/Timer.cpp
    ${SRC_DIR}/util/DateTime.cpp
    ${SRC_DIR}/util/ConsoleTable.cpp
    ${SRC_DIR}/util/EasyLogger.cpp
    ${SRC_DIR}/util/FileTool.cpp
    ${SRC_DIR}/util/Global.cpp
    ${SRC_DIR}/util/RichResult.cpp
    )

# iSmith 源代码
set(SMITH_SRC_LIST
    ${SRC_DIR}/console/Smith.cpp
    ${SRC_DIR}/util/FileTool.cpp
    ${SRC_DIR}/util/Global.cpp
)

# 生成可执行文件
add_executable(iwealth ${WEALTH_SRC_LIST})
add_executable(iconsole  ${CONSOLE_SRC_LIST})
add_executable(ismith  ${SMITH_SRC_LIST})

# 预处理器定义
target_compile_definitions(iwealth PRIVATE IWEALTH)
target_compile_definitions(iwealth PUBLIC WXUSINGDLL)
target_compile_definitions(iwealth PUBLIC __WXGTK3__)
target_compile_definitions(iwealth PUBLIC __WXGTK__)
target_compile_definitions(iwealth PUBLIC _FILE_OFFSET_BITS=64)

target_compile_definitions(iconsole PUBLIC WXUSINGDLL)
target_compile_definitions(iconsole PUBLIC __WXGTK3__)
target_compile_definitions(iconsole PUBLIC __WXGTK__)
target_compile_definitions(iconsole PUBLIC _FILE_OFFSET_BITS=64)

target_compile_definitions(ismith PUBLIC WXUSINGDLL)
target_compile_definitions(ismith PUBLIC __WXGTK3__)
target_compile_definitions(ismith PUBLIC __WXGTK__)
target_compile_definitions(ismith PUBLIC _FILE_OFFSET_BITS=64)

# 设置库搜索路径, 注意一定要带引号传递 CUSTOM_LINK_DIRS 列表参数，此路径和运行时动态库路径rpath可以不一样
custom_link_directories(iwealth "${CUSTOM_LINK_DIRS}")
custom_link_directories(iconsole "${CUSTOM_LINK_DIRS}")
custom_link_directories(ismith "${CUSTOM_LINK_DIRS}")

# 将依赖的 第三方动态库、文件和图片 拷贝到可执行文件所在的目录
copy_files(iwealth "${CUSTOM_LIB_FILES}" "libs")
file(COPY ${CMAKE_SOURCE_DIR}/src/assets DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)
file(COPY ${CMAKE_SOURCE_DIR}/src/search/ChinesePinYin.dic DESTINATION ${CMAKE_BINARY_DIR} FOLLOW_SYMLINK_CHAIN)

# 链接动态库
# set_target_properties(iwealth PROPERTIES LINK_FLAGS "-pthread")
target_link_libraries(iwealth PRIVATE pthread)
target_link_libraries(iwealth PRIVATE ${SHARE_LIBRARIES})

target_link_libraries(iconsole PRIVATE pthread)
target_link_libraries(iconsole PRIVATE ${SHARE_LIBRARIES})

target_link_libraries(ismith PRIVATE pthread)
target_link_libraries(ismith PRIVATE ${SHARE_LIBRARIES})

# 安装文件到指定目录中 (make install)
install(TARGETS iwealth iconsole ismith
        RUNTIME DESTINATION .
        PERMISSIONS
        OWNER_READ OWNER_WRITE OWNER_EXECUTE  # 所有者权限：读、写、执行
        GROUP_READ GROUP_EXECUTE              # 组权限：读、执行
        WORLD_READ WORLD_EXECUTE              # 其他用户权限：读、执行
        )
install(DIRECTORY ${CMAKE_BINARY_DIR}/libs DESTINATION .)
install(DIRECTORY ${CMAKE_BINARY_DIR}/assets DESTINATION .)
install(FILES ${CMAKE_BINARY_DIR}/ChinesePinYin.dic DESTINATION .)
create_symlink(${CMAKE_INSTALL_PREFIX}/iwealth /usr/bin/iwealth)
create_symlink(${CMAKE_INSTALL_PREFIX}/iconsole /usr/bin/iconsole)
# 修改安装目录写入权限，否者以普通用户运行程序，无法创建子目录 data
install(
    CODE "execute_process(COMMAND chmod -R a+w ${CMAKE_INSTALL_PREFIX})"
)