#include"ppe.h"
#include<bm/bm_sim/debugger.h>



void ppe_sim::cq_init()
{
    // FILE *fp;
    // fp=fopen("cq.txt","r+");
    // if (!fp)    //fail to read file
    // {
    //     printf("can't open cq setting!\n");
    //     exit(0);
    // }
    // /*eg: 0 1 0 0 0 0 0 0*/
    // for (int i=0;i<16;i++)
    // {
    //     if (!feof(fp))
    //     {
    //         fscanf(fp,"%i",&core_queues[i].align_ins);
    //     }
    //     else
    //     {
    //         core_queues[i].align_ins=-1;
    //     }
    //     printf("%d:ins%d\n",i,core_queues[i].align_ins);
    // }
    for(int i=0;i<16;++i)
    {
        core_queues[i].align_ins = -1;
    }
    core_queues[0].align_ins=1;
}

bool ppe_sim::all_queue_empty()
{
    for (int i=0;i<16;i++)
    {
        if (core_queues[i].input_queue!=NULL)
        return false;
    }
    return true;
}

bool ppe_sim::all_core_end()
{
    for (int i=0;i<16;i++)
    {
        for (int j=0;j<8;j++)
        {
            if (core_queues[i].is_used[j]==true)
            return false;
        }
    }
    return true;
}

void ppe_sim::core_queue_check()
{
    for (int i=0;i<16;i++)
    {
        //��ÿ���˼��������飺
        tlb_des_queue *cur=core_queues[i].input_queue;
        int fullused=0;
        while (cur!=NULL && !fullused)  //�����������Ƿ��й���Ҫ�������Ƿ��п��еĴ�����
        {
            fullused=1;
            //is_used��һ�����������Ƿ���ת
            //1���������ڹ���
            //0�����������У����԰�����������
            for (int j=0;j<8;j++)   //������������������Ĵ����������
            {
                if (core_queues[i].is_used[j]==0)   //�������������
                {
                    fullused=0;
                    //�ѹ����������
                    core_queues[i].is_used[j]=1;    
                    int core_id=i*8+j;//���������������Ӧ����Դ��index
                    //�������������Ҫ�����ݸ�������
                    //SMB(TLB)������ÿ����������Local Buffer(hb)
                    //SMB:16bit���ݣ�����hb[0]:8bit
                    //[hb[1],hb[0]]=smb[0]
                    for (int k=0;k<cur->length;k++)
                    {
                        //512+256=768
                        hb[core_id].head_buffer[768+k*2]=cur->tlb_num[k]%256;
                        hb[core_id].head_buffer[768+k*2+1]=cur->tlb_num[k]>>8;
                        printf("Write SMB%d:0x%x\n",k,
                        hb[core_id].head_buffer[768+k*2]+(hb[core_id].head_buffer[768+k*2+1]<<8));
                    }
                    //Mailbox: 8bit
                    for (int k=0;k<8;k++)
                    {
                        hb[core_id].head_buffer[cur->mb_addr+k]=cur->mb_content[k];
                    }
                    //�Ĵ�����ʼ��
                    #if 0
                    reg_next_init(core_id);
                    #endif
                    printf("QueNumber%d,load event into %d PPE\n",i,core_id);
                    break;
                }
            }
            if (!fullused)
            {
                //������������ͷŵ�
                tlb_des_queue *pre=cur;
                cur=cur->next;
                core_queues[i].input_queue=cur;
                free(pre);
            }
            else
                printf("QueNumber%d,All working.Wait.\n",i);
        }
    }
}

void ppe_sim::tlb_init()
{
    tlb_list * list0=(tlb_list *)malloc(sizeof(tlb_list));
    tlb[0].used=0;
    list0->tlb_num=0;
    list0->next_free=NULL;
    free_tlb_list=list0;
    tlb_list *pre=list0;
    for (int i=1;i<256;i++)
    {
        tlb_list * token=(tlb_list *)malloc(sizeof(tlb_list));
        tlb[i].used=0;
        token->tlb_num=i;
        token->next_free=NULL;
        pre->next_free=token;
        pre=token;
    }
}

bool ppe_sim::pack_to_tlb(ib_module *ingress, int fcs=0, int port=0)
{
    int mem_offset, sys_meta;
    // //���ݶ˿ںŻ�ȡƫ����
    // for (int i = 0; i < 4; i++)
    // {
    //     if (port == configMap[i].port)
    //         mem_offset = configMap[i].offset;
    // }
    //�ж��Ƿ�Ԥ��sys_metadata
    if (port == 0 || port == 1)
        sys_meta = 32;
    int used_num = (ingress->length + mem_offset + sys_meta) / 256 + ((ingress->length + mem_offset + sys_meta) % 256 != 0);
    int offset_num = (mem_offset + sys_meta) / 256;
    int offset = (mem_offset + sys_meta) % 256;
    tlb_list *cur = free_tlb_list;
    tlb_des_queue *token_tlb_des = (tlb_des_queue *)malloc(sizeof(tlb_des_queue));
    token_tlb_des->length = used_num;
    //��ib�洢��SMB/TLB��
    for (int i = 0; i < used_num; i++)
    {
        tlb[cur->tlb_num & 0x7fff].used = 1; //�������λ��Чλ
        if (i == offset_num)
        {
            for (int j = 0; j < 256 - offset; j++)
            {
                tlb[cur->tlb_num & 0x7fff].content[j + offset] = ingress->ingress_buffer[(i - offset_num) * 256 + j]; //�������λ��Чλ
            }
        }
        else if (i > offset_num)
        {
            for (int j = 0; j < 256; j++)
            {
                tlb[cur->tlb_num & 0x7fff].content[j] = ingress->ingress_buffer[(i - offset_num) * 256 + j - offset]; //�������λ��Чλ
            }
        }
        token_tlb_des->tlb_num[i] = cur->tlb_num | 0x8000; //��TLB��λ����Ϊ��Ч
        printf("set 0x%x\n", cur->tlb_num & 0x7fff);
        tlb_list *pre = cur;
        free_tlb_list = cur->next_free;
        cur = free_tlb_list;
        free(pre);
    }
    //����Mailboxλ
    token_tlb_des->mb_addr = 1024;
    token_tlb_des->mb_content[0] = ingress->length >> 8;
    token_tlb_des->mb_content[1] = ingress->length % 256;
    token_tlb_des->mb_content[2] = mem_offset;
    token_tlb_des->mb_content[3] = 0;
    token_tlb_des->mb_content[4] = fcs | (port << 2);
    token_tlb_des->mb_content[5] = 0;
    token_tlb_des->mb_content[6] = 0;
    token_tlb_des->mb_content[7] = 0;
    token_tlb_des->next = NULL;
    if (core_queues[port].input_queue == NULL)
    {
        core_queues[port].input_queue = token_tlb_des;
    }
    else
    {
        tlb_des_queue *cur = core_queues[port].input_queue;
        while (cur->next != NULL)
        {
            cur = cur->next;
        }
        cur->next = token_tlb_des;
    }
    return 1;
}

ppe_sim::ppe_sim():fcs(0),port(0)
{
    
    cq_init();
    tlb_init();
    printf("--------ppe_init-------");
  
}



void ppe_sim::transmit(ib_module &&input_buffer)
{
    // output_buffer(std::move(input_buffer));
}