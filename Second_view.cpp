#include "second_view.h"
#include "commands_types.h"
std::vector<uint8_t> FindOffsetJump(const std::vector<Lexeme>& kLexemes, std::optional<CommandInfo>& info, SizeActiveSeg& sizes, StructWithCom& structures) {
    std::vector<uint8_t> result;
    int offset;
    auto val = identifier_table.find(kLexemes.back().lexeme);
    if (sizes.seg == val->second.segment) {
        offset = val->second.offset - (sizes.local_size);
        auto bytes = CountBytesFromConstant(offset, info->size_jump);
        result.insert(result.end(), bytes.begin(), bytes.end());
    }
    else {
        if (structures.operands[0] == TypeOperand::kIdentFar) {
            for (int i = 0; i < 6; ++i) {
                result.push_back(0x00);
            }
        }
    }
    return result;

}

ExactlyOperands FindExactlyOperands(const std::vector<Lexeme>& kLexemes, StructWithCom& structures) {
    ExactlyOperands result;
    for (int i = 0; i < kLexemes.size(); i++) {
        if (kLexemes[i].type.main_type == Type::kReg32) {
            auto reg32 = kStringToReg32.find(kLexemes[i].lexeme);
            result.operands.push_back(reg32->second);
        }
        else if (kLexemes[i].type.main_type == Type::kReg8) {
            auto reg8 = kStringToReg8.find(kLexemes[i].lexeme);
            result.operands.push_back(reg8->second);
        }
        else if (kLexemes[i].type.main_type == Type::kConst) {
            int number = 0;
            if (kLexemes[i].lexeme.back() == 'b') { // Двійкова система
                std::string binary = kLexemes[i].lexeme.substr(0, kLexemes[i].lexeme.size() - 1);
                std::bitset<32> bits(binary);
                number = static_cast<int>(bits.to_ulong());
            }
            else if (kLexemes[i].lexeme.back() == 'h') {
                std::string hex = kLexemes[i].lexeme.substr(0, kLexemes[i].lexeme.size() - 1);
                std::stringstream ss;
                ss << std::hex << hex;
                ss >> number;
            }
            else { 
                number = std::stoi(kLexemes[i].lexeme);
            }
            result.operands.push_back(number);
        }
        else if (kLexemes[i].lexeme == "[") {
            while (kLexemes[i].lexeme != "]") {
                i++;
            }
            result.operands.push_back(TypeOperand::kAddr);
        }
    }
    return result;
}

std::vector<uint8_t> CountBytesForCommands(const std::vector<Lexeme>& kLexemes, std::optional<CommandInfo>& info, SizeActiveSeg& sizes, StructWithCom& structures) {
    std::vector<uint8_t> result;
    uint8_t result_modrm;

    struct FillModrm {
        uint8_t mod : 2;
        uint8_t reg : 3;
        uint8_t rm : 3;
    };
    struct FillSib {
        uint8_t scale : 2;
        uint8_t index_reg : 3;
        uint8_t base_reg : 3;
    };
    FillModrm modrm;
    FillSib sib;
    ExactlyOperands com_operands = FindExactlyOperands(kLexemes, structures);
    if (structures.seg_reg.has_value()) {
        auto code_seg = kSegmentPrefixBytes.find(*structures.seg_reg);
        result.push_back(code_seg->second);
    }
    result.push_back(info->opcode);
    if (info->byte_modrm == 1) {
        if (info->opcode_in_modrm) {
            modrm.reg = info->extra_bits;
        }
        if (!structures.addr.has_value()) {
            modrm.mod = 0b11;
            if (info->opcode_in_modrm) {
                if (auto ptr = std::get_if<GeneralReg32>(&com_operands.operands[0])) {
                    auto reg32 = kCodesReg32.find(*ptr);
                    modrm.rm = reg32->second;
                }
                else if (auto ptr = std::get_if<GeneralReg8>(&com_operands.operands[0])) {
                    auto reg8 = kCodesReg8.find(*ptr);
                    modrm.rm = reg8->second;
                }
            }
            else {
                if (auto ptr1 = std::get_if<GeneralReg32>(&com_operands.operands[0])) {
                    auto reg1 = kCodesReg32.find(*ptr1);
                    modrm.reg = reg1->second;
                    if (com_operands.operands.size() > 1) {
                        if (auto ptr2 = std::get_if<GeneralReg32>(&com_operands.operands[1])) {
                            auto reg2 = kCodesReg32.find(*ptr2);
                            modrm.rm = reg2->second;
                        }
                    }
                }
                else if (auto ptr1 = std::get_if<GeneralReg8>(&com_operands.operands[0])) {
                    auto reg1 = kCodesReg8.find(*ptr1);
                    modrm.reg = reg1->second;
                    if (com_operands.operands.size() > 1) {
                        if (auto ptr2 = std::get_if<GeneralReg8>(&com_operands.operands[1])) {
                            auto reg2 = kCodesReg8.find(*ptr2);
                            modrm.rm = reg2->second;
                        }
                    }
                }
            }
        }
        else {  
            if (structures.type_addr == TypeAddress::kType3 || structures.type_addr == TypeAddress::kType5 || 
                structures.type_addr == TypeAddress::kType7) {
                int displacement = CalculateBytesForNumber(structures.addr->const_addr.back());
                switch (displacement) {
                case 1:
                    modrm.mod = 0b01;
                    break;
                case 2:
                    modrm.mod = 0b10;
                    break;
                default:
                    // SetError
                    break;
                }
            }
            else {
                modrm.mod = 0b00;
            }
            if (structures.addr->reg_addr.size() == 2) {
                modrm.rm = 0b100;
                if (auto ptr = std::get_if<GeneralReg32>(&com_operands.operands[0])) {
                    auto reg32 = kCodesReg32.find(*ptr);
                    modrm.reg = reg32->second;
                }
                else if (auto ptr = std::get_if<GeneralReg8>(&com_operands.operands[0])) {
                    auto reg32 = kCodesReg8.find(*ptr);
                    modrm.reg = reg32->second;
                }
                else if (com_operands.operands.size() > 1) {
                    if (auto ptr = std::get_if<GeneralReg32>(&com_operands.operands[1])) {
                        auto reg32 = kCodesReg32.find(*ptr);
                        modrm.reg = reg32->second;
                    }
                    else if (auto ptr = std::get_if<GeneralReg8>(&com_operands.operands[1])) {
                        auto reg32 = kCodesReg8.find(*ptr);
                        modrm.reg = reg32->second;
                    }
                }
            }
        }
        result.push_back((modrm.mod << 6) | (modrm.reg << 3) | modrm.rm);
    }
    if (modrm.rm == 0b100) {
        auto reg1 = kCodesReg32.find(structures.addr->reg_addr[0]);
        auto reg2 = kCodesReg32.find(structures.addr->reg_addr[1]);
        sib.base_reg = reg1->second;
        sib.index_reg = reg2->second;
        if (structures.type_addr == TypeAddress::kType7 || structures.type_addr == TypeAddress::kType6) {
            switch (structures.addr->const_addr[0]) {
            case 2:
                sib.scale = 0b01;
                break;
            case 4:
                sib.scale = 0b10;
                break;
            case 8:
                sib.scale = 0b11;
                break;
            }
        }
        result.push_back((sib.scale << 6) | (sib.index_reg << 3) | sib.base_reg);
    }
    if (modrm.mod == 0b01) {
        auto bytes = CountBytesFromConstant(structures.addr->const_addr.back(), 1);
        result.insert(result.end(), bytes.begin(), bytes.end());
    }
    if (com_operands.operands.size() > 1) {
        if (auto ptr = std::get_if<int>(&com_operands.operands[1])) {
            int value = *ptr;
            if (structures.operands.back() == TypeOperand::kConstByte) {
                auto bytes = CountBytesFromConstant(value, 1);
                result.insert(result.end(), bytes.begin(), bytes.end());
            }
            else if (structures.operands.back() == TypeOperand::kConstWord) {
                auto bytes = CountBytesFromConstant(value, 4);
                result.insert(result.end(), bytes.begin(), bytes.end());
            }
        }
    }
    if (info->is_jump) {
        auto bytes = FindOffsetJump(kLexemes, info, sizes, structures);
        result.insert(result.end(), bytes.begin(), bytes.end());
    }
    return result;
}

template <typename T>
std::vector<uint8_t> CountBytesFromConstant(T value, int size) {
    std::vector<uint8_t> bytes;
    for (int i = 0; i < size; ++i) {

        bytes.push_back(static_cast<uint8_t>(value & 0xFF));
        value >>= 8;
    }
    return bytes;
}

std::vector<uint8_t> CountBytesForDirectives(const std::vector<Lexeme>& kLexemes, SizeActiveSeg& value) {
    std::vector<uint8_t> result;
    int size = 0;
    if (kLexemes[1].lexeme == "db") {
        size = 1;
    }
    else if (kLexemes[1].lexeme == "dw") {
        size = 2;
    }
    else if (kLexemes[1].lexeme == "dd") {
        size = 4;
    }
    else {
        GlobalErrorHandler.AddError(value.line, ErrorCode::kIncorrectOperands);
    }
    std::string val = kLexemes[2].lexeme;
    switch (kLexemes[2].type.const_type) {
    case TypeConst::kConst10: {
        int64_t number = std::stoll(val);
        auto bytes = CountBytesFromConstant(number, size);
        result.insert(result.end(), bytes.begin(), bytes.end());
        break;
    }
    case TypeConst::kConst16: {
        std::string hexstr = val;
        if (!hexstr.empty() && (hexstr.back() == 'h' || hexstr.back() == 'H')) {
            hexstr.pop_back();
        }
        int64_t number = std::stoll(hexstr, nullptr, 16);
        auto bytes = CountBytesFromConstant(number, size);
        result.insert(result.end(), bytes.begin(), bytes.end());
        break;
    }
    case TypeConst::kConst2: {
        std::string binstr = val;
        if (!binstr.empty() && (binstr.back() == 'b' || binstr.back() == 'B')) {
            binstr.pop_back();
        }
        int64_t number = std::stoll(binstr, nullptr, 2);
        auto bytes = CountBytesFromConstant(number, size);
        result.insert(result.end(), bytes.begin(), bytes.end());
        break;
    }
    case TypeConst::kTextConst: {
        if (val.size() >= 2 && val.front() == '\'' && val.back() == '\'') {
            for (size_t i = 1; i + 1 < val.size(); ++i) {
                result.push_back(static_cast<uint8_t>(val[i]));
            }
        }
        break;
    }
    }
    return result;
}
void CountBytesDirectives(int code, const std::vector<Lexeme>& kLexemes, SizeActiveSeg& value, LineInfo& line_info) {
    switch (code) {
    case 101:
        if (kLexemes[1].lexeme == "segment" && value.seg == "") {
            value.active_seg += 1;
            value.seg = kLexemes[0].lexeme;
        }
        else if (kLexemes[1].lexeme == "ends" && value.seg == kLexemes[0].lexeme) {
            value.active_seg = 0;
            value.seg = "";
            value.local_size = 0;
        }
        else {

        }
        break;
    case 102:
    case 103:
    case 104:
    case 105:
        line_info.bytes = CountBytesForDirectives(kLexemes, value);
        /*if (line_info.bytes.size() > line_info.size) {
            GlobalErrorHandler.AddError(value.line, ErrorCode::kDifferentSizes);
        }*/
        break;
    }
}
void SecondView(const std::vector<Lexeme>& kLexemes, std::vector<Type>& lexemes_keys, SizeActiveSeg& value, LineInfo &line_info) {
    if (kLexemes.empty()) {
        return;
    }
    if (std::any_of(lexemes_keys.begin(), lexemes_keys.end(), [](const auto& lexeme) {
        return lexeme == Type::kUnknown; }))
    {
        GlobalErrorHandler.AddError(value.line, ErrorCode::kUnknownInstruction);
    }
    int code = 0;
    int error_code = 0;
    StructWithCom values;
    code = FindLexemePattern(lexemes_keys);
    if (code > 0) {
        CountBytesDirectives(code, kLexemes, value, line_info);
    }
    else if (kLexemes[0].lexeme == "assume") {

        FillSegmentTable(kLexemes);
        value.done = 1;
    }
    else if (value.active_seg == 1 && value.done == 0) {
        values = FillStructWithCom(kLexemes, value);
        std::optional<CommandInfo> info_command = GetCommandInfo(values);
        if (info_command == std::nullopt) {
            value.done = 0;
            GlobalErrorHandler.AddError(value.line, ErrorCode::kIncorrectOperands);
            return;
        }
        CalculateOffsetString(info_command, value, values);
        line_info.bytes = CountBytesForCommands(kLexemes, info_command, value, values);
    }
    value.done = 0;
}
