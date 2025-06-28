#pragma once
#include <optional>
#include <unordered_map>
#include <variant>

enum class Type {
    kDirec,       // 
    kInstruc,     // 
    kSegReg,      // 
    kReg32,       // 
    kReg8,
    kSize,        // 
    kPtr,         // 
    kAddrSpec,    // 
    kSymb,           
    kUserIdent,  
    kConst,       
    kUnknown      
};

enum class TypeConst {
    kConst10,
    kConst2,
    kConst16,
    kTextConst,
    kNone
};

enum class IdentifierType {
    kLabel,
    kConst,
    kNameSeg,
    kUnknown
};

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

enum class TypeAddress {
    kType1, kType2, kType3, kType4, kType5, kType6, kType7
};

enum class TypeOperand {
    kReg32, kReg8, kConst, kAddr, kIdent, kConstByte, kConstWord, kIdentShort, kIdentNear, kIdentFar
};

enum class ConstSizeOperand {
    kByte, kDoubleWord, kWord, kNone
};

enum class GeneralReg32{
    kEAX, kEBX, kECX, kEDX, kESI, kEDI
};

enum class GeneralReg8 {
    kAL, kBL, kCL, kDL, kAH, kBH, kCH, kDH 
};  

enum class SegReg {
    kES, kCS, kSS, kDS, kFS, kGS
};

namespace std {
    template<>
    struct hash<std::vector<TypeOperand>> {
        size_t operator()(const std::vector<TypeOperand>& vec) const noexcept {
            size_t h = 0;
            for (auto& elem : vec) {
                // Приводим enum к int для хеширования
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

struct LineInfo {
    int line_number;             // number of line
    std::string seg;            // Segment where this line
    uint16_t offset;            // Offset in this segment
    uint8_t size;               // Size of this line
    std::vector<uint8_t> bytes; // Vector of bytes and their values
};

struct CommandInfo {
    uint8_t opcode;
    bool byte_modrm;                 // we have byte modrm and we write code of register in reg
    bool opcode_in_modrm;             // 3 bites in byte modrm(reg)
    uint8_t extra_bits : 3;        // these 3 bites
    bool r_reg;                 // we write opcode of register in r\m in byte modrm, if we have register, else, for addr
    ConstSizeOperand size_const;// operand(const) in sizes: byte, word, doubleword or NONE
    bool is_jump;               // this command is type JMP
    int size_jump;
    CommandInfo(uint8_t op, bool m, bool r_, uint8_t rx, bool r_plus, ConstSizeOperand size, bool jump, int val)
        : opcode(op), byte_modrm(m), opcode_in_modrm(r_), extra_bits(rx), r_reg(r_plus), size_const(size), is_jump(jump), size_jump(val) {}
};

struct InfoAddr {
    std::vector<GeneralReg32> reg_addr;
    std::vector<int> const_addr;
};

struct StructWithCom {
    Commands command;
    std::vector<TypeOperand> operands;
    std::optional<TypeAddress> type_addr;
    std::optional<InfoAddr> addr;
    std::optional<SegReg> seg_reg;
};

struct ExactlyOperands {
    std::vector<std::variant< GeneralReg32, GeneralReg8, TypeOperand, int>> operands;
};

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

inline const std::unordered_map<std::string, SegReg> kStringToSegReg = {
    {"cs", SegReg::kCS},
    {"ds", SegReg::kDS},
    {"gs", SegReg::kGS},
    {"es", SegReg::kES},
};

inline const std::unordered_map< GeneralReg32, uint8_t> kCodesReg32 = {
    {GeneralReg32::kEAX, 0b000},
    {GeneralReg32::kEBX, 0b011},
    {GeneralReg32::kECX, 0b001},
    {GeneralReg32::kEDX, 0b010},
    {GeneralReg32::kESI, 0b110},
    {GeneralReg32::kEDI, 0b111},
};

inline const std::unordered_map<GeneralReg8, uint8_t> kCodesReg8 = {
    { GeneralReg8::kAL, 0b000 },
    { GeneralReg8::kCL, 0b001 },
    { GeneralReg8::kDL, 0b010 },
    { GeneralReg8::kBL, 0b011 },
    { GeneralReg8::kAH, 0b100 },
    { GeneralReg8::kCH, 0b101 },
    { GeneralReg8::kDH, 0b110 },
    { GeneralReg8::kBH, 0b111 }
};

inline const std::unordered_map<SegReg, uint8_t> kSegmentPrefixBytes = {
    { SegReg::kES, 0x26 },
    { SegReg::kCS, 0x2E },
    { SegReg::kSS, 0x36 },
    { SegReg::kDS, 0x3E },
    { SegReg::kFS, 0x64 },
    { SegReg::kGS, 0x65 }
};

inline const std::unordered_map<std::string, TypeAddress> kRegexToAddressType = {
    {R"(\[\s*([a-z]{3})\s*\])", TypeAddress::kType1},                            // [reg]
    {R"(\[\s*(\d+)\s*\])", TypeAddress::kType2},                                 // [disp] 
    {R"(\[\s*([a-z]{3})\s*\+\s*(\d+)\s*\])", TypeAddress::kType3},              // [reg + disp]
    {R"(\[\s*([a-z]{3})\s*\+\s*([a-z]{3})\s*\])", TypeAddress::kType4},         // [reg + reg]
    {R"(\[\s*([a-z]{3})\s*\+\s*([a-z]{3})\s*\+\s*(\d+)\s*\])", TypeAddress::kType5}, // [reg + reg + disp]
    {R"(\[\s*([a-z]{3})\s*\+\s*([a-z]{3})\s*\*\s*(\d+)\s*\])", TypeAddress::kType6}, // [reg + reg * scale]
    {R"(\[\s*([a-z]{3})\s*\+\s*([a-z]{3})\s*\*\s*(\d+)\s*\+\s*(\d+)\s*\])", TypeAddress::kType7} // [reg + reg * scale + disp]
};

inline const std::unordered_map<std::string, TypeOperand> kRegexToJmpOperandType = {
    {R"(^\s*short\s+[a-zA-Z_][\w]*\s*$)",     TypeOperand::kIdentShort},   // short label
    {R"(^\s*near\s+[a-zA-Z_][\w]*\s*$)",      TypeOperand::kIdentNear},    // near label
    {R"(^\s*far\s+ptr\s+[a-zA-Z_][\w]*\s*$)", TypeOperand::kIdentFar},     // far ptr label
    {R"(^\s*ptr\s+[a-zA-Z_][\w]*\s*$)",       TypeOperand::kIdentShort},   // ptr label (по умолчанию SHORT)
    {R"(^\s*[a-zA-Z_][\w]*\s*$)",             TypeOperand::kIdentShort}    // просто label
};

inline const std::unordered_map<Commands, std::unordered_map<std::vector<TypeOperand>, CommandInfo>> kCommands = {
    {
        Commands::kPOPAD,
        {
            { {}, {0x61, false, false, 0, 0, ConstSizeOperand::kNone, 0, 0} }
        }
    },
    {
        Commands::kNEG,
        {
            { {TypeOperand::kAddr}, {0xF7, true, true, 3, 0, ConstSizeOperand::kNone, 0, 0} }
        }
    },
    {
        Commands::kADC,
        {
            { {TypeOperand::kReg32, TypeOperand::kConstByte}, {0x83, true, true, 2, 0, ConstSizeOperand::kByte, 0, 0} },
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