#define COMMAND_DEFINE(name, num, code, args) CMD_##name = num,

enum COMMAND_ENUMERATION
{
    #include "commands.h"
    command_count
};

#undef COMMAND_DEFINE