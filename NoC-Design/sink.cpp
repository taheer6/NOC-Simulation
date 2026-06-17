// sink.cpp
#include "sink.h"

void sink::receive_data()
{
    packet v_packet;

    if (sclk.event())
        ack_out.write(false);

    if (packet_in.event())
    {
        pkt_recv++;
        ack_out.write(true);
        last_recv_time = sc_time_stamp();

        v_packet = packet_in.read();
        cout << "            New Pkt:  " << (int)v_packet.data
             << " is received from source: " << (int)v_packet.id
             << " by sink:  " << (int)sink_id.read() << endl;
    }
}
