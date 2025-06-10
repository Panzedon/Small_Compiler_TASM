#ifndef SECOND_VIEW_H
#define SECOND_VIEW_H
#include "Lexical.h"
#include "first_view.h"
#include <unordered_map>
#include <functional>

struct Operations {      // ��������� ��� �������� ����������
	// � ���� ����, bites - �� �������� � ���� Modr\m
	std::string command;  
	std::string code;
	std::string bites;
};
struct AddressComponents { // ��������� ��� ������� � ��� ���������� Modrm � Sib �����
	// ���� � ��������� ������� [eax + esi * 2 + 6]
	std::string command;             // ������� ����������
	std::string base_register;       // ������� ������
	std::string index_register;      // ��������� ������
	std::string scale;               // �������
	std::string displacement;        // �������
};
struct SegmentPr {  // ���������, ��� ������ ���������� ������ � ���� ���
	std::string seg;
	std::string val;
};
struct Register_co { // ���������. ��� ������ ������ � ���� ���
	std::string reg;
	std::string code;
};

struct Operands { // ��������� ��� �������, ��� ������ ���������� ���
	// �������� ��� �������� ����������, � ��� ���������� �����
	std::vector<Lexeme> first_operand;
	std::vector<Lexeme> second_operand;
	std::string create_type;
};
extern std::string error;
extern std::string result_value;
extern std::string result;
extern std::string command_adrress;
extern int reserv;

std::string convertToHex(const Lexeme& input); // ����������� ����� � ��������������� ������ � ������ ������

Operands find_operands(const std::vector<Lexeme>& lexemes, int mod); // �������, ��� ���� �������� � �������� ����������� 

std::string binaryToHex(const std::string& binaryStr); // �������, ��� ���������� ������� �����(��� string) � ��������������� ������ � ������ ������ 
std::string decimalToHex(const std::string& decimalStr); // �������, ��� ���������� ��������� �����(��� string) � ��������������� ������ � ������ ������
std::string Find_Segment(const std::vector<Lexeme>& lexemes); // �������, ��� ���� ������� ����� �������� � ������

void Count_Adress_Memory(Operations& oper); // �������, ��� �������� ����� Modrm � Sib, ���� � ���������
void FindCommandAdrress(const std::vector<Lexeme>& lexemes); // ���� � �������, �� ��������� ��������� ������ ���������, � �������� 
                                                             // ��������� AddressComponents
void CountValueConst(const std::vector<Lexeme>& lexeme); // �������, ��� ������� ���� ����� ��� ��������
void CountOffset(const std::vector<Lexeme>& lexemes);  // �������, ��� �������� ������� ��� ����� � ���������
void CountCommandValue(const std::vector<Lexeme>& lexemes); // �������, ��� �������, �� �� ������� ���������� � �����, � ���� ������� ��� ����� �������


void PUSH_Com(const std::vector<Lexeme>& lexemes); // �� ��� ������� ��� ������� �������� ���������� � �� ��������, �� ��� 
void ADC_Com(const std::vector<Lexeme>& lexemes); // �����������, �� � �������� ��������, �� ��������� ���� ������� � ���� ���� ����� ������
void AND_Com(const std::vector<Lexeme>& lexemes);
void JMP_Com(const std::vector<Lexeme>& lexemes);
void LDS_Com(const std::vector<Lexeme>& lexemes);
void MOV_Com(const std::vector<Lexeme>& lexemes);
void NEG_Com(const std::vector<Lexeme>& lexemes);
void POPAD_Com(const std::vector<Lexeme>& lexemes);
void SBB_Com(const std::vector<Lexeme>& lexemes);
void JNAE_Com(const std::vector<Lexeme>& lexemes);

#endif // !SECOND_VIEW_H
