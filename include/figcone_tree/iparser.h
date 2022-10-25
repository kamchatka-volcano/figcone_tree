#ifndef FIGCONE_TREE_IPARSER_H
#define FIGCONE_TREE_IPARSER_H

#include "tree.h"
#include <istream>

namespace figcone {
class IParser{
public:
    virtual ~IParser() = default;
    virtual TreeNode parse(std::istream& stream) = 0;
};

}

#endif //FIGCONE_TREE_IPARSER_H
