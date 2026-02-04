CC = gcc
CFLAGS = -Wall -Wextra -g -std=c11
LDFLAGS = -lpthread

# 디렉토리 및 파일명
SRC_DIR = .
OBJ_DIR = .
SRC = thread_pool.c
OBJ = $(SRC:.c=.o)
TARGET = libthreadpool.a

# 기본 타겟
all: $(TARGET)

# 정적 라이브러리 생성
$(TARGET): $(OBJ)
	ar rcs $@ $^
	@echo "Build complete: $@"

# 오브젝트 파일 컴파일
%.o: %.c thread_pool.h
	$(CC) $(CFLAGS) -c $< -o $@

# 정리
clean:
	rm -f $(OBJ) $(TARGET)
	@echo "Clean complete"

# 재컴파일
rebuild: clean all

.PHONY: all clean rebuild

