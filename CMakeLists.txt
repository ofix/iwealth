cmake_minimum_required(VERSION 3.15)

project(iwealth)

set(ROOT_DIR,${PROJECT_SOURCE_DIR})
set(SOURCE_DIR,${ROOT_DIR}/src)
set(INCLUDE_DIR,${ROOT_DIR}/src)

set(ASSETS_DIR,${INCLUDE_DIR}/assets)
set(CONSOLE_DIR,${INCLUDE_DIR}/console)
set(FORMULA_DIR,${INCLUDE_DIR}/formula)
set(NET_DIR,${INCLUDE_DIR}/net)
set(SEARCH_DIR,${INCLUDE_DIR}/search)
set(SPIDER_DIR,${INCLUDE_DIR}/spider)
set(STOCK_DIR,${INCLUDE_DIR}/stock)
set(UI_DIR,${INCLUDE_DIR}/ui)
set(COMPONENTS_DIR,${UI_DIR}/components)
set(UTIL_DIR,${INCLUDE_DIR}/util)

include_directories(${INCLUDE_DIR}/src)
aux_source_directory(${SOURCE_DIR},SRC_FILES)


link_directories(${ROOT_DIR}/lib/curl/aarch64_linux, ${ROOT_DIR}/lib/wxWidgets/3.2.4/aarch64_linux)
add_library(iwealth_lib ${SRC_FILES})

file(GLOB_RECURSIVE,${SOURCE_DIR}/*.cpp)
set(CMAKE_CXX_STANDARD 11)




add_executable(iwealth,${SRC_FILES})
