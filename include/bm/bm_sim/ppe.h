#ifndef __PPE_H_
#define __PPE_H_
#include<vector>
#include<string>
#include <stdio.h>
#include <stdlib.h>
#include "ppeInit.h"
#include <bm/bm_sim/packet.h>
// #include <functional>
#include <memory>
// #include <chrono>
// #include <thread>

namespace bm
{
    
class Packet;

class ppe_sim:ppeInit
{
public:
    
    
public:
    ib_module fun(ib_module input_buffer);

    void pkt_to_tlb(Packet *pkt, int fcs, int port);

    // void pack_to_ingress(Packet *pkt)const;

    void tlb_to_pkt();

    ppe_sim():fcs(0),port(0),egr_num(0)
    {
        cq_init();
        tlb_init();
        printf("--------ppe_init-------");
    };

    // ib_module &operator=(ib_module &&buffer)=default;

    void transmit(ib_module &&input_buffer);

    void ppe_run();

    void pkt_to_ppe(Packet *pkt);

    void ppe_to_pkt(Packet *pkt);

    void ppe();

    void export_pkt(Packet *pkt);

    void handle_cq_event();

private:

    
    int fcs;

    int port;

    int egr_num;
    
};
}




#endif
