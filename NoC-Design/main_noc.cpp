// main_noc.cpp
#include "systemc.h"
#include <iostream>
#include <string>
#include <stdio.h>
#include "packet.h"
#include "source.h"
#include "sink.h"
#include "router.h"

using namespace std;

static sc_uint<4> uniform_dest(int id)
{
    return (id + 8) % 16;
}

static sc_uint<4> neighbor_dest(int id)
{
    if ((id % 2) == 0) return id + 1;
    return id - 1;
}

int sc_main(int argc, char *argv[])
{
    const int N = 4;
    const int NUM_NODES = 16;
    const int FLITS_PER_SOURCE = 25;
    const int SOURCE_CLK_NS = 125;
    const int DRAIN_TIME_NS = 2000;

    string pattern = "uniform";
    if (argc > 1)
        pattern = argv[1];

    sc_signal<packet> si_source[NUM_NODES];
    sc_signal<packet> si_sink[NUM_NODES];

    sc_signal<bool> si_ack_src[NUM_NODES];
    sc_signal<bool> si_ack_sink[NUM_NODES];

    sc_signal<sc_uint<4> > node_id[NUM_NODES];
    sc_signal<sc_uint<4> > node_dest[NUM_NODES];

    sc_signal<packet> horiz_lr[N][N - 1];
    sc_signal<packet> horiz_rl[N][N - 1];
    sc_signal<bool> ack_lr[N][N - 1];
    sc_signal<bool> ack_rl[N][N - 1];

    sc_signal<packet> vert_tb[N - 1][N];
    sc_signal<packet> vert_bt[N - 1][N];
    sc_signal<bool> ack_tb[N - 1][N];
    sc_signal<bool> ack_bt[N - 1][N];

    sc_signal<packet> dummy_in_pkt[NUM_NODES][4];
    sc_signal<packet> dummy_out_pkt[NUM_NODES][4];
    sc_signal<bool> dummy_in_ack[NUM_NODES][4];
    sc_signal<bool> dummy_out_ack[NUM_NODES][4];

    sc_clock s_clock("S_CLOCK", SOURCE_CLK_NS, SC_NS, 0.5, 0.0, SC_NS);
    sc_clock r_clock("R_CLOCK", 5, SC_NS, 0.5, 10.0, SC_NS);
    sc_clock d_clock("D_CLOCK", 5, SC_NS, 0.5, 10.0, SC_NS);

    source* sources[NUM_NODES];
    sink* sinks[NUM_NODES];
    router* routers[NUM_NODES];

    for (int i = 0; i < NUM_NODES; i++)
    {
        char sname[20];
        char kname[20];
        char rname[20];

        sprintf(sname, "source_%d", i);
        sprintf(kname, "sink_%d", i);
        sprintf(rname, "router_%d", i);

        sources[i] = new source(sname);
        sinks[i] = new sink(kname);
        routers[i] = new router(rname);

        sources[i]->max_flits = FLITS_PER_SOURCE;

        sources[i]->packet_out(si_source[i]);
        sources[i]->source_id(node_id[i]);
        sources[i]->dest_id(node_dest[i]);
        sources[i]->ach_in(si_ack_src[i]);
        sources[i]->CLK(s_clock);

        sinks[i]->packet_in(si_sink[i]);
        sinks[i]->ack_out(si_ack_sink[i]);
        sinks[i]->sink_id(node_id[i]);
        sinks[i]->sclk(d_clock);

        routers[i]->in0(si_source[i]);
        routers[i]->out0(si_sink[i]);
        routers[i]->inack0(si_ack_sink[i]);
        routers[i]->outack0(si_ack_src[i]);
        routers[i]->router_id(node_id[i]);
        routers[i]->rclk(r_clock);
    }

    for (int id = 0; id < NUM_NODES; id++)
    {
        int r = id / N;
        int c = id % N;

        node_id[id].write(id);
        if (pattern == "neighbor")
            node_dest[id].write(neighbor_dest(id));
        else
            node_dest[id].write(uniform_dest(id));

        // Port 1: North
        if (r > 0) {
            routers[id]->in1(vert_bt[r - 1][c]);
            routers[id]->out1(vert_tb[r - 1][c]);
            routers[id]->inack1(ack_tb[r - 1][c]);
            routers[id]->outack1(ack_bt[r - 1][c]);
        } else {
            routers[id]->in1(dummy_in_pkt[id][0]);
            routers[id]->out1(dummy_out_pkt[id][0]);
            routers[id]->inack1(dummy_in_ack[id][0]);
            routers[id]->outack1(dummy_out_ack[id][0]);
        }

        // Port 2: East
        if (c < N - 1) {
            routers[id]->in2(horiz_rl[r][c]);
            routers[id]->out2(horiz_lr[r][c]);
            routers[id]->inack2(ack_lr[r][c]);
            routers[id]->outack2(ack_rl[r][c]);
        } else {
            routers[id]->in2(dummy_in_pkt[id][1]);
            routers[id]->out2(dummy_out_pkt[id][1]);
            routers[id]->inack2(dummy_in_ack[id][1]);
            routers[id]->outack2(dummy_out_ack[id][1]);
        }

        // Port 3: South
        if (r < N - 1) {
            routers[id]->in3(vert_tb[r][c]);
            routers[id]->out3(vert_bt[r][c]);
            routers[id]->inack3(ack_bt[r][c]);
            routers[id]->outack3(ack_tb[r][c]);
        } else {
            routers[id]->in3(dummy_in_pkt[id][2]);
            routers[id]->out3(dummy_out_pkt[id][2]);
            routers[id]->inack3(dummy_in_ack[id][2]);
            routers[id]->outack3(dummy_out_ack[id][2]);
        }

        // Port 4: West
        if (c > 0) {
            routers[id]->in4(horiz_lr[r][c - 1]);
            routers[id]->out4(horiz_rl[r][c - 1]);
            routers[id]->inack4(ack_rl[r][c - 1]);
            routers[id]->outack4(ack_lr[r][c - 1]);
        } else {
            routers[id]->in4(dummy_in_pkt[id][3]);
            routers[id]->out4(dummy_out_pkt[id][3]);
            routers[id]->inack4(dummy_in_ack[id][3]);
            routers[id]->outack4(dummy_out_ack[id][3]);
        }
    }

    for (int i = 0; i < NUM_NODES; i++) {
        for (int p = 0; p < 4; p++) {
            dummy_in_ack[i][p].write(false);
            dummy_out_ack[i][p].write(false);

            packet zero_pkt;
            zero_pkt.data = 0;
            zero_pkt.id = 0;
            zero_pkt.dest = 0;
            zero_pkt.pkt_clk = 0;
            zero_pkt.h_t = 0;

            dummy_in_pkt[i][p].write(zero_pkt);
            dummy_out_pkt[i][p].write(zero_pkt);
        }
    }

    sc_trace_file *tf = sc_create_vcd_trace_file("graph_4x4");

    sc_trace(tf, s_clock, "s_clock");
    sc_trace(tf, r_clock, "r_clock");
    sc_trace(tf, d_clock, "d_clock");

    for (int i = 0; i < NUM_NODES; i++) {
        char name1[32];
        char name2[32];
        char name3[32];
        sprintf(name1, "si_source_%d", i);
        sprintf(name2, "si_sink_%d", i);
        sprintf(name3, "dest_%d", i);
        sc_trace(tf, si_source[i], name1);
        sc_trace(tf, si_sink[i], name2);
        sc_trace(tf, node_dest[i], name3);
    }

    cout << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "4x4 Mesh NoC Simulator" << endl;
    cout << "Traffic pattern: " << pattern << endl;
    cout << "Flits per source: " << FLITS_PER_SOURCE << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "Press Enter to start simulation..." << endl;
    getchar();

    sc_start((FLITS_PER_SOURCE * SOURCE_CLK_NS) + DRAIN_TIME_NS, SC_NS);

    sc_close_vcd_trace_file(tf);

    int total_sent = 0;
    int total_recv = 0;

    sc_time first_send_time = SC_ZERO_TIME;
    sc_time last_recv_time = SC_ZERO_TIME;
    bool first_time_found = false;

    cout << endl;
    cout << "------------------------------------------------------------" << endl;
    cout << "End of simulation" << endl;

    for (int i = 0; i < NUM_NODES; i++) {
        total_sent += sources[i]->pkt_snt;
        total_recv += sinks[i]->pkt_recv;

        if (sources[i]->first_send_recorded) {
            if (!first_time_found || sources[i]->first_send_time < first_send_time) {
                first_send_time = sources[i]->first_send_time;
                first_time_found = true;
            }
        }

        if (sinks[i]->last_recv_time > last_recv_time) {
            last_recv_time = sinks[i]->last_recv_time;
        }

        cout << "Node " << i
             << " sent: " << sources[i]->pkt_snt
             << " received: " << sinks[i]->pkt_recv
             << " dest: " << node_dest[i].read()
             << endl;
    }

    sc_time transmission_time = SC_ZERO_TIME;
    if (first_time_found && last_recv_time > first_send_time)
        transmission_time = last_recv_time - first_send_time;

    cout << "Transmission time: " << transmission_time << endl;
    cout << "Total packets/flits sent: " << total_sent << endl;
    cout << "Total packets/flits received: " << total_recv << endl;
    cout << "------------------------------------------------------------" << endl;

    getchar();
    return 0;
}
