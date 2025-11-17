#pragma once

#include <optional>
#include <string>
#include <vector>

/**
 * Абстрактный базовый шаблонный класс для решения задачи
 * "Найти первое число, превышающее заранее заданное"
 * 
 * @tparam T тип элементов массива (по условию задачи long long int)
 */
template<typename T>
class BaseSolver {
public:
    /**
     * Виртуальный деструктор, чтобы не словить UB
     */
    virtual ~BaseSolver() = default;
    
    /**
     * Решить задачу: "Найти первое число, превышающее заранее заданное"
     * 
     * @param arr массив чисел для поиска
     * @param threshold заранее заданное пороговое значение
     * @return std::nullopt, если число не найдено; иначе первое число, превышающее threshold
     */
    virtual std::optional<T> solve(const std::vector<T>& arr, T threshold) = 0;
    
    /**
     * Получить имя реализации (последовательная или параллельная)
     */
    virtual std::string get_name() const = 0;
};

