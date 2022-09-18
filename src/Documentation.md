# Vulnerable-Component

This crude example serves to demonstrate a few simple classes of programming
errors which can lead to a vulnerable component. 

## Interface Preparation
To be able to listen and send to a virtual can interface, we first need to 
create the interface. We do this by running
```
sudo ip link add dev vcan0 type vcan
```
, adding a new vcan (virtual can) interface named `vcan0`. We then configure
it using 
```
sudo ip link set up vcan0
```
.

## Driver code
The driver code (found mainly  in src/main.cpp) sets up a can socket (`vcan0`) 
and configures it to be able to handle both CAN 2.0 and CANFD messages. It then
binds the file descriptor.

## Main program flow
The program continuously (`while` loop l60-l83) listens for a can frame.
It then validates the received length (to distinguish it from a canfd frame) of
the received frame. If the length satisfies our expectations, we switch on the
id of the message. Ids 1 to 4 each represent one test case, which will be
explained shortly. Any other index terminates the program normally.

## Send commands
To send can commands to the program, one has to send valid can 2.0 or canfd
frames to the virtual can interface. Using `can-utils`, there are two ways 
one can achieve this: 

1. using `cansend <interface> <id>#<data>`, where `data` is a hexadecimal
bytestring (i.e. `AABBCC`, for `{0xAA, 0xBB, 0xCC}`). If you configured the
interface as above, use `cansend vcan0 <id>#<data>`.

2. using `canplayer`, primarily when replaying prior recordings (created 
using candump etc.). Multiple configurations are available, for a more
detailed description please refer to the help page. To indefinitely replay
a log file use 
`canplayer -I <logfile> -l i <interface>=<recorded_interface>`, where the
interface is the configured interface (`vcan0`) in the above example, and
`recorded_interface` is the interface that was used to record the packages.
(See log file for this name).

## Included Errors
There are 4 (very simple) errors which can be triggered by sending the
correct data string to one of the ids 1..4. The categories are

1. Unintended magic values 
2. Data type conversion error (misinterpreted signed data type)
3. Logic bug
4. Possibly not terminated input

In the following, each will be explained briefly and it will be shown
how to trigger each bug. For a more detailed explanation of each of the
bugs please refer to `src/simple_examples/Documentation.md`.

### 1. Magic Values
The associated function (`simple::magic_values(uint8_t, uint8_t*)`) mocks
some functionality where two actuators are controlled by one device, the
commands to these is encoded in the first and second bit of the data string.  
*(`0b1000000 = 0x80` denote _window up_, `0b01000000 = 0x40` denote _window
down_). Both flags may be set, however if the last bit is set some debugging
functionality that was not properly removed is triggered (in this case simply
resulting in a crash). Therefore every uneven `uint8_t` as the first data byte
of the frame will crash the application.
To trigger this crash use i.e. `cansend vcan0 001#C1`

### 2. Data type conversion error
Here, the intended functionality of the function
`simple::sign_error(uint8_t, uint8_t*****)` is to accept a signed as its first and
only data byte with negative values denoting error states and positive values 
between 0 and 2^7 - 1 = 0 and 127 being interpreted as valid data. However, as
there is no way to enforce the interpretation of data over can, the receiver
mistakenly interprets it as a non-signed 8-bit unsigned int, with 0 denoting
an error and also fails to do the necessary bounds-check. Therefore, a negative
argument on the side of the sender will be interpreted as a positive argument.
of magnitude greater than 127, which will then corrupt the stack and cause 
the application to crash.
To trigger this bug, use `cansend  vcan0 002#80` or higher

### 3. Logic Bug
In this example, found in `simple::logic_bug(uint8_t, uint8_t*)`, the function
takes once again one byte of data and interprets some contained bits as commands.
To be specific, the lowest as as the 4th highest bit will trigger these if they
are high (`0b00010000`=`0x10` and `0b00000001`=`0x01`), however issuing both
at once represents a forbidden state. The function does check for this, however
when actually interpreting the bits logically there is a mistake, which does 
not mask the upper 3 bits of the byte, which leads to any number greater than
`0x10` triggering the high state. Unfortunately the check interprets them 
correctly, which leads to a possible crash. To cause it, we must send a data 
byte greater than `0x10`, with the lowest bit set however the uppermost 4th 
bit may not be set. This means that any odd number where the upper digit is also
odd will trigger the bug, i.e.
`cansend vcan0 003#21`

### 4. Non-terminated Input
For this (very synthetic) example, the function 
`simple::missing_terminator(uint8_t, uint8_t*)` interprets its data_field byte
string as a sequence of commands, the values of which are defined in the enum
`simple::control_characters`. However, each command string has to be
terminated with the value `simple::control_characters::TERMINATE`. In addition,
no additional length check (i.e. >=8 or >=64 for canfd) is run, therefore one
can write arbitrary data after the end of the buffer.
To trigger this, one has to send any sequence of byte which does not end with 
`simple::control_characters::TERMINATE` (`0x66`), however if in the recent
execution the same command was issued with a terminated string of greater or
equal length, this value may still be in memory and prevent this bug from
happening. 
`cansend vcan0 004#616263646564`

### Other Ids
Any other id will cause the program to consume said can frame and then terminate
normally without doing any additional computation.

//TODO: review for relevance
## A note about relevance
Each of the present bugs is easily spotted when presented in such plain fashion, 
but can proof hard to spot and dangerously exploitable in more complex code
fragments. However, out of the 4 Bugs only the 4th one _might_ be exploitable
in the real world as is. Especially bugs 1 and 3 execute the `CRASH` macro 
outright which just mutates a forbidden pointer and prints some debug info, and
bug 2 only overfills the stack with a static value, up to at most 255 bytes
above the current `$esp`. Bug 4 may grant the option to write data, but with
but only 8 bytes of which can be controlled. Also, the location is anything but
arbitrary as static variables reside in the .bss section. TODO: Validate (is it block s)

