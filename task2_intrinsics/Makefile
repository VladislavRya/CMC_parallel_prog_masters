CXX = g++
CXXFLAGS = -O2 -mavx -mavx2 -Wall -Wextra -std=c++17
LDFLAGS = 

# Директории
SRC_DIR = src
BIN_DIR = bin
BUILD_DIR = build

# Исполняемые файлы
MAIN_TARGET = $(BUILD_DIR)/main

# Исходные файлы
MAIN_SRC = $(SRC_DIR)/main.cpp
IMAGE_SRC = $(SRC_DIR)/image.cpp
SEQUENTIAL_SRC = $(SRC_DIR)/sequential_corrector.cpp
AVX_SRC = $(SRC_DIR)/avx_corrector.cpp

# Объектные файлы
MAIN_OBJ = $(BIN_DIR)/main.o
IMAGE_OBJ = $(BIN_DIR)/image.o
SEQUENTIAL_OBJ = $(BIN_DIR)/sequential_corrector.o
AVX_OBJ = $(BIN_DIR)/avx_corrector.o

# Заголовочные файлы
HEADERS = $(SRC_DIR)/image.hpp \
          $(SRC_DIR)/base_color_corrector.hpp \
          $(SRC_DIR)/sequential_corrector.hpp \
          $(SRC_DIR)/avx_corrector.hpp

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

$(IMAGE_OBJ): $(IMAGE_SRC) $(SRC_DIR)/image.hpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $(IMAGE_SRC) -o $(IMAGE_OBJ)

$(SEQUENTIAL_OBJ): $(SEQUENTIAL_SRC) $(SRC_DIR)/sequential_corrector.hpp $(SRC_DIR)/base_color_corrector.hpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $(SEQUENTIAL_SRC) -o $(SEQUENTIAL_OBJ)

$(AVX_OBJ): $(AVX_SRC) $(SRC_DIR)/avx_corrector.hpp $(SRC_DIR)/base_color_corrector.hpp | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) -c $(AVX_SRC) -o $(AVX_OBJ)

# Линковка исполняемых файлов
$(MAIN_TARGET): $(MAIN_OBJ) $(IMAGE_OBJ) $(SEQUENTIAL_OBJ) $(AVX_OBJ) | $(BUILD_DIR)
	$(CXX) $(MAIN_OBJ) $(IMAGE_OBJ) $(SEQUENTIAL_OBJ) $(AVX_OBJ) -o $(MAIN_TARGET) $(LDFLAGS)

# Очистка артефактов сборки
clean:
	rm -rf $(BIN_DIR)/* $(BUILD_DIR)/*

help:
	@echo "Доступные цели:"
	@echo "  all          - собрать все исполняемые файлы (по умолчанию)"
	@echo "  clean        - удалить все собранные файлы"
	@echo "  help         - показать эту справку"

.PHONY: all clean help
