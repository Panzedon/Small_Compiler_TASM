#ifndef FIRST_VIEW_H
#define FIRST_VIEW_H
#include "Lexical.h"
#include <bitset>
#include <sstream>

struct Identifier {
    std::string name;      // Ім'я ідентифікатора
    std::string type;      // Тип ідентифікатора
    std::string segment;   // Ім'я сегмента
    int offset;            // Зміщення в сегменті
};

struct LogicalSegment {
    std::string name;       // Ім’я сегмента
    int defaultSize;        // Розрядність за замовчуванням (16 або 32 біти)
    int currentOffset;      // Поточне зміщення ($)
};

struct SegmentRegisters {  // Структура назначень сегментним регістрам
    std::string regis;
    std::string target;
};

extern int Active_seg;
extern int local_size;
extern int global_size;
extern int already;

extern std::vector<SegmentRegisters> SegmentRegist; // Таблиця назначень сегментним регістрам

extern std::vector<Identifier> identifierTable; // Таблиця ідентифікаторів
extern std::vector<LogicalSegment> segmentTable; // Таблиця сегментів

void FillSegmentTable(const std::vector<Lexeme>& lexemes); // Функція для заповнення таблиці 
                                                           // назначень сегментних регістрів
void CreateSegmentTable(const std::string name);  // Функція для заповнення таблиці сегментів

void CalculateMemoryUsage(const std::vector<Lexeme>& lexemes); // Функція для визначення зміщення строки для констант

void createIdentifierTable(const std::vector<Lexeme>& lexemes);  // Заповнення таблиці ідентифікаторів

int calculateBytes(int number); // Функція для визначення, скільки байтів займає число типу int

int checkNumbers(const std::string& input);  // Функція для визначення типу числа, і визначення скільки байтів він займає

void CalculateCommandMemory(const std::vector<Lexeme>& lexemes); // Функція для визначення зміщення для команд
#endif // !FIRST_VIEW_H
