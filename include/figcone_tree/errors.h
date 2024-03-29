#ifndef FIGCONE_TREE_ERRORS_H
#define FIGCONE_TREE_ERRORS_H

#include "streamposition.h"
#include <stdexcept>
#include <string>

namespace figcone {

class Error : public std::runtime_error {
public:
    explicit Error(const std::string& errorMsg)
        : std::runtime_error(errorMsg)
    {
    }
};

class ConfigError : public Error {
public:
    using Error::Error;

    ConfigError(const std::string& errorMsg, const StreamPosition& errorPosition = {})
        : Error(streamPositionToString(errorPosition) + errorMsg)
    {
    }
};

class ValidationError : public Error {
    using Error::Error;
};

} //namespace figcone

#endif //FIGCONE_TREE_ERRORS_H
