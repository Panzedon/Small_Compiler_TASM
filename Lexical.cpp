#include "Lexical.h"
std::string toLowerCase(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), ::tolower);
    return lowerStr;
}

std::string identifyType(const std::string& token) {
    std::string lowerToken = toLowerCase(token);

    // Директиви
    if (lowerToken == "segment" || lowerToken == "ends" || lowerToken == "end" || lowerToken == "db" || lowerToken == "dw" || lowerToken == "dd" || lowerToken == "assume") {
        return "Ідентифікатор директиви";
    }
    // Машинні інструкції
    if (lowerToken == "popad" || lowerToken == "push" || lowerToken == "neg" ||
        lowerToken == "adc" || lowerToken == "sbb" || lowerToken == "lds" ||
        lowerToken == "mov" || lowerToken == "and" || lowerToken == "jnae" ||
        lowerToken == "jmp") {
        return "Мнемокод машиної інструкції";
    }

    // Регістри сегментів
    if (lowerToken == "cs" || lowerToken == "ds" || lowerToken == "es" || lowerToken == "gs") {
        return "Сегментний регістр";
    }

    // Регістри загального призначення
    if (lowerToken == "eax" || lowerToken == "ebx" || lowerToken == "ecx" || lowerToken == "edx" ||
        lowerToken == "esi" || lowerToken == "edi") {
        return "32-розрядний регістр загального призначення";
    }

    if (lowerToken == "ah" || lowerToken == "al" ||
        lowerToken == "bh" || lowerToken == "bl" ||
        lowerToken == "ch" || lowerToken == "cl" ||
        lowerToken == "dh" || lowerToken == "dl") {
        return "Регістри даних";
    }

    if ((token.front() == '"' && token.back() == '"') || (token.front() == '\'' && token.back() == '\'')) {
        return "Текстова константа";
    }

    // Розрядність даних
    if (lowerToken == "byte" || lowerToken == "word" || lowerToken == "dword") {
        return "Розрядність даних";
    }
    if (lowerToken == "far") {
        return "Специфікатор адресації";
    }
    // Оператор визначення типу
    if (lowerToken == "ptr") {
        return "Оператор визначення типу";
    }
    // Ідентифікатори
    if (std::isalpha(token[0]) && token.size() <= 7 && std::all_of(token.begin(), token.end(), [](char ch) { return std::isalnum(ch); })) {
        return "Ідентифікатор користувача або не визначений";
    }
    if (token[0] == '-' && token.size() > 2 && token.back() == 'd' && std::all_of(token.begin() + 1, token.end() - 1, ::isdigit)) {
        return "Десяткова константа";
    }
    // Додаткові десяткові константи з `d`
    if (token.size() > 1 && token.back() == 'd' && std::all_of(token.begin(), token.end() - 1, ::isdigit)) {
        return "Десяткова константа";
    }
    if (token[0] == '-' && std::all_of(token.begin() + 1, token.end(), ::isdigit)) {
        return "Десяткова константа";
    }
    // Шістнадцяткові константи
    if (lowerToken.find("h") != std::string::npos) {
        return "Шістнадцяткова константа";
    }
    // Двійкові константи
    if (lowerToken.find("b") != std::string::npos) {
        return "Двійкова константа";
    }
    // Десяткові константы
    if (std::isdigit(token[0]) && std::all_of(token.begin(), token.end(), ::isdigit)) {
        return "Десяткова константа";
    }
    // Текстові константи
    if ((token.front() == '"' && token.back() == '"') || (token.front() == '\'' && token.back() == '\'')) {
        return "Текстова константа";
    }
    // Односимвольні оператори
    if (token == ":" || token == "," || token == "+" || token == "[" || token == "]" || token == "*") {
        return "Односимвольна";
    }
    // Невідомий тип
    return "Невідомо";

}

std::vector<Lexeme> lexicalAnalyzer(const std::string& line) {
    std::vector<Lexeme> lexemes;
    std::string token;
    int numb = 0;
    for (char ch : line) {
        if (std::isspace(ch) || ch == ':' || ch == ',' || ch == '+' || ch == '[' || ch == ']') {
            if (!token.empty()) {
                numb++;
                Lexeme lexeme = { numb++, token, static_cast<int>(token.length()), identifyType(token) };
                lexemes.push_back(lexeme);
                token.clear();
            }
            if (!std::isspace(ch)) {
                std::string singleChar(1, ch);
                numb++;
                lexemes.push_back({ numb, singleChar, 1, identifyType(singleChar) });
            }
        }
        else {
            token += ch;
            numb++;
        }
    }
    if (!token.empty()) {
        Lexeme lexeme = { numb, token, static_cast<int>(token.length()), identifyType(token) };
        lexemes.push_back(lexeme);
    }
    return lexemes;
}

void generateStructureTable(std::ofstream& outputFile, const std::vector<Lexeme>& lexemes, int rowNumber) {
    int labelIndex = 0, labelFirst = 0, labelCount = 0;
    int mnemonicIndex = 0, mnemonicFirst = 0, mnemonicCount = 0;
    int operand1Index = 0, operand1First = 0, operand1Count = 0;
    int operand2Index = 0, operand2First = 0, operand2Count = 0;
    int operand3Index = 0, operand3First = 0, operand3Count = 0;

    for (size_t i = 0; i < lexemes.size(); i++) {
        if (i == 0 && lexemes[i].type == "Ідентифікатор користувача або не визначений") {
            // Перша лексема — мітка або ім'я
            labelIndex = 1;
            labelCount = 1;
        }
        else if ((labelCount > 0 && i == 1) || (labelCount == 0 && i == 0)) {
            // Якщо лексема - машинна інструкція
            if (lexemes[i].type == "Мнемокод машиної інструкції" || lexemes[i].type == "Ідентифікатор директиви") {
                mnemonicIndex = i + 1;
                mnemonicCount = 1;
            }
        }
        else if (mnemonicIndex != 0) {
            // інші лексеми - операнди
            operand1Index = i + 1;
            for (i; i < lexemes.size(); i++) {
                if (lexemes[i].lexeme == ",") {
                    operand2Index = operand1Index + 2;
                    break;
                }
                operand1Count += 1;
            }
            if (lexemes.size() - i == 1) {
                break;
            }
            i++;
            for (i; i < lexemes.size(); i++) {
                if (lexemes[i].lexeme == ",") {
                    operand3Index = operand2Index + 2;
                    break;
                }
                operand2Count += 1;
            }
            if (lexemes.size() - i == 1) {
                break;
            }
            i++;
            for (i; i < lexemes.size(); i++)
            {
                operand3Count += 1;
            }
        }
    }
    // Формуємо таблицю
    /*outputFile << "Відповідний рядок таблиці структур\n";
    outputFile << std::left
        << std::setw(20) << "Поле міток (імені)"
        << std::setw(55) << "Поле мнемокоду"
        << std::setw(52) << "1-й операнд"
        << std::setw(52) << "2-й операнд"
        << std::setw(26) << "3-й операнд" << "\n";

    outputFile << std::left
        << std::setw(20) << "N лексеми поля"
        << std::setw(30) << "N першої лексеми поля"
        << std::setw(25) << "Кількість лексем поля"
        << std::setw(26) << "N першої лексеми операнда"
        << std::setw(26) << "Кількість лексем операнда"
        << std::setw(26) << "N першої лексеми операнда"
        << std::setw(26) << "Кількість лексем операнда"
        << std::setw(26) << "N першої лексеми операнда"
        << std::setw(26) << "Кількість лексем операнда" << "\n";

    outputFile << std::left
        << std::setw(20) << labelIndex
        << std::setw(30) << mnemonicIndex
        << std::setw(25) << mnemonicCount
        << std::setw(26) << (operand1Index > 0 ? operand1Index : 0)
        << std::setw(26) << operand1Count
        << std::setw(26) << (operand2Index > 0 ? operand2Index : 0)
        << std::setw(26) << operand2Count
        << std::setw(26) << (operand3Index > 0 ? operand3Index : 0)
        << std::setw(26) << operand3Count << "\n";

    outputFile << "-------------------------" << rowNumber << "-------------------------\n\n\n";*/
}