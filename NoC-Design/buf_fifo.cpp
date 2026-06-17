// buf_fifo.cpp
#include "buf_fifo.h"

static sc_uint<7> make_req(bool empty, const packet& pkt)
{
    sc_uint<7> value = 0;
    value.range(3, 0) = pkt.dest;
    value[4] = empty ? 1 : 0;
    value[5] = pkt.h_t;
    value[6] = 0;
    return value;
}

void fifo::packet_in(const packet& data_packet)
{
    if (full)
        return;

    registers[reg_num++] = data_packet;
    empty = false;

    if (reg_num == 4)
        full = true;
}

packet fifo::packet_out()
{
    packet temp = registers[0];

    if (reg_num > 0)
        reg_num--;

    if (reg_num == 0) {
        empty = true;
    } else {
        registers[0] = registers[1];
        registers[1] = registers[2];
        registers[2] = registers[3];
    }

    full = false;
    return temp;
}

void buf_fifo::func()
{
    fifo q0;
    packet b_temp;
    packet zero_pkt;
    zero_pkt.data = 0;
    zero_pkt.id = 0;
    zero_pkt.dest = 0;
    zero_pkt.pkt_clk = 0;
    zero_pkt.h_t = 0;

    ack.write(false);
    req.write(make_req(true, zero_pkt));

    while (true)
    {
        wait();

        if (wr.event())
        {
            q0.packet_in(wr.read());
            ack.write(q0.full);

            if (!q0.empty)
                req.write(make_req(false, q0.registers[0]));
            else
                req.write(make_req(true, zero_pkt));
        }

        if (bclk.event())
        {
            if (grant.read() == 1 && !q0.empty)
            {
                b_temp = q0.packet_out();
                re.write(b_temp);
                ack.write(q0.full);

                if (!q0.empty)
                    req.write(make_req(false, q0.registers[0]));
                else
                    req.write(make_req(true, zero_pkt));
            }
        }
    }
}
