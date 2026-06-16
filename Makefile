CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
SRCS = src/main.c src/student.c src/auth.c src/file_ops.c src/activity_log.c src/utils.c
TARGET = sms.exe

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

clean:
	@del /q $(TARGET) 2>nul || rm -f $(TARGET)

.PHONY: all clean
