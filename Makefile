# Makefile - 千年城影·旧石器时代防线 构建脚本
# Tower Defense Game Build Script
# 由 CIO-TD 维护，修改前请知会 CIO
# Maintained by CIO-TD — notify CIO before modifying

CC      = gcc
CFLAGS  = -std=c99 -Wall -Wextra -g
TARGET  = tower_defense

# Raylib 路径（静态库已内嵌项目，无需额外安装）
# Raylib path (static library bundled — no extra installation needed)
INCLUDE = -I./lib/raylib/include -I./src
LDFLAGS = -L./lib/raylib/lib
LIBS    = -lraylib -lopengl32 -lgdi32 -lwinmm

# 自动收集所有 .c 源文件
# Automatically collect all .c source files
SRCS    = src/main.c \
          src/core/game.c \
          src/map/map.c \
          src/enemy/enemy.c \
          src/tower/tower.c \
          src/projectile/projectile.c \
          src/ui/ui.c \
          src/utils/utils.c

OBJS    = $(SRCS:.c=.o)

# ============================================================
# 构建目标 | Build Targets
# ============================================================

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@.exe $^ $(LDFLAGS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDE) -c -o $@ $<

# 清理编译产物（Windows 兼容）
# Clean build artifacts (Windows compatible)
clean:
	del /Q src\*.o src\core\*.o src\enemy\*.o src\tower\*.o \
	       src\projectile\*.o src\ui\*.o src\utils\*.o src\map\*.o \
	       $(TARGET).exe 2>nul || true

# 重新完整构建
# Full rebuild
rebuild: clean all

.PHONY: all clean rebuild
