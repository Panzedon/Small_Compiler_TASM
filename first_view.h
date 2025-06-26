#ifndef FIRST_VIEW_H
#define FIRST_VIEW_H
#include "lexical.h"
#include <bitset>
#include <sstream>
#include <unordered_map>

const std::vector<std::pair<int, std::vector<Type>>> lexemePatterns = {
    {101, {Type::kUserIdent, Type::kDirec}},
    {102, {Type::kUserIdent, Type::kDirec, Type::kConst}},
    {103, {Type::kUserIdent, Type::kDirec, Type::kConst}},
    {104, {Type::kUserIdent, Type::kDirec, Type::kConst}},
    {105, {Type::kUserIdent, Type::kDirec, Type::kConst}},
    {106, {Type::kUserIdent, Type::kSymb}},
};

struct SizeActiveSeg {
    int global_size;
    int local_size;
    int active_seg;
    std::string seg;
    bool done = 0;
    int line;
};

struct IdentifierData {
    IdentifierType type;      // ��� ��������������
    std::string segment;   // ��'� ��������
    int offset;            // ������� � �������
};

extern std::unordered_map<std::string, IdentifierData> identifier_table;

struct LogicalSegment {
    std::string name;       // ��� ��������
    int defaultSize;        // ���������� �� ������������� (16 ��� 32 ���)
    int current_offset;      // ������� ������� ($)
};

struct SegmentRegisters {  // ��������� ��������� ���������� ��������
    std::string regis;
    std::string target;
};

extern std::vector<SegmentRegisters> segment_regist; // ������� ��������� ���������� ��������

//extern std::vector<Identifier> identifierTable; // ������� ��������������
extern std::vector<LogicalSegment> segment_table; // ������� ��������
extern std::vector<std::string> vector_identfiers;

void FillSegmentTable(const std::vector<Lexeme>& lexemes); // ������� ��� ���������� ������� 
                                                           // ��������� ���������� �������
//void CreateSegmentTable(const std::string name);  // ������� ��� ���������� ������� ��������

void CalculateMemoryUsage(const std::vector<Lexeme>& lexemes, SizeActiveSeg& value); // ������� ��� ���������� ������� ������ ��� ��������

//int createIdentifierTable(int code, const std::vector<Lexeme>& lexemes, Size_ActiveSeg& value);

int CalculateBytesForNumber(int number); // ������� ��� ����������, ������ ����� ����� ����� ���� int

int CheckConstAndCalculate(const std::string& input);  // ������� ��� ���������� ���� �����, � ���������� ������ ����� �� �����

//void CalculateCommandMemory(const std::vector<Lexeme>& lexemes); // ������� ��� ���������� ������� ��� ������

void First_View(const std::vector<Lexeme>& lexemes, std::vector<Type>& lexemes_keys, SizeActiveSeg& value);

std::optional<CommandInfo> GetCommandInfo(const StructWithCom& structure);
StructWithCom FillStructWithCom(const std::vector<Lexeme>& lexemes, SizeActiveSeg& value);
std::optional<SegReg> FindSegReg(const std::vector<Lexeme>& lexemes);
InfoAddr GetInfoAddr(const std::vector<Lexeme>& lexemes);
std::optional<ConstSizeOperand> FindConstSize(const std::vector<Lexeme>& lexemes);
std::vector<TypeOperand> FindOperands(const std::vector<Lexeme>& lexemes, std::optional<TypeAddress>& values, SizeActiveSeg& sizes);
std::optional<TypeAddress> DetectAddressType(const std::string& addr_expr, SizeActiveSeg& sizes);
int CreateIdentifierTable(int code, const std::vector<Lexeme>& lexemes, SizeActiveSeg& value);
int FindLexemePattern(std::vector<Type>& lexemes_pair);
TypeOperand GetTypeJump(const std::vector<Lexeme>& lexemes);
void FillSegmentTable(const std::vector<Lexeme>& lexemes);
std::string ParcerLexemAddr(const std::vector<Lexeme>& lexemes, int& point);
void CalculateOffsetString(std::optional<CommandInfo>& info, SizeActiveSeg& sizes, StructWithCom& structures);

#endif // !FIRST_VIEW_H
