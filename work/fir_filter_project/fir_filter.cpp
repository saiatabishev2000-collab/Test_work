#include <iostream>
#include <random>
#include <chrono>
#include <immintrin.h>
#include <cstdlib>
#include <cmath>

// Базовая скалярная реализация КИХ-фильтра
void fir_scalar(const float* x, const float* h, float* y, size_t N, size_t filter_length) {
    for (size_t n = filter_length - 1; n < N; n++) {
        float sum = 0.0f;
        for (size_t k = 0; k < filter_length; k++) {
            sum += h[k] * x[n - k];
        }
        y[n] = sum;
    }
}

// Горизонтальное суммирование 8 элементов в AVX-регистре
inline float horizontal_sum_avx(__m256 v) {
    __m128 v_low = _mm256_extractf128_ps(v, 0);
    __m128 v_high = _mm256_extractf128_ps(v, 1);
    v_low = _mm_add_ps(v_low, v_high);
    __m128 shuf = _mm_movehdup_ps(v_low);
    __m128 sums = _mm_add_ps(v_low, shuf);
    shuf = _mm_movehl_ps(shuf, sums);
    sums = _mm_add_ss(sums, shuf);
    return _mm_cvtss_f32(sums);
}

// Оптимизированная SIMD (AVX2) реализация КИХ-фильтра
void fir_simd(const float* x, const float* h, float* y, size_t N, size_t filter_length) {
    size_t aligned_filter_length = (filter_length + 7) & ~7;
    float* h_simd = static_cast<float*>(std::aligned_alloc(32, aligned_filter_length * sizeof(float)));
    if (!h_simd) {
        std::cerr << "Ошибка выделения памяти" << std::endl;
        return;
    }

    // Подготовка коэффициентов: перестановка внутри блоков по 8 элементов
    for (size_t i = 0; i < aligned_filter_length; i++) {
        if (i < filter_length) {
            size_t block = i / 8;
            size_t offset = i % 8;
            h_simd[block * 8 + (7 - offset)] = h[i];
        } else {
            h_simd[i] = 0.0f;
        }
    }

    // Обработка сигнала
    for (size_t n = filter_length - 1; n < N; n++) {
        __m256 acc = _mm256_setzero_ps();
        for (size_t k = 0; k < aligned_filter_length; k += 8) {
            __m256 coef = _mm256_load_ps(h_simd + k);
            __m256 data = _mm256_loadu_ps(x + n - k - 7);
            acc = _mm256_add_ps(acc, _mm256_mul_ps(coef, data));
        }
        y[n] = horizontal_sum_avx(acc);
    }

    std::free(h_simd);
}

int main() {
    const size_t L = 10000000; // Длина сигнала
    float* x = new float[L];
    float* y_scalar = new float[L]();
    float* y_simd = new float[L]();

    // Генерация случайного сигнала
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    for (size_t i = 0; i < L; i++) {
        x[i] = dis(gen);
    }

    // Длины фильтров для тестирования
    std::vector<size_t> M_list = {8, 16, 32, 64, 128, 256};
    std::cout << "Длина сигнала: " << L << " отсчетов\n";
    std::cout << "Тестируемые длины фильтров: ";
    for (auto M : M_list) std::cout << M << " ";
    std::cout << "\n\n";

    for (size_t M : M_list) {
        float* h = new float[M];
        for (size_t i = 0; i < M; i++) {
            h[i] = 1.0f / M; // Импульсная характеристика
        }

        // Замер времени для скалярной реализации
        auto start = std::chrono::high_resolution_clock::now();
        fir_scalar(x, h, y_scalar, L, M);
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end - start;
        double t_scalar = elapsed.count();

        // Замер времени для SIMD реализации
        start = std::chrono::high_resolution_clock::now();
        fir_simd(x, h, y_simd, L, M);
        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;
        double t_simd = elapsed.count();

        // Проверка корректности
        bool correct = true;
        for (size_t i = M - 1; i < L; i++) {
            if (std::abs(y_scalar[i] - y_simd[i]) > 1e-5f) {
                correct = false;
                break;
            }
        }

        // Вывод результатов
        std::cout << "Длина фильтра: " << M << "\n";
        std::cout << "  Скалярная версия: " << t_scalar << " с\n";
        std::cout << "  SIMD версия: " << t_simd << " с\n";
        std::cout << "  Ускорение: " << t_scalar / t_simd << "x\n";
        std::cout << "  Корректность: " << (correct ? "да" : "нет") << "\n\n";

        delete[] h;
    }

    delete[] x;
    delete[] y_scalar;
    delete[] y_simd;

    return 0;
}
