#ifndef LEXICAL_H
#define LEXICAL_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <iomanip> // ��� ������������ ������
#include <algorithm> // ��� toLowerCase
#include <unordered_set>
#include <regex>
#include "commands_types.h"

struct TypeInfo {
    Type main_type;
    TypeConst const_type; // ��� TypeConst::NONE ���� �� ���������
};

struct Lexeme {         // ���������, ��� ������
    int number;         // � ��� ���������� ��� 
    std::string lexeme; // �������, �� ����� ������, ���� �������, 
    int length;         // �� ������� � ���
    TypeInfo type;
};

static const std::unordered_set<std::string> directives = {
       "segment", "ends", "end", "db", "dw", "dd", "assume"
};
static const std::unordered_set<std::string> instructions = {
    "popad", "push", "neg", "adc", "sbb", "lds", "mov", "and", "jnae", "jmp"
};
static const std::unordered_set<std::string> segmentRegisters = {
    "cs", "ds", "es", "gs"
};
static const std::unordered_set<std::string> generalRegisters = {
    "eax", "ebx", "ecx", "edx", "esi", "edi"
};
static const std::unordered_set<std::string> dataRegisters = {
    "ah", "al", "bh", "bl", "ch", "cl", "dh", "dl"
};
static const std::unordered_set<std::string> dataSizes = {
    "byte", "word", "dword"
};
static const std::unordered_set<std::string> typeOperators = {
    "ptr"
};
static const std::unordered_set<std::string> addressSpecifiers = {
    "far"
};
static const std::unordered_set<std::string> singleCharOperators = {
    ":", ",", "+", "[", "]", "*"
};

std::string ToLowerCase(const std::string& str); // ������� ��� ����������� ������ � ����� ������

TypeInfo IdentifyType(const std::string& token); // ������� ��� ���������� ���� �������

std::vector<Lexeme> LexicalAnalyzer(const std::string& line); // ������� ��� ���������� ������ �����(������� �� �������)

std::vector<Type> ConvertLexemesToVector(const std::vector<Lexeme>& lexemes);

#endif 