// source.h
#include "packet.h"

SC_MODULE(source) {
    sc_out<packet> packet_out;
    sc_in<sc_uint<4> > source_id;
    sc_in<sc_uint<4> > dest_id;
    sc_in<bool> ach_in;
    sc_in_clk CLK;

    int pkt_snt;
    int max_flits;

    sc_time first_send_time;
    bool first_send_recorded;

    void func();

    SC_CTOR(source)
    {
        SC_CTHREAD(func, CLK.pos());
        pkt_snt = 0;
        max_flits = 25;
        first_send_time = SC_ZERO_TIME;
        first_send_recorded = false;
    }
};
