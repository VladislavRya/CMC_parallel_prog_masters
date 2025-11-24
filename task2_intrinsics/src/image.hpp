#pragma once

#include <string>
#include <memory>

class Image {
public:
    float* data;
    int width;
    int height;
    int channels;
    
    Image(int w, int h, int c);
    ~Image();
    
    // disable copy (because of raw pointer ownership)
    Image(const Image&) = delete;
    Image& operator=(const Image&) = delete;
    
    // enable move
    Image(Image&& other) noexcept;
    Image& operator=(Image&& other) noexcept;
    
    int size() const;
};

/**
 * Load image from file.
 * Returns nullptr if loading fails.
 * 
 * @param filename Path to image file
 * @return Unique pointer to loaded image, or nullptr on failure
 */
std::unique_ptr<Image> load_image(const std::string& filename);

/**
 * Save image to file.
 * 
 * @param filename Path to output file
 * @param img Image to save
 * @return true if successful, false otherwise
 */
bool save_image(const std::string& filename, const Image& img);
