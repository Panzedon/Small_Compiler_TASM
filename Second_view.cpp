#include "Second_view.h"

std::vector<Operations> Codes_Operations = { {"push", "50", ""},
	{"adc", "83", "010"},{"adc", "14", ""},
	{"and", "81", "100"},{"and", "83", "100"},
	{"jmp", "EA", ""},
	{"lds", "C5", ""},
	{"mov", "88", ""},{"mov", "89", ""},
	{"neg", "F7", "011"},{"neg", "F6", "011"},
	{"popad", "61", ""},
	{"sbb", "1A", ""},{"sbb", "1B", ""},
	{"jnae", "72", ""} };
//jmp EA cd 
//mov 88, 89 /r
//push 50+rd
//sbb 1A, 1B /r
// jnae 72 /cb

std::vector<Register_co> Register_Codes = { {"eax", "000"}, {"ecx", "001"} , {"edx", "010"} , {"ebx", "011"} , {"sib", "100"} , {"ebp", "101"}, {"esi", "110"}, {"edi", "111"} };
std::vector<Register_co> Small_Register_Codes = { {"al", "000"},
	{"cl", "001"},
	{"dl", "010"},
	{"bl", "011"} };

std::vector<SegmentPr> Segment_Codes = { {"gs", "65"}, {"fs", "64"}, {"ds", "3E"} };

AddressComponents resource = { "", "", "", "", "" };

std::string result_value = "";
std::string error = "";
std::string result = "";
std::string command_adrress = "";
int reserv;
int adress_count_1 = 0;
int adress_count_2 = 0;
int sig_dword_ptr = 0;
int sig_byte_ptr = 0;
int signal_adrress = 0;
Lexeme opera = {};

std::string convertToHex(const Lexeme& input) {
	int num;

	if (input.type == "Шістнадцяткова константа") {
		num = std::stoi(input.lexeme.substr(0, input.lexeme.size() - 1), nullptr, 16);
	}
	else if (input.type == "Двійкова константа") {
		num = std::stoi(input.lexeme.substr(0, input.lexeme.size() - 1), nullptr, 2);
	}
	else {
		num = std::stoi(input.lexeme);
	}
	std::stringstream ss;
	ss << std::uppercase << std::hex << num;
	return ss.str();
}

Operands find_operands(const std::vector<Lexeme>& lexemes, int mod) {
	int sig1 = 0, sig2 = 0;
	int sig = 0;
	int ptr_sig = 0;
	Operands result;
	if (mod == 1) { // один операнд
		for (int i = 1; i < lexemes.size(); i++) {
			if (lexemes[i].lexeme == "[") {
				FindCommandAdrress(lexemes);
				if (signal_adrress == 1) {
					signal_adrress = 0;
					for (int j = i; j < lexemes.size(); i++) {

						if (lexemes[j].lexeme == "]") {
							result.first_operand.push_back(lexemes[j]);
							break;
						}
						else {
							result.first_operand.push_back(lexemes[j]);
						}
					}
					sig++;
				}
			}
			else if (lexemes[i].type == "Розрядність даних" && lexemes[i + 1].type == "Оператор визначення типу" && (i+1) < lexemes.size()) {
				if (ptr_sig == 0 && sig == 0) {
					result.create_type = lexemes[i].lexeme;
					ptr_sig++;
				}
				else {
					error = "Error";
				}
			}
			else if (lexemes[i].type == "Регістри даних" || lexemes[i].type == "32-розрядний регістр загального призначення") {
				result.first_operand.push_back(lexemes[i]);
				//sig1 = 1;
				sig++;
			}
			else if (lexemes[i].type == "Десяткова константа" || lexemes[i].type == "Шістнадцяткова константа" || lexemes[i].type == "Двійкова константа") {
				result.first_operand.push_back(lexemes[i]);
			}
		}

	}
	else if (mod == 2) { // два операнди
		for (int i = 1; i < lexemes.size(); i++) {
			if (lexemes[i].lexeme == "[") {
				FindCommandAdrress(lexemes);
				if (signal_adrress == 1) {
					signal_adrress = 0;
					if (sig1 == 0 && sig2 == 0) {
						for (int j = i; j < lexemes.size(); j++) {

							if (lexemes[j].lexeme == "]") {
								result.first_operand.push_back(lexemes[j]);
								i = j;
								break;
							}
							else {
								result.first_operand.push_back(lexemes[j]);
							}
						}
						sig1 = 1;
					}
					else if (sig1 == 1 && sig2 == 0) {
						for (int j = i; j < lexemes.size(); j++) {

							if (lexemes[j].lexeme == "]") {
								result.second_operand.push_back(lexemes[j]);
								i = j;
								break;
							}
							else {
								result.second_operand.push_back(lexemes[j]);
							}
						}
						sig2 = 1;
					}
					else {
						error = "Error";
					}
				}
			}
			else if (lexemes[i].type == "Розрядність даних" && lexemes[i + 1].type == "Оператор визначення типу" && (i + 1) < lexemes.size()) {
				if (ptr_sig == 0 && sig1 == 0 && sig2 == 0) {
					result.create_type = lexemes[i].lexeme;
					ptr_sig++;
				}
				else {
				}
			}
			else if (lexemes[i].type == "Регістри даних" || lexemes[i].type == "32-розрядний регістр загального призначення") {
				if (sig1 == 0 && sig2 == 0) {
					result.first_operand.push_back(lexemes[i]);
					sig1 = 1;
				}
				else if (sig1 == 1 && sig2 == 0) {
					result.second_operand.push_back(lexemes[i]);
					sig2 = 1;
				}
				else {

				}
			}
			else if (lexemes[i].type == "Десяткова константа" || lexemes[i].type == "Шістнадцяткова константа" || lexemes[i].type == "Двійкова константа") {
				if (sig1 == 0 && sig2 == 0) {
					result.first_operand.push_back(lexemes[i]);
					sig1 = 1;
				}
				else if (sig1 == 1 && sig2 == 0) {
					result.second_operand.push_back(lexemes[i]);
					sig2 = 1;
				}
				else {

				}
			}
		}
	}
	else {

	}
	return result;
}

std::string binaryToHex(const std::string& binaryStr) {
	unsigned int byteValue = std::bitset<8>(binaryStr).to_ulong();

	std::stringstream ss;
	ss << std::hex << std::uppercase << byteValue;

	return ss.str();
}

std::string decimalToHex(const std::string& decimalStr) {
	int decimalValue = std::stoi(decimalStr); 
	std::stringstream ss;
	ss << std::hex << std::uppercase << decimalValue; 
	return ss.str();
}

std::string Find_Segment(const std::vector<Lexeme>& lexemes) {
	int signal = 0;
	for (const auto& id : lexemes) {
		if (id.type == "Сегментний регістр" && signal == 0) {
			for (const auto& res : Segment_Codes) {
				if (id.lexeme == res.seg) {
					return res.val;
				}
			}
		}
	}
	return "";
}

void Count_Adress_Memory(Operations & oper) {  // приймає AddressComponents resource
	std::string Mod_rm;
	std::string mod;
	std::string reg;
	std::string rm;
	std::string code_sib;
	std::string offset = "";
	std::string sib2 = "";
	if (resource.displacement == "" || resource.displacement == "0") {
		mod = "00";
	}
	else {
		mod = "01";
		offset = decimalToHex(resource.displacement);
	}
	if (resource.index_register != "") {
		rm = "100";
	}
	else{
		for (int i = 0; i < 4; i++) {
			if (resource.base_register == Register_Codes[i].reg) {
				rm = Register_Codes[i].code;
			}
		}
	}
	if(opera.type == "32-розрядний регістр загального призначення") {
		for (int i = 0; i < 4; i++) {
			if (opera.lexeme == Register_Codes[i].reg) {
				reg = Register_Codes[i].code;
			}
		}
	}
	else if (opera.type == "Регістри даних") {
		for (int i = 0; i < 4; i++) {
			if (opera.lexeme == Small_Register_Codes[i].reg) {
				reg = Small_Register_Codes[i].code;
			}
		}
	}
	if (oper.bites != "") {
		reg = oper.bites;
	}
	//Mod_rm = mod + reg + rm;
	Mod_rm = binaryToHex(mod + reg + rm);
	if (rm == "100") {
		std::string sib;
		if (resource.scale == "2") {
			sib += "01";
		}
		else if (resource.scale == "4") {
			sib += "10";
		}
		else if (resource.scale == "8") {
			sib += "11";
		}
		else {
			error = "Error";
			return;
		}
		for (const auto& id : Register_Codes) {
			if (resource.index_register == id.reg) {
				sib += id.code;
				break;
			}
		}
		for (int i = 0; i < 4; i++) {
			if (resource.base_register == Register_Codes[i].reg) {
				sib += Register_Codes[i].code;
				break;
			}
		}
		sib2 = binaryToHex(sib);
	}
	command_adrress = Mod_rm + " " + sib2 + " " + offset;
	resource = { "", "", "", "", "" };
	opera = {};


}

void FindCommandAdrress(const std::vector<Lexeme>& lexemes) {
	int signal = 0;
	int position_1 = 0;
	int position_2 = 0;
	int first_reg = 0;
	int second_reg = 0;
	int first_mul = 0;
	int offset = 0;
	int sig_mul = 0;
	int sig_first_add = 0;
	int sig_second_add = 0;
	for (int i = 0; i < lexemes.size(); i++) {
		if (lexemes[i].lexeme == "[") {
			signal += 1;
			position_1 = i;
		}
		else if (lexemes[i].lexeme == "]") {
			signal += 1;
			position_2 = i;
			break;
		}
		else if (lexemes[i].type == "32-розрядний регістр загального призначення" && (lexemes[i].lexeme == "eax" || 
			lexemes[i].lexeme == "ebx" || lexemes[i].lexeme == "edx" || lexemes[i].lexeme == "ecx") && 
			signal == 1 && first_reg == 0 && sig_first_add == 0 && second_reg == 0 && sig_mul == 0 && first_mul == 0 && sig_second_add == 0 && offset == 0) {
			first_reg += 1;
			resource.base_register = lexemes[i].lexeme;
		}
		else if (lexemes[i].type == "32-розрядний регістр загального призначення" && (lexemes[i].lexeme != resource.base_register) &&
			signal == 1 && first_reg == 1 && sig_first_add == 1 && second_reg == 0 && sig_mul == 0 && first_mul == 0 && sig_second_add == 0 && offset == 0) {
			second_reg += 1;
			resource.index_register = lexemes[i].lexeme;
		}
		else if (lexemes[i].type == "Десяткова константа" && 
			signal == 1 && first_reg == 1 && sig_first_add == 1 && second_reg == 1 && sig_mul == 1 && first_mul == 0 && sig_second_add == 0 && offset == 0) {
			first_mul += 1;
			resource.scale = lexemes[i].lexeme;
		}
		else if (lexemes[i].type == "Десяткова константа" &&
			signal == 1 && first_reg == 1 && sig_first_add == 1 && second_reg == 1 && sig_mul == 1 && first_mul == 1 && sig_second_add == 1 && offset == 0) {
			offset += 1;
			resource.displacement = lexemes[i].lexeme;
		}
		else if (lexemes[i].lexeme == "+" && 
			signal == 1 && first_reg == 1 && sig_first_add == 0 && second_reg == 0 && sig_mul == 0 && first_mul == 0 && sig_second_add == 0 && offset == 0) {
			sig_first_add += 1;
		}
		else if (lexemes[i].lexeme == "+" && 
			signal == 1 && first_reg == 1 && sig_first_add == 1 && second_reg == 1 && sig_mul == 1 && first_mul == 1 && sig_second_add == 0 && offset == 0) {
			sig_second_add += 1;
		}
		else if (lexemes[i].lexeme == "*" &&
			signal == 1 && first_reg == 1 && sig_first_add == 1 && second_reg == 1 && sig_mul == 0 && first_mul == 0 && sig_second_add == 0 && offset == 0) {
			sig_mul += 1;
		}
	}
	if (lexemes[position_1].lexeme == "[" && lexemes[position_2].lexeme == "]") {
		if ((position_2 - position_1 - 1 == 1) && (signal == 2 && first_reg == 1 &&
			sig_first_add == 0 && second_reg == 0 && sig_mul == 0 && first_mul == 0 && sig_second_add == 0 && offset == 0)) {
			signal_adrress = 1;

		}
		else if ((position_2 - position_1 - 1 == 5) && (signal == 2 && first_reg == 1 &&
			sig_first_add == 1 && second_reg == 1 && sig_mul == 1 && first_mul == 1 && sig_second_add == 0 && offset == 0)) {
			signal_adrress = 1;
		}
		else if ((position_2 - position_1 - 1 == 7) && (signal == 2 && first_reg == 1 &&
			sig_first_add == 1 && second_reg == 1 && sig_mul == 1 && first_mul == 1 && sig_second_add == 1 && offset == 1)) {
			signal_adrress = 1;
		}
		else {
			error = "Error";
		}
	}
	else {
		error = "Error";
	}

}


void PUSH_Com(const std::vector<Lexeme>& lexemes){
	if (lexemes.size() == 2 || lexemes[1].type == "32 - розрядний регістр загального призначення") {
		for (const auto& op : Codes_Operations) {
			if (op.command == "push") { 
				result_value = op.code; 
				break; 
			}
		}
	}
	else {
		error = "Error";
		return;
	}
}
void ADC_Com(const std::vector<Lexeme>& lexemes) {
	Operands res = find_operands(lexemes, 2);
	std::cout << res.create_type << "\n" << res.first_operand[0].lexeme << "\n" << res.second_operand[0].lexeme << "\n";
	if (res.first_operand[0].lexeme == "al" && (res.second_operand[0].type == "Шістнадцяткова константа" || res.second_operand[0].type == "Десяткова константа" ||
		res.second_operand[0].type == "Двійкова константа")) 
	{
		for (const auto& op : Codes_Operations) {
			if (op.command == "adc" && op.code == "14") { 
				result_value = op.code;
				break; 
			}
		}
		result_value += " " + convertToHex(res.second_operand[0]);
	}
	else if(res.first_operand[0].type == "32-розрядний регістр загального призначення" && (res.second_operand[0].type == "Шістнадцяткова константа" || res.second_operand[0].type == "Десяткова константа" ||
		res.second_operand[0].type == "Двійкова константа")) {
		for (const auto& op : Codes_Operations) {
			if (op.command == "adc" && op.code == "83") { 
				result_value = op.code; 
				std::string reg;
				for (int i = 0; i < 4; i++) {
					if (res.first_operand[0].lexeme == Register_Codes[i].reg) {
						reg = Register_Codes[i].code;
					}
				}
				result_value += " " + binaryToHex("11" + op.bites + reg);
				break; 
			}
		}
		result_value += " " + convertToHex(res.second_operand[0]);
	}
	else {
		global_size = reserv;
		error = "Error";
	}

}
void AND_Com(const std::vector<Lexeme>& lexemes) {
	Operands res = find_operands(lexemes, 2);
	if (res.first_operand.size() > 1 && (res.second_operand[0].type == "Десяткова константа" || res.second_operand[0].type == "Двійкова константа" ||
		res.second_operand[0].type == "Шістнадцяткова константа") && res.create_type != "") {
		if (lexemes[1].type == "Сегментний регістр" && lexemes[2].lexeme == ":") {
			for (auto& id : Segment_Codes) {
				if (id.seg == lexemes[1].lexeme) {
					result += id.val;
					result += ": ";
					break;
				}
			}
		}
		FindCommandAdrress(lexemes);
		if (signal_adrress == 1) {
			std::string value_res = convertToHex(res.second_operand[0]);
			if (value_res.size() > 2) {
				Count_Adress_Memory(Codes_Operations[3]);
				result_value += result + Codes_Operations[3].code + " " + command_adrress + " " + value_res;
			}
			else {
				Count_Adress_Memory(Codes_Operations[4]);
				result_value += result + Codes_Operations[4].code + " " + command_adrress + " " + value_res;
			}
		}

	}
	else {
		error = "Error";
	}
}
void JMP_Com(const std::vector<Lexeme>& lexemes) {
	if (lexemes[0].lexeme == "jmp" && lexemes[1].lexeme == "far" && lexemes[2].lexeme == "ptr"){
		for (const auto& id : identifierTable) {
			if (lexemes[3].lexeme == id.name && id.type == "Мітка") {
				for (const auto& op : Codes_Operations) {
					if (op.command == "jmp") {
						result_value += op.code;
						std::ostringstream resultStream;
						int k = 0;
						resultStream << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << id.offset 
							<< std::setw(4) << std::setfill('0') << std::hex << std::uppercase << k << "sr";
						result_value += " " + resultStream.str();
					}
				}
			}
		}

	}
}
void LDS_Com(const std::vector<Lexeme>& lexemes) {
	Operands res = find_operands(lexemes, 2);
	if (res.first_operand[0].type == "32-розрядний регістр загального призначення" && res.second_operand.size() > 1) {
		if (lexemes[3].type == "Сегментний регістр" && lexemes[4].lexeme == ":" ) {
			for (auto& id : Segment_Codes) {
				if (id.seg == lexemes[3].lexeme) {
					result += id.val;
					result += ": ";
					break;
				}
			}
		}
		FindCommandAdrress(lexemes);
		if (signal_adrress == 1) {
			opera = res.first_operand[0];
			Count_Adress_Memory(Codes_Operations[6]);
			result_value += result + Codes_Operations[6].code + " " + command_adrress;
		}

	}
}
void MOV_Com(const std::vector<Lexeme>& lexemes) {
	Operands res = find_operands(lexemes, 2);
	if (res.first_operand.size() > 1 && res.second_operand[0].type == "32-розрядний регістр загального призначення" && res.create_type == "") {
		if (lexemes[1].type == "Сегментний регістр" && lexemes[2].lexeme == ":") {
			for (auto& id : Segment_Codes) {
				if (id.seg == lexemes[1].lexeme) {
					result += id.val;
					result += ": ";
					break;
				}
			}
		}
		FindCommandAdrress(lexemes);
		if (signal_adrress == 1) {
			opera = res.second_operand[0];
			Count_Adress_Memory(Codes_Operations[8]);
			result_value += result + Codes_Operations[8].code + " " + command_adrress;
		}

	}
	else if (res.first_operand.size() > 1 && res.second_operand[0].type == "Регістри даних" && res.create_type != "") {
		if (lexemes[1].type == "Сегментний регістр" && lexemes[2].lexeme == ":") {
			for (auto& id : Segment_Codes) {
				if (id.seg == lexemes[1].lexeme) {
					result += id.val;
					result += ": ";
					break;
				}
			}
		}
		FindCommandAdrress(lexemes);
		if (signal_adrress == 1) {
			opera = res.second_operand[0];
			Count_Adress_Memory(Codes_Operations[7]);
			result_value += result + Codes_Operations[7].code + " " + command_adrress;
		}
	}
	else {
		global_size = reserv;
		error = "Error";
	}
}
void NEG_Com(const std::vector<Lexeme>& lexemes) {
	int ptr = 0;
	int seg = 0;
	int sig = 0;
	resource = { "neg", "", "", "", ""};
	if (lexemes[1].lexeme == "dword" && lexemes[2].lexeme == "ptr") {
		sig_dword_ptr = 1;
		ptr = 1;
		sig = 2;
	}
	else if (lexemes[1].lexeme == "byte" && lexemes[2].lexeme == "ptr") {
		sig_byte_ptr = 1;
		ptr = 1;
		sig = 2;
	}
	if (sig_dword_ptr == 1 || sig_byte_ptr == 1) {
		
	}
	if(lexemes[1+sig].type == "Сегментний регістр") {
		for (auto& id : Segment_Codes) {
			if (id.seg == lexemes[1+sig].lexeme) {
				result += id.val;
				result += ": ";
				seg = 1;
				break;
			}
		}
	}
	FindCommandAdrress(lexemes);
	if (signal_adrress == 1) {
		Count_Adress_Memory(Codes_Operations[9]);
		result_value += result + Codes_Operations[9].code + " " + command_adrress;
	}
	else {
		error = "Error";
	}
	result = "";
	command_adrress = "";

}

void POPAD_Com(const std::vector<Lexeme>& lexemes) {
	if (lexemes.size() > 1) {
		error = "Error";
		return;
	}
	for (const auto& op : Codes_Operations) {
		if (op.command == "popad") { 
			result_value = op.code; 
			break; 
		}
	}
}

void SBB_Com(const std::vector<Lexeme>& lexemes) {
	Operands res = find_operands(lexemes, 2);
	if (res.first_operand[0].type == "Регістри даних" && res.second_operand[0].type == "Регістри даних" && res.first_operand[0].lexeme != res.second_operand[0].lexeme) {
		for (const auto& id : Codes_Operations) {
			if (id.command == "sbb" && id.code == "1A") {
				result_value = id.code;
				std::string reg1;
				std::string reg2;
				for (int i = 0; i < 4; i++) {
					if (res.first_operand[0].lexeme == Small_Register_Codes[i].reg) {
						reg1 = Small_Register_Codes[i].code;
					}
				}
				for (int i = 0; i < 4; i++) {
					if (res.second_operand[0].lexeme == Small_Register_Codes[i].reg) {
						reg2 = Small_Register_Codes[i].code;
					}
				}
				result_value += " " + binaryToHex("11" + reg1 + reg2);
				break; 
			}
		}
	}
	else if (res.first_operand[0].type == "32-розрядний регістр загального призначення" && res.second_operand[0].type == "32-розрядний регістр загального призначення" 
		&& res.first_operand[0].lexeme != res.second_operand[0].lexeme) {
		for (const auto& id : Codes_Operations) {
			if (id.command == "sbb" && id.code == "1B") {
				result_value = id.code;
				std::string reg1;
				std::string reg2;
				for (int i = 0; i < 4; i++) {
					if (res.first_operand[0].lexeme == Register_Codes[i].reg) {
						reg1 = Register_Codes[i].code;
					}
				}
				for (int i = 0; i < 4; i++) {
					if (res.second_operand[0].lexeme == Register_Codes[i].reg) {
						reg2 = Register_Codes[i].code;
					}
				}
				result_value += " " + binaryToHex("11" + reg1 + reg2);
				break; 
			}
		}
	}
	else {
		error = "Error";
	}

}

void JNAE_Com(const std::vector<Lexeme>& lexemes) {
	for (const auto& id : identifierTable) {
		if (lexemes[1].lexeme == id.name && id.type == "Мітка") {
			for (const auto& op : Codes_Operations) {
				if (op.command == "jnae") {
					result_value += op.code;
					int res;
					res = id.offset - (global_size);
					std::ostringstream resultStream;
					res &= 0xFF;  // 8 бит
					resultStream << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << res;
					result_value += " " + resultStream.str();
				}
			}
		}
	}
}


void CountValueConst(const std::vector<Lexeme> &lexeme) {
	std::ostringstream resultStream;
	int value;
	if (lexeme[2].type == "Шістнадцяткова константа") {
		value = std::stoi(lexeme[2].lexeme.substr(0, lexeme[2].lexeme.size() - 1), nullptr, 16);
		resultStream << std::hex << std::uppercase << value;
	}
	// Обробка двійкових констант
	else if (lexeme[2].type == "Двійкова константа") {
		value = std::stoi(lexeme[2].lexeme.substr(0, lexeme[2].lexeme.size() - 1), nullptr, 2);
		if (lexeme[1].lexeme == "db") {  // Для типа db
			value &= 0xFF;  // 8 бит
			resultStream << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << value;
		}
		else if (lexeme[1].lexeme == "dw") {  // Для типа dw
			value &= 0xFFFF;  // 16 бит
			resultStream << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << value;
		}
		else if (lexeme[1].lexeme == "dd") {  // Для типа dd
			value &= 0xFFFFFFFF;  // 32 бит
			resultStream << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << value;
		}
	}
	// Обробка десяткових констант
	else if (lexeme[2].type == "Десяткова константа") {
		value = std::stoi(lexeme[2].lexeme);
		if (lexeme[1].lexeme == "db") {  // Для типа db
			value &= 0xFF;  // 8 бит
			resultStream << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << value;
		}
		else if (lexeme[1].lexeme == "dw") {  // Для типа dw
			value &= 0xFFFF;  // 16 бит
			resultStream << std::setw(4) << std::setfill('0') << std::hex << std::uppercase << value;
		}
		else if (lexeme[1].lexeme == "dd") {  // Для типа dd
			value &= 0xFFFFFFFF;  // 32 бит
			resultStream << std::setw(8) << std::setfill('0') << std::hex << std::uppercase << value;
		}
		//resultStream << std::hex << std::uppercase << value;
	}
	// Обробка текстових констант
	else if (lexeme[2].type == "Текстова константа") {
		for (char c : lexeme[2].lexeme) {
			if (c != '\'') {
				resultStream << std::hex << std::uppercase << std::setw(2) << std::setfill('0') << static_cast<int>(c) << " ";
			}
		}
	}
	result_value = resultStream.str();
}


void CountCommandValue(const std::vector<Lexeme>& lexemes) {
	std::unordered_map<std::string, std::function<void(const std::vector<Lexeme>&)>> commandHandlers = {
		{"push", PUSH_Com},
		{"adc", ADC_Com},
		{"and", AND_Com},
		{"jmp", JMP_Com},
		{"lds", LDS_Com},
		{"mov", MOV_Com},
		{"neg", NEG_Com},
		{"popad", POPAD_Com},
		{"sbb", SBB_Com},
		{"jnae", JNAE_Com}
	};
	const std::string& command = lexemes[0].lexeme;

	if (commandHandlers.find(command) != commandHandlers.end()) {
		CalculateCommandMemory(lexemes);
		commandHandlers[command](lexemes);

	}
}


void CountOffset(const std::vector<Lexeme>& lexemes) {
	int sig = 0;
	int sig1 = 0;
	if (lexemes.empty()) {
		return; 
	}
	for (const auto& id : lexemes) {
		if (id.type == "Невідомо") {
			error = "Error";
			result_value = "";
			result = "";
			command_adrress = "";
			return;
		}
	}

	if (lexemes[0].type == "Ідентифікатор користувача або не визначений" || lexemes[0].type == "Мнемокод машиної інструкції") {
		sig = 1; // пропускаємо строку, якщо перша лексема не Ідентифікатор або машинна інструкція
	}
	if (sig == 0) {
		return;
	}
	if (lexemes[0].type == "Ідентифікатор користувача або не визначений") {
		for (const auto& id : identifierTable) {
			if (lexemes[0].lexeme == id.name) {
				sig1 = 1;
				if (id.type == "Ім'я змінної" && Active_seg == 1) {
					CalculateMemoryUsage(lexemes);
					CountValueConst(lexemes);
				}
				else if (id.type == "Ім'я логічного сегмента" && lexemes[1].lexeme == "ends") {
					Active_seg = 0;
					local_size = global_size;
					global_size = 0;
				}
				else if (id.type == "Ім'я логічного сегмента" && lexemes[1].lexeme == "segment") {
					Active_seg = 1;
				}
				else if (id.type == "Мітка") {

				}
			}
		}
		if (sig1 == 0) {
			error = "Error";
			result_value = "";
			result = "";
			command_adrress = "";
			return;
		}
		
	}
	else {
		CountCommandValue(lexemes);
	}
}