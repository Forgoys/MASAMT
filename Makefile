# 编译器设置
CXX = g++
CXXFLAGS = -std=c++11 -Wall -I include

# 目录设置
SRC_DIR = src
INC_DIR = include
BIN_DIR = bin

# 源文件和可执行文件
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
TARGET = $(BIN_DIR)/masamt

# 创建必要的目录
$(shell mkdir -p $(BIN_DIR))

# 默认目标
all: $(TARGET)

# 直接编译规则
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

# 清理规则
clean:
	rm -rf $(BIN_DIR)

# 运行程序
run: $(TARGET)
	./$(TARGET)

# 伪目标声明
.PHONY: all clean run 