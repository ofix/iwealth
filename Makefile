CXX=c++
CXXFLAGS= -Wall -Wextra
INCLUDE_DIRS= -Isrc \
			  -Ilib \
			  -Ilib/curl/include \
			  -Ilib/wxsvg/include \
			  -Ilib/wxWidgets/3.2.4/include \
			  -Ilib/wxWidgets/3.2.4/aarch64_linux/wx/include/gtk3-unicode-3.2 \
			  -DWXUSINGDLL \
			  -D__WXGTK3__ \
              -D__WXGTK__ \
              -D_FILE_OFFSET_BITS=64
LDFLAGS= -L/home/greatwall/work/iwealth/lib/wxWidgets/3.2.4/aarch64_linux \
         -L/home/greatwall/work/iwealth/lib/wxsvg/aarch64_linux' \
      -pthread \
      -Wl,-rpath,/home/greatwall/work/iwealth/lib/wxWidgets/3.2.4/aarch64_linux \
      -lwxsvg \
      -lwx_gtk3u_xrc-3.2 \
      -lwx_gtk3u_html-3.2 \
      -lwx_gtk3u_qa-3.2 \
      -lwx_gtk3u_core-3.2 \
      -lwx_baseu_xml-3.2 \
      -lwx_baseu_net-3.2 \
      -lwx_baseu-3.2
TARGET=dist/aarch64/iwealth
SRC_DIR=src
BUILD_DIR=build/make_aarch64

SRCS=$(shell find $(SRC_DIR) -name "*.cpp")
OBJS=$(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)

all:$(TARGET)

# 链接生成可执行文件
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) $(OBJS) $(LDFLAGS) -o $@

# 编译 .cpp 文件生成 .o 文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# 清理
clean:
	rm -rf $(BUILD_DIR) $(TARGET)


