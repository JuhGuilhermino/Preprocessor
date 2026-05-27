#include <cstdint>

enum token_types_e : std::uint8_t{
    RESERVED_WORD = 0,
    IDENTIFIER,
    INTEGER,
    FLOAT,
    OPERATORS,
    DELIMITERS,
    EOF_TOKEN,
    OTHERS
    
};