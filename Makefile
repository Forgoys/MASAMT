# 编译器设置 - 自动检测平台
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -I include

# 检测操作系统
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS 特定设置
    CXXFLAGS += -stdlib=libc++
endif

# 目录设置
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

# 源文件和可执行文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
TARGET = $(BIN_DIR)/masamt

# 头文件依赖
HEADERS = $(wildcard $(INC_DIR)/*.hpp)

# 创建必要的目录
$(shell mkdir -p $(BIN_DIR))

# 默认目标
all: $(TARGET)

# 编译规则
$(TARGET): $(SRCS) $(HEADERS)
	@echo "正在编译 MASAMT..."
	@echo "平台: $(UNAME_S)"
	@echo "编译器: $(CXX)"
	@echo "编译选项: $(CXXFLAGS)"
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)
	@echo "✅ 编译完成: $(TARGET)"

# 清理规则
clean:
	rm -rf $(BIN_DIR)
	@echo "清理完成"

# 运行程序
run: $(TARGET)
	./$(TARGET)

# 测试特定CSV文件
test-csv: $(TARGET)
	./$(TARGET) -f test.csv -c

# 显示帮助
help: $(TARGET)
	./$(TARGET) -h

# 跨平台测试
test: $(TARGET)
	@echo "=== 跨平台兼容性测试 ==="
	@echo "系统: $(UNAME_S)"
	@$(TARGET) -h

# 安装 (可选)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# 调试版本
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# 发布版本
release: CXXFLAGS += -O3 -DNDEBUG
release: $(TARGET)

# 检查代码风格 (如果有相关工具)
check:
	@echo "检查代码风格..."
	@if command -v cppcheck >/dev/null 2>&1; then \
		cppcheck --enable=all --std=c++11 $(SRC_DIR); \
	else \
		echo "cppcheck 未安装，跳过静态分析"; \
	fi

# 伪目标声明
.PHONY: all clean run test-csv help test install debug release check