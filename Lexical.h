#ifndef LEXICAL_H
#define LEXICAL_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <iomanip> // ��� ������������ ������
#include <algorithm> // ��� toLowerCase

struct Lexeme {         // ���������, ��� ������
    int number;         // � ��� ���������� ��� 
    std::string lexeme; // �������, �� ����� ������, ���� �������, 
    int length;         // �� ������� � ���
    std::string type;
};

extern std::vector<Lexeme> allLexemes; //���������� ������ � ���������, ��������� ���� ������ �����, 
                                       //��� ���������� ������������ � First_view.h � Second_view.h

std::string toLowerCase(const std::string& str); // ������� ��� ����������� ������ � ����� ������

std::string identifyType(const std::string& token); // ������� ��� ���������� ���� �������

std::vector<Lexeme> lexicalAnalyzer(const std::string& line); // ������� ��� ���������� ������ �����(������� �� �������)

void generateStructureTable(std::ofstream& outputFile, const std::vector<Lexeme>& lexemes, int rowNumber); // ������� ��� ������ ������� ��������� �������

#endif 