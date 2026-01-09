#include "transpilate.hpp"

std::string regex_replace_fn(
    const std::string& input,
    const std::regex& re,
    std::function<std::string(const std::smatch&)> fn
) {
    std::string result;
    std::sregex_iterator it(input.begin(), input.end(), re);
    std::sregex_iterator end;

    size_t last = 0;

    for (; it != end; ++it) {
        result += input.substr(last, it->position() - last);
        result += fn(*it);
        last = it->position() + it->length();
    }

    result += input.substr(last);
    return result;
}


namespace verify {

bool types(const std::string& code, const std::string& file) {
    std::unordered_set<std::string> declared;

    // Solo asignación simple '=' (excluye +=, ==, etc.)
    std::regex assign(
        R"(^\s*(?!.*(==|!=|<=|>=|=>|\+=|-=|\*=|/=|%=))(.+?)\s*:?=\s*(.+)$)"
    );

    // Identificador válido
    std::regex ident(R"([A-Za-z_]\w*)");

    std::istringstream lines(code);
    std::string line;
    int lineno = 0;

    while (std::getline(lines, line)) {
        lineno++;

        std::smatch m;
        if (!std::regex_match(line, m, assign))
            continue;

        std::string lhs = m[2].str();
        bool has_type = lhs.find("::") != std::string::npos;

        // Buscar todos los identificadores en el LHS
        auto it = std::sregex_iterator(lhs.begin(), lhs.end(), ident);
        auto end = std::sregex_iterator();

        for (; it != end; ++it) {
            std::string var = it->str();

            // Ignorar palabras clave simples
            if (var == "const" || var == "let")
                continue;

            // Si no estaba declarado antes, debe tener tipo
            if (declared.count(var) == 0) {
                if ((!has_type) && (line.find("#no-strict") == std::string::npos && line.find(":=") == std::string::npos)) {

                    std::cerr
                        << "Type error on line " << lineno
                        << " in file " << file
                        << ": variable '" << var
                        << "' declared without type\n";
                    return false;
                }
                declared.insert(var);
            }
        }
    }

    return true;
}

} // namespace verify

namespace translate {

std::string map(const std::string& code, const std::map<std::string, std::string>& map) {
    std::string result = code;
    for (auto [k, v] : map) {
        result = std::regex_replace(result, std::regex(k), v);
    }
    return result;
}

std::string formatStrings(const std::string& code) {
    std::string result = code;
    std::regex pattern("f\"((?:\\\\.|[^\"\\\\])*)\"");
    result = regex_replace_fn(result, pattern, [](const std::smatch& m) {
    std::string content = m[1].str();

    content = std::regex_replace(content, std::regex(R"(\{\{)"), "____LBRACE____");
    content = std::regex_replace(content, std::regex(R"(\}\})"), "____RBRACE____");

    content = std::regex_replace(content, std::regex(R"(\{([^{}]+)\})"), R"($($1))");

    content = std::regex_replace(content, std::regex("____LBRACE____"), "{");
    content = std::regex_replace(content, std::regex("____RBRACE____"), "}");

    return "\"" + content + "\"";
});
    return result;
}

// Función helper que evalúa un valor a booleano en tiempo de transpile
bool evalToBoolean(const std::string& literal) {
    std::string s = literal;

    // trim
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
        [](unsigned char ch){ return !std::isspace(ch); }));
    s.erase(std::find_if(s.rbegin(), s.rend(),
        [](unsigned char ch){ return !std::isspace(ch); }).base(), s.end());

    // lowercase
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    // bool literal
    if (s == "true") return true;
    if (s == "false") return false;

    // número
    try {
        double num = std::stod(s);
        return num != 0;
    } catch(...) {}

    // string entre comillas
    if ((s.front() == '"' && s.back() == '"') ||
        (s.front() == '\'' && s.back() == '\'')) {
        std::string content = s.substr(1, s.size() - 2);
        // trim
        content.erase(content.begin(), std::find_if(content.begin(), content.end(),
            [](unsigned char ch){ return !std::isspace(ch); }));
        content.erase(std::find_if(content.rbegin(), content.rend(),
            [](unsigned char ch){ return !std::isspace(ch); }).base(), content.end());
        return !content.empty();
    }

    // por defecto true
    return !s.empty();
}

// Función compatible con regex_replace_fn
std::string toBooleanReplace(const std::smatch& m) {
    std::string arg = m[1].str();
    bool val = evalToBoolean(arg);
    return val ? "true" : "false";
}

} // namespace translate
