#include "image.hpp"

#include <cassert>
#include <cstdlib>
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../include/stb_image_write.h"

Image::Image(int w, int h, int c) : width(w), height(h), channels(c) {
    assert(w > 0 && h > 0 && c == 3);
    // allocate aligned memory for AVX
    size_t total_size = w * h * c;
    size_t aligned_size = ((total_size * sizeof(float) + 31) / 32) * 32;
    data = (float*)aligned_alloc(32, aligned_size);
    
    if (!data) {
        std::cerr << "Error: failed to allocate aligned memory for image" << std::endl;
        throw std::bad_alloc();
    }
}

Image::~Image() {
    free(data);
}

Image::Image(Image&& other) noexcept
    : data(other.data), width(other.width), height(other.height), channels(other.channels) {
    other.data = nullptr;
    other.width = 0;
    other.height = 0;
    other.channels = 0;
}

Image& Image::operator=(Image&& other) noexcept {
    if (this != &other) {
        free(data);
        
        data = other.data;
        width = other.width;
        height = other.height;
        channels = other.channels;
        
        other.data = nullptr;
        other.width = 0;
        other.height = 0;
        other.channels = 0;
    }
    return *this;
}

int Image::size() const {
    return width * height * channels;
}

std::unique_ptr<Image> load_image(const std::string& filename) {
    int width, height, channels;
    unsigned char* img_data = stbi_load(filename.c_str(), &width, &height, &channels, 3);
    
    if (!img_data) {
        std::cerr << "Error: failed to load image " << filename << std::endl;
        std::cerr << "Reason: " << stbi_failure_reason() << std::endl;
        return nullptr;
    }
    
    std::cout << "Loaded image: " << width << "x" << height << " (" << channels << " channels)" << std::endl;
    
    // convert to float (0.0 - 1.0)
    auto img = std::make_unique<Image>(width, height, 3);
    for (int i = 0; i < img->size(); i++) {
        img->data[i] = img_data[i] / 255.0f;
    }
    
    stbi_image_free(img_data);
    return img;
}

bool save_image(const std::string& filename, const Image& img) {
    unsigned char* img_data = new unsigned char[img.size()];
    
    for (int i = 0; i < img.size(); i++) {
        // clamp values to [0, 1] and convert to [0, 255]
        float val = img.data[i];
        if (val < 0.0f) val = 0.0f;
        if (val > 1.0f) val = 1.0f;
        img_data[i] = static_cast<unsigned char>(val * 255.0f);
    }
    
    int result = stbi_write_jpg(filename.c_str(), img.width, img.height, img.channels, img_data, 95);
    
    delete[] img_data;
    
    if (!result) {
        std::cerr << "Error: failed to save " << filename << std::endl;
        return false;
    }
    
    std::cout << "Saved: " << filename << std::endl;
    return true;
}

