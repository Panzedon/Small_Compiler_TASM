#include "lexical.h"
std::string ToLowerCase(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

TypeInfo IdentifyType(const std::string& token) {
    std::string lowerToken = ToLowerCase(token);

    if (directives.count(lowerToken)) return { Type::kDirec, TypeConst::kNone };
    if (instructions.count(lowerToken)) return { Type::kInstruc, TypeConst::kNone };
    if (segmentRegisters.count(lowerToken)) return { Type::kSegReg, TypeConst::kNone };
    if (generalRegisters.count(lowerToken)) return { Type::kReg32, TypeConst::kNone };
    if (dataRegisters.count(lowerToken)) return { Type::kReg8, TypeConst::kNone };
    if (dataSizes.count(lowerToken)) return { Type::kSize, TypeConst::kNone };
    if (typeOperators.count(lowerToken)) return { Type::kPtr, TypeConst::kNone };
    if (addressSpecifiers.count(lowerToken)) return { Type::kAddrSpec, TypeConst::kNone };
    if (singleCharOperators.count(token)) return { Type::kSymb, TypeConst::kNone };

    if ((token.front() == '"' && token.back() == '"') || (token.front() == '\'' && token.back() == '\'')) {
        return { Type::kConst, TypeConst::kTextConst };
    }

    if (std::isalpha(token[0]) && token.size() <= 7 && std::all_of(token.begin(), token.end(), [](char ch) { return std::isalnum(ch); })) {
        return { Type::kUserIdent, TypeConst::kTextConst };
    }

    static const std::regex decimalPattern(R"(^-?\d+(\d*d)?$)");
    static const std::regex hexPattern(R"(^[0-9A-Fa-f]+h$)");
    static const std::regex binaryPattern(R"(^[01]+b$)");

    if (std::regex_match(token, decimalPattern)) return { Type::kConst, TypeConst::kConst10 };
    if (std::regex_match(token, hexPattern)) return { Type::kConst, TypeConst::kConst16 };
    if (std::regex_match(token, binaryPattern)) return { Type::kConst, TypeConst::kConst2 };

    return { Type::kUnknown, TypeConst::kNone };
}

std::vector<Lexeme> LexicalAnalyzer(const std::string& line) {
    std::vector<Lexeme> lexemes;
    std::string token;
    int numb = 0;
    for (char ch : line) {
        if (std::isspace(ch) || ch == ':' || ch == ',' || ch == '+' || ch == '[' || ch == ']') {
            if (!token.empty()) {
                numb++;
                Lexeme lexeme = { numb++, token, static_cast<int>(token.length()), IdentifyType(token) };
                lexemes.push_back(lexeme);
                token.clear();
            }
            if (!std::isspace(ch)) {
                std::string singleChar(1, ch);
                numb++;
                lexemes.push_back({ numb, singleChar, 1, IdentifyType(singleChar) });
            }
        }
        else {
            token += ch;
            numb++;
        }
    }
    if (!token.empty()) {
        Lexeme lexeme = { numb, token, static_cast<int>(token.length()), IdentifyType(token) };
        lexemes.push_back(lexeme);
    }
    return lexemes;
}

std::vector<Type> ConvertLexemesToVector(const std::vector<Lexeme>& lexemes) {
    std::vector<Type> lexemeVector;
    for (const auto& lexeme : lexemes) {
        lexemeVector.emplace_back(lexeme.type.main_type); // Добавляем пару {тип, значение}
    }
    return lexemeVector;
}