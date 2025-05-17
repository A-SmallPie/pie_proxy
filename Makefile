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

HEADERS := $(shell find $(INC_DIR) -name '*.h' -o -name '*.hpp')

SERVER_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SERVER_SRCS))
CLIENT_OBJS := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CLIENT_SRCS))
SERVER_DEPS := $(patsubst $(SRC_DIR)/%.cpp, $(DEP_DIR)/%.d, $(SERVER_SRCS))
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
all: $(TARGETS)

$(shell mkdir -p $(OBJ_DIR)/client $(OBJ_DIR)/server $(DEP_DIR)/client $(DEP_DIR)/server $(BIN_DIR))

# 后面生成的文件为clent.o，但是目标名称可以不叫这个
# -c为只编译不链接，编译就是把代码翻译成机器能看懂的东西， -o用来指定输出文件的文件名和位置
# 编译服务器可执行文件
$(BIN_DIR)/server: $(SERVER_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(BIN_DIR)/client: $(CLIENT_OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# 通用编译规则,同时生成依赖文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(DEP_DIR)/%.d
	$(CXX) $(CXXFLAGS) -c -o $@ $< 
	@cp $(DEP_DIR)/$*.td $(DEP_DIR)/$*.d && rm -f $(DEP_DIR)/$*.td

# 生成依赖文件的规则
$(DEP_DIR)/%.d: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -MM -MP -MT '$(OBJ_DIR)/$*.o $(DEP_DIR)/$*.d' -MF $(DEP_DIR)/$*.td $<


# 包含所有依赖文件
-include $(SERVER_DEPS) $(CLIENT_DEPS)

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: debug
debug:
	@echo "Server sources: $(SERVER_SRCS)"
	@echo "Client sources: $(CLIENT_SRCS)"
	@echo "Headers: $(HEADERS)"
	@echo "Server objects: $(SERVER_OBJS)"
	@echo "Client objects: $(CLIENT_OBJS)"
	@echo "Server deps: $(SERVER_DEPS)"
	@echo "Client deps: $(CLIENT_DEPS)"