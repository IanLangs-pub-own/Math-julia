#include "transpilate.hpp"
#include "iostream"
#include <vector>
#include <fstream>
#include <sstream>
#include <nlohmann/json.hpp>
#include <regex>
#include <cstdlib>
#include <sstream>
#include <readline/readline.h>
#include <string>
std::vector<std::string> split(const std::string& input) {
    std::vector<std::string> result;
    std::stringstream ss(input);
    std::string word;

    while (ss >> word) {
        result.push_back(word);
    }

    return result;
}

using json = nlohmann::json;

// Función helper para transpilar
bool runTranspile(const std::vector<std::string>& files, bool strict) {
    for (auto& filename : files) {
        // Leer archivo completo
        std::ifstream mjlfile(filename);
        if (!mjlfile.is_open()) {
            std::cerr << "No se pudo abrir el archivo " << filename << '\n';
            return false;
        }
        std::stringstream buffer;
        buffer << mjlfile.rdbuf();
        std::string code = buffer.str();
        mjlfile.close();

        // Verificar tipos
        if (strict) {
            if (!verify::types(code, filename)) return false;
        }

        // Traducir
        code = translate::map(code, translate::dict);
        code = translate::formatStrings(code);
        // Guardar .jl
        std::ofstream jlfile(std::regex_replace(filename, std::regex("\\.mjl$"), ".jl"));
        if (!jlfile.is_open()) {
            std::cerr << "No se pudo crear el archivo " << filename << '\n';
            return false;
        }
        jlfile << code;
    }
    return true;
}

int main(int argc, char** RAWargv) {
    if (argc < 2) {
        std::cerr << "Uso:\n";
        std::cerr << "  " << RAWargv[0] << " init\n";
        std::cerr << "  " << RAWargv[0] << " config.json\n";
        std::cerr << "  " << RAWargv[0] << " config.json julia\n";
        return 1;
    }


    if (std::string(RAWargv[1]) == "init") {
        std::ofstream config("config.json");
        if (!config.is_open()) {
            std::cerr << "No se pudo crear config.json\n";
            return 1;
        }

        std::string input;
        json jsonConfig;

        std::cout << "Typing Strict mode? [Y/n]: ";
        std::cin >> input;
        jsonConfig["strict"] = !(input == "n" || input == "N");

        std::cout << "Main file: ";
        std::cin >> input;
        jsonConfig["Main"] = input;

        std::cout << "Files (space separated): ";
        std::cin.ignore();
        std::getline(std::cin, input);

        std::vector<std::string> files = split(input);
        jsonConfig["Files"] = files;

        config << jsonConfig.dump(4);
        config.close();

        std::cout << "config.json creado.\n";
        return 0;
    }


    std::ifstream jfile(RAWargv[1]);
    if (!jfile.is_open()) {
        std::cerr << "No se pudo abrir el archivo " << RAWargv[1] << '\n';
        return 1;
    }

    json config;
    jfile >> config;
    jfile.close();

    if (!config.contains("Files") || !config["Files"].is_array()) {
        std::cerr << "El JSON debe contener 'Files' como array de strings\n";
        return 1;
    }

    std::vector<std::string> files;
    for (auto& f : config["Files"]) files.push_back(f);

    bool strict = config.value("strict", false);

    if (!runTranspile(files, strict)) return 1;

    // Si argc==3, ejecutar el Main
    if (argc == 3) {
        if (!config.contains("Main") || !config["Main"].is_string()) {
            std::cerr << "El JSON debe contener 'Main' como string\n";
            return 1;
        }
        std::string mainFile = config["Main"];
        std::string jlFile = std::regex_replace(mainFile, std::regex("\\.mjl$"), ".jl");

        std::string command = std::string(RAWargv[2]) + " " + jlFile;
        std::system(command.c_str());
    }

    return 0;
}
