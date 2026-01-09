#pragma once
#include <string>
#include <sstream>
#include <regex>
#include <unordered_set>
#include <map>
#include <iostream>

std::string regex_replace_fn( const std::string& input, const std::regex& re, std::function<std::string(const std::smatch&)> fn);

namespace verify {
    bool types(const std::string& code, const std::string& file);
}

namespace translate {
    std::string map(const std::string& code, const std::map<std::string, std::string>& map);

    // Diccionario de tipos para transpilar a Julia
    inline std::map<std::string, std::string> dict = {
        {"\\bC\\b", "Complex"},
        {"\\bQ\\b", "Rational{Int64}"},
        {"\\bR\\b", "Float64"},
        {"\\bZ\\b", "Int64"},
        {"\\bN\\b", "UInt64"},
        {"Str", "String"},
        {"Array(\\d+)d\\{(.*?)\\}", "Array{$2, $1}"},
        {"input\\((.*)\\)", "(prompt -> (print(prompt); readline()))($1)"},
        {":=", "="},
        {"printf\\((.*)\\)","@printf($1)"},
    };

    std::string formatStrings(const std::string& code);

    bool evalToBoolean(const std::string& literal);

    std::string toBooleanReplace(const std::smatch& m);
}
