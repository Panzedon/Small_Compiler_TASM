#ifndef SECOND_VIEW_H
#define SECOND_VIEW_H
#include "Lexical.h"
#include "first_view.h"
#include <unordered_map>
#include <functional>

struct Operations {      // Структура для машинних інструкцій
	// і їхніх кодів, bites - їх значення в байті Modr\m
	std::string command;  
	std::string code;
	std::string bites;
};
struct AddressComponents { // Структура для обробки і для заповнення Modrm і Sib байту
	// коли є адресація формату [eax + esi * 2 + 6]
	std::string command;             // Машинна інструкція
	std::string base_register;       // Базовий регістр
	std::string index_register;      // Індексний регістр
	std::string scale;               // Множник
	std::string displacement;        // Зміщення
};
struct SegmentPr {  // Структура, яке зберігає сегментний регістр і його код
	std::string seg;
	std::string val;
};
struct Register_co { // Структура. яка зберігає регістр і його код
	std::string reg;
	std::string code;
};

struct Operands { // Структура для обробок, яка зберігає інформацію про
	// операнди для машинних інструкцій, і про розрядність даних
	std::vector<Lexeme> first_operand;
	std::vector<Lexeme> second_operand;
	std::string create_type;
};
extern std::string error;
extern std::string result_value;
extern std::string result;
extern std::string command_adrress;
extern int reserv;

std::string convertToHex(const Lexeme& input); // переведення числа в шістнадцятковий формат у вигляді строки

Operands find_operands(const std::vector<Lexeme>& lexemes, int mod); // Функція, яка шукає операнди в машинних інструкціях 

std::string binaryToHex(const std::string& binaryStr); // Функція, яка переводить двійкове число(тип string) у шістнадцятковий формат у вигляді строки 
std::string decimalToHex(const std::string& decimalStr); // Функція, яка переводить десяткове число(тип string) у шістнадцятковий формат у вигляді строки
std::string Find_Segment(const std::vector<Lexeme>& lexemes); // Функція, яка шукає префікс заміни сегмента у строці

void Count_Adress_Memory(Operations& oper); // Функція, яка вираховує байти Modrm і Sib, коли є адресація
void FindCommandAdrress(const std::vector<Lexeme>& lexemes); // Шукає і визначає, чи правильно записаний формат адресації, і заповнює 
                                                             // структуру AddressComponents
void CountValueConst(const std::vector<Lexeme>& lexeme); // Функція, яка визначає вміст байтів для констант
void CountOffset(const std::vector<Lexeme>& lexemes);  // Функція, яка вираховує зміщення для строк в сегментах
void CountCommandValue(const std::vector<Lexeme>& lexemes); // Функція, яка визначає, що за машинна інструкція в рядку, і потім викликає для нього функцію


void PUSH_Com(const std::vector<Lexeme>& lexemes); // Це все функції для обробки машинних інструкцій і їх операндів, де вже 
void ADC_Com(const std::vector<Lexeme>& lexemes); // визначається, чи є правильні операнди, чи правильно вони записані і видає вміст байтів строки
void AND_Com(const std::vector<Lexeme>& lexemes);
void JMP_Com(const std::vector<Lexeme>& lexemes);
void LDS_Com(const std::vector<Lexeme>& lexemes);
void MOV_Com(const std::vector<Lexeme>& lexemes);
void NEG_Com(const std::vector<Lexeme>& lexemes);
void POPAD_Com(const std::vector<Lexeme>& lexemes);
void SBB_Com(const std::vector<Lexeme>& lexemes);
void JNAE_Com(const std::vector<Lexeme>& lexemes);

#endif // !SECOND_VIEW_H
