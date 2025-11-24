#include <iostream>
#include <chrono>
#include <memory>
#include "image.hpp"
#include "base_color_corrector.hpp"
#include "sequential_corrector.hpp"
#include "avx_corrector.hpp"

/**
 * Extract filename without extension from a path.
 * 
 * @param path Full file path
 * @return Filename without path and extension
 */
std::string get_filename_without_ext(const std::string& path) {
    // find last path separator
    size_t last_slash = path.find_last_of("/\\");
    size_t start = (last_slash == std::string::npos) ? 0 : last_slash + 1;
    
    // find extension
    size_t last_dot = path.find_last_of('.');
    size_t end = (last_dot == std::string::npos || last_dot < start) ? path.length() : last_dot;
    
    return path.substr(start, end - start);
}

/**
 * Test a color corrector implementation.
 * 
 * @param corrector  Color corrector to test
 * @param input      Input image
 * @param red_mult   Red channel multiplier
 * @param green_mult Green channel multiplier
 * @param blue_mult  Blue channel multiplier
 * @param input_name Name of the input image
 */
void test_corrector(
    BaseColorCorrector& corrector, 
    const Image& input,
    float red_mult,
    float green_mult,
    float blue_mult,
    const std::string& input_name
) {
    std::cout << "\n--- Processing: " << corrector.get_name() << " ---" << std::endl;

    Image output(input.width, input.height, input.channels);

    auto start = std::chrono::high_resolution_clock::now();
    corrector.apply(input, output, red_mult, green_mult, blue_mult);
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    
    std::cout << "Color correction time: " << duration << " milliseconds" << std::endl;
    
    std::string output_filename = "images_output/" + input_name + "_" + corrector.get_name() + ".jpg";
    save_image(output_filename, output);
}

int main(int argc, char* argv[]) {
    std::cout << "\n========================================" << std::endl;
    // color correction parameters for "warm sunset" effect
    const float RED_MULTIPLIER   = 1.25f;  // boost red (+25%)
    const float GREEN_MULTIPLIER = 1.05f;  // slightly boost green (+5%)
    const float BLUE_MULTIPLIER  = 0.75f;  // reduce blue (-25%)
    
    std::cout << "Color correction coefficients (warm sunset):" << std::endl;
    std::cout << "  Red:   × " << RED_MULTIPLIER << std::endl;
    std::cout << "  Green: × " << GREEN_MULTIPLIER << std::endl;
    std::cout << "  Blue:  × " << BLUE_MULTIPLIER << std::endl;
    std::cout << std::endl;
    
    // load image
    const char* input_filename = (argc > 1) ? argv[1] : "images_input/sunset.jpg";
    auto input = load_image(input_filename);
    
    if (!input) {
        std::cerr << "Error: failed to load image" << std::endl;
        return 1;
    }
    
    std::string input_name = get_filename_without_ext(input_filename);

    // test sequential implementation
    SequentialCorrector seq_corrector;
    test_corrector(
        seq_corrector, *input, 
        RED_MULTIPLIER, GREEN_MULTIPLIER, BLUE_MULTIPLIER,
        input_name
    );
    // test AVX implementation
    AVXCorrector avx_corrector;
    test_corrector(
        avx_corrector, *input,
        RED_MULTIPLIER, GREEN_MULTIPLIER, BLUE_MULTIPLIER,
        input_name
    );

    std::cout << "\n========================================" << std::endl;
    std::cout << "✓ Done! Check the images_output/ folder" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}
