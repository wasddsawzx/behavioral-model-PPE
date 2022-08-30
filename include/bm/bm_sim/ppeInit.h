#ifndef __PPEINIT_H_
#define __PPEINIT_H_
#include<vector>
#include<string>
#include<memory>

namespace bm
{
typedef unsigned char IB_ADDR_TYPE; //Ingress Buffer˫�ֽڱ�ַ
typedef short HB_ADDR_TYPE; //Head Buffer ˫�ֽڱ�ַ
typedef short *IB_ADDR_PTR_TYPE; //Ingress Bufferָ���ַ
typedef unsigned short REG_TYPE; //�Ĵ������� //unsigned
class ppeInit
{

public:
    
struct tlb_des_queue
{
    unsigned short tlb_num[64];
    struct tlb_des_queue *next;
    int length;
    int mb_addr;
    short mb_content[8];
};

struct core_queue
{
    bool is_used[8];
    int align_ins;
    tlb_des_queue *input_queue;
};

struct tlb_module
{
    unsigned char content[256];
    short used;
};

struct tlb_list
{
    short tlb_num;
    struct tlb_list *next_free;
};

struct ib_module
{
    IB_ADDR_TYPE *ingress_buffer;
    int length;
};

struct hb_module
{
    HB_ADDR_TYPE head_buffer[1088];
};

public:
    void cq_init();
    bool all_queue_empty();
    bool all_core_end();
    void core_queue_check();
    void tlb_init();
    

public:
    core_queue core_queues[20];

    tlb_module tlb[256];

    tlb_list *free_tlb_list;

    ib_module ib;
    
    ib_module eb;

    hb_module hb[128];

    int clk=0;

};
}





#endif