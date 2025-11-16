CXX = g++
CXXFLAGS = -O2 -fopenmp -Wall -Wextra -std=c++17
LDFLAGS = -fopenmp

# Директории
SRC_DIR = src
BIN_DIR = bin
BUILD_DIR = build

# Исполняемые файлы
MAIN_TARGET = $(BUILD_DIR)/main

# Исходные файлы
MAIN_SRC = $(SRC_DIR)/main.cpp

# Объектные файлы
MAIN_OBJ = $(BIN_DIR)/main.o

# Заголовочные файлы
HEADERS = $(SRC_DIR)/base_solver.hpp \
          $(SRC_DIR)/sequential_solver.hpp \
          $(SRC_DIR)/parallel_solver.hpp

# Сборка всех исполняемых файлов
all: $(MAIN_TARGET)

# Создание директорий
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Компиляция объектных файлов
$(MAIN_OBJ): $(MAIN_SRC) $(HEADERS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $(MAIN_SRC) -o $(MAIN_OBJ)

# Линковка исполняемых файлов
$(MAIN_TARGET): $(MAIN_OBJ) | $(BUILD_DIR)
	$(CXX) $(MAIN_OBJ) -o $(MAIN_TARGET) $(LDFLAGS)

# Очистка артефактов сборки
clean:
	rm -rf $(BIN_DIR)/* $(BUILD_DIR)/*

help:
	@echo "Доступные цели:"
	@echo "  all          - собрать все исполняемые файлы (по умолчанию)"
	@echo "  clean        - удалить все собранные файлы"
	@echo "  help         - показать эту справку"

.PHONY: all clean help
