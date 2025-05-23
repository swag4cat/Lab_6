# Компилятор и флаги
CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2

# Исходные файлы
SRCS := aes.cpp main.cpp
TARGET := aes_cbc

# Файлы для очистки
OUTPUT_FILES := output.txt cipher.bin aes_key.bin

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -f $(TARGET) $(OUTPUT_FILES)
