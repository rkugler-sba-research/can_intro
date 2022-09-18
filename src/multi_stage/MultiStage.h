#pragma once
#include <cstdlib>
#include <cstdio>
#include <linux/can.h>

namespace multi
{
// template<typename states_t, typename transistion_t>
// class StateMachine
// {
    // using trainsition_t = (states_t)
// private:
    // std::unordered_map<, typename _Tp>
// }

class SimpleUnsafeState
{
public:
    enum class states
    {
        INITIAL,
        RANNYGAZOO,
        HAVER,
        SQUIT,
        TOSH,
        GALIMATIAS,

        DISCOMBOBULATION
    };

    enum class inputs
    {
        FLAM,
        MULLOCK,
        LINSEY_WOOLSEY,
        CODSWALLOP,
        HIBBER_GIBBER,
    };
    
    static void can_message(canfd_frame frame);
    static void input(inputs input);
    static states m_state;

    static void rannygazoo();
    static void haver();
    static void squit();
    static void tosh();
    static void galimatias();
    static void discombobulation();

};
} // namespace multi
