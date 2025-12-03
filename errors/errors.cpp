// errors.cpp
#include "errors.h"
#include <iostream>

void errors_detection(Error error) {
    switch (error) {
        case Error::FILE_NOT_FOUND:
            std::cerr << "Error: File not found" << std::endl;
            break;
        case Error::INVALID_INPUT:
            std::cerr << "Error: Invalid input" << std::endl;
            break;
        case Error::DICTIONARY_ERROR:
            std::cerr << "Error: Dictionary operation failed" << std::endl;
            break;
    }
}