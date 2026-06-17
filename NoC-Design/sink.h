// sink.h
#include "packet.h"

SC_MODULE(sink) {
    sc_in<packet> packet_in;
    sc_out<bool> ack_out;
    sc_in<sc_uint<4> > sink_id;
    sc_in<bool> sclk;

    int pkt_recv;
    sc_time last_recv_time;

    void receive_data();

    SC_CTOR(sink) {
        SC_METHOD(receive_data);
        dont_initialize();
        sensitive << packet_in;
        sensitive << sclk.pos();
        pkt_recv = 0;
        last_recv_time = SC_ZERO_TIME;
    }
};
