#pragma once
#include <optional>
#include <unordered_map>
#include <variant>

// Enum class for lexical token types
enum class Type {
    kDirec,       // Directive
    kInstruc,     // Instruction
    kSegReg,      // Segment register
    kReg32,       // 32-bit general-purpose register
    kReg8,        // 8-bit general-purpose register
    kSize,        // Size specifier (e.g. byte, word)
    kPtr,         // Pointer specifier
    kAddrSpec,    // Addressing specification
    kSymb,        // Symbol
    kUserIdent,   // User-defined identifier
    kConst,       // Constant value
    kUnknown      // Unknown type
};

// Enum class for types of constants
enum class TypeConst {
    kConst10,     // Decimal constant
    kConst2,      // Binary constant
    kConst16,     // Hexadecimal constant
    kTextConst,   // Text/string constant
    kNone         // No constant
};

// Enum class for identifier types
enum class IdentifierType {
    kLabel,       // Label identifier
    kConst,       // Constant identifier
    kNameSeg,     // Segment name identifier
    kUnknown      // Unknown identifier type
};

// Enum class for supported commands/instructions
enum class Commands {
    kMOV,
    kADD,
    kNEG,
    kPOPAD,
    kPUSH,
    kADC,
    kSBB,
    kJNAE,
    kLDS,
    kAND,
    kJMP,
};

// Enum class for different addressing types
enum class TypeAddress {
    kType1, kType2, kType3, kType4, kType5, kType6, kType7
};

// Enum class for operand types
enum class TypeOperand {
    kReg32,       // 32-bit register operand
    kReg8,        // 8-bit register operand
    kConst,       // Constant operand
    kAddr,        // Memory address operand
    kIdent,       // Identifier operand
    kConstByte,   // Constant byte operand
    kConstWord,   // Constant word operand
    kIdentShort,  // Short identifier
    kIdentNear,   // Near identifier
    kIdentFar     // Far identifier
};

// Enum class for constant operand sizes
enum class ConstSizeOperand {
    kByte,       // 1 byte
    kDoubleWord, // 4 bytes
    kWord,       // 2 bytes
    kNone        // No size
};

// Enum class for general 32-bit registers
enum class GeneralReg32 {
    kEAX, kEBX, kECX, kEDX, kESI, kEDI
};

// Enum class for general 8-bit registers
enum class GeneralReg8 {
    kAL, kBL, kCL, kDL, kAH, kBH, kCH, kDH
};

// Enum class for segment registers
enum class SegReg {
    kES, kCS, kSS, kDS, kFS, kGS
};

namespace std {
    template<>
    struct hash<std::vector<TypeOperand>> {
        size_t operator()(const std::vector<TypeOperand>& vec) const noexcept {
            size_t h = 0;
            for (auto& elem : vec) {
                // ѕриводим enum к int дл€ хешировани€
                h ^= std::hash<int>{}(static_cast<int>(elem)) + 0x9e3779b9 + (h << 6) + (h >> 2);
            }
            return h;
        }
    };

    template <>
    struct hash<SegReg> {
        std::size_t operator()(const SegReg& reg) const noexcept {
            return static_cast<std::size_t>(reg);
        }
    };

    template <>
    struct hash<GeneralReg32> {
        std::size_t operator()(const GeneralReg32& reg) const noexcept {
            return static_cast<std::size_t>(reg);
        }
    };

    template <>
    struct hash<GeneralReg8> {
        std::size_t operator()(const GeneralReg8& reg) const noexcept {
            return static_cast<std::size_t>(reg);
        }
    };
}

inline const std::unordered_map<std::string, GeneralReg32> kStringToReg32 = {
    {"eax", GeneralReg32::kEAX},
    {"ebx", GeneralReg32::kEBX},
    {"ecx", GeneralReg32::kECX},
    {"edx", GeneralReg32::kEDX},
    {"esi", GeneralReg32::kESI},
    {"edi", GeneralReg32::kEDI}
};

inline const std::unordered_map<std::string, GeneralReg8> kStringToReg8 = {
    {"al", GeneralReg8::kAL},
    {"bl", GeneralReg8::kBL},
    {"cl", GeneralReg8::kCL},
    {"dl", GeneralReg8::kDL},
    {"ah", GeneralReg8::kAH},
    {"bh", GeneralReg8::kBH},
    {"ch", GeneralReg8::kCH},
    {"dh", GeneralReg8::kDH}
};

// Structure that stores information about a source code line
struct LineInfo {
    int line_number;             // Line number in the source code
    std::string seg;             // Segment to which this line belongs
    uint16_t offset;             // Offset within the segment
    uint8_t size;                // Size in bytes of the assembled instruction/data in this line
    std::vector<uint8_t> bytes; // Byte values generated from this line
};

// Structure storing data about a command with a specific set of operands
struct CommandInfo {
    uint8_t opcode;              // Opcode byte
    bool byte_modrm;             // True if ModRM byte is used and register code is written in reg field
    bool opcode_in_modrm;        // True if 3 bits in ModRM (reg field) represent opcode extension
    uint8_t extra_bits : 3;      // The 3 bits used in ModRM for opcode extension
    bool r_reg;                  // True if opcode of register is in r/m field of ModRM (for register operand), else for memory addressing
    ConstSizeOperand size_const; // Size of constant operand (byte, word, doubleword or none)
    bool is_jump;                // True if this command is a jump instruction
    int size_jump;               // Size of the jump offset
    CommandInfo(uint8_t op, bool m, bool r_, uint8_t rx, bool r_plus, ConstSizeOperand size, bool jump, int val)
        : opcode(op), byte_modrm(m), opcode_in_modrm(r_), extra_bits(rx), r_reg(r_plus), size_const(size), is_jump(jump), size_jump(val) {}
};

// Structure holding addressing info Ч registers and constants used in addressing
struct InfoAddr {
    std::vector<GeneralReg32> reg_addr; // Registers involved in the address calculation
    std::vector<int> const_addr;        // Constant displacements or immediates in the address
};

// Structure holding parsed information about an analyzed line
struct StructWithCom {
    Commands command;                    // Instruction command
    std::vector<TypeOperand> operands;  // Operands vector
    std::optional<TypeAddress> type_addr; // Optional addressing mode/type
    std::optional<InfoAddr> addr;          // Optional detailed addressing info
    std::optional<SegReg> seg_reg;         // Optional segment register override
};

// Structure holding exact operands Ч specific registers or constant values
struct ExactlyOperands {
    std::vector<std::variant<GeneralReg32, GeneralReg8, TypeOperand, int>> operands;
};

// Mapping string instruction names to enum Commands
inline const std::unordered_map<std::string, Commands> kStringToCommand = {
    {"mov", Commands::kMOV},
    {"add", Commands::kADD},
    {"neg", Commands::kNEG},
    {"popad", Commands::kPOPAD},
    {"push", Commands::kPUSH},
    {"adc", Commands::kADC},
    {"sbb", Commands::kSBB},
    {"jnae", Commands::kJNAE},
    {"lds", Commands::kLDS},
    {"and", Commands::kAND},
    {"jmp", Commands::kJMP},
};

// Mapping string segment register names to enum SegReg
inline const std::unordered_map<std::string, SegReg> kStringToSegReg = {
    {"cs", SegReg::kCS},
    {"ds", SegReg::kDS},
    {"gs", SegReg::kGS},
    {"es", SegReg::kES},
};

// Mapping 32-bit general registers to their opcode bits
inline const std::unordered_map<GeneralReg32, uint8_t> kCodesReg32 = {
    {GeneralReg32::kEAX, 0b000},
    {GeneralReg32::kEBX, 0b011},
    {GeneralReg32::kECX, 0b001},
    {GeneralReg32::kEDX, 0b010},
    {GeneralReg32::kESI, 0b110},
    {GeneralReg32::kEDI, 0b111},
};

// Mapping 8-bit general registers to their opcode bits
inline const std::unordered_map<GeneralReg8, uint8_t> kCodesReg8 = {
    {GeneralReg8::kAL, 0b000},
    {GeneralReg8::kCL, 0b001},
    {GeneralReg8::kDL, 0b010},
    {GeneralReg8::kBL, 0b011},
    {GeneralReg8::kAH, 0b100},
    {GeneralReg8::kCH, 0b101},
    {GeneralReg8::kDH, 0b110},
    {GeneralReg8::kBH, 0b111}
};

// Mapping segment registers to their opcode prefix bytes
inline const std::unordered_map<SegReg, uint8_t> kSegmentPrefixBytes = {
    {SegReg::kES, 0x26},
    {SegReg::kCS, 0x2E},
    {SegReg::kSS, 0x36},
    {SegReg::kDS, 0x3E},
    {SegReg::kFS, 0x64},
    {SegReg::kGS, 0x65}
};

// Regex patterns mapping to addressing types (various memory addressing modes)
inline const std::unordered_map<std::string, TypeAddress> kRegexToAddressType = {
    {R"(\[\s*([a-z]{3})\s*\])", TypeAddress::kType1},                            // [reg]
    {R"(\[\s*(\d+)\s*\])", TypeAddress::kType2},                                 // [disp] (displacement only)
    {R"(\[\s*([a-z]{3})\s*\+\s*(\d+)\s*\])", TypeAddress::kType3},              // [reg + disp]
    {R"(\[\s*([a-z]{3})\s*\+\s*([a-z]{3})\s*\])", TypeAddress::kType4},         // [reg + reg]
    {R"(\[\s*([a-z]{3})\s*\+\s*([a-z]{3})\s*\+\s*(\d+)\s*\])", TypeAddress::kType5}, // [reg + reg + disp]
    {R"(\[\s*([a-z]{3})\s*\+\s*([a-z]{3})\s*\*\s*(\d+)\s*\])", TypeAddress::kType6}, // [reg + reg * scale]
    {R"(\[\s*([a-z]{3})\s*\+\s*([a-z]{3})\s*\*\s*(\d+)\s*\+\s*(\d+)\s*\])", TypeAddress::kType7} // [reg + reg * scale + disp]
};

// Regex patterns to identify jump operand types (short, near, far labels)
inline const std::unordered_map<std::string, TypeOperand> kRegexToJmpOperandType = {
    {R"(^\s*short\s+[a-zA-Z_][\w]*\s*$)",     TypeOperand::kIdentShort},   // short label
    {R"(^\s*near\s+[a-zA-Z_][\w]*\s*$)",      TypeOperand::kIdentNear},    // near label
    {R"(^\s*far\s+ptr\s+[a-zA-Z_][\w]*\s*$)", TypeOperand::kIdentFar},     // far ptr label
    {R"(^\s*ptr\s+[a-zA-Z_][\w]*\s*$)",       TypeOperand::kIdentShort},   // ptr label (default SHORT)
    {R"(^\s*[a-zA-Z_][\w]*\s*$)",             TypeOperand::kIdentShort}    // plain label
};

// Nested hash map: 
// - outer key: command (Commands enum)
// - outer value: a hash map where:
//    - key: vector of operand types (TypeOperand)
//    - value: CommandInfo structure storing encoding details for this command + operand combination
inline const std::unordered_map<Commands, std::unordered_map<std::vector<TypeOperand>, CommandInfo>> kCommands = {
    {
        Commands::kPOPAD,
        {
            // POPAD has no operands, fixed opcode 0x61, no ModRM, etc.
            { {}, {0x61, false, false, 0, 0, ConstSizeOperand::kNone, 0, 0} }
        }
    },
    {
        Commands::kNEG,
        {   
            // NEG mem
            { {TypeOperand::kAddr}, {0xF7, true, true, 3, 0, ConstSizeOperand::kNone, 0, 0} }
        }
    },
    {
        Commands::kADC,
        {
            // ADD reg32, imm
            { {TypeOperand::kReg32, TypeOperand::kConstByte}, {0x83, true, true, 2, 0, ConstSizeOperand::kByte, 0, 0} },
            // ADD reg8, imm
            { {TypeOperand::kReg8, TypeOperand::kConstByte}, {0x14, false, false, 0, 0, ConstSizeOperand::kByte, 0, 0} }
        }
    },
    {
        Commands::kPUSH,
        {
            { {TypeOperand::kReg32}, {0x50, false, false, 0, 1, ConstSizeOperand::kNone, 0, 0} }
        }
    },
    {
        Commands::kSBB,
        {
            { {TypeOperand::kReg32, TypeOperand::kReg32}, {0x1B, true, false, 0, 1, ConstSizeOperand::kNone, 0, 0} },
            { {TypeOperand::kReg8, TypeOperand::kReg8}, {0x1A, true, false, 0, 1, ConstSizeOperand::kNone, 0, 0} }
        }
    },
    {
        Commands::kJNAE,
        {
            { {TypeOperand::kIdentShort}, {0x72, false, false, 0, 0, ConstSizeOperand::kByte, 1, 1} }
        }
    },
    {
        Commands::kLDS,
        {
            { {TypeOperand::kReg32, TypeOperand::kAddr}, {0xC5, true, false, 0, 0, ConstSizeOperand::kNone, 0, 0} }
        }
    },
    {
        Commands::kAND,
        {
            { {TypeOperand::kAddr, TypeOperand::kConstByte}, {0x83, true, true, 4, 1, ConstSizeOperand::kByte, 0, 0} },
            { {TypeOperand::kAddr, TypeOperand::kConstWord}, {0x81, true, true, 4, 1, ConstSizeOperand::kWord, 0, 0} }
        }
    },
    {
        Commands::kJMP,
        {
            { {TypeOperand::kIdentFar}, {0xEA, false, false, 0, 0, ConstSizeOperand::kNone, 1, 6} }
        }
    },
    {
        Commands::kMOV,
        {
            { {TypeOperand::kAddr, TypeOperand::kReg32}, {0x89, true, false, 0, 1, ConstSizeOperand::kNone, 0, 0} },
            { {TypeOperand::kAddr, TypeOperand::kReg8}, {0x88, true, false, 0, 1, ConstSizeOperand::kNone, 0, 0} }
        }
    },
    {
        Commands::kADD,
        {
            { {TypeOperand::kReg32, TypeOperand::kReg32}, {0x03, true, true, 0, 0, ConstSizeOperand::kNone, 0, 0} }
        }
    }
};