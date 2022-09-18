#include "SimpleExamples.h"

#include <cstdint>
#include <cstdio>
#include <cstring>

#define CRASH()                                                                           \
    {                                                                                     \
        fprintf(stderr, "Program reached intended crash at %s:%d\n", __FILE__, __LINE__); \
        *((char*)0x01) = 'i';                                                             \
    }

namespace simple
{

void magic_values(uint8_t data_length_code, uint8_t* data_field)
{
    if (data_length_code != 1)
    {
        printf("Length does not match case\n");
        return;
    }

    if (data_field[0] & 0x80) // upper most bit for up
        printf("Sending window up command...\n");

    if (data_field[0] & 0x40) // second bit for down
        printf("Sending window down command...\n");

    if (data_field[0] & 0x01) // ups, debug function should have been removed
        CRASH();
}

void sign_error(uint8_t data_length_code, uint8_t* data_field)
{
    uint8_t buf[128];

    if (data_length_code != 1)
    {
        printf("Length does not match case\n");
        return;
    }

    uint8_t position = *data_field;
    if (position < 0) // trivially false;
    {
        printf("Error!");
        return;
    }

    memset(buf, 0x6a, position);
}

void logic_bug(uint8_t data_length_code, uint8_t* data_field)
{
    bool first, second;

    if (data_length_code != 1)
    {
        printf("length does not match case\n");
        return;
    }

    uint8_t flags = *data_field;

    if (!(((flags >> 4) & 0x01) ^ (flags & 0x01)))
    {
        printf("both commands are set \n");
        return;
    }

    first = flags & 0x01;
    second = flags & 0xf0;

    if (first && second) // forbidden state
        CRASH();
}


static uint8_t commands[control_characters::TERMINATE - control_characters::START];
void missing_terminator(uint8_t data_length_code, uint8_t* data_field)
{
    memset(commands, 0, control_characters::TERMINATE - control_characters::START);

    while (*data_field != static_cast<uint8_t>(control_characters::TERMINATE))
    {
        printf("%c", *data_field);
        commands[*(data_field++) - control_characters::START] = 1;
    }
    printf("\n");

    // execute commands in commands[]...
}

} // namespace simple

