#ifndef LEXICAL_H
#define LEXICAL_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <iomanip> // Для форматування виводу
#include <algorithm> // Для toLowerCase

struct Lexeme {         // структура, яка зберігає
    int number;         // в собі інформацію про 
    std::string lexeme; // лексему, як номер строки, сама лексема, 
    int length;         // її довжина і тип
    std::string type;
};

extern std::vector<Lexeme> allLexemes; //глобальний вектор з лексемами, отриманих після аналізу рядка, 
                                       //для подальшого використання в First_view.h і Second_view.h

std::string toLowerCase(const std::string& str); // Функція для переведення строки в нижній регістр

std::string identifyType(const std::string& token); // Функція для визначення типу лексеми

std::vector<Lexeme> lexicalAnalyzer(const std::string& line); // Функція для лексичного аналізу рядка(розбирає на лексеми)

void generateStructureTable(std::ofstream& outputFile, const std::vector<Lexeme>& lexemes, int rowNumber); // Функція яка генерує таблицю структури речення

#endif 