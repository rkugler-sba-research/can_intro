# Vulnerable-Component - Simple example

The namespace `simple` contains 4 vulnerable functions, each having the
prototype `simple::<bug_type>(uint8_t data_length, uint8_t *data_field)`.
The following documentation aims to describe the source code in detail.
For instructions on how to prepare and run the demonstrations please 
refer to `src/Documentation.md`. 

## Utility
In `src/simple_examples/SimpleExamples.cpp`, the macro `CRASH()` is
defined, which causes the application to crash by modifying a
forbidden memory address after printing some information from where
it has been called. It is defined as:
```
#define CRASH()                                                                           \
    {                                                                                     \
        fprintf(stderr, "Program reached intended crash at %s:%d\n", __FILE__, __LINE__); \
        *((char*)0x01) = 'i';                                                             \
    }
```

## Magic values
`void magic_values(uint8_t data_length_code, uint8_t* data_field);`
This function mimics the mistake of including a debug option and not 
removing it. In this case this flag just triggers the `CRASH()` macro,
but in a more realistic setting some valuable debug information may be
leaked over the readable CAN bus or another channel, or the controller
may enter a debug state which makes it susceptible to further manipulation.

The code looks like this:
```
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
```
The first if check validates that indeed only one byte of data has been
passed to the function and returns with a message if this is not the case.
The program will in this case not terminate but instead just continue with
the next packages.
The subsequent if checks represent the intended functionality of the 
function, interpreting the first and second highest bits in the input 
byte as flags (in this case mocking an electrical window controller).
The last if check is the "bug" as this functionality clearly should have
been removed. In this case a simple review would suffice to spot this,
but in a more complex artifact, one may look for quite some time. Using
a fuzzer with an adequate oracle to determine unsafe states however,
it should at least be easier to determine the input causing said state.


## sign\_error
`void sign_error(uint8_t data_length_code, uint8_t* data_field);`
This function demonstrates a dangerous bug where the sender and the receiver
do not share the same interpretation of the transmitted data. (And the 
receiver also fails to sanity check its input). In this case, the sender
intended for the 8-bit signed integer to represent valid values in between 0
and 0x7f, so 0 and 127, and the receiver reserves enough memory for this case, 
however it does not interpret the data in the intended way. The subsequent 
check is trivially false, which results in the stack being smashed and the
program crashing.
```
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
        printf("Error, sent data invalid!\n");
    }

    memset(buf, 0x6a, position);
}
```

## logic\_bug
`void logic_bug(uint8_t data_length_code, uint8_t* data_field);`
This function contains a logic bug which allows malformed input to cause a
forbidden state and crash the application. The bug lies in the difference
between the input validation that is being done and the interpretation of the
input afterwards.
```
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
```
First we declare two bools, first and second. These represent states, both of
which are ok individually, but they may not occur at the same time. To satisfy 
this constraint, the program checks if the last bit of each quartet is set, 
which would be used by the program to encode each of the desired states and
passes the input if indeed both are requested. The latter setting of the states 
however mistakenly takes the whole upper 4 bits of the input as a 
