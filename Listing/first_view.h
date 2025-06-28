#ifndef FIRST_VIEW_H
#define FIRST_VIEW_H

#include "lexical.h"
#include <bitset>
#include <sstream>
#include <unordered_map>

// Patterns for matching lexeme sequences (used for pattern recognition)
const std::vector<std::pair<int, std::vector<Type>>> lexemePatterns = {
    {101, {Type::kUserIdent, Type::kDirec}},
    {102, {Type::kUserIdent, Type::kDirec, Type::kConst}},
    {103, {Type::kUserIdent, Type::kDirec, Type::kConst}},
    {104, {Type::kUserIdent, Type::kDirec, Type::kConst}},
    {105, {Type::kUserIdent, Type::kDirec, Type::kConst}},
    {106, {Type::kUserIdent, Type::kSymb}},
};

// Structure that holds information about memory usage in the active segment
struct SizeActiveSeg {
    int global_size;
    int local_size;
    int active_seg;
    std::string seg;
    bool done = 0;
    int line;
};

// Structure representing data of an identifier
struct IdentifierData {
    IdentifierType type;     // Identifier type (e.g., label, variable)
    std::string segment;     // Name of the segment where it belongs
    int offset;              // Offset within the segment
};

// Global identifier table (name → data)
extern std::unordered_map<std::string, IdentifierData> identifier_table;

// Structure representing a logical segment
struct LogicalSegment {
    std::string name;       // Segment name
    int defaultSize;        // Default bit width (16 or 32 bits)
    int current_offset;     // Current offset ($)
};

// Structure for segment register assignments
struct SegmentRegisters {
    std::string regis;      // Segment register (e.g., DS, ES)
    std::string target;     // Assigned segment name
};

// Segment register assignment table
extern std::vector<SegmentRegisters> segment_regist;

// Logical segment table
extern std::vector<LogicalSegment> segment_table;

// List of known identifier names
extern std::vector<std::string> vector_identfiers;

// Fills the segment table with data from lexemes
void FillSegmentTable(const std::vector<Lexeme>& lexemes);

// Calculates memory usage for constants in a segment
void CalculateMemoryUsage(const std::vector<Lexeme>& lexemes, SizeActiveSeg& value);

// Calculates the number of bytes required to store an integer
int CalculateBytesForNumber(int number);

// Determines the type of constant and how many bytes it occupies
int CheckConstAndCalculate(const std::string& input);

// Performs the first pass over the source code
void First_View(const std::vector<Lexeme>& lexemes, std::vector<Type>& lexemes_keys, SizeActiveSeg& value);

// Retrieves command info if matching command and operand types found
std::optional<CommandInfo> GetCommandInfo(const StructWithCom& structure);

// Constructs a structure with command and its operands
StructWithCom FillStructWithCom(const std::vector<Lexeme>& lexemes, SizeActiveSeg& value);

// Searches for a segment register override in the lexeme list
std::optional<SegReg> FindSegReg(const std::vector<Lexeme>& lexemes);

// Extracts addressing information from a lexeme sequence
InfoAddr GetInfoAddr(const std::vector<Lexeme>& lexemes);

// Finds if a constant with an explicitly defined size is present
std::optional<ConstSizeOperand> FindConstSize(const std::vector<Lexeme>& lexemes);

// Analyzes operands and returns their types (register, memory, constant, etc.)
std::vector<TypeOperand> FindOperands(const std::vector<Lexeme>& lexemes, std::optional<TypeAddress>& values, SizeActiveSeg& sizes);

// Detects the addressing type from the address expression
std::optional<TypeAddress> DetectAddressType(const std::string& addr_expr, SizeActiveSeg& sizes);

// Creates an entry in the identifier table based on directive code and lexemes
int CreateIdentifierTable(int code, const std::vector<Lexeme>& lexemes, SizeActiveSeg& value);

// Matches a lexeme type sequence to a known pattern code
int FindLexemePattern(std::vector<Type>& lexemes_pair);

// Returns operand type for jump instructions
TypeOperand GetTypeJump(const std::vector<Lexeme>& lexemes);

// Parses lexemes to extract address expressions
std::string ParcerLexemAddr(const std::vector<Lexeme>& lexemes, int& point);

// Calculates the total offset a string will occupy (for directives like db, dw, etc.)
void CalculateOffsetString(std::optional<CommandInfo>& info, SizeActiveSeg& sizes, StructWithCom& structures);

#endif
