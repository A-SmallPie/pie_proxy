CXX := g++

# 用于控制编译阶段的行为
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
# 用于控制链接阶段的行为
LDFLAGS :=
# 用于制定链接的库文件
LDLIBS :=-lpthread
SRC_DIR := src
BIN_DIR := bin
INC_DIR := include
BUILD_DIR := build
OBJ_DIR := $(BUILD_DIR)/obj
DEP_DIR := $(BUILD_DIR)/dep

SERVER_SRCS := $(shell find $(SRC_DIR)/server -name '*.cpp')
CLIENT_SRCS := $(shell find $(SRC_DIR)/client -name '*.cpp')
COMMON_SRCS := $(shell find $(SRC_DIR)/common -name '*.cpp')

HEADERS := $(shell find $(INC_DIR) -name '*.h' -o -name '*.hpp')

COMMON_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(COMMON_SRCS))
COMMON_DEPS := $(patsubst $(SRC_DIR)/%.cpp, $(DEP_DIR)/%.d, $(COMMON_SRCS))
SERVER_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SERVER_SRCS))
SERVER_DEPS := $(patsubst $(SRC_DIR)/%.cpp, $(DEP_DIR)/%.d, $(SERVER_SRCS))
CLIENT_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CLIENT_SRCS))
CLIENT_DEPS := $(patsubst $(SRC_DIR)/%.cpp, $(DEP_DIR)/%.d, $(CLIENT_SRCS))

TARGETS := $(BIN_DIR)/client $(BIN_DIR)/server
# 默认目标，系统会自动分析能够生成这些目标的依赖
# 系统想生成client，但是发现client的生成需要client.o，于是系统就去检查client.o是否存在或者需要更新
# 如果o文件比client新，就直接执行client的指令，否则就触发client.o的生成规则
# 检查client.o是否需要更新，检查是否需要更新就需要查找client.o的生成规则，系统通过目标名称来匹配生成规则
# 必须有一个目标名称为client.o，系统才能通过这个目标的依赖项判断是否需要更新（如果确实没有系统就会尝试一些隐士规则）
# 如果需要更新或者压根没有这个文件，系统就需要执行生成client.o的编译指令，生成这个文件
# client.o的编译指令完成后也就具备了client的编译条件，系统回到client的生成规则生成client
# 在依赖分析阶段就会构建完整的依赖图CAG，因此在发现client需要client需要client.o的时候就会立刻跳转过去生成client.o
.PHONY: all
all: $(TARGETS)


$(BUILD_DIR):
	mkdir -p $(BIN_DIR)
	mkdir -p $(OBJ_DIR)/client $(DEP_DIR)/client
	mkdir -p $(OBJ_DIR)/server $(DEP_DIR)/server
	mkdir -p $(OBJ_DIR)/common $(DEP_DIR)/common $(OBJ_DIR)/common/task $(DEP_DIR)/common/task



# 后面生成的文件为clent.o，但是目标名称可以不叫这个
# -c为只编译不链接，编译就是把代码翻译成机器能看懂的东西， -o用来指定输出文件的文件名和位置
# 编译服务器可执行文件
$(BIN_DIR)/server: $(COMMON_OBJS) $(SERVER_OBJS) | $(BUILD_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BIN_DIR)/client: $(CLIENT_OBJS) | $(BUILD_DIR)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(BUILD_DIR)
	mkdir -p $(dir $@) $(dir $(DEP_DIR)/$*.d)
	$(CXX) $(CXXFLAGS) -MMD -MP -MF $(DEP_DIR)/$*.d -c -o $@ $<


# 包含所有依赖文件
-include $(COMMON_DEPS) $(SERVER_DEPS) $(CLIENT_DEPS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: debug
debug:
	@echo "Common sources: $(COMMON_SRCS)"
	@echo "Server sources: $(SERVER_SRCS)"
	@echo "Client sources: $(CLIENT_SRCS)"
	@echo "Headers: $(HEADERS)"
	@echo "Common objects: $(COMMON_OBJS)"
	@echo "Server objects: $(SERVER_OBJS)"
	@echo "Client objects: $(CLIENT_OBJS)"
	@echo "Common deps: $(COMMON_DEPS)"
	@echo "Server deps: $(SERVER_DEPS)"
	@echo "Client deps: $(CLIENT_DEPS)"