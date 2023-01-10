#ifndef FIGCONE_TREE_STREAMPOSITION_H
#define FIGCONE_TREE_STREAMPOSITION_H

#include <optional>
#include <sstream>
#include <string>

namespace figcone {

struct StreamPosition {
    std::optional<int> line;
    std::optional<int> column;
};

inline std::string streamPositionToString(const StreamPosition& pos)
{
    auto ss = std::stringstream{};
    if (pos.line) {
        ss << "[line:" << *pos.line;
        if (pos.column)
            ss << ", column:" << *pos.column;
        ss << "] ";
    }
    return ss.str();
}

} //namespace figcone

#endif //FIGCONE_TREE_STREAMPOSITION_H
