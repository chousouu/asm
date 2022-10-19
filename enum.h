#define DEBUG_MODE

#ifdef DEBUG_MODE
    #define DEB(...) printf(__VA_ARGS__)
#else 
    #define DEB(...) 
#endif// DEBUG_MODE

enum CMD
{
    #define DEF_CMD(name, num, arg, code) CMD_##name = num, 
    #include "commands.h"
    #undef DEF_CMD
};

enum CMD_BYTES 
{
    _CMD   = 0,
    _RAM   = 1,
    _REG   = 2,
    _KONST = 3,
};
    // CMD_PUSH  = 1,
    // CMD_POP   = 2,
    // CMD_ADD   = 3,
    // CMD_MUL   = 4,
    // CMD_DIV   = 5,
    // CMD_DUP   = 6,
    // CMD_JMP   = 7,
    // CMD_OUT   = 10,
    // CMD_HALT  = 11,