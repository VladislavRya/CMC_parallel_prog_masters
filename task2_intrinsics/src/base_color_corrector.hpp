#pragma once

#include "image.hpp"

#include <string>

// base abstract class for color correction
class BaseColorCorrector {
public:
    virtual ~BaseColorCorrector() = default;
    
    /**
     * Apply color correction to an image by multiplying each channel by a coefficient.
     * 
     * @param input Input image (read-only)
     * @param output Output image (will be modified)
     * @param red_mult Multiplier for red channel (1.0 = no change)
     * @param green_mult Multiplier for green channel (1.0 = no change)
     * @param blue_mult Multiplier for blue channel (1.0 = no change)
     */
    virtual void apply(const Image& input, Image& output, float red_mult, float green_mult, float blue_mult) = 0;
    
    /**
     * Get the name of the corrector implementation.
     * 
     * @return Name string (e.g., "sequential", "avx")
     */
    virtual std::string get_name() const = 0;
};
