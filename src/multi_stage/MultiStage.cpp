#include "MultiStage.h"


namespace multi
{
SimpleUnsafeState::states SimpleUnsafeState::m_state = SimpleUnsafeState::states::INITIAL;

void SimpleUnsafeState::can_message(canfd_frame frame)
{
    if (frame.len < 1 || static_cast<inputs>(frame.data[0]) > inputs::HIBBER_GIBBER)
        return;

    input(static_cast<inputs>(frame.data[0]));
}

void SimpleUnsafeState::input(SimpleUnsafeState::inputs input)
{
    switch (input)
    {
        case inputs::FLAM:
            if (m_state == states::SQUIT)
            {
                printf(" --FLAM--> \n");
                galimatias();
                break;
            }
            else if (m_state == states::GALIMATIAS)
            {
                printf(" --FLAM--> \n");
                tosh();
                break;
            }
        case inputs::MULLOCK:
            if (m_state == states::GALIMATIAS)
            {
                printf(" --MULLOCK--> \n");
                discombobulation();
                break;
            }
        case inputs::LINSEY_WOOLSEY:
            if (m_state == states::RANNYGAZOO)
            {
                printf(" --LINSEY WOOLSEY--> \n");
                squit();
                break;
            }
            else if (m_state == states::HAVER)
            {
                printf(" --LINSEY WOOLSEY--> \n");
                galimatias();
                break;
            }
        case inputs::CODSWALLOP:
            if (m_state == states::RANNYGAZOO)
            {
                printf(" --CODSWALLOP--> \n");
                haver();
                break;
            }
        case inputs::HIBBER_GIBBER:;
            if (m_state == states::HAVER)
            {
                printf(" --HIBBER GIBBER--> \n");
                discombobulation();
                break;
            }
            else if (m_state == states::GALIMATIAS)
            {
                printf(" --HIBBER GIBBER--> \n");
                rannygazoo();
                break;
            }
            else if (m_state == states::TOSH)
            {
                printf(" --HIBBER GIBBER--> \n");
                rannygazoo();
                break;
            }

        default: break;
    }
}
void SimpleUnsafeState::rannygazoo()
{
    m_state = states::RANNYGAZOO;
    printf("[RANNYGAZOO]\n");
}

void SimpleUnsafeState::haver()
{
    m_state = states::HAVER;
    printf("[HAVER]\n");
}

void SimpleUnsafeState::squit()
{
    m_state = states::SQUIT;
    printf("[SQUIT]\n");
}

void SimpleUnsafeState::tosh()
{
    m_state = states::TOSH;
    printf("[TOSH]\n");
}

void SimpleUnsafeState::galimatias()
{
    m_state = states::GALIMATIAS;
    printf("[GALIMATIAS]\n");
}

void SimpleUnsafeState::discombobulation()
{
    m_state = states::DISCOMBOBULATION;
    printf("[DISCOMBOBULATION]\n");

    *((volatile int*)nullptr) = 1;
}
} // namespace multi
