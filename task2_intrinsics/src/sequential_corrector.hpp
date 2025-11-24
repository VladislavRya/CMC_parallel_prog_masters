#pragma once

#include "base_color_corrector.hpp"

// Sequential color corrector
// Uses simple sequential processing to apply color correction
class SequentialCorrector : public BaseColorCorrector {
public:
    void apply(const Image& input, Image& output, float red_mult, float green_mult, float blue_mult) override;
    std::string get_name() const override;
};
