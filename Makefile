CXX := g++

# 编译和链接选项
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
LDFLAGS :=
LDLIBS := -lpthread

# 目录结构
SRC_DIR := src
BIN_DIR := bin
INC_DIR := include
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
DEP_DIR := $(BUILD_DIR)/dep

# 自动查找源文件
SERVER_SRCS := $(shell find $(SRC_DIR)/server -name '*.cpp')
CLIENT_SRCS := $(shell find $(SRC_DIR)/client -name '*.cpp')
CORE_SRCS := $(shell find $(SRC_DIR)/core -name '*.cpp')

# 自动生成对象文件和依赖文件路径
CORE_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CORE_SRCS))
CORE_DEPS := $(patsubst $(SRC_DIR)/%.cpp, $(DEP_DIR)/%.d, $(CORE_SRCS))
SERVER_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SERVER_SRCS))
SERVER_DEPS := $(patsubst $(SRC_DIR)/%.cpp, $(DEP_DIR)/%.d, $(SERVER_SRCS))
CLIENT_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CLIENT_SRCS))
CLIENT_DEPS := $(patsubst $(SRC_DIR)/%.cpp, $(DEP_DIR)/%.d, $(CLIENT_SRCS))

# 最终目标
TARGETS := $(BIN_DIR)/client $(BIN_DIR)/server

.PHONY: all
all: $(TARGETS)

# 自动创建所需目录的函数
define make-dir
@mkdir -p $(dir $@)
@mkdir -p $(dir $(patsubst $(OBJ_DIR)/%.o, $(DEP_DIR)/%.d, $@))
endef

# 构建目标文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(call make-dir)
	$(CXX) $(CXXFLAGS) -MMD -MP -MF $(patsubst $(OBJ_DIR)/%.o, $(DEP_DIR)/%.d, $@) -c -o $@ $<

# 构建可执行文件
$(BIN_DIR)/server: $(CORE_OBJS) $(SERVER_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BIN_DIR)/client: $(CLIENT_OBJS)
	@mkdir -p $(@D)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# 包含依赖文件
-include $(CORE_DEPS) $(SERVER_DEPS) $(CLIENT_DEPS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: debug
debug:
	@echo "Core sources: $(CORE_SRCS)"
	@echo "Server sources: $(SERVER_SRCS)"
	@echo "Client sources: $(CLIENT_SRCS)"
	@echo "Core objects: $(CORE_OBJS)"
	@echo "Server objects: $(SERVER_OBJS)"
	@echo "Client objects: $(CLIENT_OBJS)"
	@echo "Core deps: $(CORE_DEPS)"
	@echo "Server deps: $(SERVER_DEPS)"
	@echo "Client deps: $(CLIENT_DEPS)"