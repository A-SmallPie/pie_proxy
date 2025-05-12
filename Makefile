CXX := g++

SRC_DIR := src
BIN_DIR := bin
INC_DIR := include
BUILD_DIR := build

# 用于控制编译阶段的行为
CXXFLAGS := -std=c++17 -Wall -Wextra -I$(INC_DIR)
# 用于控制链接阶段的行为
LDFLAGS :=
# 用于制定链接的库文件
LDLIBS :=

# TARGET := program

# 默认目标，系统会自动分析能够生成这些目标的依赖
# 系统想生成client，但是发现client的生成需要client.o，于是系统就去检查client.o是否存在或者需要更新
# 如果o文件比client新，就直接执行client的指令，否则就触发client.o的生成规则
# 检查client.o是否需要更新，检查是否需要更新就需要查找client.o的生成规则，系统通过目标名称来匹配生成规则
# 必须有一个目标名称为client.o，系统才能通过这个目标的依赖项判断是否需要更新（如果确实没有系统就会尝试一些隐士规则）
# 如果需要更新或者压根没有这个文件，系统就需要执行生成client.o的编译指令，生成这个文件
# client.o的编译指令完成后也就具备了client的编译条件，系统回到client的生成规则生成client
# 在依赖分析阶段就会构建完整的依赖图CAG，因此在发现client需要client需要client.o的时候就会立刻跳转过去生成client.o
all: server client


# 后面生成的文件为clent.o，但是目标名称可以不叫这个
# -c为只编译不链接，编译就是把代码翻译成机器能看懂的东西， -o用来指定输出文件的文件名和位置
client.o: src/client/client.cpp
	g++ -c src/client/client.cpp -o src/client/client.o

server.o: src/server/server.cpp
	g++ -c src/server/server.cpp -o src/server/server.o

client: src/client/client.o
	g++ src/client/client.o -o bin/client

server: src/server/server.o
	g++ src/server/server.o -o bin/server

.PHONY: clean
clean:
	rm -f src/client/client.o src/server/server.o