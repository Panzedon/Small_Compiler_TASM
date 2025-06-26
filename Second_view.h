#ifndef SECOND_VIEW_H
#define SECOND_VIEW_H
#include "lexical.h"
#include "first_view.h"
#include <unordered_map>
#include <functional>
#include <variant>
#include "error_handler.h"
void SecondView(const std::vector<Lexeme>& kLexemes, std::vector<Type>& lexemes_keys, SizeActiveSeg& value, LineInfo& line_info);

template <typename T>
std::vector<uint8_t> CountBytesFromConstant(T value, int size);

#endif // !SECOND_VIEW_H
