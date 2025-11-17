#pragma once

#include "base_solver.hpp"

#include <cassert>
#include <future>
#include <limits>
#include <mutex>
#include <thread>
#include <optional>
#include <vector>
#include <iostream>


/**
 * Параллельная реализация задачи с использованием std::thread
 * Использует mutex для синхронизации доступа к общему минимальному индексу
 * Использует promise/future для возврата результатов из потоков
 * 
 * Суть:
 *   - массив делится на части, каждая часть обрабатывается отдельным потоком
 *   - каждый поток ищет минимальный индекс элемента, большего порога, в своей части
 *   - если другой поток уже нашёл элемент с меньшим индексом, то текущий поток завершает свою работу досрочно
 *   - (для этого используется mutex, который лочит переменную global_min_index_, которая и будет хранить минимальный индекс)
 *   - каждый поток возвращает локальный минимальный индекс через FutureResult, либо же проставляет флажок exited_early,
 *         сигнализирующий о том, что поток завершился досрочно, так как дальше не имеет смысла его выполнять
 *   - зная индекс - получаем сам элемент (если такой существует)
 */
template<typename T>
class ParallelSolver : public BaseSolver<T> {
public:
    /**
     * Конструктор
     * @param num_threads количество потоков (опционально, если не указано - используется значение hardware_concurrency)
     */
    explicit ParallelSolver(std::optional<int> num_threads = std::nullopt) {
        if (num_threads.has_value()) {
            num_threads_ = num_threads.value();
        } else {
            auto hc = std::thread::hardware_concurrency();
            if (hc == 0) {
                num_threads_ = 2; // fallback на случай, если hardware_concurrency не работает
            } else {
                num_threads_ = hc;
            }
        }
    }

private:
    struct FutureResult {
        // Локальный индекс элемента, большего порога
        // или std::nullopt, если элемент не найден
        std::optional<std::size_t> index = std::nullopt;
        // Флаг, сигнализирующий о досрочном завершении потока
        // Если true, то поток завершился досрочно, и результат не имеет смысла
        // Досрочное завершение - когда понятно, что другой поток уже нашёл элемент с меньшим индексом
        bool exited_early = false;
    };

public:
    std::optional<T> solve(const std::vector<T>& arr, T threshold) override {
        if (arr.empty()) {
            return std::nullopt;
        }
        
        int actual_threads = std::min(num_threads_, static_cast<int>(arr.size()));
        
        std::vector<std::thread> threads;
        std::vector<std::future<FutureResult>> futures;
        threads.reserve(actual_threads);
        futures.reserve(actual_threads);
        
        // Размер части для каждого потока
        std::size_t chunk_size = arr.size() / actual_threads;
        std::size_t remainder = arr.size() % actual_threads;
        
        std::size_t current_start = 0;
        // Создаём и запускаем потоки
        for (int i = 0; i < actual_threads; i++) {
            std::size_t current_chunk_size = chunk_size + (i < static_cast<int>(remainder) ? 1 : 0);
            std::size_t current_end = current_start + current_chunk_size;
            
            std::promise<FutureResult> promise;
            futures.push_back(promise.get_future());
            
            threads.emplace_back(
                &ParallelSolver::worker_thread,
                this,
                std::cref(arr),
                threshold,
                current_start,
                current_end,
                std::move(promise)
            );
            
            current_start = current_end;
        }
        
        // Ждём завершения всех потоков
        for (auto& thread : threads) {
            thread.join();
        }
        
        // Получаем результаты через фьючи (для демонстрации использования future/promise)
        // В данном случае индекс уже есть в global_min_index_, ради которой лочится мьютекс
        // Так что в демонстрационных целях печатаем, был ли поток досрочно завершен
        // Придумать какое-то другое красивое решение и с фьючами, и с мьютексом - у меня не получилось
        // Потому что выглядит так, что на самом деле достаточно чего-то одного:
        // 1) Либо фьючи возвращают свои результаты, а дальше по сути делаем редукцию по минимуму из индексов
        // 2) Либо мьютекс, который лочит переменную global_min_index_, которая и будет хранить минимальный индекс
        // Так что полученное решение - чисто для демонстрации совместного использования фьючей/промисов и мьютекса
        for (size_t i = 0; i < futures.size(); i++) {
            auto result = futures[i].get();
            if (result.exited_early) {
                std::cerr << "Поток " << i << " завершился досрочно" << std::endl;
            }
        }
        
        // Элемент, больший порога, не найден
        if (!global_min_index_.has_value()) {
            return std::nullopt;
        }
        
        return arr[global_min_index_.value()];
    }
    
    std::string get_name() const override {
        return "Параллельная версия (std::thread, " + std::to_string(num_threads_) + " потоков)";
    }

private:
    /**
     * Функция, выполняемая каждым потоком
     * 
     * @param arr ссылка на массив данных
     * @param threshold пороговое значение
     * @param start_idx начальный индекс для обработки этим потоком
     * @param end_idx конечный индекс (не включительно)
     * @param result_promise promise для возврата локального минимального индекса
     */
    void worker_thread(
        const std::vector<T>& arr,
        T threshold,
        std::size_t start_idx,
        std::size_t end_idx,
        std::promise<FutureResult>&& result_promise
    ) {
        std::optional<std::size_t> local_min_index = std::nullopt;
        bool exited_early = false;
        
        // Поиск минимального индекса в своей части массива
        for (std::size_t i = start_idx; i < end_idx; i++) {
            // Если другой поток уже нашёл элемент с меньшим индексом, то завершаем свою работу досрочно
            {
                std::lock_guard<std::mutex> lock(mutex_);
                if (global_min_index_.has_value() && global_min_index_.value() < i) {
                    exited_early = true;
                    break;
                }
            }
            if (arr[i] > threshold) {
                local_min_index = i;
                break; // Нашли первый элемент в своей части - можно выходить
            }
        }

        // Досрочное завершение
        // Единственная причина не делать этого раньше в цикле - чтобы поскорее отпустить мьютекс
        if (exited_early) {
            result_promise.set_value(FutureResult{std::nullopt, true});
            return;
        }

        // Элемент не найден - обновлять глобальный индекс не требуется
        if (!local_min_index.has_value()) {
            result_promise.set_value(FutureResult{std::nullopt, false});
            return;
        }
        
        // Элемент найден - может потребоваться обновление глобального индекса (под мьютекстом!)
        {
            std::lock_guard<std::mutex> lock(mutex_);
            if (!global_min_index_.has_value() || global_min_index_.value() > local_min_index.value()) {
                global_min_index_ = local_min_index.value();
            }
        }
        
        // Возвращаем локальный результат через promise
        result_promise.set_value(FutureResult{local_min_index, false});
    }

private:
    int num_threads_;
    std::mutex mutex_ = {};
    std::optional<std::size_t> global_min_index_ = std::nullopt;
};

