#ifndef FIRST_VIEW_H
#define FIRST_VIEW_H
#include "Lexical.h"
#include <bitset>
#include <sstream>

struct Identifier {
    std::string name;      // ��'� ��������������
    std::string type;      // ��� ��������������
    std::string segment;   // ��'� ��������
    int offset;            // ������� � �������
};

struct LogicalSegment {
    std::string name;       // ��� ��������
    int defaultSize;        // ���������� �� ������������� (16 ��� 32 ���)
    int currentOffset;      // ������� ������� ($)
};

struct SegmentRegisters {  // ��������� ��������� ���������� ��������
    std::string regis;
    std::string target;
};

extern int Active_seg;
extern int local_size;
extern int global_size;
extern int already;

extern std::vector<SegmentRegisters> SegmentRegist; // ������� ��������� ���������� ��������

extern std::vector<Identifier> identifierTable; // ������� ��������������
extern std::vector<LogicalSegment> segmentTable; // ������� ��������

void FillSegmentTable(const std::vector<Lexeme>& lexemes); // ������� ��� ���������� ������� 
                                                           // ��������� ���������� �������
void CreateSegmentTable(const std::string name);  // ������� ��� ���������� ������� ��������

void CalculateMemoryUsage(const std::vector<Lexeme>& lexemes); // ������� ��� ���������� ������� ������ ��� ��������

void createIdentifierTable(const std::vector<Lexeme>& lexemes);  // ���������� ������� ��������������

int calculateBytes(int number); // ������� ��� ����������, ������ ����� ����� ����� ���� int

int checkNumbers(const std::string& input);  // ������� ��� ���������� ���� �����, � ���������� ������ ����� �� �����

void CalculateCommandMemory(const std::vector<Lexeme>& lexemes); // ������� ��� ���������� ������� ��� ������
#endif // !FIRST_VIEW_H
