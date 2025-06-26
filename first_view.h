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
    IdentifierType type;      // Тип ідентифікатора
    std::string segment;   // Ім'я сегмента
    int offset;            // Зміщення в сегменті
};

extern std::unordered_map<std::string, IdentifierData> identifier_table;

struct LogicalSegment {
    std::string name;       // Ім’я сегмента
    int defaultSize;        // Розрядність за замовчуванням (16 або 32 біти)
    int current_offset;      // Поточне зміщення ($)
};

struct SegmentRegisters {  // Структура назначень сегментним регістрам
    std::string regis;
    std::string target;
};

extern std::vector<SegmentRegisters> segment_regist; // Таблиця назначень сегментним регістрам

//extern std::vector<Identifier> identifierTable; // Таблиця ідентифікаторів
extern std::vector<LogicalSegment> segment_table; // Таблиця сегментів
extern std::vector<std::string> vector_identfiers;

void FillSegmentTable(const std::vector<Lexeme>& lexemes); // Функція для заповнення таблиці 
                                                           // назначень сегментних регістрів
//void CreateSegmentTable(const std::string name);  // Функція для заповнення таблиці сегментів

void CalculateMemoryUsage(const std::vector<Lexeme>& lexemes, SizeActiveSeg& value); // Функція для визначення зміщення строки для констант

//int createIdentifierTable(int code, const std::vector<Lexeme>& lexemes, Size_ActiveSeg& value);

int CalculateBytesForNumber(int number); // Функція для визначення, скільки байтів займає число типу int

int CheckConstAndCalculate(const std::string& input);  // Функція для визначення типу числа, і визначення скільки байтів він займає

//void CalculateCommandMemory(const std::vector<Lexeme>& lexemes); // Функція для визначення зміщення для команд

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
