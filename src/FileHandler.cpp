#include "FileHandler.h"
#include <vector>
#include <fstream>
#include <iostream>

std::vector<std::string> FileHandler::readFile(const std::string& path){
    std::ifstream ifs{ path };
    std::string line;
    std::vector<std::string> code;

    /// Open the file and process each line.
    if (ifs.is_open()) {
      while (std::getline(ifs, line)) {
        code.push_back(line);
      }
      ifs.close();
    } else {
      std::cerr << "Erro ao abrir o arquivo: " << path << std::endl;
    }

    return code;
  }
