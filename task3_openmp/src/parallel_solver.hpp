#pragma once

#include "base_solver.hpp"

#include <cassert>

#include <limits>

#include <omp.h>

/**
 * Параллельная реализация задачи с использованием OpenMP
 * Использует директиву parallel for reduction(min:)
 
 * Суть:
 *   - каждый поток ищет минимальный индекс элемента, большего порога
 *   - затем редукция по полученным минимальным индексам
 *   - зная индекс - получаем сам элемент (если такой существует)
 */
template<typename T>
class ParallelSolver : public BaseSolver<T> {
public:
    /**
     * Конструктор
     * @param num_threads количество потоков (опционально, если не указано - используется дефолтное значение OpenMP)
     */
    explicit ParallelSolver(std::optional<int> num_threads = std::nullopt) {
        if (num_threads.has_value()) {
            omp_set_num_threads(num_threads.value());
        }
    }
    
    std::optional<T> solve(const std::vector<T>& arr, T threshold) override {
        if (arr.empty()) {
            return std::nullopt;
        }
        
        // Мир жесток - не хочется писать что-то умное с std::optional<size_t> для индекса
        // Поэтому будем считать, что совсем уж большого массива не будет в программе
        assert(arr.size() != std::numeric_limits<std::size_t>::max());

        std::size_t min_index = std::numeric_limits<std::size_t>::max();
        
        // Редуцируем по минимальному индексу
        #pragma omp parallel for reduction(min:min_index)
        for (std::size_t i = 0; i < arr.size(); i++) {
            if (arr[i] > threshold) {
                min_index = std::min(min_index, i);
            }
        }
        
        // Элемент, больший порога, не найден
        if (min_index == std::numeric_limits<std::size_t>::max()) {
            return std::nullopt;
        }
        
        return arr[min_index];
    }
    
    std::string get_name() const override {
        return "Параллельная версия (OpenMP)";
    }
};
