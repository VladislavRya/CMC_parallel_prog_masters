#pragma once

#include "base_color_corrector.hpp"

// AVX color corrector
// Uses AVX instructions to process image data faster
class AVXCorrector : public BaseColorCorrector {
public:
    void apply(const Image& input, Image& output, float red_mult, float green_mult, float blue_mult) override;
    std::string get_name() const override;
};
