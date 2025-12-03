#pragma once

enum class Error {
    FILE_NOT_FOUND,
    INVALID_INPUT,
    DICTIONARY_ERROR
};

void errors_detection(Error error);