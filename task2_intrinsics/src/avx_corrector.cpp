#include "avx_corrector.hpp"
#include <immintrin.h>

void AVXCorrector::apply(const Image& input, Image& output, float red_mult, float green_mult, float blue_mult) {
    const int size = input.size();
    
    // create vector of coefficients for 8 values
    // pattern: R G B R G B R G (and so on)
    __m256 color_multipliers = _mm256_setr_ps(
        red_mult, green_mult, blue_mult,      // pixel 1
        red_mult, green_mult, blue_mult,      // pixel 2
        red_mult, green_mult                  // pixel 3 (partially)
    );
    
    __m256 color_multipliers_shift = _mm256_setr_ps(
        blue_mult,                            // pixel 3 (remainder)
        red_mult, green_mult, blue_mult,      // pixel 4
        red_mult, green_mult, blue_mult,      // pixel 5
        red_mult                              // pixel 6 (partially)
    );
    
    __m256 color_multipliers_shift2 = _mm256_setr_ps(
        green_mult, blue_mult,                // pixel 6 (remainder)
        red_mult, green_mult, blue_mult,      // pixel 7
        red_mult, green_mult, blue_mult       // pixel 8
    );
    
    int i = 0;
    // process blocks of 24 values (8 full pixels)
    for (; i <= size - 24; i += 24) {
        __m256 pixels1 = _mm256_load_ps(&input.data[i]);
        __m256 pixels2 = _mm256_load_ps(&input.data[i + 8]);
        __m256 pixels3 = _mm256_load_ps(&input.data[i + 16]);

        __m256 result1 = _mm256_mul_ps(pixels1, color_multipliers);
        __m256 result2 = _mm256_mul_ps(pixels2, color_multipliers_shift);
        __m256 result3 = _mm256_mul_ps(pixels3, color_multipliers_shift2);

        _mm256_store_ps(&output.data[i], result1);
        _mm256_store_ps(&output.data[i + 8], result2);
        _mm256_store_ps(&output.data[i + 16], result3);
    }
    
    // process remainder (if size is not a multiple of 24)
    for (; i < size; i += 3) {
        output.data[i + 0] = input.data[i + 0] * red_mult;    // R
        output.data[i + 1] = input.data[i + 1] * green_mult;  // G
        output.data[i + 2] = input.data[i + 2] * blue_mult;   // B
    }
}

std::string AVXCorrector::get_name() const {
    return "avx";
}
