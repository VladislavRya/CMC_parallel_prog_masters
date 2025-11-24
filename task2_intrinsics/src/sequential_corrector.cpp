#include "sequential_corrector.hpp"

void SequentialCorrector::apply(const Image& input, Image& output, float red_mult, float green_mult, float blue_mult) {
    const int size = input.size();
    
    for (int i = 0; i < size; i += 3) {
        output.data[i + 0] = input.data[i + 0] * red_mult;    // R
        output.data[i + 1] = input.data[i + 1] * green_mult;  // G
        output.data[i + 2] = input.data[i + 2] * blue_mult;   // B
    }
}

std::string SequentialCorrector::get_name() const {
    return "sequential";
}
