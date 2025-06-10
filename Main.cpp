#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <iomanip> // Для форматування виводу
#include <algorithm> // Для toLowerCase
#include <unordered_map>
#include <stdexcept>
#include <bitset>
#include <sstream>
#include "Lexical.h"
#include "first_view.h"
#include "Second_view.h"
std::string trimLeft(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    return (start == std::string::npos) ? "" : str.substr(start);
}

int main() {
    std::string inputFileName = "test1.asm";
    std::string outputFileName = "analysis.txt";

    std::ifstream inputFile(inputFileName);
    if (!inputFile) {
        std::cerr << "Помилка: не вдалося відкрити файл " << inputFileName << '\n';
        return 2;
    }

    std::ofstream outputFile(outputFileName);
    if (!outputFile) {
        std::cerr << "Помилка: не вдалося створити файл " << outputFileName << '\n';
        return 1;
    }

    std::string line;
    int rowNumber = 1;

    while (std::getline(inputFile, line)) {
        // Ігноруємо пусті рядки
        if (line.empty()) {
            continue;
        }
        line = trimLeft(line);

        // Запис рядка програми
        //outputFile << "Рядок програми: \"" << line << "\"\n";

        //Лексичний аналіз рядка
        /*outputFile << std::setw(3) << std::setfill('0') << rowNumber << "  "
           << std::hex << std::uppercase 
            << std::setw(8) << std::setfill('0') << global_size  
            << "   " << line << "    " << error << "\n";*/
        error = "";
        //reserv = global_size;
        std::vector<Lexeme> lexemes = lexicalAnalyzer(line);

        createIdentifierTable(lexemes);
        // чи є першою лексемою assume
        if (lexemes[0].lexeme == "assume") {
            FillSegmentTable(lexemes);
        }
        else if (Active_seg == 1) {
            if (already == 1) {
                already = 0;
            }
            else {
                CalculateCommandMemory(lexemes);
            }
        }
        generateStructureTable(outputFile, lexemes, rowNumber);
        rowNumber++;

    }

    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);
    global_size = 0;
    local_size = 0;
    rowNumber = 1;
    int vs = 0;
    while (std::getline(inputFile, line)) {
        if (line.empty()) {
            continue;
        }
        //std::cout << global_size << std::endl;
        line = trimLeft(line);
        std::vector<Lexeme> lexemes = lexicalAnalyzer(line);
        CountOffset(lexemes);

        outputFile <<  std::setw(3) << std::setfill('0') << std::right << rowNumber << "  "
           << std::hex << std::uppercase 
           << std::setw(8) << std::setfill('0') << vs << std::dec  
           << "   " << std::setw(20) << std::setfill(' ') << std::left << result_value << "   " << std::setw(20) << line << "   " << error << "\n";
        vs = global_size;
        reserv = global_size;
        rowNumber++;
        result_value = "";
        result = "";
        command_adrress = "";
        error = "";

    }

    /*outputFile << "Таблиця індентифікаторів:\n";
    outputFile << std::setw(20) << "Ім'я" << std::setw(30) << "Тип" << "\n";
    outputFile << std::string(50, '-') << "\n\n";

    for (const auto& id : identifierTable) {
        outputFile << std::setfill(' ') << std::setw(20) << id.name << std::setw(30) << id.type << std::setw(30) << id.segment << std::setw(30) << id.offset << "\n";
    }

    outputFile << "\n\nТаблиця сегментів:\n";
    outputFile << std::setw(20) << "Ім'я" << std::setw(30) << "Розмір" << std::setw(30) << "Зміщення" << "\n";
    outputFile << std::string(50, '-') << "\n";
    for (const auto& id : segmentTable) {
        outputFile << std::setw(20) << id.name << std::setw(30) << id.defaultSize << std::setw(30) << id.currentOffset << "\n";
    }

    outputFile << "\n\n\nТаблиця сегментних регістрів:\n";
    outputFile << std::setw(20) << "Сегменттний регістр" << std::setw(30) << "Сегмент" << "\n";
    outputFile << std::string(50, '-') << "\n";
    for (const auto& id : SegmentRegist) {
        outputFile << std::setw(20) << id.regis << std::setw(30) << id.target << "\n";
    }*/

    inputFile.close();
    outputFile.close();

    std::cout << "Аналіз завершено! Результати збережені у файл: " << outputFileName << '\n';
    return 0;
}




