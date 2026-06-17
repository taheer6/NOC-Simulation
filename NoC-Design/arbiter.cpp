// arbiter.cpp
#undef SC_INCLUDE_FX

#include "packet.h"
#include "arbiter.h"

static sc_uint<3> calc_route(sc_uint<4> router_id, sc_uint<4> dest_id)
{
    sc_uint<2> router_row = router_id / 4;
    sc_uint<2> router_col = router_id % 4;
    sc_uint<2> dest_row = dest_id / 4;
    sc_uint<2> dest_col = dest_id % 4;

    if (dest_col > router_col) return 3; // east
    if (dest_col < router_col) return 5; // west
    if (dest_row > router_row) return 4; // south
    if (dest_row < router_row) return 2; // north
    return 1; // local
}

static sc_uint<5> route_mask(sc_uint<3> route)
{
    switch (route) {
        case 1: return 1;
        case 2: return 2;
        case 3: return 4;
        case 4: return 8;
        case 5: return 16;
        default: return 0;
    }
}

void arbiter::func()
{
    sc_uint<1> v_connected_input[5];
    sc_uint<1> v_reserved_output[6];
    sc_uint<3> v_req[5];
    sc_uint<5> v_free;
    sc_uint<4> v_id;
    sc_uint<5> v_arbit;
    sc_uint<15> v_select;

    for (int i = 0; i < 5; i++) {
        v_connected_input[i] = 0;
        v_req[i] = 0;
    }
    for (int i = 0; i < 6; i++) {
        v_reserved_output[i] = 0;
    }

    while (true)
    {
        wait();

        grant0.write(0);
        grant1.write(0);
        grant2.write(0);
        grant3.write(0);
        grant4.write(0);
        v_select = 0;
        v_free = 0;
        v_id = arbiter_id.read();

        if (!free_out0.read()) v_free |= 1;
        if (!free_out1.read()) v_free |= 2;
        if (!free_out2.read()) v_free |= 4;
        if (!free_out3.read()) v_free |= 8;
        if (!free_out4.read()) v_free |= 16;

        sc_uint<7> reqs[5];
        reqs[0] = req0.read();
        reqs[1] = req1.read();
        reqs[2] = req2.read();
        reqs[3] = req3.read();
        reqs[4] = req4.read();

        for (int i = 0; i < 5; i++)
        {
            if (reqs[i][4])
                continue; // empty FIFO

            if (!v_connected_input[i]) {
                sc_uint<4> dest = reqs[i].range(3, 0);
                v_req[i] = calc_route(v_id, dest);
            }

            v_arbit = route_mask(v_req[i]);
            v_arbit = v_free & v_arbit;

            if (!v_connected_input[i] && v_reserved_output[v_req[i]])
                v_arbit = 0;

            if (v_arbit != 0)
            {
                switch (i) {
                    case 0:
                        grant0.write(1);
                        v_select.range(2, 0) = v_req[i];
                        break;
                    case 1:
                        grant1.write(1);
                        v_select.range(5, 3) = v_req[i];
                        break;
                    case 2:
                        grant2.write(1);
                        v_select.range(8, 6) = v_req[i];
                        break;
                    case 3:
                        grant3.write(1);
                        v_select.range(11, 9) = v_req[i];
                        break;
                    case 4:
                        grant4.write(1);
                        v_select.range(14, 12) = v_req[i];
                        break;
                }

                v_free = v_free & (~v_arbit);
                v_connected_input[i] = 1;
                v_reserved_output[v_req[i]] = 1;

                if (reqs[i][5]) {
                    v_connected_input[i] = 0;
                    v_reserved_output[v_req[i]] = 0;
                }
            }
        }

        aselect.write(v_select);
    }
}
