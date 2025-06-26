#include "first_view.h"
#include "error_handler.h"
//#include "Second_view.h"

int Active_seg = 0;
int local_size = 0;
int global_size = 0;
int already = 0;

std::vector<SegmentRegisters> segment_regist = { {"cs", "Nothing"}, {"ds", "Nothing"}, {"ss", "Nothing"},
    {"es", "Nothing"}, {"gs", "Nothing"}, {"fs", "Nothing"} };

std::vector<LogicalSegment> segment_table = {};
std::unordered_map<std::string, IdentifierData> identifier_table;
std::vector<std::string> vector_identfiers;

std::string ParcerLexemAddr(const std::vector<Lexeme>& lexemes, int &point) {
    std::string result = "";
    std::vector<Lexeme> copy;
    int k = 0;
    for (int i = point; i < lexemes.size(); i++) {
        if (lexemes[i].lexeme == "0") {
            if (lexemes[i - 1].lexeme == "+") {
                copy.pop_back();
                k--;
                continue;
            }
            else if (lexemes[i - 1].lexeme == "*") {
                copy.erase(copy.begin() - 3, copy.end());
                k = k - 3;
                continue;
            }
            else if (lexemes[point + 2].lexeme == "+") {
                i++;
                continue;
            }
        }
        else if (lexemes[i].lexeme == "]") {
            copy.push_back(lexemes[i]);
            point = i;
            break;
        }
        copy.push_back(lexemes[i]);
        k++;
    }
    for (int i = 0; i <= k; i++) {
        result += copy[i].lexeme;
    }
    return result;
}
void FillSegmentTable(const std::vector<Lexeme>& lexemes) {
    for (size_t i = 1; i < lexemes.size(); ++i) {
        const Lexeme& lexeme = lexemes[i];
        if (lexeme.type.main_type == Type::kSegReg) {
            for (int j = 0; j < 6; j++) {
                if (lexeme.lexeme == segment_regist[j].regis) {
                    if (lexemes[i + 2].type.main_type == Type::kUserIdent) {
                        segment_regist[j].target = lexemes[i + 2].lexeme;
                        i = i + 2;
                    }
                }
            }
        }
    }
}
TypeOperand GetTypeJump(const std::vector<Lexeme>& lexemes) {
    static std::unordered_map<std::string, std::regex> regex_cache;
    std::string addr_expr;
    for (int i = 1; i < lexemes.size(); i++) {
        if (!addr_expr.empty() && std::isalnum(addr_expr.back()) && std::isalnum(lexemes[i].lexeme.front()))
            addr_expr += " "; // 
        addr_expr += lexemes[i].lexeme;
    }

    for (const auto& [pattern, type] : kRegexToJmpOperandType) {
        if (regex_cache.find(pattern) == regex_cache.end()) {
            regex_cache.emplace(pattern, std::regex(pattern, std::regex::icase));
        }
        const auto& re = regex_cache[pattern];

        if (std::regex_match(addr_expr, re)) {
            return type;
        }
    }

    // 
    return TypeOperand::kIdentShort;
}
void CreateSegmentTable(const std::string name, SizeActiveSeg& value) {
    LogicalSegment seg = { name, 32, value.local_size };
    segment_table.push_back(seg);
}
void CalculateMemoryUsage(const std::vector<Lexeme>& lexemes, SizeActiveSeg& value) {
    int totalBytes = 0; // 
    int typeSize = 0;
    for (const auto& lexeme : lexemes) {
        //  
        if (lexeme.lexeme == "db") {
            typeSize = 1;
        }
        else if (lexeme.lexeme == "dw") {
            typeSize = 2;
        }
        else if (lexeme.lexeme == "dd") {
            typeSize = 4;
        }
        if (typeSize > 0) {
            int count = 1;
            if (lexeme.type.const_type == TypeConst::kTextConst) {
                count = lexeme.length - 2;
            }
            totalBytes = typeSize * count;
        }
    }
    value.local_size += totalBytes;
    //line_info.size
}
int FindLexemePattern(std::vector<Type>& lexemes_pair)
{
    auto it = std::find_if(lexemePatterns.begin(), lexemePatterns.end(),
        [&lexemes_pair](const std::pair<const int, std::vector<Type>>& pair) {
            return pair.second == lexemes_pair;
        });
    return (it != lexemePatterns.end()) ? it->first : -1;
}
int CreateIdentifierTable(int code, const std::vector<Lexeme>& lexemes, SizeActiveSeg& value) {
    std::string identifierName = lexemes[0].lexeme;
    IdentifierType type_ident = IdentifierType::kUnknown;
    std::string identifierSegment;
    switch (code) {
        case 101:
            if (lexemes[1].lexeme == "segment" && code == 101 && value.seg == "") {
                //identifierType = "NAME_SEGMENT";
                type_ident = IdentifierType::kNameSeg;
                value.active_seg += 1;
                value.seg = lexemes[0].lexeme;
            }
            else if (lexemes[1].lexeme == "ends") {
                if (value.seg == lexemes[0].lexeme) {
                    CreateSegmentTable(identifierName, value);
                    value.active_seg = 0;
                    value.seg = "";
                    value.local_size = 0;
                }
            }
            else {
                return -2;
            }
            break;
        case 102:
        case 103:
        case 104:
        case 105:
            type_ident = IdentifierType::kConst;
            CalculateMemoryUsage(lexemes, value);
            break;
        case 106:
            if (lexemes[1].lexeme == ":") {
                type_ident = IdentifierType::kLabel;
                identifierSegment = segment_table.back().name;
            }
            break;
    }
    if (identifier_table.find(identifierName) == identifier_table.end()) {
        // 
        vector_identfiers.push_back(identifierName);
        identifier_table[identifierName] = { type_ident, value.seg, value.local_size };
        value.done = 1;
    }
    else {
        return -3;
    }
    return 0;
}

std::optional<TypeAddress> DetectAddressType(const std::string& addr_expr, SizeActiveSeg& sizes) {
    static std::unordered_map<std::string, std::regex> regex_cache;

    for (const auto& [pattern, type] : kRegexToAddressType) {
        if (regex_cache.find(pattern) == regex_cache.end()) {
            regex_cache.emplace(pattern, std::regex(pattern, std::regex::icase));
        }
        const auto& re = regex_cache[pattern];

        if (std::regex_match(addr_expr, re)) {
            return type;
        }
    }
    GlobalErrorHandler.AddError(sizes.line, ErrorCode::kUnknownInstruction);
    return std::nullopt; 
}

int CalculateBytesForNumber(int number) {
    if (number >= -128 && number <= 127) {
        return 1; // 
    }
    else if (number >= -32768 && number <= 32767) {
        return 2; // 
    }
    else if (number >= -2147483648 && number <= 2147483647) {
        return 4; // 
    }
}

// 
int CheckConstAndCalculate(const std::string& input) {
    int number = 0;

    if (input.back() == 'b') { // Двійкова система
        std::string binary = input.substr(0, input.size() - 1); 
        std::bitset<32> bits(binary); 
        number = static_cast<int>(bits.to_ulong()); 
        
        return CalculateBytesForNumber(number);
    }
    else if (input.back() == 'h') { 
        std::string hex = input.substr(0, input.size() - 1); 
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> number; 
        return CalculateBytesForNumber(number);
    }
    else { // 
        number = std::stoi(input); 
        return CalculateBytesForNumber(number);

    }
}

std::vector<TypeOperand> FindOperands(const std::vector<Lexeme>& lexemes, std::optional<TypeAddress>& values, SizeActiveSeg& sizes) {
    std::vector<TypeOperand> result;
    std::string addr;
    int countsymbol = 0;
    int next_index = 0;
    for (int i = 1; i < lexemes.size(); i++) {
        if (lexemes[i].type.main_type == Type::kReg32) {
            result.push_back(TypeOperand::kReg32);
            next_index = i;
            break;
        }
        else if (lexemes[i].type.main_type == Type::kReg8) {
            result.push_back(TypeOperand::kReg8);
            next_index = i;
            break;
        }
        else if (lexemes[i].type.main_type == Type::kConst) {
            int val = CheckConstAndCalculate(lexemes[i].lexeme);
            switch (val) {
            case 1:
                result.push_back(TypeOperand::kConstByte);
                next_index = i;
                break;
            case 2:
            case 4:
                result.push_back(TypeOperand::kConstWord);
                next_index = i;
                break;
            default:
                break;
            }
        }
        else if (lexemes[i].type.main_type == Type::kUserIdent) {
            result.push_back(GetTypeJump(lexemes));
            return result;
        }
        else if (lexemes[i].lexeme == "[") {
            //int j;
            addr = ParcerLexemAddr(lexemes, i);
            auto type = DetectAddressType(addr, sizes);
            if (type.has_value()) {
                result.push_back(TypeOperand::kAddr);
                values = type;
            }
            next_index = i;
            break;
        }
    }
    if (lexemes.size() - next_index == 1) {
        return result;
    }
    else if(lexemes[next_index+1].lexeme == "," && lexemes.size() - next_index >= 2)
    {
        for (int i = next_index+2; i < lexemes.size(); i++) {
            if (lexemes[i].type.main_type == Type::kReg32) {
                result.push_back(TypeOperand::kReg32);
            }
            else if (lexemes[i].type.main_type == Type::kReg8) {
                result.push_back(TypeOperand::kReg8);
            }
            else if (lexemes[i].type.main_type == Type::kConst) {
                int val = CheckConstAndCalculate(lexemes[i].lexeme);
                switch (val) {
                case 1:
                    result.push_back(TypeOperand::kConstByte);
                    break;
                case 2:
                case 4:
                    result.push_back(TypeOperand::kConstWord);
                    break;
                default:
                    break;
                }
            }
            else if (lexemes[i].lexeme == "[") {
                addr = ParcerLexemAddr(lexemes, i);
                auto type = DetectAddressType(addr, sizes);
                if (type.has_value()) {
                    result.push_back(TypeOperand::kAddr);
                    values = type;
                }
                next_index = i;
                break;
            }
            else {

            }
            //else if(lexemes[i].lexeme == "," && )
        }
        return result;
    }
    else {

    }

}
std::optional<ConstSizeOperand> FindConstSize(const std::vector<Lexeme>& lexemes) {
    if (lexemes[1].type.main_type == Type::kConst) {
        int val = CheckConstAndCalculate(lexemes[1].lexeme);
        switch (val) {
        case 1:
            return ConstSizeOperand::kByte;
            break;
        case 2:
            return ConstSizeOperand::kDoubleWord;
            break;
        case 4:
            return ConstSizeOperand::kWord;
            break;
        default:
            break;
        }
    }
    else if (lexemes[lexemes.size() - 1].type.main_type == Type::kConst) {
        int val = CheckConstAndCalculate(lexemes[lexemes.size() - 1].lexeme);
        switch (val) {
        case 1:
            return ConstSizeOperand::kByte;
            break;
        case 2:
            return ConstSizeOperand::kDoubleWord;
            break;
        case 4:
            return ConstSizeOperand::kWord;
            break;
        default:
            break;
        }
    }
}
InfoAddr GetInfoAddr(const std::vector<Lexeme>& lexemes) {
    InfoAddr result;
    int i = 0;
    for (i; i < lexemes.size(); i++) {
        if (lexemes[i].lexeme == "[") {
            while (lexemes[i].lexeme != "]") {
                if (lexemes[i].type.main_type == Type::kReg32) {
                    auto reg = kStringToReg32.find(lexemes[i].lexeme);
                    result.reg_addr.push_back(reg->second);
                }
                else if (lexemes[i].type.const_type == TypeConst::kConst10) {
                    result.const_addr.push_back(std::stoi(lexemes[i].lexeme));
                }
                i++;
            }
        }
    }
    return result;
}
std::optional<SegReg> FindSegReg(const std::vector<Lexeme>& lexemes) {
    for (int i = 0; i < lexemes.size(); i++) {
        if (lexemes[i].lexeme == "[" && lexemes[i - 1].lexeme == ":" && lexemes[i - 2].type.main_type == Type::kSegReg) {
            auto seg = kStringToSegReg.find(lexemes[i - 2].lexeme);
            if (seg != kStringToSegReg.end())
                return seg->second;
        }
    }
    return std::nullopt;
}
StructWithCom FillStructWithCom(const std::vector<Lexeme>& lexemes, SizeActiveSeg& value) {
    StructWithCom structure;
    auto com = kStringToCommand.find(lexemes[0].lexeme);
    if (com == kStringToCommand.end()) {
        GlobalErrorHandler.AddError(value.line, ErrorCode::kUnknownInstruction);
    }
    else {
        structure.command = com->second;
    }
    structure.operands = FindOperands(lexemes, structure.type_addr, value);
    if (structure.type_addr) {
        structure.addr = GetInfoAddr(lexemes);
        structure.seg_reg = FindSegReg(lexemes);
    }
    return structure;
}

std::optional<CommandInfo> GetCommandInfo(const StructWithCom& structure) {
    auto outer_it = kCommands.find(structure.command);
    if (outer_it == kCommands.end()) {
        std::cout << "NotFound1";
        return std::nullopt; 
    }

    const auto& inner_map = outer_it->second;
    auto inner_it = inner_map.find(structure.operands);
    if (inner_it == inner_map.end()) {
        std::cout << "NotFound2";
        return std::nullopt; 
    }

    return inner_it->second; 
}

void CalculateOffsetString(std::optional<CommandInfo>& info, SizeActiveSeg& sizes, StructWithCom &structures) {
    sizes.local_size += 1;
    if (info->byte_modrm == true) {
        sizes.local_size += 1;
        if (structures.type_addr.has_value()) {
            if (structures.seg_reg.has_value()) {
                sizes.local_size += 1;
            }
            switch (*structures.type_addr) {
            case TypeAddress::kType2:
                sizes.local_size += 1; 
                break;

            case TypeAddress::kType3:
                sizes.local_size += 1; 
                break;

            case TypeAddress::kType4:
                sizes.local_size += 1; 
                break;

            case TypeAddress::kType5:
                sizes.local_size += 1; 
                sizes.local_size += 1; 
                break;

            case TypeAddress::kType6:
                sizes.local_size += 1; 
                break;

            case TypeAddress::kType7:
                sizes.local_size += 1; 
                sizes.local_size += 1; 
                break;
            default:
                break;
            }
        }
    }
    if (std::any_of(structures.operands.begin(), structures.operands.end(),
        [](TypeOperand op) { return op == TypeOperand::kConstByte || op == TypeOperand::kConstWord; })) {
        switch (info->size_const) {
        case ConstSizeOperand::kByte:
            sizes.local_size += 1;
            break;
        case ConstSizeOperand::kDoubleWord:
            sizes.local_size += 2;
            break;
        case ConstSizeOperand::kWord:
            sizes.local_size += 4;
            break;
        default:
            break;
        }
    }
    if (std::any_of(structures.operands.begin(), structures.operands.end(),
        [](TypeOperand op) { return op == TypeOperand::kIdentFar; })) {
        sizes.local_size += 6;
    }
    else if (std::any_of(structures.operands.begin(), structures.operands.end(),
        [](TypeOperand op) { return op == TypeOperand::kIdentShort; })) {
        sizes.local_size += 1;
    }
    else if (std::any_of(structures.operands.begin(), structures.operands.end(),
        [](TypeOperand op) { return op == TypeOperand::kIdentNear; })) {
        sizes.local_size += 2;
    }
}

void First_View(const std::vector<Lexeme>& lexemes, std::vector<Type>& lexemes_keys, SizeActiveSeg& value) {
    //value.local_size = 0;
    if (lexemes.empty()) {
        return;
    }
    if (std::any_of(lexemes_keys.begin(), lexemes_keys.end(), [](const auto& lexeme) {
        return lexeme == Type::kUnknown;}))
    {
        GlobalErrorHandler.AddError(value.line, ErrorCode::kUnknownInstruction);
    }
    int code = 0;
    int error_code = 0;
    StructWithCom values;
    code = FindLexemePattern(lexemes_keys);
    if (code > 0) {
        if (CreateIdentifierTable(code, lexemes, value) != 0) {
        }
    }
    if (code == -1 && lexemes[0].type.main_type == Type::kUserIdent) {
        GlobalErrorHandler.AddError(value.line, ErrorCode::kUnknownInstruction);
        return;
    }
    if (lexemes[0].lexeme == "assume") {
        if (value.active_seg == 1) {
            FillSegmentTable(lexemes);
            value.done = 1;
        }
        else {
            GlobalErrorHandler.AddError(value.line, ErrorCode::kUnknownInstruction);
        }
    }
    if (value.active_seg == 1 && value.done == 0) {
        values = FillStructWithCom(lexemes, value);
        std::optional<CommandInfo> info_command = GetCommandInfo(values);
        if (info_command == std::nullopt) {
            value.done = 0;
            GlobalErrorHandler.AddError(value.line, ErrorCode::kIncorrectOperands);
            return;
        }
        CalculateOffsetString(info_command, value, values);

        //std::cout << value.local_size << " ";
    }
    value.done = 0;
}