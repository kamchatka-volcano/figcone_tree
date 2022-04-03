#pragma once
#include "tree.h"
#include <istream>

namespace figcone {
class IParser{
public:
    virtual ~IParser() = default;
    virtual TreeNode parse(std::istream& stream) = 0;
};

}