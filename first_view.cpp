#include "first_view.h"
#include "Second_view.h"

int Active_seg = 0;
int local_size = 0;
int global_size = 0;
int already = 0;

std::vector<SegmentRegisters> SegmentRegist = { {"cs", "Nothing"}, {"ds", "Nothing"}, {"ss", "Nothing"},
    {"es", "Nothing"}, {"gs", "Nothing"}, {"fs", "Nothing"} };

std::vector<Identifier> identifierTable = {};
std::vector<LogicalSegment> segmentTable = {};

void FillSegmentTable(const std::vector<Lexeme>& lexemes) {
    for (size_t i = 1; i < lexemes.size(); ++i) {
        const Lexeme& lexeme = lexemes[i];
        if (lexeme.type == "Сегментний регістр") {
            for (int j = 0; j < 6; j++) {
                if (lexeme.lexeme == SegmentRegist[j].regis) {
                    if (lexemes[i + 2].type == "Ідентифікатор користувача або не визначений") {
                        SegmentRegist[j].target = lexemes[i + 2].lexeme;
                        i = i + 2;
                    }
                }
            }
        }
    }
}

void CreateSegmentTable(const std::string name) {
    Active_seg = 1;
    LogicalSegment seg = { name, 32, local_size };
    segmentTable.push_back(seg);
}
void CalculateMemoryUsage(const std::vector<Lexeme>& lexemes) {
    int totalBytes = 0; // Загальна пам'ять для строки
    int typeSize = 0;
    for (const auto& lexeme : lexemes) {

        // Визначаємо тип розміру 
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

            if (lexeme.type == "Текстова константа") {
                count = lexeme.length - 2;
            }
            else if (lexeme.type == "Десяткова константа" ||
                lexeme.type == "Шістнадцяткова константа" ||
                lexeme.type == "Двійкова константа") {
                count = 1;
            }
            totalBytes = typeSize * count;
        }
    }
    //local_size = totalBytes;
    global_size += totalBytes;

}
void createIdentifierTable(const std::vector<Lexeme>& lexemes) {
    if (lexemes.empty()) {
        return; 
    }
    for (const auto& id : lexemes) {
        if (id.type == "Невідомо") {

            error = "Error";
            return;
        }
    }
    const Lexeme& firstLexeme = lexemes[0];

    
    if (firstLexeme.type != "Ідентифікатор користувача або не визначений") {
        return; 
    }

    std::string identifierName = firstLexeme.lexeme;
    std::string identifierType;
    std::string identifierSegment;

    
    if (lexemes.size() > 1) {
        const Lexeme& secondLexeme = lexemes[1];
        if (secondLexeme.type == "Ідентифікатор директиви" && secondLexeme.lexeme == "segment") {
            identifierType = "Ім'я логічного сегмента";
            int sig = 0;
            for (const auto& segments : segmentTable) {
                if (segments.name == identifierName) {
                    sig = 1;
                }
            }
            if (sig == 0) {
                CreateSegmentTable(identifierName);
            }
        }
        else if (secondLexeme.type == "Ідентифікатор директиви" &&
            (secondLexeme.lexeme == "db" || secondLexeme.lexeme == "dw" || secondLexeme.lexeme == "dd")) {
            identifierType = "Ім'я змінної";
            CalculateMemoryUsage(lexemes);
            already = 1;
            identifierSegment = segmentTable.back().name;

        }
        else if (secondLexeme.lexeme == ":") {
            identifierType = "Мітка";
            identifierSegment = segmentTable.back().name;
        }
        else if (secondLexeme.lexeme == "proc") {
            identifierType = "Ім'я процедури";
        }
        else if (secondLexeme.lexeme == "ends") {
            Active_seg = 0;
            local_size = global_size;
            global_size = 0;
        }
        else {
            global_size = reserv;
            error = "Error";
            already = 0;
            return;
        }
    }
    else {
        identifierType = "Ідентифікатор користувача"; 
    }

    // Перевірка на дублювання
    auto it = std::find_if(identifierTable.begin(), identifierTable.end(),
        [&identifierName](const Identifier& id) { return id.name == identifierName; });

    if (it == identifierTable.end()) {
        // Додаємо ідентифікатор в таблицю
        identifierTable.push_back({ identifierName, identifierType, identifierSegment, global_size });
    }
}
int calculateBytes(int number) {
    if (number >= -128 && number <= 127) {
        return 1; // 1 байт для 8-бітних чисел
    }
    else if (number >= -32768 && number <= 32767) {
        return 2; // 2 байта для 16-бітних чисел
    }
    else if (number >= -2147483648 && number <= 2147483647) {
        return 4; // 4 байта для 32-бітних чисел
    }
}

// Функція перевірки числа в різних системах численнях
int checkNumbers(const std::string& input) {
    int number = 0;

    if (input.back() == 'b') { // Двійкова система
        std::string binary = input.substr(0, input.size() - 1); 
        std::bitset<32> bits(binary); 
        number = static_cast<int>(bits.to_ulong()); 
        //std::cout << number << std::endl;
        return calculateBytes(number);
    }
    else if (input.back() == 'h') { 
        std::string hex = input.substr(0, input.size() - 1); 
        std::stringstream ss;
        ss << std::hex << hex;
        ss >> number; 
        //std::cout << number << std::endl;
        return calculateBytes(number);
    }
    else { // Десяткова система
        number = std::stoi(input); 
        //std::cout << number << std::endl;
        return calculateBytes(number);

    }
}
void CalculateCommandMemory(const std::vector<Lexeme>& lexemes) {
    int totalBytes = 0;
    int count_small = 0;
    int count_big = 0;
    int value = -1;
    int we_have_and = 0;
    int we_have_const = 0;
    int sig = 0;
    
    if (lexemes[lexemes.size() - 1].type == "Двійкова константа" ||
        lexemes[lexemes.size() - 1].type == "Шістнадцяткова константа" ||
        lexemes[lexemes.size() - 1].type == "Десяткова константа") {

        value = checkNumbers(lexemes[lexemes.size() - 1].lexeme);
        //std::cout << value << std::endl;
        totalBytes += value; 
        we_have_const = 1;
        sig = 1;
    }
    else if (lexemes.size() == 2 && lexemes[0].lexeme == "jnae" && lexemes[1].type == "Ідентифікатор користувача або не визначений") {
        totalBytes = 2;
        local_size = totalBytes;
        global_size += local_size;

        return;
    }
    for (size_t i = 0; i < lexemes.size(); ++i) {
        const Lexeme& lexeme = lexemes[i];

        if (lexeme.type == "Мнемокод машиної інструкції") {
            if (lexeme.lexeme == "and") {
                we_have_and = 1;
            }

            totalBytes += 1; 
            if (lexemes.size() == 2 && lexemes[1].type == "32-розрядний регістр загального призначення") {
                break;

            }
        }


        else if (lexeme.type == "Розрядність даних" && lexemes[i + 1].type == "Оператор визначення типу") {
            if (we_have_and == 1 && we_have_const == 1) {
                if (value == 1) {
                    totalBytes += 1;
                    i++;
                }
                else if (value >= 2) {
                    totalBytes += 1 + (4 - value);
                    i++;
                }
            }
            else {
                totalBytes += 1; 
                i++;
            }
        }

        else if (lexeme.type == "Сегментний регістр") {
            totalBytes += 1;
        }

        else if (lexeme.type == "32-розрядний регістр загального призначення") {
            count_big++;
        }
        else if (lexeme.type == "Регістри даних") {
            count_small++; 
        }
        else if (lexeme.type == "Специфікатор адресації") {
            totalBytes += 6;
            break;
        }
        else if (lexeme.lexeme == "]") {
            if (lexemes[i - 1].lexeme == "0") {
                break;
            }
            else if (lexemes[i - 1].type == "Десяткова константа") {
                totalBytes++;
            }

        }
    }
    if (count_small == 2) {
        totalBytes++;
    }
    else if (count_big == 2 || count_big == 1) {
        totalBytes++;
    }
    else if (count_big == 3) {
        totalBytes += 2;
    }
    local_size = totalBytes;
    global_size += local_size;
}