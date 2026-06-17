// source.cpp
#include "source.h"

void source::func()
{
    packet v_packet_out;
    v_packet_out.data = 1000;
    v_packet_out.pkt_clk = 0;
    v_packet_out.h_t = 0;
    v_packet_out.id = 0;
    v_packet_out.dest = 0;

    while (true)
    {
        wait();

        if (pkt_snt >= max_flits)
            continue;

        if (!ach_in.read())
        {
            sc_uint<4> src = source_id.read();
            sc_uint<4> dst = dest_id.read();

            if (src == dst)
                continue;

            if (!first_send_recorded) {
                first_send_time = sc_time_stamp();
                first_send_recorded = true;
            }

            v_packet_out.data = v_packet_out.data + src + 1;
            v_packet_out.id = src;
            v_packet_out.dest = dst;
            v_packet_out.pkt_clk = ~v_packet_out.pkt_clk;
            v_packet_out.h_t = false;

            pkt_snt++;
            if ((pkt_snt % 5) == 0)
                v_packet_out.h_t = true;

            packet_out.write(v_packet_out);

            cout << "New Pkt: " << v_packet_out.data
                 << " is sent by source: " << src
                 << " to Destination: " << dst << endl;
        }
    }
}
