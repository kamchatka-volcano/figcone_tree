#ifndef FIGCONE_TREE_STRINGCONVERTER_H
#define FIGCONE_TREE_STRINGCONVERTER_H

#include "errors.h"
#include "detail/external/sfun/type_traits.h"
#include <optional>
#include <sstream>
#include <string>

namespace figcone {

template<typename T>
struct StringConverter {
    static std::optional<T> fromString(const std::string& data)
    {
        [[maybe_unused]] auto setValue = [](auto& value, const std::string& data) -> std::optional<T>
        {
            auto stream = std::stringstream{data};
            stream >> value;

            if (stream.bad() || stream.fail() || !stream.eof())
                return {};
            return value;
        };

        if constexpr (std::is_convertible_v<std::string, tree::sfun::remove_optional_t<T>>) {
            return data;
        }
        else if constexpr (tree::sfun::is_optional_v<T>) {
            auto value = T{};
            value.emplace();
            return setValue(*value, data);
        }
        else {
            auto value = T{};
            return setValue(value, data);
        }
    }
};

} //namespace figcone

#endif //FIGCONE_TREE_STRINGCONVERTER_H
