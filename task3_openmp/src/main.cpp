#include "sequential_solver.hpp"
#include "parallel_solver.hpp"

#include <cstdlib>

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <random>
#include <vector>

void print_usage(const char* prog_name) {
    std::cout << "Использование: " << prog_name << " [количество_потоков]" << std::endl;
    std::string usage = R"(
Программа находит первое число в массиве, превышающее заданное значение
Массив заполняется случайными числами типа long long
Параметры:
  0                             - использовать последовательную версию
  N [положительное целое число] - использовать параллельную версию с N потоками
  не указано                    - использовать дефолтное количество потоков OpenMP
)";
    std::cout << usage << std::endl;
}

void print_description() {
    std::string description = R"(
Индивидуальный номер: 51
Задача:
- Найти первое число, превышающее заранее заданное
- Использование parallel for и reduction
- long int / long long int (64-bit)
)";
    std::cout << description << std::endl;
}

int main(int argc, char* argv[]) {
    // nullopt означает использовать дефолтное значение числа потоков для OpenMP
    std::optional<int> num_threads = std::nullopt;

    // Обработка аргументов командной строки
    if (argc > 2) {
        std::cerr << "Ошибка: слишком много аргументов" << std::endl;
        print_usage(argv[0]);
        return 1;
    }
    if (argc == 2) {
        if (std::string(argv[1]) == "-h" || std::string(argv[1]) == "--help") {
            print_usage(argv[0]);
            return 0;
        }
        num_threads = std::atoi(argv[1]);
        if (num_threads.value() < 0) {
            std::cerr << "Ошибка: число потоков не может быть отрицательным" << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }

    print_description();
    
    std::unique_ptr<BaseSolver<long long>> solver;
    if (num_threads.has_value() && num_threads.value() == 0) {
        solver = std::make_unique<SequentialSolver<long long>>();
    } else {
        solver = std::make_unique<ParallelSolver<long long>>(num_threads);
    }
    std::cout << "Выбранная Вами реализация: " << solver->get_name() << std::endl;
    
    // Параметры задачи
    constexpr size_t array_size = 10000000;
    constexpr long long threshold = 5000000;
    std::cout << "========================================" << std::endl;
    std::cout << "Размер массива:     " << array_size << std::endl;
    std::cout << "Пороговое значение: " << threshold << std::endl;
    std::cout << "Массив будет заполнен случайными числами" << std::endl;
    // Создаём массив случайных чисел
    std::vector<long long> arr(array_size);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<long long> distr(std::numeric_limits<long long>::min(), std::numeric_limits<long long>::max());
    std::generate(arr.begin(), arr.end(), [&]() { return distr(gen); });
    std::cout << "========================================" << std::endl;
        
    auto result = solver->solve(arr, threshold);
    if (result.has_value()) {
        std::cout << "Найденное значение: " << result.value() << std::endl;
    } else {
        std::cout << "Элемент не найден :(" << std::endl;
    }
    
    return 0;
}

