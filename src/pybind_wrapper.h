#ifndef PYBIND_WRAPPER_H
#define PYBIND_WRAPPER_H

#include <string>

extern const char* image_processor_script;

std::string process_images(const std::string& image1_base64, const std::string& image2_base64, const std::string& user_prompt);

#endif // PYBIND_WRAPPER_H
