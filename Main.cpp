#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <iomanip> // For formatted output
#include <algorithm> // For toLowerCase
#include <unordered_map>
#include <stdexcept>
#include <bitset>
#include <sstream>
#include "lexical.h"
#include "first_view.h"
#include "error_handler.h"
#include "second_view.h"

SizeActiveSeg values = { 0, 0, 0 , "" };
std::vector<LineInfo> lines_info;

// Trim leading whitespace
std::string trimLeft(const std::string& str) {
    size_t start = str.find_first_not_of(" \t");
    return (start == std::string::npos) ? "" : str.substr(start);
}

int main() {
    std::string input_file_name = "test1.asm";
    std::string output_file_name = "analysis.lst";

    std::ifstream inputFile(input_file_name);
    if (!inputFile) {
        std::cerr << "Error: failed to open file " << input_file_name << '\n';
        return 2;
    }

    std::ofstream outputFile(output_file_name);
    if (!outputFile) {
        std::cerr << "Error: failed to create file " << output_file_name << '\n';
        return 1;
    }

    std::string line;
    int rowNumber = 1;
    values.line = rowNumber;

    // First pass
    while (std::getline(inputFile, line)) {
        std::string trimmedLine = trimLeft(line);
        if (!trimmedLine.empty()) {
            int last_val = values.local_size;
            lines_info.push_back({ values.line , values.seg, static_cast<uint16_t>(values.local_size), static_cast<uint8_t>(values.local_size - last_val), {} });
            std::vector<Lexeme> lexemes = LexicalAnalyzer(trimmedLine);
            std::vector<Type> Type_Lexeme = ConvertLexemesToVector(lexemes);
            First_View(lexemes, Type_Lexeme, values);
        }
        values.line++;
        rowNumber++;
    }

    // Second pass
    inputFile.clear();
    inputFile.seekg(0, std::ios::beg);
    rowNumber = 1;
    int line_code = 0;
    while (std::getline(inputFile, line)) {
        std::string trimmedLine = trimLeft(line);

        outputFile << std::setw(3) << std::setfill(' ') << std::dec << rowNumber << "  ";
        if (trimmedLine.empty()) {
            outputFile << "\n";
        }
        else {
            std::vector<Lexeme> lexemes = LexicalAnalyzer(trimmedLine);
            std::vector<Type> Type_Lexeme = ConvertLexemesToVector(lexemes);
            SecondView(lexemes, Type_Lexeme, values, lines_info[line_code]);
            outputFile << std::hex << std::uppercase
                << std::setw(8) << std::setfill('0') << lines_info[line_code].offset
                << "   ";
            const auto& bytes = lines_info[line_code].bytes;
            for (auto b : bytes) {
                outputFile << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)b << " ";
            }
            int bytesFieldWidth = 3 * 8;
            int currentWidth = 3 * (int)bytes.size();
            if (currentWidth < bytesFieldWidth) {
                outputFile << std::string(bytesFieldWidth - currentWidth, ' ');
            }
            outputFile << trimmedLine << "\n";

            line_code++;
        }
        rowNumber++;
    }

    // If errors exist, print them
    if (GlobalErrorHandler.HaveError()) {
        outputFile << "\n\n";
        GlobalErrorHandler.print(outputFile);
    }

    // Identifier table
    outputFile << std::setfill(' ');
    outputFile << "\n\nIdentifier Table:\n";
    outputFile << std::setw(20) << "Name" << std::setw(20) << "Type" << std::setw(15) << "Segment" << std::setw(10) << "Offset" << "\n";
    outputFile << std::string(50, '-') << "\n\n";

    for (const auto& idName : vector_identfiers) {
        const IdentifierData& data = identifier_table[idName];
        std::string typeStr;
        switch (data.type) {
        case IdentifierType::kConst: typeStr = "CONST"; break;
        case IdentifierType::kLabel: typeStr = "LABEL"; break;
        case IdentifierType::kNameSeg: typeStr = "NAME_SEG"; break;
        default: typeStr = "UNKNOWN"; break;
        }
        outputFile << std::setw(20) << idName
            << std::setw(20) << typeStr
            << std::setw(15) << data.segment
            << std::setw(10) << std::hex << std::uppercase << data.offset
            << std::dec << "\n";  // Offset in hex, then switch back to decimal
    }

    // Segment table
    outputFile << "\n\nSegment Table:\n";
    outputFile << std::setw(20) << "Name" << std::setw(30) << "Size" << std::setw(30) << "Offset" << "\n";
    outputFile << std::string(50, '-') << "\n";
    for (const auto& id : segment_table) {
        outputFile << std::dec << std::setw(20) << id.name << std::setw(30) << id.defaultSize << std::setw(30) << std::hex << id.current_offset << "\n";
    }

    // Segment register table
    outputFile << "\n\n\nSegment Register Table:\n";
    outputFile << std::setw(20) << "Segment Register" << std::setw(30) << "Segment" << "\n";
    outputFile << std::string(50, '-') << "\n";
    for (const auto& id : segment_regist) {
        outputFile << std::setw(20) << id.regis << std::setw(30) << id.target << "\n";
    }

    inputFile.close();
    outputFile.close();

    std::cout << "Analysis complete! Results saved to file: " << output_file_name << '\n';
    return 0;
}



