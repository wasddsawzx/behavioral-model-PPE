#ifndef __PPE_H_
#define __PPE_H_
#include<vector>
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
        table_init();
        multi_ins_init();
        printf("--------ppe_init-------");
    };

    // ib_module &operator=(ib_module &&buffer)=default;

    void transmit(ib_module &&input_buffer);

    

    void pkt_to_ppe(Packet *pkt);

    void ppe_to_pkt(Packet *pkt);

    

    void export_pkt(Packet *pkt,int *packet_out);


    // table and register init 
    virtual bool reg_next_init(int ppn)override;

    //core_queue function
    virtual bool all_queue_empty()override;
    virtual bool all_core_end()override;
    virtual void core_queue_check()override;
    
    // register function
    virtual int find_sr_num(char *reg_name)override;
    virtual bool sr_check_correct(int ppn)override;
    virtual void srpc_change(int ppn)override;
    virtual void set_sr_sta_alu(int ppn,ex_wb_bus bus)override;
    virtual void clear_sr_sta(int ppn,ex_wb_bus bus)override;
    virtual void sr_ts_count(int ppn)override;

    //table out and gen_table out
    // virtual void flow_table_out()override;
    // virtual void gen_table_out()override;

    
    void sim(Packet *pkt);
    
    
    
    ~ppe_sim(){ printf("delete ppe_sim");   }
    
private:


    void ppe_run();
    void ppe(int ppenum);
    //ppe function
    void if_stage(int ppn,int pc);
    void is_forward_id(int ppn, int rs_type, int val_type);
    void id_stage(int ppn, if_id_bus bus);
    void is_forward_ex(int ppn,int rs_type,int val_type);
    void ex_stage(int ppn, id_ex_bus bus);
    unsigned int hash32(unsigned short data_in,unsigned int lfsr_q);
    void wb_stage(int ppn,ex_wb_bus bus);

    //event list
    int first_stms2[16];
    event_node *event_list;
    event_node *check_event(int ppn,long clk);
    void handle_event(event_node *event);

    //cq_event list
    cq_event_node *cq_queue;
    cq_event_node *check_cq_event(long clk);
    void handle_cq_event(cq_event_node *cq_event);


    int fcs;

    int port;

    int egr_num;
    
};
}




#endif
