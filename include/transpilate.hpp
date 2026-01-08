#pragma once
#include <string>
#include <sstream>
#include <regex>
#include <unordered_set>
#include <map>
#include <iostream>

namespace verify {
    bool types(const std::string& code, const std::string& file);
}

namespace translate {
    std::string map(const std::string& code, const std::map<std::string, std::string>& map);

    // Diccionario de tipos para transpilar a Julia
    inline std::map<std::string, std::string> dict = {
        {"R", "Float64"},
        {"Z", "Int64"},
        {"str", "String"},
        {"Array(\\d+)d\\{(.*?)\\}", "Array{$2, $1}"},
        {"input\\((.*)\\)", "(prompt -> (print(prompt); readline()))($1)"}
    };
}
