#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
enum class ErrorCode {
    kUnknownInstruction,
    kUnexpectedToken,
    kUndefinedLabel,
    kDifferentSizes,
    kIncorrectOperands,
    kIncorrectAddr,

};

struct Error {
    int line;
    ErrorCode code;
    std::string message;
};

class ErrorHandler {

private:

    std::unordered_map< ErrorCode, std::string> ListError = {
        {ErrorCode::kDifferentSizes, "Different sizes of operands"},
        {ErrorCode::kIncorrectOperands, "Incorrect operands in command"},
        {ErrorCode::kIncorrectAddr, "Incorrect addresing"},
        {ErrorCode::kUndefinedLabel, "Undefined label"}
    };

    std::vector<Error> errors_;
public:
    bool HaveError();
    void AddError(int line, ErrorCode error_code);
    const std::vector<Error>& getErrors() const;
    void print(std::ostream& out = std::cout) const;
    void clear();
};

extern ErrorHandler GlobalErrorHandler;