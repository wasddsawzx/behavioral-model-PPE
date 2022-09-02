#ifndef __PPEINIT_H_
#define __PPEINIT_H_
#include<vector>
#include<string>
#include<memory>
#include<stdlib.h>
#include<bm/bm_sim/macro_caculate.h>

namespace bm
{
//file_path
 /************************************************************************************/ 
#define def_gpr_path "/home/hrc/catkin_ws/behavioral-model-main/src/ppe_config_file/gpr.txt"
#define def_mr_path "/home/hrc/catkin_ws/behavioral-model-main/src/ppe_config_file/mr.txt"    
#define def_sr_path "/home/hrc/catkin_ws/behavioral-model-main/src/ppe_config_file/sr.txt"
#define def_ins_path "/home/hrc/catkin_ws/behavioral-model-main/src/ppe_config_file/ins.txt"
#define def_ft_path "/home/hrc/catkin_ws/behavioral-model-main/src/ppe_config_file/flow_table.txt"
#define def_gt_path "/home/hrc/catkin_ws/behavioral-model-main/src/ppe_config_file/gen_table.txt"
#define def_cq_path "/home/hrc/catkin_ws/behavioral-model-main/src/ppe_config_file/cq.txt"
#define def_stm_path "/home/hrc/catkin_ws/behavioral-model-main/src/ppe_config_file/storem_out.txt"
#define def_fto_path "/home/hrc/catkin_ws/behavioral-model-main/src/ppe_config_file/flow_table_output.txt"
#define def_gto_path "/home/hrc/catkin_ws/behavioral-model-main/src/ppe_config_file/gen_table_output.txt"
 /************************************************************************************/ 

//SR
#define SR_PC 0
#define SR_COREID 1
#define SR_STA_FLAG 2
#define SR_STA_ERR 3
#define SR_SYNC_CLUST 4
#define SR_SYNC_GLB 5
#define SR_SYNC_NBR 5
#define SR_SYNC_CFG 6
// no used:
#define SR_MBR_PKTLEN 8
#define SR_MBR_PORTID 9
#define SR_MBT_MSG0 12
#define SR_MBT_MSG1 13
#define SR_MBT_CMD 14
// used:
#define SR_SAR_HASHL 8
#define SR_SAR_HASHH 9
#define SR_SAR_CKS 10
#define SR_TS_T0 12
#define SR_TS_T1 13
#define SR_TS_T2 14
#define SR_TS_T3 15
#define SR_GPT_T0 16
#define SR_GPT_T1 17
#define SR_GPT_T2 18
#define SR_GPT_CTRL 19
#define SR_HCFG_CFG0 20
#define SR_HCFG_CFG1 21
#define SR_HCFG_CFG2 22
#define SR_HCFG_CFG3 23

// no used:
#define SR_WM_IBM 28
#define SR_WM_EBM 29
#define SR_WM_IBL 30
#define SR_WM_EBL 31
//ע�⣺CMD��δ��ȷ���壬��ǰ��Ϊ����
// #define NO_CMD 0
// #define CMD_IBDROP 1
// #define CMD_EBREQ 2
// #define CMD_EBO 3
// #define CMD_EBE 4
// TODO: SYNC MASK

#define SR_STA_FLAG_ON 0x0001
#define SR_STA_FLAG_ERR 0x0008
#define SR_STA_FLAG_ZR 0x0002
#define SR_STA_FLAG_CY 0x0004
// #define SR_STA_FLAG_CKS 0x0010
#define SR_STA_FLAG_TOF 0x0010
#define SR_STA_FLAG_HBLC 0x0020
#define SR_STA_FLAG_HBSC 0x0040
// QC=TBQC
#define SR_STA_FLAG_QC 0x0080
#define SR_STA_FLAG_TBIC 0x0100
#define SR_STA_FLAG_TBLC 0x0200
#define SR_STA_FLAG_TBSC 0x0400
// QM=TBQM
#define SR_STA_FLAG_QM 0x0800
#define SR_STA_FLAG_TLB 0x1000
#define SR_STA_FLAG_MBT 0x2000
#define SR_STA_FLAG_MBR0 0x4000
#define SR_STA_FLAG_MBR1 0x8000

#define SR_STA_ERR_HBLE 0x0001
#define SR_STA_ERR_HBSE 0x0002
#define SR_STA_ERR_TBIE 0x0004
#define SR_STA_ERR_TBDE 0x0008
#define SR_STA_ERR_TBQE 0x0010
#define SR_STA_ERR_TBLE 0x0020
#define SR_STA_ERR_TBSE 0x0040
#define SR_STA_ERR_TLBRE 0x0040

/************************************************************************************/ 

#define HB_BASEADDR 0
#define ISMB_BASEADDR 768
#define ESMB_BASEADDR 896
#define MAB_BASEADDR 512
#define MBB_BASEADDR 1024

/************************************************************************************/ 
#define PUSHQ_EVENT 1
#define PUSHQ_CYCLE(a) 10+(a+4)/4+1

/************************************************************************************/ 

#define HB_BASEADDR 0
#define ISMB_BASEADDR 768
#define ESMB_BASEADDR 896
#define MAB_BASEADDR 512
#define MBB_BASEADDR 1024
//event list define
/************************************************************************************/ 
#define EVENT_LOADM 1
#define EVENT_STOREM 2
#define EVENT_QUERYT 3
#define EVENT_LOADT 4
#define EVENT_STORET 5
#define EVENT_INSERTT 6
#define EVENT_DELETET 7
#define EVENT_LOADTLB 8
#define EVENT_DELETETLB 9
#define EVENT_REUSETLB 10
#define EVENT_SENDMB 11
#define EVENT_LOADMS2 12
#define EVENT_STOREMS2 13
#define EVENT_QUERYTS2 14
#define EVENT_LOADTS2 15
#define EVENT_STORETS2 16

#define LOAD_CYCLE0 2 //�ж�LOADM��ַ��λ��Чλ
#define STORE_CYCLE0 2 //�ж�STOREM��ַ��λ��Чλ
#define LOAD_CYCLE(a) 3+1+12+0+15+(a+8)/8-1-2 //�������¼�
#define STORE_CYCLE(a) 3+(a+8)/8-2 //�������¼�
#define QUERY_CYCLE0(a) 11+(a+4)/4+3+6
#define QUERY_CYCLE(a) (a+8+8)/8 //need to be fixed!
#define LOADT_CYCLE0 11+1+3+14+6
#define LOADT_CYCLE(a) (a+8)/8 //need to be fixed
#define STORET_CYCLE0 11
#define STORET_CYCLE(a) (a+8+8)/8+3 //need to be fixed
#define INSERTT_CYCLE(a) 11+(a+4)/4+3+(a+4+4)/4 //need to be fixed
#define DELETET_CYCLE(a) 11+(a+4)/4+3+(a+4+4)/4 //need to be fixed
#define LOADTLB_CYCLE(a) 3+1+12+15+2+(a+4)/4
#define DELETETLB_CYCLE(a) 3+(a+4)/4+12
#define REUSETLB_CYCLE(a) 3+(a+4)/4+12
#define SENDMB_CYCLE 19

/************************************************************************************/ 
#define bget(a,n) ((a&(1<<(n)))>>(n))
/************************************************************************************/ 

typedef unsigned char IB_ADDR_TYPE; //Ingress Buffer
typedef short HB_ADDR_TYPE; //Head Buffer 
typedef short *IB_ADDR_PTR_TYPE; //Ingress Bufferָ
typedef unsigned short REG_TYPE; //


/************************************************************************************/ 
class ppeInit
{

public:
 /************************************************************************************/   
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
/************************************************************************************/
struct ins
{
    char  opname[10];
    unsigned char rd;
    unsigned char rs1;
    unsigned char rs2;
    unsigned short imm;
    unsigned short line;
};

struct if_id_bus
{
    int pc;
    ins fst_ins;
    bool stage_valid;
};

struct id_ex_bus
{
    int pc;
    ins snd_ins;
    unsigned short value1;
    unsigned short value2;
    unsigned short value3;
    bool stage_valid;
    bool is_gpr_des;
    bool is_mr_des;
    bool is_hb_des;
    bool is_branch;
    int is_lsm;
    int is_lt;
    int is_mhm;
    bool is_be;
};

struct ex_wb_bus
{
    int pc;
    ins trd_ins;
    unsigned short ex_result;
    unsigned short ex_result2;
    bool stage_valid;
    bool is_gpr_des;
    bool is_mul;
    bool is_mr_des;
    bool is_hb_des;
    short pc_change;
    short pc_halt;
    int is_sr_alu;
    bool is_sr_des;
    bool is_read_hb;
    unsigned short hb_addr;
    bool forward;
    int is_mhm;
    bool sr_cy;
    bool sr_zr;
    bool is_be; 
    bool is_clear_sr_sta;
};

struct wb_bus
{
    int pc;
    ins fth_ins;
    bool wen;    //valid
    unsigned short waddr;
    bool is_mul; //write 2 gpr
    unsigned short wdata;
    unsigned short wdata2;
    bool is_hash;    //write 2 sr
    bool is_gpr_des; //write_gpr
    bool is_sr_des;  //write_sr'
    bool is_mr_des;  //write_mr
    bool is_hb_des;  //write_hb
    bool is_sr_cy;
};


/************************************************************************************/
struct lb_list{
    char label_name[64];
    int value;
    struct lb_list *next;
};

struct mfun_list
{
    char fun_name[30];
    mfun_list *next;
    char instruction[10][200];
    int ins_num;
};



/************************************************************************************/
struct cq_event_node
{
    int event_type;
    int event_para1;
    int event_para2;
    int event_para3;
    long trigger_clk;
    int core_id;
    struct cq_event_node* next;
};
// table_out
struct item_list{
    long ItemID;
    short Item[128];
    struct item_list *next;
};

struct q_table{
    int TableID;
    int ItemNumMax;
    int ItemSize;
    int QuerySize;
    item_list *Table;
};

 struct g_table{
    int TableID;
    int ItemSize;
    item_list *Table;
};

/************************************************************************************/

struct event_node{
    int event_type;
    int event_para1;
    int event_para2;
    int event_para3;
    int event_para4;
    long trigger_clk;
    int core_id;
    struct event_node* next;
};
/************************************************************************************/ 
public:
    //tlb and core_queue init
    void cq_init();
    void tlb_init();
    

    // table and register init 
    bool table_init();
    virtual bool reg_next_init(int ppn)=0;

    // ins_init
    void Include_read_label(char *path_label);
    void lb_init(char *str_ins);
    void lb_free();
    int lb_search(char *token);
    // void ins_init();
    void ins_next_init(int ppn,int cn);
    void multi_ins_init();
    int GPR_value(char *gpr);


    //core_queue function
    virtual bool all_queue_empty()=0;
    virtual bool all_core_end()=0;
    virtual void core_queue_check()=0;
    
    // register function
    virtual int find_sr_num(char *reg_name)=0;
    int find_sr_sta(char *cond_name);
    virtual bool sr_check_correct(int ppn)=0;
    virtual void srpc_change(int ppn)=0;
    virtual void set_sr_sta_alu(int ppn,ex_wb_bus bus)=0;
    virtual void clear_sr_sta(int ppn,ex_wb_bus bus)=0;
    virtual void sr_ts_count(int ppn)=0;


    //table out and gen_table out
    // virtual void flow_table_out()=0;
    // virtual void gen_table_out()=0;

    //macro_caculate
    int calcDirect(char str[], lb_list *L);

    virtual ~ppeInit(){}
public:
//core_queue and tlb module
/************************************************************************************/
    core_queue core_queues[20];

    tlb_module tlb[256];

    tlb_list *free_tlb_list;
/************************************************************************************/
    ib_module ib;
    
    ib_module eb;

    hb_module hb[128];

    long int clk=0;

    bool end_cond;

    int ppn=1;

// pipeline module
/************************************************************************************/
    ins ins_mem[16][4096];

    if_id_bus cur_if_id_bus[128];

    id_ex_bus cur_id_ex_bus[128];

    ex_wb_bus cur_ex_wb_bus[128];

    wb_bus cur_wb_bus[128];

    REG_TYPE sr[128][32];
// GPR
/************************************************************************************/
    REG_TYPE gpr[128][32];

//lb_list
/************************************************************************************/
    lb_list *label_list;
    
/************************************************************************************/

// table_out
    q_table q_table_pool[64];
    g_table g_table_pool[64];

    REG_TYPE mr[64];

private:
    lb_list *cur;
    mfun_list *macrofun_list;
    mfun_list *mFun_index = NULL;


};


}





#endif