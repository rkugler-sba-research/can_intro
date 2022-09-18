#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

#include <linux/can.h>
#include <linux/can/raw.h>
#include <linux/if.h>
#include <memory.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "simple_examples/SimpleExamples.h"

struct data
{
    static const size_t MAX_DATA_LENGTH = 8;

    uint16_t id;
    uint8_t dlc;
    uint8_t data_field[MAX_DATA_LENGTH];
};

void read_stdin_data(data* d)
{
    if (scanf("%hu, %hhu, ", &d->id, &d->dlc) != 2)
        exit(0);

    memset(&d->data_field, 0x69, data::MAX_DATA_LENGTH);

    for (size_t i = 0; i < d->dlc && i < data::MAX_DATA_LENGTH; i++)
    {
        scanf("%hhx", d->data_field + i);
    }
}

int main(int argc, char** argv)
{
    struct sockaddr_can addr;
    struct ifreq ifr;

    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    strcpy(ifr.ifr_name, "vcan0");
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    int can_raw_fd_frames_value = 1;
    setsockopt(s, SOL_CAN_RAW, CAN_RAW_FD_FRAMES, &can_raw_fd_frames_value, sizeof(can_raw_fd_frames_value));

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
        fprintf(stderr, "ERROR: Unable to bind socket\n");
        exit(-1);
    }

    struct canfd_frame frame;
    while (true)
    {
        size_t n_bytes = read(s, &frame, sizeof(struct canfd_frame));
        if (n_bytes == sizeof(struct can_frame))
        {
            switch (frame.can_id)
            {
                case 1: simple::magic_values(frame.len, reinterpret_cast<uint8_t*>(&frame.data)); break;

                case 2: simple::sign_error(frame.len, reinterpret_cast<uint8_t*>(&frame.data)); break;

                case 3: simple::logic_bug(frame.len, reinterpret_cast<uint8_t*>(&frame.data)); break;

                case 4: simple::missing_terminator(frame.len, reinterpret_cast<uint8_t*>(&frame.data)); break;

                default: return 0;
            }
        }
        else
        {
            fprintf(stderr, "ERROR: Unable to read CAN_FRAME!\n");
            exit(-1);
        }
    }

    return 0;
}
