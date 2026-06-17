// router.cpp
#include "router.h"

void router::func()
{
    while (true)
    {
        wait();

        if (in0.event()) pkt_sent++;
        if (in1.event()) pkt_sent++;
        if (in2.event()) pkt_sent++;
        if (in3.event()) pkt_sent++;
        if (in4.event()) pkt_sent++;
    }
}
