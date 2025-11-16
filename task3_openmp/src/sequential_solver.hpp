#pragma once

#include "base_solver.hpp"

/**
 * Последовательная реализация задачи
 * Использует примитивный проход по массиву слева направо
 * Если нашли нужный элемент - возвращаем его
 * Если не нашли - возвращаем std::nullopt в конце функции
 */
template<typename T>
class SequentialSolver : public BaseSolver<T> {
public:
    std::optional<T> solve(const std::vector<T>& arr, T threshold) override {
        if (arr.empty()) {
            return std::nullopt;
        }
        
        for (std::size_t i = 0; i < arr.size(); i++) {
            if (arr[i] > threshold) {
                return arr[i];
            }
        }

        return std::nullopt;
    }
    
    std::string get_name() const override {
        return "Последовательная версия";
    }
};
