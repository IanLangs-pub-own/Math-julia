#include "transpilate.hpp"

namespace verify {

bool types(const std::string& code, const std::string& file) {
    std::unordered_set<std::string> declared;

    // Solo asignación simple '=' (excluye +=, ==, etc.)
    std::regex assign(
        R"(^\s*(?!.*(==|!=|<=|>=|=>|\+=|-=|\*=|/=|%=))(.+?)\s*=\s*(.+)$)"
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
                if (line.find("#no-strict") != std::string::npos) {
                    declared.insert(var);
                    continue;
                }
                if (!has_type) {
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

} // namespace translate
