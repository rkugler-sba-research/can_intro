#pragma once
#include <cstdint>

namespace simple
{

/**
 * @brief this function (crudely) simulates an error caused by a check for a magic (debug) value that has not been
 * removed. The placeholder for a more complex error behaviour is a simple crash.
 *
 * The data field is interpreted as a number of flags representing certain commands
 *
 * @param data_length_code the length of the data sent. Expects 1 byte
 * @param data_field the buffer of the data
 */
void magic_values(uint8_t data_length_code, uint8_t* data_field);

/**
 * @brief this function simulates an error caused by a faulty type conversion. The sender sends a signed 8-bit integral
 * value where negative values represent a failure state, positive values are used to control some actuator. By misinterpreting the intended signed value as unsigned, one can smash the stack above the reserved buffer.
 *
 * The crash is simulated by writing to an array which is 128 byte in size (bit enough for all positive signed 8 bit values, but not big enough for uint8_t.
 * values)
 *
 * @param data_length_code the length of the data sent. Expects 1 byte
 * @param data_field the buffer of the data
 */
void sign_error(uint8_t data_length_code, uint8_t* data_field);

/**
 * @brief this function simulates a logic bug by first looking at the lower and then the upper 4 bit of the data,
 * however it does not use the correct way to access these and discard any other. This makes the validation if only one
 * of those bits is set useless
 *
 * @param data_length_code the length of the data sent. Expects 1 byte
 * @param data_field the buffer of the data
 */
void logic_bug(uint8_t data_length_code, uint8_t* data_field);

enum control_characters
{
    START = 'a',
    STOP,
    PAUSE,
    NEXT,
    PREV,

    TERMINATE
};

/**
 * @brief this function simulates a crash caused by a missing terminator in a command string. Each character in the
 * data_field will be interpretet as one (nonsensical) command, which will be buffered and then executed. But if the
 * terminator is missing, the functions continues to read until either reading or writing to a protected area in memory
 *
 * @param data_length_code length of the data. Should be at most 8, but this is not checked
 * @param data_field the data, will be interpretet as chars
 */
void missing_terminator(uint8_t data_length_code, uint8_t* data_field);

} // namespace simple

