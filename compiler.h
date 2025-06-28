#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>
#include <iomanip> // For formatted output
#include <algorithm> // For toLowerCase
#include <unordered_map>
#include <stdexcept>
#include <bitset>
#include <sstream>
#include "Listing/lexical.h"
#include "Listing/first_view.h"
#include "Listing/error_handler.h"
#include "Listing/second_view.h"

//SizeActiveSeg values = { 0, 0, 0 , "" };
//std::vector<LineInfo> lines_info;
//
//// Trim leading whitespace
//std::string trimLeft(const std::string& str) {
//    size_t start = str.find_first_not_of(" \t");
//    return (start == std::string::npos) ? "" : str.substr(start);
//}
extern SizeActiveSeg values;
extern std::vector<LineInfo> lines_info;
int CallCompiler(std::string input_file_name);