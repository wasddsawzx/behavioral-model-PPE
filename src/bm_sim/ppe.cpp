#include<bm/bm_sim/ppe.h>
#include<bm/bm_sim/debugger.h>
#include<string.h>
// #include<bm/bm_sim/packet.h>

namespace bm
{

int ppeInit::calcDirect(char str[], lb_list *L)
{
    struct
    {
        char data[150];
        int top;
    } op_symbol;
    struct
    {
        int data[150];
        int top;
    } op_data;

    op_symbol.top = -1;
    op_data.top = -1;

    char ch;
    char tempStr[50];
    int tempData;
    int od1, od2;
    char temp_op;
    bool last_digit=0;
    bool last_op=0;
    int last_cal=0; //+:1,-:2 other:0
    bool double_op=0;

    int length = strlen(str);
    int i = 0;
    int j = 0;

    while (i < length)
    {
        ch = str[i];

        if ((ch >= '0' && ch <= '9')) //��Ϊ����
        {
            j = 0;
            memset(tempStr, 0, 50);
            tempData = 0;
            int is_hex=0;
            int is_bin=0;
            if (str[i]=='0' && (str[i+1]=='b' || str[i+1]=='B'))
            {
                is_bin=1;
            }
            else if (str[i]=='0' && (str[i+1]=='x' || str[i+1]=='X'))
            {
                is_hex=1;
            }
            if (is_bin==0 && is_hex==0)
            {
                while ((ch >= '0' && ch <= '9'))
                {
                    tempStr[j++] = ch;
                    ch = str[++i];
                }
                tempStr[j] = '\0';
                int scrt=sscanf(tempStr,"%d",&tempData);
                if (scrt==0)
                {
                    printf("error! data cannot be recognized\n");
                    exit(0);
                }
            }
            else if (is_bin==1)
            {
                while ((ch >= '0' && ch <= '1') || ch=='b' || ch=='B')
                {
                    tempStr[j++] = ch;
                    ch = str[++i];
                }
                tempStr[j] = '\0';
                int scrt=sscanf(tempStr,"%i",&tempData);
                if (scrt==0)
                {
                    printf("error! data cannot be recognized\n");
                    exit(0);
                }
            }
            else if (is_hex==1)
            {
                while ((ch >= '0' && ch <= '9') || (ch=='x'|| ch=='X') || (ch>='a' && ch<='f') || (ch>='A' && ch<='F'))
                {
                    tempStr[j++] = ch;
                    ch = str[++i];
                }
                tempStr[j] = '\0';
                int scrt=sscanf(tempStr,"%i",&tempData);
                if (scrt==0)
                {
                    printf("error! data cannot be recognized\n");
                    exit(0);
                }
            }
            op_data.data[++op_data.top] = tempData;
            last_digit=1;
            last_op=0;
            last_cal=0;
        }
        else if (!double_op && ch=='>' && str[i+1]=='>')
        {
            ch==str[++i];
            double_op=1;
        }
        else if (!double_op && ch=='>')
        {
            printf("error! symbol >\n");
            exit(0);
        }
        else if (!double_op && ch=='<' && str[i+1]=='<')
        {
            ch==str[++i];
            double_op=1;
        }
        else if (!double_op && ch=='<')
        {
            printf("error! symbol <\n");
            exit(0);
        }
        else if (isOpsymbol(ch)) //��Ϊ��������
        {
            if (ch=='>' || ch=='<')
                double_op=0;
            if (ch=='-' && last_cal!=2 && last_digit==0)
            {
                ch = '@'; 
            }
            else if (ch=='-' && last_cal==2)
            {
                printf("error!--\n");
                exit(0);
            }
            if (ch=='+' && last_cal!=1 && last_digit==0) 
            {
                ch = '#';
            }
            else if (ch=='-' && last_cal==1)
            {
                printf("error!++\n");
                exit(0);
            }
            if (ch!='@' && ch!='#' && ch!='~' && ch!='(' && last_op==1)
            {
                printf("error! extra op\n");
                exit(0);
            }
            if (op_symbol.top == -1)
            {
                op_symbol.data[++op_symbol.top] = ch;
                if (ch==')')
                {
                    printf("error! extra )\n");
                    exit(0);
                }
                else if (ch=='(')
                {
                    last_op=0;
                    last_cal=0;
                    last_digit=0;
                }
                else if (ch=='+' || ch=='-')
                {
                    last_op=1;
                    last_cal=(ch=='+')?1:2;
                    last_digit=0;
                }
                else
                {
                    last_op=1;
                    last_cal=0;
                    last_digit=0;
                }
            }
            else if (ch == '(')
            {
                op_symbol.data[++op_symbol.top] = ch;
                last_op=0;
                last_cal=0;
                last_digit=0;
            }
            else if (ch == ')')
            {
                
                while (levelOp(op_symbol.data[op_symbol.top]) != -1)
                {
                    if (op_symbol.data[op_symbol.top]=='@' || op_symbol.data[op_symbol.top]=='#'
                    || op_symbol.data[op_symbol.top]=='~')
                    {
                        od1 = op_data.data[op_data.top];
                        od2 = 0;
                        temp_op = op_symbol.data[op_symbol.top--];
                        op_data.data[op_data.top]=calcValue(od1,od2,temp_op);
                    }
                    else
                    {
                        od2 = op_data.data[op_data.top--];
                        od1 = op_data.data[op_data.top];
                        temp_op = op_symbol.data[op_symbol.top--];
                        op_data.data[op_data.top] = calcValue(od1, od2, temp_op);
                    }
                    if (op_symbol.top == -1)
                        {
                            printf("error!extra )\n");
                            exit(0);
                        } 
                }
                op_symbol.top--; 
                last_digit=1;
                last_op=0;
                last_cal=0;
            }
            
            
            else if ((levelOp(ch) > levelOp(op_symbol.data[op_symbol.top])) ||
            ((levelOp(ch) == levelOp(op_symbol.data[op_symbol.top])) &&
            levelOp(ch)==7))
            {
                op_symbol.data[++op_symbol.top] = ch;
                last_digit=0;
                last_cal=(ch=='+')?1:(ch=='-')?2:0;
                last_op=1;
            }
            else
            {
                //�����ɨ��Ĳ��������ȵȼ�û��ջ��Ԫ�ظߣ�
                //һֱ��ջֱ����ջ��Ԫ�����ȼ��ߣ����������ջ
                while (levelOp(ch) <= levelOp(op_symbol.data[op_symbol.top]))
                {
                    if (op_symbol.data[op_symbol.top]=='@' || op_symbol.data[op_symbol.top]=='#'
                    || op_symbol.data[op_symbol.top]=='~')
                    {
                        od1 = op_data.data[op_data.top];
                        od2 = 0;
                        temp_op = op_symbol.data[op_symbol.top--];
                        op_data.data[op_data.top]=calcValue(od1,od2,temp_op);
                    }
                    else
                    {
                        od2 = op_data.data[op_data.top--];
                        if (op_data.top==-1)
                        {
                            printf("error! lack op\n");
                            exit(0);
                        }
                        od1 = op_data.data[op_data.top];
                        temp_op = op_symbol.data[op_symbol.top--];
                        op_data.data[op_data.top] = calcValue(od1, od2, temp_op);
                    }
                        if (op_symbol.top == -1)
                            break; //���������ջΪ�գ�����ѭ��
                }
                op_symbol.data[++op_symbol.top] = ch;
                last_digit=0;
                last_op=1;
                last_cal=(ch=='+')?1:(ch=='-')?2:0;
            }

            i++;
        }
        else if ((ch >= 'A' && ch <= 'Z') || (ch == '_') || (ch >= 'a' && ch <= 'z')) //��Ϊ�Ѷ���ĺ�
        {
            j = 0;
            lb_list *s = L;
            while (!isOpsymbol(str[i]) && str[i]!='\0')
            {
                tempStr[j] = str[i];
                j++;
                i++;
            }
            tempStr[j] = '\0';
            while (s != NULL)
            {
                if (strcmp(s->label_name, tempStr) == 0)
                {
                    tempData = s->value;
                    break;
                }
                s = s->next;
            }
            if (s==NULL)
            {
                printf("error! wrong label/macro\n");
                break;
            }
            
            op_data.data[++op_data.top] = tempData;
            last_digit=1;
            last_op=0;
            last_cal=0;
        }
        else
        {
            printf("Expression %s error!\n",tempStr);
            exit(0);
        }
    }
    while (op_symbol.top != -1) //ɨ����������������ջ��Ϊ�գ���ջֱ��Ϊ��
    {
        if (op_symbol.data[op_symbol.top]=='(')
        {
            printf("error! extra (\n");
            exit(0);
        }
        if (op_symbol.data[op_symbol.top]=='@' || op_symbol.data[op_symbol.top]=='#'
                    || op_symbol.data[op_symbol.top]=='~')
        {
            od2=0;
            od1 = op_data.data[op_data.top];
            temp_op = op_symbol.data[op_symbol.top--];
            op_data.data[op_data.top] = calcValue(od1, od2, temp_op);
        }
        else
        {
            od2 = op_data.data[op_data.top--];
            if (op_data.top==-1)
            {
                printf("error! lack num\n");
                exit(0);
            }
            od1 = op_data.data[op_data.top];
            temp_op = op_symbol.data[op_symbol.top--];
            op_data.data[op_data.top] = calcValue(od1, od2, temp_op); //����������ֵջ
        }
    }
    if (op_data.top > 0)
    {
        printf("error! extra number\n");
        exit(0);
    }
    return op_data.data[op_data.top]; //��������ֵջ�е���pop��������Ϊ������
}



void ppeInit::cq_init()
{
    printf("cq_init!\n");
    FILE *fp;
    fp=fopen(def_cq_path,"r+");
    if (!fp)    //fail to read file
    {
        printf("can't open cq setting!\n");
        exit(0);
    }
    /*eg: 0 1 0 0 0 0 0 0*/
    for (int i=0;i<16;i++)
    {
        if (!feof(fp))
        {
            fscanf(fp,"%i",&core_queues[i].align_ins);
        }
        else
        {
            core_queues[i].align_ins=-1;
        }
        printf("%d:ins%d\n",i,core_queues[i].align_ins);
    }
    // for(int i=0;i<16;++i)
    // {
    //     core_queues[i].align_ins = -1;
    // }
    // core_queues[0].align_ins=1;
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
/************************************************************************************/
bool ppe_sim::reg_next_init(int ppn)
{
    int i = 0;
    int num = 0;
    char str_gpr[100] = {0};
    char str_fin[100] = {0};
    char str_ppn[100] = {0};
    char str_sr[100] = {0};
    memcpy(str_gpr, def_gpr_path, strlen(def_gpr_path)-4);
    memcpy(str_sr, def_sr_path, strlen(def_sr_path)-4);
    memcpy(str_fin, ".txt", 4);
    sprintf(str_ppn, "_%d", ppn >> 3);
    strcat(str_gpr, str_ppn);
    strcat(str_gpr, str_fin);
    strcat(str_sr, str_ppn);
    strcat(str_sr, str_fin);
    FILE *fp;
    fp = fopen(str_gpr, "r+");
    if (!fp)
    {
        printf("can't open %s\n", str_gpr);
        for (i = 0; i < 32; i++)
        {
            gpr[ppn][i] = 0;
        }
        // exit(0);
    }
    else
    {
        for (i = 0; i < 32; i++)
        {
            gpr[ppn][i] = 0;
        }
        while (!feof(fp))
        {
            fscanf(fp, "%d", &num);
            fscanf(fp, "%i", &gpr[ppn][num]);
        }
        if (gpr[ppn][31] != 0)
        {
            gpr[ppn][31] = 0;
            printf("Warning: gprz(31) need to be 0,has been corrected\n");
        }
        fclose(fp);
    }
    fp = fopen(str_sr, "r+");
    if (!fp)
    {
        printf("can't open %s\n", str_sr);
        for (i = 0; i < 32; i++)
        {
            sr[ppn][i] = 0;
        }
        // exit(0);
    }
    else
    {
        char c[100]; // temporary SR name
        char *token;
        for (i = 0; i < 32; i++)
        {
            sr[ppn][i] = 0;
        }
        while (!feof(fp))
        {
            char *ce = fgets(c, 100, fp);
            if (ce == NULL)
                continue;
            token = strtok(c, " ");
            if (token == NULL)
                continue;
            if (strcmp(token, "END") == 0 || strcmp(token, "END\n") == 0)
                break;
            i = find_sr_num(token);
            if (i == -1)
            {
                printf("Error in %s\n", c);
                exit(0);
            }
            else
            {
                token = strtok(NULL, " \t\r\n");
                sscanf(token, "%i", &sr[ppn][i]);
                printf("set SR %d: %d\n", i, sr[ppn][i]);
            }
        }
        if (sr_check_correct(ppn))
        {
            printf("Error occurred and been corrected\n");
        }
        //    sr[ppn][SR_MBR_PKTLEN]=ib.length;

        fclose(fp);
    }
    sr[ppn][SR_STA_FLAG] |= SR_STA_FLAG_ON;
    
    return 1;
}





bool ppe_sim::sr_check_correct(int ppn)
{
    int flag = 0;

    sr[ppn][SR_COREID] = ppn;
    if (sr[ppn][SR_STA_FLAG] != 0)
    {
        printf("FLAG cannot be set\n");
        sr[ppn][SR_STA_FLAG] = 0;
        flag = 1;
    }
    if (sr[ppn][SR_STA_ERR] != 0)
    {
        printf("ERR cannot be set\n");
        sr[ppn][SR_STA_ERR] = 0;
        flag = 1;
    }
    return flag;
}
int ppe_sim::find_sr_num(char *reg_name)
{
    int i;
    char *space = strchr(reg_name, ' ');
    char *script = strchr(reg_name, '/');
    char *tab = strchr(reg_name, '\t');
    if (space != NULL)
        *space = '\0';
    if (script != NULL)
        *script = '\0';
    if (tab != NULL)
        *tab = '\0';
    if (strcmp(reg_name, "SR_SYS.PC") == 0)
        return 0;
    else if (strcmp(reg_name, "SR_SYS.COREID") == 0)
        return 1;
    else if (strcmp(reg_name, "SR_SYS.STA") == 0)
        return 2;
    else if (strcmp(reg_name, "SR_SYS.ERR") == 0)
        return 3;
    else if (strcmp(reg_name, "SR_SYNC.CLUST") == 0)
        return 4;
    else if (strcmp(reg_name, "SR_SYNC.GLB") == 0)
        return 5;
    else if (strcmp(reg_name, "SR_SYNC.CFG") == 0)
        return 6;
    // else if (strcmp(reg_name,"SR_SYNC_GLB")==0)
    // return 7;
    else if (strcmp(reg_name, "SR_STA.FLAG") == 0)
        return 4;
    else if (strcmp(reg_name, "SR_STA.ERR") == 0)
        return 5;
    else if (strcmp(reg_name, "SR_MBR.PKTLEN") == 0)
        return 8;
    else if (strcmp(reg_name, "SR_MBR.PORTID") == 0)
        return 9;
    else if (strcmp(reg_name, "SR_MBT.MSG0") == 0)
        return 12;
    else if (strcmp(reg_name, "SR_MBT.MSG1") == 0)
        return 13;
    else if (strcmp(reg_name, "SR_MBT.CMD") == 0)
        return 14;
    else if (strcmp(reg_name, "SR_SAR.HASHL") == 0)
        return 8;
    else if (strcmp(reg_name, "SR_SAR.HASHH") == 0)
        return 9;
    else if (strcmp(reg_name, "SR_SAR.CKS") == 0)
        return 10;
    else if (strcmp(reg_name, "SR_TS.T0") == 0)
        return 12;
    else if (strcmp(reg_name, "SR_TS.T1") == 0)
        return 13;
    else if (strcmp(reg_name, "SR_TS.T2") == 0)
        return 14;
    else if (strcmp(reg_name, "SR_TS.T3") == 0)
        return 15;
    else if (strcmp(reg_name, "SR_GPT.T0") == 0)
        return 16;
    else if (strcmp(reg_name, "SR_GPT.T1") == 0)
        return 17;
    else if (strcmp(reg_name, "SR_GPT.T2") == 0)
        return 18;
    else if (strcmp(reg_name, "SR_GPT.CTRL") == 0)
        return 19;
    else if (strcmp(reg_name, "SR_WM.IBM") == 0)
        return 28;
    else if (strcmp(reg_name, "SR_WM.EBM") == 0)
        return 29;
    else if (strcmp(reg_name, "SR_WM.IBL") == 0)
        return 30;
    else if (strcmp(reg_name, "SR_WM.EBL") == 0)
        return 31;
    else if (strcmp(reg_name, "SR_HCFG.CFG0") == 0)
        return 20;
    else if (strcmp(reg_name, "SR_HCFG.CFG1") == 0)
        return 21;
    else if (strcmp(reg_name, "SR_HCFG.CFG2") == 0)
        return 22;
    else if (strcmp(reg_name, "SR_HCFG.CFG3") == 0)
        return 23;
    else
    {
        printf("no such SR!\n");
        exit(-1);
    }
    if (space != NULL)
        *space = ' ';
    if (script != NULL)
        *script = '/';
    if (tab != NULL)
        *tab = '\t';
}




int ppeInit::find_sr_sta(char *cond_name)
{
    int i;
    // if (strcmp(cond_name,"CKS")==0)
    // return SR_STA_FLAG_CKS;
    char *space = strchr(cond_name, ' ');
    char *script = strchr(cond_name, '/');
    char *tab = strchr(cond_name, '\t');
    if (space != NULL)
        *space = '\0';
    if (script != NULL)
        *script = '\0';
    if (tab != NULL)
        *tab = '\0';
    if (strcmp(cond_name, "TOF") == 0)
        return SR_STA_FLAG_TOF;
    else if (strcmp(cond_name, "PBLC") == 0)
        return SR_STA_FLAG_HBLC;
    else if (strcmp(cond_name, "PBSC") == 0)
        return SR_STA_FLAG_HBSC;
    else if (strcmp(cond_name, "TBQC") == 0)
        return SR_STA_FLAG_QC;
    else if (strcmp(cond_name, "TBIC") == 0)
        return SR_STA_FLAG_TBIC;
    else if (strcmp(cond_name, "TBLC") == 0)
        return SR_STA_FLAG_TBLC;
    else if (strcmp(cond_name, "TBSC") == 0)
        return SR_STA_FLAG_TBSC;
    else if (strcmp(cond_name, "SMB") == 0)
        return SR_STA_FLAG_TLB;
    else if (strcmp(cond_name, "ICT") == 0)
        return SR_STA_FLAG_MBT;
    else if (strcmp(cond_name, "ICR") == 0)
        return SR_STA_FLAG_MBR0;
    else if (strcmp(cond_name, "MBR1") == 0)
        return SR_STA_FLAG_MBR1;
    else
    {
        printf("no such FLAG!\n");
        exit(-1);
    }
    if (space != NULL)
        *space = ' ';
    if (script != NULL)
        *script = '/';
    if (tab != NULL)
        *tab = '\t';
}

void ppe_sim::srpc_change(int ppn)
{
    if (cur_ex_wb_bus[ppn].stage_valid && cur_ex_wb_bus[ppn].pc_change == 1)
    {
        gpr[ppn][30] = sr[ppn][SR_PC];
        sr[ppn][SR_PC] = cur_ex_wb_bus[ppn].ex_result;
    }
    else if (cur_ex_wb_bus[ppn].stage_valid && cur_ex_wb_bus[ppn].pc_halt == 1)
        sr[ppn][SR_PC] = sr[ppn][SR_PC];
    else if (cur_id_ex_bus[ppn].stage_valid && cur_id_ex_bus[ppn].is_branch == 1)
        sr[ppn][SR_PC] = sr[ppn][SR_PC];
    else if (cur_id_ex_bus[ppn].stage_valid && cur_id_ex_bus[ppn].is_lsm == 1)
        sr[ppn][SR_PC] = sr[ppn][SR_PC];
    else if (cur_id_ex_bus[ppn].stage_valid && cur_ex_wb_bus[ppn].is_mhm > 0)
        sr[ppn][SR_PC] = sr[ppn][SR_PC];
    else
        sr[ppn][SR_PC]++;
}
void ppe_sim::set_sr_sta_alu(int ppn, ex_wb_bus bus)
{
    if (bus.stage_valid && bus.is_sr_alu == 1 && bus.sr_cy)
    {
        printf("ins:%s set sr_cy 1\n", bus.trd_ins.opname);
        sr[ppn][SR_STA_FLAG] |= SR_STA_FLAG_CY;
        bus.sr_cy = 0;
    }
    else if (bus.stage_valid && bus.is_sr_alu == 1)
    {
        printf("ins:%s set sr_cy 0\n", bus.trd_ins.opname);
        sr[ppn][SR_STA_FLAG] &= (~SR_STA_FLAG_CY);
        bus.sr_cy = 0;
    }

    if (bus.stage_valid && bus.is_sr_alu == 1 && bus.sr_zr)
    {
        printf("ins:%s set sr_zr 1\n", bus.trd_ins.opname);
        sr[ppn][SR_STA_FLAG] |= SR_STA_FLAG_ZR;
        bus.sr_zr = 0;
    }
    else if (bus.stage_valid && bus.is_sr_alu == 1)
    {
        printf("ins:%s set sr_zr 0\n", bus.trd_ins.opname);
        sr[ppn][SR_STA_FLAG] &= (~SR_STA_FLAG_ZR);
        bus.sr_zr = 0;
    }
    else if (bus.stage_valid && bus.is_sr_alu == 2 && bus.sr_zr == 1)
    {
        printf("ins:%s set sr_zr 1\n", bus.trd_ins.opname);
        sr[ppn][SR_STA_FLAG] |= SR_STA_FLAG_ZR;
        bus.sr_zr = 0;
    }
    else if (bus.stage_valid && bus.is_sr_alu == 2)
    {
        printf("ins:%s set sr_zr 0\n", bus.trd_ins.opname);
        sr[ppn][SR_STA_FLAG] &= (~SR_STA_FLAG_ZR);
        bus.sr_zr = 0;
    }
    bus.is_sr_alu = 0;
}
void ppe_sim::clear_sr_sta(int ppn, ex_wb_bus bus)
{
    if (bus.stage_valid && bus.is_clear_sr_sta)
    {
        sr[ppn][SR_STA_FLAG] &= (~bus.ex_result);
        printf("ins:%s set sr_sta 0x%x\n", bus.trd_ins.opname, sr[ppn][SR_STA_FLAG]);
    }
}
void ppe_sim::sr_ts_count(int ppn)
{
    sr[ppn][SR_TS_T0] = clk % 0x10000;
    sr[ppn][SR_TS_T1] = (clk / 0x10000) % 0x10000;
    sr[ppn][SR_TS_T2] = ((clk / 0x10000) / 0x10000) % 0x10000;
    sr[ppn][SR_TS_T3] = (((clk / 0x10000) / 0x10000) / 0x10000);
    // printf("SR_TS %d %d %d %d\n",sr[ppn][SR_TS_T3],sr[ppn][SR_TS_T2],sr[ppn][SR_TS_T1],sr[ppn][SR_TS_T0]);
    if ((sr[ppn][SR_GPT_CTRL] & 0x0001) != 0)
    {
        bool ofe = false;
        if ((sr[ppn][SR_GPT_CTRL] & 0x0002) != 0)
            ofe = true;
        if ((sr[ppn][SR_GPT_T0] == 0xffff) && (sr[ppn][SR_GPT_T1] == 0xffff) && (sr[ppn][SR_GPT_T2] == 0xffff))
        {
            sr[ppn][SR_GPT_T0] = 0;
            sr[ppn][SR_GPT_T1] = 0;
            sr[ppn][SR_GPT_T2] = 0;
            if (ofe)
                sr[ppn][SR_STA_FLAG] |= SR_STA_FLAG_TOF;
        }
        else if ((sr[ppn][SR_GPT_T0] == 0xffff) && (sr[ppn][SR_GPT_T1] == 0xffff))
        {
            sr[ppn][SR_GPT_T0] = 0;
            sr[ppn][SR_GPT_T1] = 0;
            sr[ppn][SR_GPT_T2]++;
        }
        else if (sr[ppn][SR_GPT_T0] == 0xffff)
        {
            sr[ppn][SR_GPT_T0] = 0;
            sr[ppn][SR_GPT_T1]++;
        }
        else
        {
            sr[ppn][SR_GPT_T0]++;
        }
    }
}


/************************************************************************************/



bool ppeInit::table_init()
{
    long i, count = 0;
    long table_num = 0;
    int buf;
    FILE *fp;
    //���ļ�
    printf("load flowt\n");
    fp = fopen(def_ft_path, "r+");
    if (!fp) // fail to read file
    {
        printf("can't open flow table setting!\n");
        exit(0);
    }
    char c[100];
    char *token;
    fgets(c, 100, fp);
    token = strtok(c, ":");
    token = strtok(NULL, "\r\n");
    sscanf(token, "%i", &table_num);
    int table_cnt = 0;
    int item_cnt = -1;
    int data_cnt = -1;
    item_list *token_item;
    while (!feof(fp)) //��Ƕ�ף���
    {
        char *ce = fgets(c, 100, fp);
        if (ce == NULL)
            continue;
        token = strtok(c, ":");
        if (strcmp(token, "TableID") == 0)
        {
            token = strtok(NULL, "\r\n");

            sscanf(token, "%i", &q_table_pool[table_cnt].TableID);
        }
        else if (strcmp(token, "ItemNumMax") == 0)
        {
            token = strtok(NULL, "\r\n");
            sscanf(token, "%i", &q_table_pool[table_cnt].ItemNumMax);
        }
        else if (strcmp(token, "ItemSize") == 0)
        {
            token = strtok(NULL, "\r\n");
            sscanf(token, "%i", &q_table_pool[table_cnt].ItemSize);
        }
        else if (strcmp(token, "QuerySize") == 0)
        {
            token = strtok(NULL, "\r\n");
            sscanf(token, "%i", &q_table_pool[table_cnt].QuerySize);
        }
        else if (strcmp(token, "Item") == 0)
        {
            token_item = (item_list *)malloc(sizeof(item_list));
            token_item->next = NULL;
            item_cnt++;
            data_cnt = 0;
            token_item->ItemID = item_cnt;
            if (q_table_pool[table_cnt].Table == NULL)
            {
                q_table_pool[table_cnt].Table = token_item;
            }
            else
            {
                item_list *cur = q_table_pool[table_cnt].Table;
                while (cur->next != NULL)
                {
                    cur = cur->next;
                }
                cur->next = token_item;
            }
        }
        else if (strcmp(token, "TableEnd\n") == 0 || strcmp(token, "TableEnd") == 0)
        {
            item_list *cur = q_table_pool[table_cnt].Table;
            while (cur != NULL)
            {
                for (int i = 0; i < q_table_pool[table_cnt].ItemSize; i++)
                {
                    printf("%d ", cur->Item[i]);
                }
                printf("\r\n");
                cur = cur->next;
            }
            table_cnt++;
        }
        else
        {
            token = strtok(c, " \t\r\n");
            while (token != NULL)
            {
                sscanf(token, "%i", &token_item->Item[data_cnt]);
                data_cnt++;
                token = strtok(NULL, " \t\r\n");
            }
        }
    }
    fclose(fp);

    printf("load gent\n");
    fp = fopen(def_gt_path, "r+");
    if (!fp) // fail to read file
    {
        printf("can't open generic table setting!\n");
        exit(0);
    }

    fgets(c, 100, fp);
    token = strtok(c, ":");
    token = strtok(NULL, "\r\n");
    sscanf(token, "%i", &table_num);
    table_cnt = 0;
    item_cnt = -1;
    data_cnt = -1;

    while (!feof(fp)) //��Ƕ�ף���
    {
        char *ce = fgets(c, 100, fp);
        if (ce == NULL)
            continue;
        token = strtok(c, ":");
        if (strcmp(token, "TableID") == 0)
        {
            token = strtok(NULL, "\r\n");

            sscanf(token, "%i", &g_table_pool[table_cnt].TableID);
        }

        else if (strcmp(token, "ItemSize") == 0)
        {
            token = strtok(NULL, "\r\n");
            sscanf(token, "%i", &g_table_pool[table_cnt].ItemSize);
        }
        else if (strcmp(token, "ItemID") == 0)
        {
            token = strtok(NULL, "\r\n");
            sscanf(token, "%i", &item_cnt);
        }
        else if (strcmp(token, "Item") == 0)
        {
            token_item = (item_list *)malloc(sizeof(item_list));
            token_item->next = NULL;
            // item_cnt++;
            data_cnt = 0;
            token_item->ItemID = item_cnt;
            if (g_table_pool[table_cnt].Table == NULL)
            {
                g_table_pool[table_cnt].Table = token_item;
            }
            else
            {
                item_list *cur = g_table_pool[table_cnt].Table;
                while (cur->next != NULL)
                {
                    cur = cur->next;
                }
                cur->next = token_item;
            }
        }
        else if (strcmp(token, "TableEnd\n") == 0 || strcmp(token, "TableEnd") == 0)
        {
            item_list *cur = g_table_pool[table_cnt].Table;
            while (cur != NULL)
            {
                for (int i = 0; i < g_table_pool[table_cnt].ItemSize; i++)
                {
                    printf("%d ", cur->Item[i]);
                }
                printf("\n");
                cur = cur->next;
            }
            table_cnt++;
            item_cnt = -1;
        }
        else
        {
            token = strtok(c, " \t\r\n");
            while (token != NULL)
            {
                sscanf(token, "%i", &token_item->Item[data_cnt]);
                data_cnt++;
                token = strtok(NULL, " \t\r\n");
            }
        }
    }
    fclose(fp);

    return 1;
}

/************************************************************************************/

void ppeInit::multi_ins_init()
{
    for (int i = 0; i < 16; i++)
    {
        ins_next_init(core_queues[i].align_ins, i);
    }
}

void ppeInit::ins_next_init(int ppn,int cn)
{
    char str_ins[100] = {0};
    char str_fin[100] = {0};
    char str_ppn[100] = {0};
    memcpy(str_ins, def_ins_path, strlen(def_ins_path)-4);
    memcpy(str_fin, ".txt", 4);
    if (core_queues[cn].align_ins != -1)
    {
        sprintf(str_ppn, "_%d", ppn);
        strcat(str_ins, str_ppn);
    }
    strcat(str_ins, str_fin);
    printf("core[%d]  ins_file: %s\n", cn,str_ins);
    lb_init(str_ins);
    
    long i, count = 0;
    unsigned short line = 0;
    long size = 0;
    int buf;
    int macro_fun = 0; 
    int fun_ins = 0;   
    int mfun_flag = 0; 
    FILE *fp;
    //���ļ�
    printf("3\n");

    fp = fopen(str_ins, "r+");
    if (!fp) // fail to read file
    {
        printf("can't open ins_mem!\n");
        exit(0);
    }
    char c[200]; // temporary SR name
    char *token;
    int len;
    int multi_signal = 0;
    while (!feof(fp))
    {
        int begin_index = 0;
        int s_index = 0;
        int define_signal = 0;
        memset(c, 0, 200);
        if (macro_fun) //�жϺ꺯���Ƿ�ִ�����
        {
            if (fun_ins >= mFun_index->ins_num)
            {
                fun_ins = 0;
                macro_fun = 0;
                // strcpy(c,mFun_index->instruction[fun_ins]);
                // *strstr(c,".END")='\0';
            }
            else
            {
                // if(fun_ins==0)
                //    strcpy(c,strstr(mFun_index->instruction[fun_ins++],"/"));
                //     continue;
                // else
                strcpy(c, mFun_index->instruction[fun_ins++]);
                printf("funaction:%s", c);
                // char *ce = c;
                // if(!ce)
                //     continue;
            }
        }
        else
        {
            char *ce = fgets(c, 200, fp);
            if (ce == NULL)
                continue;
            line++;
        }
        // printf("file_line:%s+++++++++++++++++++\n", c);
        
        if (mfun_flag)
        {
            if (strstr(c, ".END"))
            {
                mfun_flag = 0;
                define_signal = 1;
            }
            else
                continue;
        }

        char *retb = strstr(c, "/*");
        char *rete = strstr(c, "*/");
        
        if (multi_signal == 1 && rete != NULL)
        {
            begin_index = rete + 2 - c; 
            multi_signal = 0;           
        }
        else if (multi_signal == 1)
            continue; 

        while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
            begin_index++;

        if (c[begin_index] == '\n' || c[begin_index] == '\r')
            continue;
        if (c[begin_index] == '/' && c[begin_index + 1] == '/')
            continue;
        else if (c[begin_index] == '/' && c[begin_index + 1] == '*')
        {
            
            rete = strstr(c + begin_index, "*/");
            int skip_signal = 0;
            while (rete != NULL && c[begin_index] == '/' && c[begin_index + 1] == '*')
            {
                begin_index = rete + 2 - c;
                while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
                    begin_index++;
                //�����һ���� //������
                if (c[begin_index] == '/' && c[begin_index + 1] == '/')
                {
                    skip_signal = 1;
                    break;
                }
                rete = strstr(c + begin_index, "*/");
            }
            if (rete == NULL && c[begin_index] == '/' && c[begin_index + 1] == '*')
            {
                multi_signal = 1;
                continue;
            }
            if (skip_signal)
                continue;
            /* */ /* */
        }
        while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
            begin_index++;
        if (c[begin_index] == '\n' || c[begin_index] == '\r')
            continue;
        token = strchr(c + begin_index, ':');
        if (token == NULL)
        {
            token = strtok(c + begin_index, " \t\r\n");
        }
        else
        {
            retb = strstr(c + begin_index, "/*");
            char *rets = strstr(c + begin_index, "//");
            if ((retb == NULL || token < retb) && (rets == NULL || token < rets))
            {
                begin_index = token + 1 - c;
                while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
                    begin_index++;
                if (c[begin_index] == '/' && c[begin_index + 1] == '/')
                    continue;
                else if (c[begin_index] == '/' && c[begin_index + 1] == '*')
                {
                    
                    rete = strstr(c + begin_index, "*/");
                    int skip_signal = 0;
                    while (rete != NULL && c[begin_index] == '/' && c[begin_index + 1] == '*')
                    {
                        begin_index = rete + 2 - c;
                        while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
                            begin_index++;
                       
                        if (c[begin_index] == '/' && c[begin_index + 1] == '/')
                        {
                            skip_signal = 1;
                            break;
                        }
                        rete = strstr(c + begin_index, "*/");
                    }
                    if (rete == NULL && c[begin_index] == '/' && c[begin_index + 1] == '*')
                    {
                        multi_signal = 1;
                        continue;
                    }
                    if (skip_signal)
                        continue;
                    /* */ /* */
                }
                while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
                    begin_index++;
                if (c[begin_index] == '\n' || c[begin_index] == '\r')
                    continue;
                token = strtok(c + begin_index, " \t");
            }
            else
            {
                token = strtok(c + begin_index, " \t");
            }
        }

        //          /* */ /* */ label: /* */
        if (token == NULL)
            continue;
        if (*token == '\n' || *token == '\r')
            continue;
        if (strcmp(token, "ADDI") == 0 || strcmp(token, "SUBI") == 0 || strcmp(token, "ADDCI") == 0 ||
            strcmp(token, "SUBBI") == 0 || strcmp(token, "ORI") == 0 || strcmp(token, "XORI") == 0 ||
            strcmp(token, "ANDI") == 0 || strcmp(token, "SRL") == 0 || strcmp(token, "SLL") == 0) // parse ALU-I
        {
            // ADDI Rd,Rs,#im16
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            len = strlen(token);
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + s_index + 3);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + s_index + 3);
            s_index = 0;
            token = strtok(NULL, ",");
            // ins_mem[cn][count].imm=atoi(token+1);
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            int ins_value = lb_search(token + 1 + s_index);
            if (ins_value == -1)
            {
                printf("Error label!\n");
                exit(0);
            }
            else
                ins_mem[cn][count].imm = ins_value;

            s_index = 0;
        }
        else if (strcmp(token, "CMP") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs2 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "MOVI") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, "MOVI");
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            // ins_mem[cn][count].rs1=31;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            int ins_value = lb_search(token + 1 + s_index);
            if (ins_value == -1)
            {
                printf("Error label!\n");
                exit(0);
            }
            else
                ins_mem[cn][count].imm = ins_value;
            s_index = 0;
        }
        else if (strcmp(token, "ADD") == 0 || strcmp(token, "ADDC") == 0 || strcmp(token, "SUBB") == 0 || strcmp(token, "SUB") == 0 || strcmp(token, "AND") == 0 || strcmp(token, "OR") == 0 || strcmp(token, "XOR") == 0 ||
                 strcmp(token, "MUL") == 0) // parse-ALU_R
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs2 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "BEQI") == 0 || strcmp(token, "BGTI") == 0 || strcmp(token, "BI") == 0 ||
                 strcmp(token, "BNEI") == 0 || strcmp(token, "BLTI") == 0) // branch-I
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, " \t");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            int ins_value = lb_search(token + 1 + s_index);
            if (ins_value == -1)
            {
                printf("Error label!\r\n");
                exit(0);
            }
            else
                ins_mem[cn][count].imm = ins_value;
            s_index = 0;
        }
        else if (strcmp(token, "B") == 0 || strcmp(token, "BNE") == 0 || strcmp(token, "BLT") == 0 || strcmp(token, "BGT") == 0 || strcmp(token, "BEQ") == 0) // branch-R
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, " \t");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "ISB") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = (strtok(NULL, "\r\n"));
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].imm = find_sr_sta(token + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "ISBC") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = (strtok(NULL, "\r\n"));
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].imm = find_sr_sta(token + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "LOADM") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs2 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "STOREM") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs2 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "LOADT") == 0)
        { // LOADT GPRt,MRd0,MRs0
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs2 = GPR_value(token + 4 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "STORET") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            int ins_value = lb_search(token + 1 + s_index);
            if (ins_value == -1)
            {
                printf("Error label!\n");
                exit(0);
            }
            else
                ins_mem[cn][count].imm = ins_value;
            s_index = 0;
        }
        else if (strcmp(token, "QUERYT") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs2 = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            int ins_value = lb_search(token + 1 + s_index);
            if (ins_value == -1)
            {
                printf("Error label!\n");
                exit(0);
            }
            else
                ins_mem[cn][count].imm = ins_value;
            s_index = 0;
        }
        else if (strcmp(token, "INSERTT") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            int ins_value = lb_search(token + 1 + s_index);
            if (ins_value == -1)
            {
                printf("Error label!\n");
                exit(0);
            }
            else
                ins_mem[cn][count].imm = ins_value;
            s_index = 0;
        }
        else if (strcmp(token, "DELETET") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            int ins_value = lb_search(token + 1 + s_index);
            if (ins_value == -1)
            {
                printf("Error label!\n");
                exit(0);
            }
            else
                ins_mem[cn][count].imm = ins_value;
            s_index = 0;
        }
        else if (strcmp(token, "NOP\n") == 0 || strcmp(token, "NOP") == 0 ||
                 strcmp(token, "NOP\r") == 0 || strcmp(token, "NOP\r\n") == 0)
        {
            ins_mem[cn][count].line = line;
            token[3] = '\0';
            strcpy(ins_mem[cn][count].opname, token);
        }
        else if (strcmp(token, "END") == 0 || strcmp(token, "END\n") == 0 ||
                 strcmp(token, "END\r") == 0 || strcmp(token, "END\r\n") == 0)
        {
            ins_mem[cn][count].line = line;
            token[3] = '\0';
            strcpy(ins_mem[cn][count].opname, token);
        }
        else if (strcmp(token, "MOVSR") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = find_sr_num(token + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "MOVRR") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "MOVRS") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = find_sr_num(token + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        // khh added:MOVBRLE,MOVBRBE
        else if (strcmp(token, "MOVBRLE") == 0 || strcmp(token, "MOVBRBE") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 4 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "MOVRBLE") == 0 || strcmp(token, "MOVRBBE") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "MOVHM") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            ins_mem[cn][count].rd = GPR_value(token + 2);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            int ins_value = lb_search(token + 1 + s_index);
            if (ins_value == -1)
            {
                printf("Error label!\n");
                exit(0);
            }
            else
                ins_mem[cn][count].imm = ins_value;
            s_index = 0;
        }
        else if (strcmp(token, "MOVMH") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            ins_mem[cn][count].rs1 = GPR_value(token + 2);
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            int ins_value = lb_search(token + 1 + s_index);
            if (ins_value == -1)
            {
                printf("Error label!\n");
                exit(0);
            }
            else
                ins_mem[cn][count].imm = ins_value;
            s_index = 0;
        }
        else if (strcmp(token, "MOVBB") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs2 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        // wzy added:CKSUMLE,CKSUMBE
        else if (strcmp(token, "CKSUMLE") == 0 || strcmp(token, "CKSUMBE") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "HASHLE") == 0 || strcmp(token, "HASHBE") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "MALCSMB") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "FREESMB") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "REFSMB") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "PUSHQ") == 0)
        {
            ins_mem[cn][count].line = line;
            // PUSHQ [GPRd],[GPRs],GPRI
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 4 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs2 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "SENDMB") == 0)
        {
            ins_mem[cn][count].line = line;
            strcpy(ins_mem[cn][count].opname, token);
            token = strtok(NULL, ",");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rd = GPR_value(token + 3 + s_index);
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            ins_mem[cn][count].rs1 = GPR_value(token + 3 + s_index);
            s_index = 0;
        }
        else if (strcmp(token, "#define") == 0)
        {
            token = strtok(NULL, " \t");
            // while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
            //     s_index++;
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            s_index = 0;
            define_signal = 1;
        }
        else if (strcmp(token, "#include") == 0)
        {
            continue;
        }
        else if (strcmp(token, ".MACRO") == 0)
        {
            mfun_flag = 1;
            define_signal = 1;
        }
        else if (!macro_fun)
        {
            
            mFun_index = macrofun_list;
            while (mFun_index->next)
            {
                mFun_index = mFun_index->next;
                if (strcmp(token, mFun_index->fun_name) == 0)
                {
                    printf("\nFIND MACROFUN!!!!!!!!!\n");
                    // fun_ins = mFun_index->ins_num;
                    macro_fun = 1;
                    define_signal = 1;
                    break;
                }
            }
        }
        else
        {
            printf("No such flag! Exit!");
            exit(0);
        }

        if (!define_signal)
        {
            printf("ins_file%d,Line%d:%s %d %d %d %d\n", ppn, ins_mem[cn][count].line, ins_mem[cn][count].opname, ins_mem[cn][count].rd, ins_mem[cn][count].rs1, ins_mem[cn][count].rs2, ins_mem[cn][count].imm);
            count++;
        }


        char *rets = strstr(token, "//");
        retb = strstr(token, "/*");
        rete = strstr(token, "*/");
        if ((rets != NULL && retb != NULL && rets > retb) ||
            (rets == NULL && retb != NULL))
        {
            if (rete == NULL)
            {
                multi_signal = 1;
                continue;
            }
            while (rete != NULL)
            {
                rets = strstr(rete + 2, "//");
                retb = strstr(rete + 2, "/*");
                if (retb != NULL)
                    rete = strstr(retb + 2, "*/");
                else
                    rete = NULL;
                if (rets != NULL && retb != NULL && rets < retb)
                    break;
            }
            if ((rets == NULL || retb < rets) && rete == NULL && retb != NULL)
                multi_signal = 1;
        }
    }
    fclose(fp);
    lb_free();
}

void ppeInit::lb_free()
{
    lb_list *cur = label_list;
    lb_list *pre = cur;

    while (cur != NULL)
    {
        pre = cur;
        cur = cur->next;
        pre->next = NULL;
        free(pre);
    }

    label_list = NULL;
}

int ppeInit::lb_search(char *token)
{
    char cmp_str[100];
    char *str_spa = strchr(token, ' ');
    char *str_tab = strchr(token, '\t');
    char *str_ent = strchr(token, '\r');
    if (str_ent == NULL)
        str_ent = strchr(token, '\n');
    char *str_scr = strchr(token, '/');
    int rel = 0;
    if (str_spa != NULL && (str_scr == NULL || str_spa < str_scr) && (str_tab == NULL || str_spa < str_tab))
        rel = strlen(str_spa);
    else if (str_tab != NULL && (str_scr == NULL || str_tab < str_scr) && (str_spa == NULL || str_tab < str_spa))
        rel = strlen(str_tab);
    else if (str_scr != NULL && (str_spa == NULL || str_scr < str_spa) && (str_tab == NULL || str_scr < str_tab))
        rel = strlen(str_scr);
    else if (str_ent != NULL)
        rel = strlen(str_ent);
    else
        rel = 0;
    // cmp_str=(char *)malloc(sizeof(strlen(token)-rel));
    memcpy(cmp_str, token, strlen(token) - rel);
    *(cmp_str + strlen(token) - rel) = '\0';
    lb_list *cur = label_list;
    int value = -1;
    if (*cmp_str >= '0' && *cmp_str <= '9')
    {
        sscanf(cmp_str, "%i", &value);
    }
    else
    { // printf("%s\n",cmp_str);
        while (cur != NULL)
        {
            // printf("%s\t",cur->label_name);
            if (strcmp(cmp_str, cur->label_name) == 0)
            {
                value = cur->value;
                break;
            }
            cur = cur->next;
        }
        // printf("\n");
    }
    return value;
}

void ppeInit::Include_read_label(char *path_label)
{
    FILE *fp_1 = fopen(path_label, "r+");
    FILE *fp_2;
    char token_2[100] = {0};
    int num = 0;                       
    char *str_1 = (char *)malloc(200); 

    int len_1;
    int multi_signal_1 = 0;
    long i = 0;
    int macro_fun = 0;
    mfun_list *mfun_cur = macrofun_list;
    while (mfun_cur->next != NULL)
    {
        mfun_cur = mfun_cur->next;
    }
    if (!fp_1)
    {
        printf("can't open ins_mem!\n");
        exit(0);
    }
    //�����ļ�����û��#include
    while (!feof(fp_1))
    {
        char *token_1;

        int flag = 0; //�ж��ļ����Ƿ���include
        int begin_index_1 = 0;
        int define_signal_1 = 0;
        memset(str_1, 0, 200);
        char *str_one_line = fgets(str_1, 200, fp_1);
        int s_index_1 = 0;

        if (str_one_line == NULL)
            continue;
        char *retb_1 = strstr(str_1, "/*");
        char *rete_1 = strstr(str_1, "*/");
        //ǰ��/**/����
        if (multi_signal_1 == 1 && rete_1 != NULL)
        {
            begin_index_1 = rete_1 + 2 - str_1; //������һ��*/ǰ�Ĳ���
            multi_signal_1 = 0;                 //�����־λ
        }
        else if (multi_signal_1 == 1)
            continue; //����δ����

        while (*(str_1 + begin_index_1) == ' ' || *(str_1 + begin_index_1) == '\t')
            begin_index_1++;

        if (str_1[begin_index_1] == '\n' || str_1[begin_index_1] == '\r')
            continue;

        if (str_1[begin_index_1] == '/' && str_1[begin_index_1 + 1] == '/')
            continue;
        else if (str_1[begin_index_1] == '/' && str_1[begin_index_1 + 1] == '*')
        {
            //����һ��*/,����Ҳ�������ômulti=1,conti,����ҵ���,�е����������ж�һ��
            rete_1 = strstr(str_1 + begin_index_1, "*/");
            int skip_signal_1 = 0;
            while (rete_1 != NULL && str_1[begin_index_1] == '/' && str_1[begin_index_1 + 1] == '*')
            {
                begin_index_1 = rete_1 + 2 - str_1;
                while (*(str_1 + begin_index_1) == ' ' || *(str_1 + begin_index_1) == '\t')
                    begin_index_1++;
                //�����һ���� //������
                if (str_1[begin_index_1] == '/' && str_1[begin_index_1 + 1] == '/')
                {
                    skip_signal_1 = 1;
                    break;
                }
                rete_1 = strstr(str_1 + begin_index_1, "*/");
            }
            if (rete_1 == NULL && str_1[begin_index_1] == '/' && str_1[begin_index_1 + 1] == '*')
            {
                multi_signal_1 = 1;
                continue;
            }
            if (skip_signal_1)
                continue;
            /* */ /* */
        }
        while (*(str_1 + begin_index_1) == ' ' || *(str_1 + begin_index_1) == '\t')
            begin_index_1++;
        if (str_1[begin_index_1] == '\n' || str_1[begin_index_1] == '\r')
            continue;
        token_1 = strtok(str_1 + begin_index_1, " \t");

        if (token_1 == NULL)
            continue;
        if (*token_1 == '\n' || *token_1 == '\r')
            continue;

        // token_1 = strtok(str_one_line, " \n\t\r");
        if (strcmp(token_1, "#include") == 0)
        {

            flag = 1;
            token_1 = strtok(NULL, " \r\n\t");
            printf("%s\n", token_1);
            memcpy(token_2, token_1 + 1, (strlen(token_1) - 2));
            // printf("token_path_name=%s", token_2);
        }
        if (strcmp(token_1, "#define") == 0)
        {
            lb_list *lb_token = (lb_list *)malloc(sizeof(lb_list));
            token_1 = strtok(NULL, " \t");
            // printf("%s\n",token);
            // while (*(token_1 + s_index_1) == ' ' || *(token_1 + s_index_1) == '\t')
            //     s_index_1++;
            if (token_1 == NULL)
            {
                printf("Macro definition lack name!");
                exit(1);
            }
            int macro_len = strlen(token_1);
            if ((*token_1 <= '9') && (*token_1 >= '0'))
            {
                printf("macro_name can't start with a number!\n");
                exit(0);
            }
            // printf("s1:%s\n",token+s_index);
            for (int i = 0; i < macro_len; i++)
            {
                if (!(
                        (*token_1 <= 'Z' && *token_1 >= 'A') ||
                        (*token_1 <= 'z' && *token_1 >= 'a') ||
                        (*token_1 <= '9' && *token_1 >= '0') ||
                        (*token_1 == '_')))
                {
                    printf("macro_name type error!\n");
                    exit(0);
                }
            }
            // lb_token->label_name=(char *)malloc(sizeof(strlen(token+s_index)));
            memcpy(lb_token->label_name, token_1, strlen(token_1));
            *((lb_token->label_name) + strlen(token_1)) = '\0';
            s_index_1 = 0;
            token_1 = strtok(NULL, "\r\n");
            if (token_1 == NULL)
            {
                printf("Macro definition lack value!");
                exit(1);
            }
            while (*(token_1 + s_index_1) == ' ' || *(token_1 + s_index_1) == '\t')
                s_index_1++;
            printf("\ntoken%s\n", token_1);
            char tempstr[80] = "";
            int is_anno = 0;
            token_1 = strtok(token_1, " \t\r\n");
            if (!token_1)
            {
                printf("Macro lack value!\n");
                exit(0);
            }
            while (token_1)
            {
                strcat(tempstr, token_1);
                token_1 = strtok(NULL, " \t\r\n");
            }
            printf("tempstr:%s\n", tempstr);
            token_1 = tempstr;
            while (*token_1 != '\0')
            {
                if (*token_1 == '/')
                {
                    if (*(token_1 + 1) == '/' || *(token_1 + 1) == '*')
                    {
                        printf("ano_content:%s\n", token_1);
                        *token_1 = '\0';
                        is_anno = 1;
                        break;
                    }
                }
                token_1++;
            }
            if (*tempstr == '\0')
            {
                printf("macro lack value!\n");
                continue;
            }
            printf("final:%s\n", tempstr);
            lb_token->value = calcDirect(tempstr, label_list);
            if (is_anno)
            {
                *token_1 = '/';
                printf("re_anno:%s\n", token_1);
            }
            s_index_1 = 0;
            define_signal_1 = 1;
            cur->next = lb_token;
            cur = cur->next;
            printf("new label:%s,value %d\n", lb_token->label_name, lb_token->value);
        }
        else if (strcmp(token_1, ".MACRO") == 0)
        {
            printf("\nFUNCTION\n");
            macro_fun = 1;
            int macro_ins_count = 0;
            mfun_list *fun_cur = (mfun_list *)malloc(sizeof(mfun_list));
            fun_cur->next = NULL;
            // strcpy(fun_cur->instruction[macro_ins_count++],token);
            if (!(token_1 = strtok(NULL, " \t\r\n")))
            {
                printf("Lack function name\n");
                exit(0);
            }
            // else if(strstr(token,"/"))
            // {
            //     *strstr(token,"/")='\0';
            // }
            mFun_index = macrofun_list;
            while (mFun_index->next)
            {
                mFun_index = mFun_index->next;
                if (strcmp(token_1, mFun_index->fun_name) == 0)
                {
                    printf("Duplicate macrofunction name!\n");
                    exit(0);
                }
            }
            strcpy(fun_cur->fun_name, token_1);
            mfun_cur->next = fun_cur;
            mfun_cur = fun_cur;
            printf("fun_name:%s\n", fun_cur->fun_name);
            char *m_line = NULL;
            while (macro_fun && !feof(fp_1))
            {
                fgets(str_1, 200, fp_1);
                strncpy(fun_cur->instruction[macro_ins_count++], str_1, 200);
                printf("NEW_LINE:%s", fun_cur->instruction[macro_ins_count - 1]);
                m_line = strtok(str_1, " \t\r\n");
                if (!m_line)
                    continue;
                else if (strcmp(m_line, ".MACRO") == 0)
                {
                    printf("Redundant .MACRO!\n");
                    exit(0);
                }
                else if (strcmp(m_line, ".END") == 0)
                {
                    if (macro_fun)
                    {
                        fun_cur->ins_num = macro_ins_count - 1;
                        printf("FUNCTION  END!\n");
                        macro_fun = 0;
                    }
                    else
                    {
                        printf("Redundant .END\n");
                        exit(0);
                    }
                }
            }
            // }
        }
        else if (strcmp(token_1, ".END") == 0)
        {
            if (macro_fun)
                macro_fun = 0;
            else
            {
                printf("Redundant .END!\n");
                exit(0);
            }
        }
        if (flag == 1)
        {
            Include_read_label(token_2);
        }
    }
}



void ppeInit::lb_init(char *str_ins)
{
    printf("lb_init\n");
    lb_list *token0 = (lb_list *)malloc(sizeof(lb_list));
    memcpy(token0->label_name, "HB_BASEADDR", strlen("HB_BASEADDR"));
    *((token0->label_name) + strlen("HB_BASEADDR")) = '\0';
    token0->value = HB_BASEADDR;
    token0->next = NULL;
    label_list = token0;
    lb_list *token1 = (lb_list *)malloc(sizeof(lb_list));
    memcpy(token1->label_name, "ISMB_BASEADDR", strlen("ISMB_BASEADDR"));
    *((token1->label_name) + strlen("ISMB_BASEADDR")) = '\0';
    token1->value = ISMB_BASEADDR;
    token1->next = NULL;
    token0->next = token1;
    lb_list *token2 = (lb_list *)malloc(sizeof(lb_list));
    memcpy(token2->label_name, "ESMB_BASEADDR", strlen("ESMB_BASEADDR"));
    *((token2->label_name) + strlen("ESMB_BASEADDR")) = '\0';
    token2->value = ESMB_BASEADDR;
    token2->next = NULL;
    token1->next = token2;
    lb_list *token3 = (lb_list *)malloc(sizeof(lb_list));
    memcpy(token3->label_name, "MAB_BASEADDR", strlen("MAB_BASEADDR"));
    *((token3->label_name) + strlen("MAB_BASEADDR")) = '\0';
    token3->value = MAB_BASEADDR;
    token3->next = NULL;
    token2->next = token3;
    lb_list *token4 = (lb_list *)malloc(sizeof(lb_list));
    memcpy(token4->label_name, "MBB_BASEADDR", strlen("MBB_BASEADDR"));
    *((token4->label_name) + strlen("MBB_BASEADDR")) = '\0';
    token4->value = MBB_BASEADDR;
    token4->next = NULL;
    token3->next = token4;
    // lb_list *cur = token4;
    cur = token4;
    printf("label:%s,value %d\n", token0->label_name, token0->value);
    printf("label:%s,value %d\n", token1->label_name, token1->value);
    printf("label:%s,value %d\n", token2->label_name, token2->value);
    printf("label:%s,value %d\n", token3->label_name, token3->value);
    printf("label:%s,value %d\n", token4->label_name, token4->value);
    long i, count = 0;
    long size = 0;
    int buf;
    //�����꺯������
    mfun_list *mfun_cur = (mfun_list *)malloc(sizeof(mfun_list));
    mfun_cur->next = NULL;
    macrofun_list = mfun_cur;

    FILE *fp;
    //���ļ�
    fp = fopen(str_ins, "r+");
    if (!fp) // fail to read file
    {
        printf("can't open ins_mem!\n");
        exit(0);
    }
    char c[200]; // temporary SR name
    char *token;
    int len;
    int macro_fun = 0;
    int multi_signal = 0;
    while (!feof(fp))
    {
        int begin_index = 0;
        int s_index = 0;
        int define_signal = 0;
        memset(c, 0, 200);
        char *ce = fgets(c, 200, fp);
        if (ce == NULL)
            continue;
        // printf("\nSTR(c):%s", c);
        char *retb = strstr(c, "/*");
        char *rete = strstr(c, "*/");
        //ǰ��/* */����
        if (multi_signal == 1 && rete != NULL)
        {
            begin_index = rete + 2 - c; //������һ��*/ǰ�Ĳ���
            multi_signal = 0;           //�����־λ
        }
        else if (multi_signal == 1)
            continue; //����δ����

        while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
            begin_index++;

        if (c[begin_index] == '\n' || c[begin_index] == '\r')
            continue;

        if (c[begin_index] == '/' && c[begin_index + 1] == '/')
            continue;
        else if (c[begin_index] == '/' && c[begin_index + 1] == '*')
        {
            //����һ��*/,����Ҳ�������ômulti=1,conti,����ҵ���,�е����������ж�һ��
            rete = strstr(c + begin_index, "*/");
            int skip_signal = 0;
            while (rete != NULL && c[begin_index] == '/' && c[begin_index + 1] == '*')
            {
                begin_index = rete + 2 - c;
                while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
                    begin_index++;
                //�����һ���� //������
                if (c[begin_index] == '/' && c[begin_index + 1] == '/')
                {
                    skip_signal = 1;
                    break;
                }
                rete = strstr(c + begin_index, "*/");
            }
            if (rete == NULL && c[begin_index] == '/' && c[begin_index + 1] == '*')
            {
                multi_signal = 1;
                continue;
            }
            if (skip_signal)
                continue;
            /* */ /* */
        }
        while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
            begin_index++;
        if (c[begin_index] == '\n' || c[begin_index] == '\r')
            continue;

        token = strchr(c + begin_index, ':');
        if (token == NULL)
        {
            token = strtok(c + begin_index, " \t\r\n"); // filter��" \t" => " \t\r\n"
        }
        else
        {
            retb = strstr(c + begin_index, "/*");
            char *rets = strstr(c + begin_index, "//");
            if ((retb == NULL || token < retb) && (rets == NULL || token < rets))
            {
                lb_list *lb_token = (lb_list *)malloc(sizeof(lb_list));
                // lb_token->label_name=(char *)malloc(sizeof(strlen(c+begin_index)-strlen(token)));
                memcpy(lb_token->label_name, c + begin_index, strlen(c + begin_index) - strlen(token));
                *((lb_token->label_name) + strlen(c + begin_index) - strlen(token)) = '\0';
                lb_token->value = count;
                lb_token->next = NULL;
                cur->next = lb_token;
                cur = lb_token;
                printf("new label:%s,value %d\n", lb_token->label_name, lb_token->value);
                begin_index = token + 1 - c;
                while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
                    begin_index++;
                if (c[begin_index] == '/' && c[begin_index + 1] == '/')
                    continue;
                else if (c[begin_index] == '/' && c[begin_index + 1] == '*')
                {
                    
                    rete = strstr(c + begin_index, "*/");
                    int skip_signal = 0;
                    while (rete != NULL && c[begin_index] == '/' && c[begin_index + 1] == '*')
                    {
                        begin_index = rete + 2 - c;
                        while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
                            begin_index++;
                        
                        if (c[begin_index] == '/' && c[begin_index + 1] == '/')
                        {
                            skip_signal = 1;
                            break;
                        }
                        rete = strstr(c + begin_index, "*/");
                    }
                    if (rete == NULL && c[begin_index] == '/' && c[begin_index + 1] == '*')
                    {
                        multi_signal = 1;
                        continue;
                    }
                    if (skip_signal)
                        continue;
                    /* */ /* */
                }
                while (*(c + begin_index) == ' ' || *(c + begin_index) == '\t')
                    begin_index++;
                if (c[begin_index] == '\n' || c[begin_index] == '\r')
                    continue;
                token = strtok(c + begin_index, " \t");
            }
            else
            {
                token = strtok(c + begin_index, " \t");
            }
        }

        //          /* */ /* */ label: /* */
        if (token == NULL)
            continue;
        if (*token == '\n' || *token == '\r')
            continue;
        // printf("Head:%s************|\n", token);
        if (strcmp(token, "#define") == 0)
        {
            lb_list *lb_token = (lb_list *)malloc(sizeof(lb_list));
            token = strtok(NULL, " \t");
            // printf("%s\n",token);
            // while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
            //     s_index++;
            int macro_len = strlen(token);
            if ((*token <= '9') && (*token >= '0'))
            {
                printf("macro_name can't start with a number!\n");
                exit(0);
            }
            // printf("s1:%s\n",token+s_index);
            for (int i = 0; i < macro_len; i++)
            {
                if (!(
                        (*token <= 'Z' && *token >= 'A') ||
                        (*token <= 'z' && *token >= 'a') ||
                        (*token <= '9' && *token >= '0') ||
                        (*token == '_')))
                {
                    printf("macro_name type error!\n");
                    exit(0);
                }
            }
            // lb_token->label_name=(char *)malloc(sizeof(strlen(token+s_index)));
            memcpy(lb_token->label_name, token, strlen(token));
            *((lb_token->label_name) + strlen(token)) = '\0';
            s_index = 0;
            token = strtok(NULL, "\r\n");
            while (*(token + s_index) == ' ' || *(token + s_index) == '\t')
                s_index++;
            printf("\ntoken%s\n", token);
            char tempstr[80] = "";
            int is_anno = 0;
            token = strtok(token, " \t\r\n");
            if (!token)
            {
                printf("Macro lack value!\n");
                exit(0);
            }
            while (token)
            {
                strcat(tempstr, token);
                token = strtok(NULL, " \t\r\n");
            }
            printf("tempstr:%s\n", tempstr);
            token = tempstr;
            while (*token != '\0')
            {
                if (*token == '/')
                {
                    if (*(token + 1) == '/' || *(token + 1) == '*')
                    {
                        printf("ano_content:%s\n", token);
                        *token = '\0';
                        is_anno = 1;
                        break;
                    }
                }
                token++;
            }
            if (*tempstr == '\0')
            {
                printf("macro lack value!\n");
                continue;
            }
            printf("final:%s\n", tempstr);
            lb_token->value = calcDirect(tempstr, label_list);
            if (is_anno)
            {
                *token = '/';
                printf("re_anno:%s\n", token);
            }
            s_index = 0;
            define_signal = 1;
            cur->next = lb_token;
            cur = cur->next;
            printf("new label:%s,value %d\n", lb_token->label_name, lb_token->value);
        }
        else if (strcmp(token, ".MACRO") == 0)
        {
            printf("\nFUNCTION\n");
            macro_fun = 1;
            int macro_ins_count = 0;
            mfun_list *fun_cur = (mfun_list *)malloc(sizeof(mfun_list));
            fun_cur->next = NULL;
            // strcpy(fun_cur->instruction[macro_ins_count++],token);
            if (!(token = strtok(NULL, " \t\r\n")))
            {
                printf("Lack function name\n");
                exit(0);
            }
            // else if(strstr(token,"/"))
            // {
            //     *strstr(token,"/")='\0';
            // }
            mFun_index = macrofun_list;
            while (mFun_index->next)
            {
                mFun_index = mFun_index->next;
                if (strcmp(token, mFun_index->fun_name) == 0)
                {
                    printf("Duplicate macrofunction name!\n");
                    exit(0);
                }
            }
            strcpy(fun_cur->fun_name, token);
            mfun_cur->next = fun_cur;
            mfun_cur = fun_cur;
            printf("fun_name:%s\n", fun_cur->fun_name);
            char *m_line = NULL;
            while (macro_fun && !feof(fp))
            {
                fgets(c, 200, fp);
                strncpy(fun_cur->instruction[macro_ins_count++], c, 200);
                printf("NEW_LINE:%s", fun_cur->instruction[macro_ins_count - 1]);
                m_line = strtok(c, " \t\r\n");
                if (!m_line)
                    continue;
                else if (strcmp(m_line, ".MACRO") == 0)
                {
                    printf("Redundant .MACRO!\n");
                    exit(0);
                }
                else if (strcmp(m_line, ".END") == 0)
                {
                    if (macro_fun)
                    {
                        fun_cur->ins_num = macro_ins_count - 1;
                        printf("FUNCTION  END!\n");
                        macro_fun = 0;
                    }
                    else
                    {
                        printf("Redundant .END\n");
                        exit(0);
                    }
                }
            }
            // }
        }
        else if (strcmp(token, ".END") == 0)
        {
            if (macro_fun)
                macro_fun = 0;
            else
            {
                printf("Redundant .END!\n");
                exit(0);
            }
        }
        else if (strcmp(token, "#include") == 0)
        {
            //�õ�·��path_label
            char token_path_name[200] = {0};
            token = strtok(NULL, " \r\n\t");
            printf("%s\n", token);
            memcpy(token_path_name, token + 1, (strlen(token) - 2));
            // printf("token_path_name=%s\n", token_path_name);
            Include_read_label(token_path_name);
        }
        else
        {
            int index = 0, flag = 0, func_match = 0;
            while (*(token + index) != '\0')
            {
                if (*(token + index) == '/' && (*(token + index + 1) == '/' || *(token + index + 1) == '*'))
                {
                    flag = 1;
                    *(token + index) = '\0';
                    break;
                }
                else
                    index++;
            }
            mFun_index = macrofun_list;
            while (mFun_index->next)
            {
                mFun_index = mFun_index->next;
                if (strcmp(token, mFun_index->fun_name) == 0)
                {
                    func_match = 1;
                    break;
                }
            }
            if (func_match)
            {
                count += mFun_index->ins_num;
                printf("HAVE:%d\n", count);
            }
            else
                count++;
            if (flag)
                *(token + index) = '/';
        }
        if (token == NULL)
            continue;
        // printf("content:%s\n",token);
        char *rets = strstr(token, "//");
        retb = strstr(token, "/*");
        rete = strstr(token, "*/");
        if ((rets != NULL && retb != NULL && rets > retb) ||
            (rets == NULL && retb != NULL))
        {
            if (rete == NULL)
            {
                multi_signal = 1;
                continue;
            }
            while (rete != NULL)
            {
                rets = strstr(rete + 2, "//");
                retb = strstr(rete + 2, "/*");
                if (retb != NULL)
                    rete = strstr(retb + 2, "*/");
                else
                    rete = NULL;
                if (rets != NULL && retb != NULL && rets < retb)
                    break;
            }
            if ((rets == NULL || retb < rets) && rete == NULL && retb != NULL)
                multi_signal = 1;
        }
    }
    fclose(fp);
    mfun_list *mfun_index = macrofun_list;
    printf("\n");
    while (mfun_index->next)
    {
        mfun_index = mfun_index->next;
        printf("\nMacroFun:%s|||LEN:%d\n", mfun_index->fun_name, mfun_index->ins_num);
        for (int t = 0; t < mfun_index->ins_num; t++)
        {
            printf("%s", mfun_index->instruction[t]);
        }
    }
}


int ppeInit::GPR_value(char *gpr)
{
    int value;
    if (*gpr == 'Z')
        value = 31;
    else if (*gpr == 'L' && *(gpr + 1) == 'R')
        value = 30;
    else if (*gpr == '0')
        value = 0;
    else if (atoi(gpr) > 0 && atoi(gpr) < 32)
        value = atoi(gpr);
    else
    {
        printf("GPR format error!");
        exit(0);
    }
    return value;
}

/************************************************************************************/




void ppe_sim::core_queue_check()
{
    for (int i=0;i<16;i++)
    {
        
        tlb_des_queue *cur=core_queues[i].input_queue;
        int fullused=0;
        while (cur!=NULL && !fullused)  
        {
            fullused=1;
            for (int j=0;j<8;j++)  
            {
                if (core_queues[i].is_used[j]==0)   
                {
                    fullused=0;
                    
                    core_queues[i].is_used[j]=1;    
                    int core_id=i*8+j;
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
                    
                    
                    reg_next_init(core_id);
                   
                    printf("QueNumber%d,load event into %d PPE\n",i,core_id);
                    break;
                }
            }
            if (!fullused)
            {
                
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
 

void ppeInit::tlb_init()
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

void ppe_sim::pkt_to_tlb(Packet *pkt,int fcs=0, int port=0)
{
    int mem_offset=0, sys_meta=0;
    
    pkt->set_checksum_error(false);
    const char *data = pkt->data();
    size_t data_size = pkt->get_data_size();
    printf("data_size: %ld\n",data_size);
    if (port == 0 || port == 1)
        sys_meta = 32;
    int used_num = (data_size + mem_offset + sys_meta) / 256 + ((data_size + mem_offset + sys_meta) % 256 != 0);
    printf("used_num: %d\n",used_num);
    int remain = data_size % 256;
    int slice_size = 0;
    int offset_num = (mem_offset + sys_meta) / 256;
    int offset = (mem_offset + sys_meta) % 256;
    tlb_list *cur = free_tlb_list;
    tlb_des_queue *token_tlb_des = (tlb_des_queue *)malloc(sizeof(tlb_des_queue));
    token_tlb_des->length = used_num;
    
    for (int i = 0; i < used_num; i++)
    {
        tlb[cur->tlb_num & 0x7fff].used = 1; 
        if (i == offset_num)
        {
            if (i == used_num-1)
                slice_size = remain;
            else
                slice_size = 256 - offset;
            for (int j = 0; j < slice_size; j++)
            {
                // tlb[cur->tlb_num & 0x7fff].content[j + offset] = ingress->ingress_buffer[(i - offset_num) * 256 + j]; 
                tlb[cur->tlb_num & 0x7fff].content[j + offset] = *(data+(i - offset_num) * 256 + j); 
            }
        }
        else if (i > offset_num)
        {
            if (i == used_num-1)
                slice_size = (remain + offset) % 256;
            else
                slice_size = 256;
            for (int j = 0; j < slice_size; j++)
            {
                // tlb[cur->tlb_num & 0x7fff].content[j] = *ingress->ingress_buffer[(i - offset_num) * 256 + j - offset]; 
                tlb[cur->tlb_num & 0x7fff].content[j] = *(data+(i - offset_num) * 256 + j - offset); 
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
    token_tlb_des->mb_content[0] = data_size >> 8;
    token_tlb_des->mb_content[1] = data_size % 256;
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
}


/************************************************************************************/
void ppe_sim::if_stage(int ppn, int pc)
{
    //����PC�õ�ָ���ָ�����ID
    printf("IF:pc=%d\n", pc);
    cur_if_id_bus[ppn].pc = pc;
    int index = pc >> 2;
    if (cur_ex_wb_bus[ppn].is_mhm == 0)
    {
        strcpy(cur_if_id_bus[ppn].fst_ins.opname, ins_mem[ppn >> 3][index].opname);
        cur_if_id_bus[ppn].fst_ins.rd = ins_mem[ppn >> 3][index].rd;
        cur_if_id_bus[ppn].fst_ins.rs1 = ins_mem[ppn >> 3][index].rs1;
        cur_if_id_bus[ppn].fst_ins.rs2 = ins_mem[ppn >> 3][index].rs2;
        cur_if_id_bus[ppn].fst_ins.imm = ins_mem[ppn >> 3][index].imm;
        cur_if_id_bus[ppn].fst_ins.line = ins_mem[ppn >> 3][index].line;
        cur_if_id_bus[ppn].stage_valid = 1;
    }
}

void ppe_sim::is_forward_id(int ppn,int rs_type,int val_type)
{
    unsigned char rs_value;
    if (rs_type==0)
        rs_value=cur_id_ex_bus[ppn].snd_ins.rd;
    else if (rs_type==1)
        rs_value=cur_id_ex_bus[ppn].snd_ins.rs1;
    else if (rs_type==2)
        rs_value=cur_id_ex_bus[ppn].snd_ins.rs2;
    
    if (val_type==1)
    {
        if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==rs_value))
        {
            cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
        }
        else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==rs_value))
        {
            cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
        }
        if (cur_ex_wb_bus[ppn].forward && (cur_ex_wb_bus[ppn].trd_ins.rd==rs_value))
        {
            cur_id_ex_bus[ppn].value1=cur_ex_wb_bus[ppn].ex_result;
        }
        if (rs_value==31)
        cur_id_ex_bus[ppn].value1=0;
            
    }

    else if (val_type==2)
    {
        if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==rs_value))
        {
            cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
        }
        else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==rs_value))
        {
            cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
        }
        if (cur_ex_wb_bus[ppn].forward && (cur_ex_wb_bus[ppn].trd_ins.rd==rs_value))
        {
            cur_id_ex_bus[ppn].value2=cur_ex_wb_bus[ppn].ex_result;
        }
        if (rs_value==31)
        cur_id_ex_bus[ppn].value2=0;
    }

    else if (val_type==3)
    {
        if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==rs_value))
        {
            cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata;
        }
        else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==rs_value))
        {
            cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata2;
        }
        if (cur_ex_wb_bus[ppn].forward && (cur_ex_wb_bus[ppn].trd_ins.rd==rs_value))
        {
            cur_id_ex_bus[ppn].value3=cur_ex_wb_bus[ppn].ex_result;
        }
        if (rs_value==31)
        cur_id_ex_bus[ppn].value3=0;
    }



    else if (val_type==4)
    {
        if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==rs_value))
        {
            cur_id_ex_bus[ppn].is_mhm=(cur_wb_bus[ppn].wdata>>1<<1)-2;
        }
        else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==rs_value))
        {
            cur_id_ex_bus[ppn].is_mhm=(cur_wb_bus[ppn].wdata2>>1<<1)-2;
        }
        if (cur_ex_wb_bus[ppn].forward && (cur_ex_wb_bus[ppn].trd_ins.rd==rs_value))
        {
            cur_id_ex_bus[ppn].is_mhm=(cur_ex_wb_bus[ppn].ex_result>>1<<1)-2;
        }
    }
}
void ppe_sim::id_stage(int ppn,if_id_bus bus){
    printf("ID:from IF, PC=%d, Line %d:%s %d %d %d %d\n",bus.pc,
    bus.fst_ins.line,bus.fst_ins.opname,bus.fst_ins.rd,bus.fst_ins.rs1,bus.fst_ins.rs2,bus.fst_ins.imm);
    //��IF�õ���ָ���������롱�����룺�������ָ������ݣ��Լ����Ĵ���ֵ
    //ADD GPR0,GPR1,GPR2 GPR0=GPR1+GPR2
    //�õ�GPR1��GPR2�����뼶
    //�������Ϣ�͸���ִ�м�
    if (bus.stage_valid && (cur_id_ex_bus[ppn].is_branch==0) && !cur_id_ex_bus[ppn].is_lsm && !cur_id_ex_bus[ppn].is_mhm
    && !cur_id_ex_bus[ppn].is_lt)
    {
        
        cur_id_ex_bus[ppn].pc=bus.pc;
        cur_id_ex_bus[ppn].snd_ins=bus.fst_ins;
        cur_id_ex_bus[ppn].stage_valid=1;
        cur_id_ex_bus[ppn].is_gpr_des=0;
        cur_id_ex_bus[ppn].is_mr_des=0;
        cur_id_ex_bus[ppn].is_branch=0;
        cur_id_ex_bus[ppn].is_lsm=0;
        cur_id_ex_bus[ppn].is_lt=0;
        cur_id_ex_bus[ppn].is_mhm=0;
        cur_id_ex_bus[ppn].is_be=0;
        if (strcmp(bus.fst_ins.opname,"ADDI")==0||strcmp(bus.fst_ins.opname,"SUBI")==0 || strcmp(bus.fst_ins.opname,"SUBBI")==0 || 
            strcmp(bus.fst_ins.opname,"ANDI")==0||strcmp(bus.fst_ins.opname,"SRL")==0 || strcmp(bus.fst_ins.opname,"ORI")==0 ||
            strcmp(bus.fst_ins.opname,"SLL")==0 || strcmp(bus.fst_ins.opname,"ADDCI")==0 || strcmp(bus.fst_ins.opname,"XORI")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs2];
            is_forward_id(ppn,1,1);
            is_forward_id(ppn,2,2);
        }
        else if (strcmp(bus.fst_ins.opname,"MOVI")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=1;
        }
        else if (strcmp(bus.fst_ins.opname,"CMP")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs2];
            is_forward_id(ppn,1,1);
            is_forward_id(ppn,2,2);
        }
        else if (strcmp(bus.fst_ins.opname,"ADD")==0 || strcmp(bus.fst_ins.opname,"ADDC")==0 || strcmp(bus.fst_ins.opname,"SUBB")==0 ||
        strcmp(bus.fst_ins.opname,"SUB")==0 || strcmp(bus.fst_ins.opname,"AND")==0 ||
        strcmp(bus.fst_ins.opname,"OR")==0 || strcmp(bus.fst_ins.opname,"XOR")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs2];
            is_forward_id(ppn,1,1);
            is_forward_id(ppn,2,2);
        }
        else if (strcmp(bus.fst_ins.opname,"MOVRR")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs2];
            is_forward_id(ppn,1,1);
            is_forward_id(ppn,2,2);
        }

        else if (strcmp(bus.fst_ins.opname,"MOVSR")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=1;
        }
        else if (strcmp(bus.fst_ins.opname,"MOVRS")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            is_forward_id(ppn,1,1);
        }
        //khh added:MOVBRLE,MOVBRBE
        else if (strcmp(bus.fst_ins.opname,"MOVBRLE")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            is_forward_id(ppn,1,1);
        }
        else if (strcmp(bus.fst_ins.opname,"MOVBRBE")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=1;
            cur_id_ex_bus[ppn].is_be = 1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            is_forward_id(ppn,1,1);
        }
        else if (strcmp(bus.fst_ins.opname,"MOVRBLE")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            is_forward_id(ppn,1,1);
            is_forward_id(ppn,0,2);
        }
        else if (strcmp(bus.fst_ins.opname,"MOVRBBE")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].is_be=1;
            is_forward_id(ppn,1,1);
            is_forward_id(ppn,0,2);
        }
        else if (strcmp(bus.fst_ins.opname,"MOVHM")==0)
        {
            cur_id_ex_bus[ppn].is_mr_des=1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].is_mhm=bus.fst_ins.imm-1;
            is_forward_id(ppn,1,1);
            //is_forward_id(ppn,0,2);
        }
        else if (strcmp(bus.fst_ins.opname,"MOVMH")==0)
        {
            cur_id_ex_bus[ppn].is_hb_des=1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].is_mhm=bus.fst_ins.imm-1;
            is_forward_id(ppn,1,1);
        }
        else if (strcmp(bus.fst_ins.opname,"MOVBB")==0)
        {
            cur_id_ex_bus[ppn].is_hb_des=1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].is_mhm=(gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs2]>>1<<1)-2;
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2);
            is_forward_id(ppn,2,4);
        }
        else if (strcmp(bus.fst_ins.opname,"MUL")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].snd_ins.rd=cur_id_ex_bus[ppn].snd_ins.rd>>1<<1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs2];
            is_forward_id(ppn,1,1);
            is_forward_id(ppn,2,2);
            //cur_id_ex_bus[ppn].is_mhm=4;
            cur_id_ex_bus[ppn].is_mhm=0;
        }
        //merge by wzy
        else if (strcmp(bus.fst_ins.opname,"CKSUMLE")==0 || strcmp(bus.fst_ins.opname,"CKSUMBE")==0)
        {
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].is_mhm=(gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1]>>1<<1)-2;
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2);
            is_forward_id(ppn,1,4);
        }
        //khh added:HASHLE HASHBE
        else if (strcmp(bus.fst_ins.opname,"HASHLE")==0 || strcmp(bus.fst_ins.opname,"HASHBE")==0)
        {
            if(strcmp(bus.fst_ins.opname,"HASHBE")==0)
                cur_id_ex_bus[ppn].is_be = 1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].is_mhm=(gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1]>>1<<1)-2;
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2);
            is_forward_id(ppn,1,4);
        }
        else if (strcmp(bus.fst_ins.opname,"LOADM")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs2];
            cur_id_ex_bus[ppn].value3=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            is_forward_id(ppn,1,1);
            is_forward_id(ppn,2,2);
            is_forward_id(ppn,0,3);
        }
        else if (strcmp(bus.fst_ins.opname,"STOREM")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs2];
            cur_id_ex_bus[ppn].value3=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            is_forward_id(ppn,1,1);
            is_forward_id(ppn,2,2);
            is_forward_id(ppn,0,3);
        }
        else if (strcmp(bus.fst_ins.opname,"LOADT")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].is_lt=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs2];
            cur_id_ex_bus[ppn].value3=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,2,2);
            is_forward_id(ppn,1,3);
        }
        else if (strcmp(bus.fst_ins.opname,"STORET")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2);
        }
        else if (strcmp(bus.fst_ins.opname,"QUERYT")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].value3=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs2];
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2);
            is_forward_id(ppn,2,3);
        }
        else if (strcmp(bus.fst_ins.opname,"INSERTT")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2); 
        }
        else if (strcmp(bus.fst_ins.opname,"DELETET")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2); 
        }
        else if (strcmp(bus.fst_ins.opname,"MALCSMB")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2);
        }
        else if (strcmp(bus.fst_ins.opname,"FREESMB")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2);
        }
        else if (strcmp(bus.fst_ins.opname,"REFSMB")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2);
        }
        else if (strcmp(bus.fst_ins.opname,"PUSHQ")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            cur_id_ex_bus[ppn].value3=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs2];
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2);
            is_forward_id(ppn,2,3);
        }
        else if (strcmp(bus.fst_ins.opname,"SENDMB")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_lsm=0;
            cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
            cur_id_ex_bus[ppn].value2=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
            is_forward_id(ppn,0,1);
            is_forward_id(ppn,1,2);
        }
        else if (strcmp(bus.fst_ins.opname,"BEQI")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_branch=1;
        }
        else if (strcmp(bus.fst_ins.opname,"BGTI")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_branch=1;
        }
        else if (strcmp(bus.fst_ins.opname,"BI")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_branch=1;
        }
        else if (strcmp(bus.fst_ins.opname,"BNEI")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_branch=1;
        }
        else if (strcmp(bus.fst_ins.opname,"BLTI")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_branch=1;
        }
        else if (strcmp(bus.fst_ins.opname,"B")==0 || strcmp(bus.fst_ins.opname,"BNE")==0||
        strcmp(bus.fst_ins.opname,"BLT")==0 || strcmp(bus.fst_ins.opname,"BGT")==0 || strcmp(bus.fst_ins.opname,"BEQ")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_branch=1;
            cur_id_ex_bus[ppn].value1=gpr[ppn][bus.fst_ins.rd];
            is_forward_id(ppn,0,1);
        }
        else if (strcmp(bus.fst_ins.opname,"ISB")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_branch=1;
        }
        else if (strcmp(bus.fst_ins.opname,"ISBC")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_branch=1;
        }
        else if (strcmp(bus.fst_ins.opname,"END")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
        }
        else cur_id_ex_bus[ppn].is_gpr_des=0;
    }
    else if (bus.stage_valid && (cur_id_ex_bus[ppn].is_branch==0) && cur_id_ex_bus[ppn].is_lsm==1) //lsm stage2
    {
        cur_id_ex_bus[ppn].is_gpr_des=0;
        cur_id_ex_bus[ppn].is_lsm=0;
        cur_id_ex_bus[ppn].is_branch=0;
        cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rd];
        if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
        {
            cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
        }
        if (cur_ex_wb_bus[ppn].forward && (cur_ex_wb_bus[ppn].trd_ins.rd==cur_id_ex_bus[ppn].snd_ins.rd))
        {
            cur_id_ex_bus[ppn].value1=cur_ex_wb_bus[ppn].ex_result;
        }
        if (cur_id_ex_bus[ppn].snd_ins.rd==31)
            cur_id_ex_bus[ppn].value1=0;
    }
    else if (bus.stage_valid && (cur_id_ex_bus[ppn].is_branch==0) && cur_id_ex_bus[ppn].is_lt==1)//loadt stage 2
    {
        cur_id_ex_bus[ppn].is_gpr_des=0;
        cur_id_ex_bus[ppn].is_lt=0;
        cur_id_ex_bus[ppn].is_branch=0;
        cur_id_ex_bus[ppn].value1=gpr[ppn][cur_id_ex_bus[ppn].snd_ins.rs1];
        if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
        {
            cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
        }
        if (cur_ex_wb_bus[ppn].forward && (cur_ex_wb_bus[ppn].trd_ins.rd==cur_id_ex_bus[ppn].snd_ins.rs1))
        {
            cur_id_ex_bus[ppn].value1=cur_ex_wb_bus[ppn].ex_result;
        }
        if (cur_id_ex_bus[ppn].snd_ins.rs1==31)
                cur_id_ex_bus[ppn].value1=0;
    }
    else if (bus.stage_valid && (cur_id_ex_bus[ppn].is_branch==0) && cur_id_ex_bus[ppn].is_mhm>0)
    {
        if (strcmp(cur_id_ex_bus[ppn].snd_ins.opname,"MOVHM")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_mr_des=1;
            cur_id_ex_bus[ppn].is_mhm--;
            cur_id_ex_bus[ppn].snd_ins.rd=(cur_id_ex_bus[ppn].snd_ins.rd+1)%64;
        }
        else if (strcmp(cur_id_ex_bus[ppn].snd_ins.opname,"MOVMH")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_hb_des=1;
            cur_id_ex_bus[ppn].is_mhm--;
            cur_id_ex_bus[ppn].snd_ins.rs1=(cur_id_ex_bus[ppn].snd_ins.rs1+1)%64;
        }
        else if (strcmp(cur_id_ex_bus[ppn].snd_ins.opname,"MOVBB")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_hb_des=1;
            cur_id_ex_bus[ppn].is_mhm-=2;
            cur_id_ex_bus[ppn].value1=cur_id_ex_bus[ppn].value1+2;
            cur_id_ex_bus[ppn].value2=cur_id_ex_bus[ppn].value2+2;
        }
        //merge by wzy: cksumle,cksumbe
        else if (strcmp(cur_id_ex_bus[ppn].snd_ins.opname,"CKSUMLE")==0 || 
        strcmp(cur_id_ex_bus[ppn].snd_ins.opname,"CKSUMBE")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_mhm-=2;
            cur_id_ex_bus[ppn].value1=cur_id_ex_bus[ppn].value1+2;
            cur_id_ex_bus[ppn].value2=cur_id_ex_bus[ppn].value2+2;
        }
        //khh added:HASHLE,HASHBE
        else if (strcmp(cur_id_ex_bus[ppn].snd_ins.opname,"HASHLE")==0 || strcmp(cur_id_ex_bus[ppn].snd_ins.opname,"HASHBE")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=0;
            cur_id_ex_bus[ppn].is_mhm-=2;
            cur_id_ex_bus[ppn].value1=cur_id_ex_bus[ppn].value1+2;
            cur_id_ex_bus[ppn].value2=cur_id_ex_bus[ppn].value2+2;
        }
        // else if (strcmp(cur_id_ex_bus[ppn].snd_ins.opname,"HASH")==0)
        // {
        //     cur_id_ex_bus[ppn].is_gpr_des=0;
        //     cur_id_ex_bus[ppn].is_mhm-=2;
        //     cur_id_ex_bus[ppn].value1=cur_id_ex_bus[ppn].value1+2;
        //     cur_id_ex_bus[ppn].value2=cur_id_ex_bus[ppn].value2+2;
        // }
        else if (strcmp(cur_id_ex_bus[ppn].snd_ins.opname,"MUL")==0)
        {
            cur_id_ex_bus[ppn].is_gpr_des=1;
            cur_id_ex_bus[ppn].is_mhm-=2;
        }
    }
    else
    {
        if (cur_ex_wb_bus[ppn].pc_halt==0)
        {
            cur_id_ex_bus[ppn].stage_valid=0;
            cur_id_ex_bus[ppn].is_branch=0;
            cur_id_ex_bus[ppn].is_lsm=0;
        }
        
    }
    printf("ID:decode PC=%d, Line %d:%s %d %d %d %d\n",cur_id_ex_bus[ppn].pc,
    cur_id_ex_bus[ppn].snd_ins.line,cur_id_ex_bus[ppn].snd_ins.opname,
    cur_id_ex_bus[ppn].snd_ins.rd,cur_id_ex_bus[ppn].snd_ins.rs1,cur_id_ex_bus[ppn].snd_ins.rs2,cur_id_ex_bus[ppn].snd_ins.imm);
}

void ppe_sim::is_forward_ex(int ppn,int rs_type,int val_type)
{
    //merge by ssy: is forward ex
    unsigned char rs_value;
    if (rs_type==0)
    rs_value=cur_id_ex_bus[ppn].snd_ins.rd;
    else if (rs_type==1)
    rs_value=cur_id_ex_bus[ppn].snd_ins.rs1;
    else if (rs_type==2)
    rs_value=cur_id_ex_bus[ppn].snd_ins.rs2;



    if (val_type==1)
    {
        if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==rs_value))
        {
            cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
        }
        else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==rs_value))
        {
            cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
        }
        if (rs_value==31)
            cur_id_ex_bus[ppn].value1=0;

    }

    else if (val_type==2)
    {
        if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==rs_value))
        {
            cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
        }
        else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==rs_value))
        {
            cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
        }
        if (rs_value==31)
            cur_id_ex_bus[ppn].value2=0;
    }

    else if (val_type==3)
    {
        if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==rs_value))
        {
            cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata;
        }
        else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==rs_value))
        {
            cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata2;
        }
        if (rs_value==31)
            cur_id_ex_bus[ppn].value3=0;
    }



    else if (val_type==4)
    {
        if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==rs_value))
        {
            cur_id_ex_bus[ppn].is_mhm=(cur_wb_bus[ppn].wdata>>1<<1)-2;
        }
        else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==rs_value))
        {
            cur_id_ex_bus[ppn].is_mhm=(cur_wb_bus[ppn].wdata2>>1<<1)-2;
        }
    }
}
void ppe_sim::ex_stage(int ppn,id_ex_bus bus){
    printf("EX:PC=%d, Line %d:%s %d %d %d %d\n",bus.pc,bus.snd_ins.line,bus.snd_ins.opname,bus.snd_ins.rd,bus.snd_ins.rs1,
    bus.snd_ins.rs2,bus.snd_ins.imm);
    cur_ex_wb_bus[ppn].pc_change=0;
    //ADD GPR0��GPR1��GPR2
    //GPR1=10��GPR2=20
    //10+20=30
    //��30����д�ؼ�
    if (bus.stage_valid)
    {
        cur_ex_wb_bus[ppn].pc=bus.pc;
        cur_ex_wb_bus[ppn].pc_halt=0;
        cur_ex_wb_bus[ppn].trd_ins=bus.snd_ins;
        cur_ex_wb_bus[ppn].stage_valid=1;
        cur_ex_wb_bus[ppn].is_gpr_des=0;
        cur_ex_wb_bus[ppn].is_hb_des=0;
        cur_ex_wb_bus[ppn].is_read_hb=0;
        cur_ex_wb_bus[ppn].forward=0;
        cur_ex_wb_bus[ppn].sr_cy=0;
        cur_ex_wb_bus[ppn].sr_zr=0;
        cur_ex_wb_bus[ppn].is_sr_alu=0;
        cur_ex_wb_bus[ppn].is_sr_des=0;
        cur_ex_wb_bus[ppn].hb_addr=0;
        cur_ex_wb_bus[ppn].is_mhm=0;
        cur_ex_wb_bus[ppn].is_mul=0;
        cur_ex_wb_bus[ppn].is_be=bus.is_be;
        cur_ex_wb_bus[ppn].is_clear_sr_sta=0;
        if (strcmp(bus.snd_ins.opname,"ADDI")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=1;
            cur_ex_wb_bus[ppn].forward=1;
            cur_ex_wb_bus[ppn].is_sr_alu=1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1+bus.snd_ins.imm;
            if (cur_id_ex_bus[ppn].value1>cur_ex_wb_bus[ppn].ex_result)
                //(cur_id_ex_bus.value1>0 && bus.snd_ins.imm<0 && cur_ex_wb_bus[ppn].ex_result<0))
            cur_ex_wb_bus[ppn].sr_cy=1;
            else cur_ex_wb_bus[ppn].sr_cy=0;
            if (cur_ex_wb_bus[ppn].ex_result==0)
            cur_ex_wb_bus[ppn].sr_zr=1;
            else cur_ex_wb_bus[ppn].sr_zr=0;
            // printf("ADDI: %d\n",cur_ex_wb_bus[ppn].ex_result);
        }
        else if (strcmp(bus.snd_ins.opname,"MOVI")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=1;
            cur_ex_wb_bus[ppn].forward=1;
            cur_ex_wb_bus[ppn].ex_result=bus.snd_ins.imm;
        }
        // merge-khh ADDC,ADDCI,SUBB,XOR,OR
        else if (strcmp(bus.snd_ins.opname,"ADDC")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des = 1;
            cur_ex_wb_bus[ppn].forward = 1;
            cur_ex_wb_bus[ppn].is_sr_alu = 1; 
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1)
            // {    
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2)
            // {
            //     cur_id_ex_bus[ppn].value2 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,2,2);
            cur_ex_wb_bus[ppn].ex_result = cur_id_ex_bus[ppn].value1+cur_id_ex_bus[ppn].value2+((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)>>2);
            if(cur_id_ex_bus[ppn].value1>cur_ex_wb_bus[ppn].ex_result-((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)>>2))
                cur_ex_wb_bus[ppn].sr_cy = 1;
            else
                cur_ex_wb_bus[ppn].sr_cy = 0;
            if(cur_ex_wb_bus[ppn].ex_result==0)
                cur_ex_wb_bus[ppn].sr_zr = 1;
            else
                cur_ex_wb_bus[ppn].sr_zr = 0;
        }
        else if (strcmp(bus.snd_ins.opname,"ADDCI")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des = 1;
            cur_ex_wb_bus[ppn].forward = 1;
            cur_ex_wb_bus[ppn].is_sr_alu = 1; 
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1)
            // {    
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].ex_result = cur_id_ex_bus[ppn].value1+bus.snd_ins.imm+((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)>>2);
            if(cur_id_ex_bus[ppn].value1>cur_ex_wb_bus[ppn].ex_result-((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)>>2))
                cur_ex_wb_bus[ppn].sr_cy = 1;
            else
                cur_ex_wb_bus[ppn].sr_cy = 0;
            if(cur_ex_wb_bus[ppn].ex_result==0)
                cur_ex_wb_bus[ppn].sr_zr = 1;
            else
                cur_ex_wb_bus[ppn].sr_zr = 0;
        }
        else if (strcmp(bus.snd_ins.opname,"SUBB")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des = 1;
            cur_ex_wb_bus[ppn].forward = 1;
            cur_ex_wb_bus[ppn].is_sr_alu = 1; 
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1)
            // {    
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2)
            // {
            //     cur_id_ex_bus[ppn].value2 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,2,2);
            cur_ex_wb_bus[ppn].ex_result = cur_id_ex_bus[ppn].value1-cur_id_ex_bus[ppn].value2-((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)>>2);
            if(cur_id_ex_bus[ppn].value1<cur_ex_wb_bus[ppn].ex_result+((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)>>2))
                cur_ex_wb_bus[ppn].sr_cy = 1;
            else
                cur_ex_wb_bus[ppn].sr_cy = 0;
            if(cur_ex_wb_bus[ppn].ex_result==0)
                cur_ex_wb_bus[ppn].sr_zr = 1;
            else
                cur_ex_wb_bus[ppn].sr_zr = 0;
        }
        else if (strcmp(bus.snd_ins.opname,"SUBBI")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des = 1;
            cur_ex_wb_bus[ppn].forward = 1;
            cur_ex_wb_bus[ppn].is_sr_alu = 1; 
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1)
            // {    
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].ex_result = cur_id_ex_bus[ppn].value1-bus.snd_ins.imm-((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)>>2);
            if(cur_id_ex_bus[ppn].value1<cur_ex_wb_bus[ppn].ex_result+((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)>>2))
                cur_ex_wb_bus[ppn].sr_cy = 1;
            else
                cur_ex_wb_bus[ppn].sr_cy = 0;
            if(cur_ex_wb_bus[ppn].ex_result==0)
                cur_ex_wb_bus[ppn].sr_zr = 1;
            else
                cur_ex_wb_bus[ppn].sr_zr = 0;
        }
        else if (strcmp(bus.snd_ins.opname,"XOR")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des = 1;
            cur_ex_wb_bus[ppn].forward = 1;
            cur_ex_wb_bus[ppn].is_sr_alu = 2; 
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1)
            // {    
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,2,2);
            cur_ex_wb_bus[ppn].ex_result = (cur_id_ex_bus[ppn].value1)^(cur_id_ex_bus[ppn].value2);
            if(cur_ex_wb_bus[ppn].ex_result==0)
                cur_ex_wb_bus[ppn].sr_zr = 1;
            else
                cur_ex_wb_bus[ppn].sr_zr = 0;
        }
        //merge 2.53:XORI
        else if (strcmp(bus.snd_ins.opname,"XORI")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des = 1;
            cur_ex_wb_bus[ppn].forward = 1;
            cur_ex_wb_bus[ppn].is_sr_alu = 2; 
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].ex_result = (cur_id_ex_bus[ppn].value1)^(bus.snd_ins.imm);
            if(cur_ex_wb_bus[ppn].ex_result==0)
                cur_ex_wb_bus[ppn].sr_zr = 1;
            else
                cur_ex_wb_bus[ppn].sr_zr = 0;
        }
        else if (strcmp(bus.snd_ins.opname,"OR")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des = 1;
            cur_ex_wb_bus[ppn].forward = 1;
            cur_ex_wb_bus[ppn].is_sr_alu = 2; 
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1)
            // {    
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,2,2);
            cur_ex_wb_bus[ppn].ex_result = (cur_id_ex_bus[ppn].value1)|(cur_id_ex_bus[ppn].value2);
            if(cur_ex_wb_bus[ppn].ex_result==0)
                cur_ex_wb_bus[ppn].sr_zr = 1;
            else
                cur_ex_wb_bus[ppn].sr_zr = 0;
        }
        //merge-khh2:ORI
        else if (strcmp(bus.snd_ins.opname,"ORI")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des = 1;
            cur_ex_wb_bus[ppn].forward = 1;
            cur_ex_wb_bus[ppn].is_sr_alu = 2; 
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1)
            // {    
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].ex_result = (cur_id_ex_bus[ppn].value1)|(bus.snd_ins.imm);
            if(cur_ex_wb_bus[ppn].ex_result==0)
                cur_ex_wb_bus[ppn].sr_zr = 1;
            else
                cur_ex_wb_bus[ppn].sr_zr = 0;
        }
        else if (strcmp(bus.snd_ins.opname,"SUBI")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=1;
            cur_ex_wb_bus[ppn].forward=1;
            cur_ex_wb_bus[ppn].is_sr_alu=1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1-bus.snd_ins.imm;
            if (cur_id_ex_bus[ppn].value1<cur_ex_wb_bus[ppn].ex_result)
                //(bus.value1>0 && bus.snd_ins.imm<0 && cur_ex_wb_bus[ppn].ex_result<0))
            cur_ex_wb_bus[ppn].sr_cy=1;
            else cur_ex_wb_bus[ppn].sr_cy=0;
            if (cur_ex_wb_bus[ppn].ex_result==0)
            cur_ex_wb_bus[ppn].sr_zr=1;
            else cur_ex_wb_bus[ppn].sr_zr=0;
        }
        else if (strcmp(bus.snd_ins.opname,"ANDI")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=1;
            cur_ex_wb_bus[ppn].forward=1;
            cur_ex_wb_bus[ppn].is_sr_alu=2;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1&bus.snd_ins.imm;
            if (cur_ex_wb_bus[ppn].ex_result==0)
            cur_ex_wb_bus[ppn].sr_zr=1;
            else cur_ex_wb_bus[ppn].sr_zr=0;
        }
        //khh 2.53:AND
        else if (strcmp(bus.snd_ins.opname,"AND")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=1;
            cur_ex_wb_bus[ppn].forward=1;
            cur_ex_wb_bus[ppn].is_sr_alu=2;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,2,2);
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1&cur_id_ex_bus[ppn].value2;
            if (cur_ex_wb_bus[ppn].ex_result==0)
            cur_ex_wb_bus[ppn].sr_zr=1;
            else cur_ex_wb_bus[ppn].sr_zr=0;
        }
        else if (strcmp(bus.snd_ins.opname,"SRL")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_id_ex_bus[ppn].snd_ins.rs1==31)
            //     cur_id_ex_bus[ppn].value1=0;
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].forward=1;
            cur_ex_wb_bus[ppn].is_sr_alu=2;
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1>>bus.snd_ins.imm;
            if (cur_ex_wb_bus[ppn].ex_result==0)
            cur_ex_wb_bus[ppn].sr_zr=1;
            else cur_ex_wb_bus[ppn].sr_zr=0;
        }
        else if (strcmp(bus.snd_ins.opname,"SLL")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_id_ex_bus[ppn].snd_ins.rs1==31)
            //     cur_id_ex_bus[ppn].value1=0;
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].forward=1;
            cur_ex_wb_bus[ppn].is_sr_alu=2;
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1<<bus.snd_ins.imm;
            if (cur_ex_wb_bus[ppn].ex_result==0)
                cur_ex_wb_bus[ppn].sr_zr=1;
            else cur_ex_wb_bus[ppn].sr_zr=0;
        }
        else if (strcmp(bus.snd_ins.opname,"CMP")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=0;
            cur_ex_wb_bus[ppn].forward=0;
            cur_ex_wb_bus[ppn].is_sr_alu=1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,2,2);
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1-cur_id_ex_bus[ppn].value2;
            if ((bus.value1>0 && bus.value2>0 && cur_ex_wb_bus[ppn].ex_result<0) ||
                (bus.value1>0 && bus.value2<0 && cur_ex_wb_bus[ppn].ex_result<0))
            cur_ex_wb_bus[ppn].sr_cy=1;
            else cur_ex_wb_bus[ppn].sr_cy=0;
            if (cur_ex_wb_bus[ppn].ex_result==0)
            cur_ex_wb_bus[ppn].sr_zr=1;
            else cur_ex_wb_bus[ppn].sr_zr=0;
        }
        else if (strcmp(bus.snd_ins.opname,"ADD")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=1; //����ָ����ҪдGPR��ָ��
            cur_ex_wb_bus[ppn].forward=1;   //����ָ����EX���Ϳ��Բ����������ҪдGPR
            cur_ex_wb_bus[ppn].is_sr_alu=1; //����ָ��Ҫ�޸�sr_cy��sr_zrλ
            //�����һ��ָ�WB���������û�б�д��Ĵ�������������Ҫ�õ�������
            //��ô�ͽ�������ֱ�Ӹ���EX��
            //MOVI GPR0, #16
            //ADD GPR2,GPR0,GPR1 //GPR2=GPR0+GPR1
            //��һ��ָ���õ�GPRs��ֵʱ
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,2,2);
            //Ex_result: ��ż�����
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1+cur_id_ex_bus[ppn].value2;
            //e.g:65535+4=3(65539) �������Ҫ��λ
            //�����������ӣ����С��ǰ����κ�һ�����ͱ������������λ��1
            // 65535+65535+1=65535
            //ADDC��
            //�������������ӣ����С��ǰ�����������1
            //����������ǰ�����������ԭ�ȵĽ�λΪ1�����Ƿ����˽�λ����1
            // 3+ 65535 +1 =3
            // value1+value2+((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)>>2);
            // SR
            // 15 14 13 ... 3 2 1 0
            //                CY
            //
            if (cur_id_ex_bus[ppn].value1>cur_ex_wb_bus[ppn].ex_result)
                //(bus.value1>0 && bus.value2<0 && cur_ex_wb_bus[ppn].ex_result<0))
            cur_ex_wb_bus[ppn].sr_cy=1;
            else cur_ex_wb_bus[ppn].sr_cy=0;
            //������Ϊ0��sr_zrΪ0
            if (cur_ex_wb_bus[ppn].ex_result==0)
            cur_ex_wb_bus[ppn].sr_zr=1;
            else cur_ex_wb_bus[ppn].sr_zr=0;
        }
        else if (strcmp(bus.snd_ins.opname,"SUB")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des = 1;
            cur_ex_wb_bus[ppn].forward = 1;
            cur_ex_wb_bus[ppn].is_sr_alu = 1; 
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,2,2);
            cur_ex_wb_bus[ppn].ex_result = cur_id_ex_bus[ppn].value1-cur_id_ex_bus[ppn].value2;
            if(cur_id_ex_bus[ppn].value1<cur_ex_wb_bus[ppn].ex_result)
                cur_ex_wb_bus[ppn].sr_cy = 1;
            else
                cur_ex_wb_bus[ppn].sr_cy = 0;
            if(cur_ex_wb_bus[ppn].ex_result==0)
                cur_ex_wb_bus[ppn].sr_zr = 1;
            else
                cur_ex_wb_bus[ppn].sr_zr = 0;
        }
        else if (strcmp(bus.snd_ins.opname,"MOVRR")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=1;
            cur_ex_wb_bus[ppn].forward=1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1;
            cur_ex_wb_bus[ppn].sr_cy=0;
            cur_ex_wb_bus[ppn].sr_zr=0;
        }
        else if (strcmp(bus.snd_ins.opname,"MOVSR")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=1;
            cur_ex_wb_bus[ppn].forward=1;
            // if (cur_wb_bus[ppn].is_sr_des && 
            // ((cur_wb_bus[ppn].waddr==SR_SAR_CKS) && cur_ex_wb_bus[ppn].trd_ins.rs1==SR_SAR_CKS) ||
            // ((cur_wb_bus[ppn].waddr==SR_SAR_HASHH) && cur_ex_wb_bus[ppn].trd_ins.rs1==SR_SAR_HASHH) ||
            // ((cur_wb_bus[ppn].waddr==SR_SAR_HASHL) && cur_ex_wb_bus[ppn].trd_ins.rs1==SR_SAR_HASHL) ||
            // ((cur_wb_bus[ppn].waddr==SR_GPT_CTRL) && cur_ex_wb_bus[ppn].trd_ins.rs1==SR_GPT_CTRL) ||
            // ((cur_wb_bus[ppn].waddr==SR_GPT_T0) && cur_ex_wb_bus[ppn].trd_ins.rs1==SR_GPT_T0) ||
            // ((cur_wb_bus[ppn].waddr==SR_GPT_T1) && cur_ex_wb_bus[ppn].trd_ins.rs1==SR_GPT_T1) ||
            // ((cur_wb_bus[ppn].waddr==SR_GPT_T2) && cur_ex_wb_bus[ppn].trd_ins.rs1==SR_GPT_T2))
            //     cur_ex_wb_bus[ppn].ex_result=cur_wb_bus[ppn].wdata;
            // else if (cur_wb_bus[ppn].is_sr_des && cur_wb_bus[ppn].is_hash
            // && cur_ex_wb_bus[ppn].trd_ins.rs1==SR_SAR_HASHH)
            //     cur_ex_wb_bus[ppn].is_sr_des=cur_wb_bus[ppn].wdata;
            // else if (cur_wb_bus[ppn].is_sr_des && cur_wb_bus[ppn].is_hash
            // && cur_ex_wb_bus[ppn].trd_ins.rs2==SR_SAR_HASHL)
            //     cur_ex_wb_bus[ppn].is_sr_des=cur_wb_bus[ppn].wdata2;
            // else
                cur_ex_wb_bus[ppn].ex_result=sr[ppn][bus.snd_ins.rs1];//sr�Ĵ����������(����)
            cur_ex_wb_bus[ppn].sr_cy=0;
            cur_ex_wb_bus[ppn].sr_zr=0;
        }
        else if (strcmp(bus.snd_ins.opname,"MOVRS")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=0;
            cur_ex_wb_bus[ppn].forward=0;
            cur_ex_wb_bus[ppn].is_sr_des=1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1;
            cur_ex_wb_bus[ppn].sr_cy=0;
            cur_ex_wb_bus[ppn].sr_zr=0;
        }
        //khh added: MOVBRLE,MOVBRBE
        else if (strcmp(bus.snd_ins.opname,"MOVBRLE")==0 || strcmp(bus.snd_ins.opname,"MOVBRBE")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=1;
            cur_ex_wb_bus[ppn].forward=0;//warning:need to change
            cur_ex_wb_bus[ppn].is_read_hb=1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            // if (cur_id_ex_bus[ppn].snd_ins.rs1==31)
            //     cur_id_ex_bus[ppn].value1=0;
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1>>1<<1;
            cur_ex_wb_bus[ppn].sr_cy=0;
            cur_ex_wb_bus[ppn].sr_zr=0;
        }
        // else if (strcmp(bus.snd_ins.opname,"MOVBR")==0)
        // {
        //     cur_ex_wb_bus[ppn].is_gpr_des=1;
        //     cur_ex_wb_bus[ppn].forward=0;//warning:need to change
        //     cur_ex_wb_bus[ppn].is_read_hb=1;
        //     if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
        //     {
        //         cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
        //     }
        //     else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
        //     {
        //         cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
        //     }
        //     if (cur_id_ex_bus[ppn].snd_ins.rs1==31)
        //         cur_id_ex_bus[ppn].value1=0;
        //     cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1>>1<<1;
        //     cur_ex_wb_bus[ppn].sr_cy=0;
        //     cur_ex_wb_bus[ppn].sr_zr=0;
        // }
        //merge-wzy:MOVRBBE,MOVRBLE
        else if (strcmp(bus.snd_ins.opname, "MOVRBLE") == 0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des = 0;
            cur_ex_wb_bus[ppn].forward = 0;
            cur_ex_wb_bus[ppn].is_hb_des = 1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr == cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr + 1 == cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr == cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value2 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr + 1 == cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value2 = cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,0,2);
            cur_ex_wb_bus[ppn].ex_result = cur_id_ex_bus[ppn].value1;
            cur_ex_wb_bus[ppn].hb_addr = cur_id_ex_bus[ppn].value2 >> 1 << 1;
            cur_ex_wb_bus[ppn].sr_cy = 0;
            cur_ex_wb_bus[ppn].sr_zr = 0;
        }
        else if (strcmp(bus.snd_ins.opname, "MOVRBBE") == 0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des = 0;
            cur_ex_wb_bus[ppn].forward = 0;
            cur_ex_wb_bus[ppn].is_hb_des = 1;
            cur_ex_wb_bus[ppn].is_be = 1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr == cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr + 1 == cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr == cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value2 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr + 1 == cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value2 = cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,0,2);
            unsigned short value1_high = 0, value1_low = 0;
            value1_high = (cur_id_ex_bus[ppn].value1 & 0xFF00)>>8;
            value1_low = (cur_id_ex_bus[ppn].value1 & 0x00FF)<<8;
            //printf("source line: %d, in function: %s,0x%x,0x%x \n",__LINE__,__FUNCTION__,value1_high,value1_low);
            cur_id_ex_bus[ppn].value1 = value1_high + value1_low;
            cur_ex_wb_bus[ppn].ex_result = cur_id_ex_bus[ppn].value1;
            cur_ex_wb_bus[ppn].hb_addr = cur_id_ex_bus[ppn].value2 >> 1 << 1;
            cur_ex_wb_bus[ppn].sr_cy = 0;
            cur_ex_wb_bus[ppn].sr_zr = 0;
        }
        // else if (strcmp(bus.snd_ins.opname,"MOVRB")==0)
        // {
        //     cur_ex_wb_bus[ppn].is_gpr_des=0;
        //     cur_ex_wb_bus[ppn].forward=0;
        //     cur_ex_wb_bus[ppn].is_hb_des=1;
        //     if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
        //     {
        //         cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
        //     }
        //     else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
        //     {
        //         cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
        //     }
        //     if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
        //     {
        //         cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
        //     }
        //     else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
        //     {
        //         cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
        //     }
        //     cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1;
        //     cur_ex_wb_bus[ppn].hb_addr=cur_id_ex_bus[ppn].value2>>1<<1;
        //     cur_ex_wb_bus[ppn].sr_cy=0;
        //     cur_ex_wb_bus[ppn].sr_zr=0;
        // }
        else if (strcmp(bus.snd_ins.opname,"MOVHM")==0)
        {
            cur_ex_wb_bus[ppn].is_mr_des=1;
            cur_ex_wb_bus[ppn].forward=0;
            cur_ex_wb_bus[ppn].is_read_hb=1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            cur_ex_wb_bus[ppn].ex_result=(cur_id_ex_bus[ppn].value1>>1<<1)+((bus.snd_ins.imm-1-bus.is_mhm)<<1);
            cur_ex_wb_bus[ppn].sr_cy=0;
            cur_ex_wb_bus[ppn].sr_zr=0;
        }
        else if (strcmp(bus.snd_ins.opname,"MOVMH")==0)
        {
            cur_ex_wb_bus[ppn].is_hb_des=1;
            cur_ex_wb_bus[ppn].forward=0;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,0,1);
            cur_ex_wb_bus[ppn].ex_result=mr[bus.snd_ins.rs1];
            cur_ex_wb_bus[ppn].hb_addr=(cur_id_ex_bus[ppn].value1>>1<<1)+((bus.snd_ins.imm-1-bus.is_mhm)<<1);
            cur_ex_wb_bus[ppn].sr_cy=0;
            cur_ex_wb_bus[ppn].sr_zr=0;
        }
        else if (strcmp(bus.snd_ins.opname,"MOVBB")==0)
        {
            cur_ex_wb_bus[ppn].is_hb_des=1;
            cur_ex_wb_bus[ppn].is_read_hb=1;
            cur_ex_wb_bus[ppn].forward=0;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].is_mhm=cur_wb_bus[ppn].wdata-2;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].is_mhm=cur_wb_bus[ppn].wdata2-2;
            // }
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,2);
            is_forward_ex(ppn,2,4);
            cur_ex_wb_bus[ppn].is_mhm=cur_id_ex_bus[ppn].is_mhm;
            cur_ex_wb_bus[ppn].ex_result=(cur_id_ex_bus[ppn].value2>>1<<1);
            cur_ex_wb_bus[ppn].hb_addr=(cur_id_ex_bus[ppn].value1>>1<<1);
        }
        else if (strcmp(bus.snd_ins.opname,"MUL")==0)
        {
            //cur_ex_wb_bus[ppn].is_gpr_des=(cur_id_ex_bus[ppn].is_mhm==4)?0:1;//��һ��EX��������
            cur_ex_wb_bus[ppn].is_gpr_des=1;
            cur_ex_wb_bus[ppn].is_mul=1;
            cur_ex_wb_bus[ppn].is_sr_alu=0;
            cur_ex_wb_bus[ppn].forward=0;//��Ϊ�˷��Ľ������EXǰ�ݸ�ID�������˷��Ķ��۽���WB��
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,2,2);
            // cur_ex_wb_bus[ppn].is_mhm=cur_id_ex_bus[ppn].is_mhm;
            // if (cur_id_ex_bus[ppn].is_mhm!=0) //���һ��EX�����˷�
            // {
                unsigned int mul_result=cur_id_ex_bus[ppn].value1*cur_id_ex_bus[ppn].value2;
                cur_ex_wb_bus[ppn].ex_result=mul_result & 0x0000ffff;
                cur_ex_wb_bus[ppn].ex_result2=mul_result >> 16;
            // }
            // if (cur_id_ex_bus[ppn].is_mhm==0) //���һ��EX�������2�������1���Ĵ���++
            // {
            //     cur_ex_wb_bus[ppn].ex_result=cur_ex_wb_bus[ppn].ex_result2;
            //     cur_ex_wb_bus[ppn].trd_ins.rd++;
            // }
            // cur_ex_wb_bus[ppn].hb_addr=(cur_id_ex_bus[ppn].value1>>1<<1);
        }
        //merge by wzy: CKSUMLE,CKSUMBE
        else if (strcmp(bus.snd_ins.opname, "CKSUMLE") == 0||strcmp(bus.snd_ins.opname, "CKSUMBE") == 0)
        {
            cur_ex_wb_bus[ppn].is_sr_des=1;
            cur_ex_wb_bus[ppn].is_read_hb=1;
            cur_ex_wb_bus[ppn].forward=0;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].is_mhm=cur_wb_bus[ppn].wdata-2;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].is_mhm=cur_wb_bus[ppn].wdata2-2;
            // }
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,4);
            if (cur_wb_bus[ppn].is_sr_des && (cur_wb_bus[ppn].waddr==SR_SAR_CKS))
            {
                cur_ex_wb_bus[ppn].ex_result=cur_wb_bus[ppn].wdata;
            }
            else
                cur_ex_wb_bus[ppn].ex_result=sr[ppn][SR_SAR_CKS];
            cur_ex_wb_bus[ppn].is_mhm=cur_id_ex_bus[ppn].is_mhm;
            cur_ex_wb_bus[ppn].hb_addr=(cur_id_ex_bus[ppn].value1>>1<<1);
        }
        //khh added:HASHLE,HASHBE
        else if (strcmp(bus.snd_ins.opname,"HASHLE")==0 || strcmp(bus.snd_ins.opname,"HASHBE")==0)
        {
            cur_ex_wb_bus[ppn].is_sr_des=1;
            cur_ex_wb_bus[ppn].is_read_hb=1;
            cur_ex_wb_bus[ppn].forward=0;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].is_mhm=cur_wb_bus[ppn].wdata-2;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].is_mhm=cur_wb_bus[ppn].wdata2-2;
            // }
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,4);
            if (cur_wb_bus[ppn].is_sr_des && (cur_wb_bus[ppn].waddr==SR_SAR_HASHH))
            {
                cur_ex_wb_bus[ppn].ex_result=cur_wb_bus[ppn].wdata;
            }
            else if (cur_wb_bus[ppn].is_sr_des && cur_wb_bus[ppn].is_hash)
                cur_ex_wb_bus[ppn].ex_result=cur_wb_bus[ppn].wdata;
            else
                cur_ex_wb_bus[ppn].ex_result=sr[ppn][SR_SAR_HASHH];
            if (cur_wb_bus[ppn].is_sr_des && (cur_wb_bus[ppn].waddr==SR_SAR_HASHL))
            {
                cur_ex_wb_bus[ppn].ex_result2=cur_wb_bus[ppn].wdata;
            }
            else if (cur_wb_bus[ppn].is_sr_des && cur_wb_bus[ppn].is_hash)
                cur_ex_wb_bus[ppn].ex_result2=cur_wb_bus[ppn].wdata2;
            else
                cur_ex_wb_bus[ppn].ex_result2=sr[ppn][SR_SAR_HASHL];
            cur_ex_wb_bus[ppn].is_mhm=cur_id_ex_bus[ppn].is_mhm;
            cur_ex_wb_bus[ppn].hb_addr=(cur_id_ex_bus[ppn].value1>>1<<1);
        }
        // else if (strcmp(bus.snd_ins.opname,"HASH")==0)
        // {
        //     cur_ex_wb_bus[ppn].is_sr_des=1;
        //     cur_ex_wb_bus[ppn].is_read_hb=1;
        //     cur_ex_wb_bus[ppn].forward=0;
        //     if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
        //     {
        //         cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
        //     }
        //     else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
        //     {
        //         cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
        //     }
        //     if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
        //     {
        //         cur_id_ex_bus[ppn].is_mhm=cur_wb_bus[ppn].wdata-2;
        //     }
        //     else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
        //     {
        //         cur_id_ex_bus[ppn].is_mhm=cur_wb_bus[ppn].wdata2-2;
        //     }
        //     if (cur_wb_bus[ppn].is_sr_des && (cur_wb_bus[ppn].waddr==SR_SAR_HASHH))
        //     {
        //         cur_ex_wb_bus[ppn].ex_result=cur_wb_bus[ppn].wdata;
        //     }
        //     else if (cur_wb_bus[ppn].is_sr_des && cur_wb_bus[ppn].is_hash)
        //         cur_ex_wb_bus[ppn].ex_result=cur_wb_bus[ppn].wdata;
        //     else
        //         cur_ex_wb_bus[ppn].ex_result=sr[ppn][SR_SAR_HASHH];
        //     if (cur_wb_bus[ppn].is_sr_des && (cur_wb_bus[ppn].waddr==SR_SAR_HASHL))
        //     {
        //         cur_ex_wb_bus[ppn].ex_result2=cur_wb_bus[ppn].wdata;
        //     }
        //     else if (cur_wb_bus[ppn].is_sr_des && cur_wb_bus[ppn].is_hash)
        //         cur_ex_wb_bus[ppn].ex_result2=cur_wb_bus[ppn].wdata2;
        //     else
        //         cur_ex_wb_bus[ppn].ex_result2=sr[ppn][SR_SAR_HASHL];
        //     cur_ex_wb_bus[ppn].is_mhm=cur_id_ex_bus[ppn].is_mhm;
        //     cur_ex_wb_bus[ppn].hb_addr=(cur_id_ex_bus[ppn].value1>>1<<1);
        // }
        else if (strcmp(bus.snd_ins.opname,"LOADM")==0)
        {
            event_node *load_event=(event_node *)malloc(sizeof(event_node));
            event_node *token=event_list;
            event_node *previous=token;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_id_ex_bus[ppn].snd_ins.rs1==31)
            //     cur_id_ex_bus[ppn].value1=0;
            // if (cur_id_ex_bus[ppn].snd_ins.rs2==31)
            //     cur_id_ex_bus[ppn].value2=0;
            // if (cur_id_ex_bus[ppn].snd_ins.rd==31)
            //     cur_id_ex_bus[ppn].value3=0;
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,2,2);
            is_forward_ex(ppn,0,3);
            cur_ex_wb_bus[ppn].is_clear_sr_sta=1;
            cur_ex_wb_bus[ppn].ex_result=SR_STA_FLAG_HBLC;
            if (event_list==NULL)
            {
                load_event->event_type=EVENT_LOADM;
                load_event->event_para1=cur_id_ex_bus[ppn].value1>>1<<1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2>>1<<1;
                load_event->event_para3=cur_id_ex_bus[ppn].value3>>1<<1;
                load_event->core_id=ppn;
                load_event->next=NULL;
                //load_event->trigger_clk=clk+LOAD_CYCLE(load_event->event_para2);
                load_event->trigger_clk=clk+LOAD_CYCLE0;
                event_list=load_event;
                printf("LOADM from %d len %d to %d judged in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->event_para3,load_event->trigger_clk);
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                load_event->event_type=EVENT_LOADM;
                load_event->event_para1=cur_id_ex_bus[ppn].value1>>1<<1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2>>1<<1;
                load_event->event_para3=cur_id_ex_bus[ppn].value3>>1<<1;
                load_event->core_id=ppn;
                load_event->next=NULL;
                //load_event->trigger_clk=clk+LOAD_CYCLE(load_event->event_para2);
                load_event->trigger_clk=clk+LOAD_CYCLE0;
                previous->next=load_event;
                printf("LOADM from %d len %d to %d judged in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->event_para3,load_event->trigger_clk);
            }
        }
        // else if (strcmp(bus.snd_ins.opname,"LOADM")==0)
        // {
        //     event_node *token=event_list;
        //     event_node *previous=token;
        //     while (token!=NULL)
        //     {
        //         previous=token;
        //         token=token->next;
        //     }
        //     previous->event_para3=bus.value1>>1<<1;
        //     printf("LOADM from %d len %d to %d finished in %d;\n",previous->event_para1,previous->event_para2,previous->event_para3,previous->trigger_clk);
        // }
        else if (strcmp(bus.snd_ins.opname,"STOREM")==0)
        {
            event_node *load_event=(event_node *)malloc(sizeof(event_node));
            event_node *token=event_list;
            event_node *previous=token;
            //  if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_id_ex_bus[ppn].snd_ins.rs1==31)
            //     cur_id_ex_bus[ppn].value1=0;
            // if (cur_id_ex_bus[ppn].snd_ins.rs2==31)
            //     cur_id_ex_bus[ppn].value2=0;
            // if (cur_id_ex_bus[ppn].snd_ins.rd==31)
            //     cur_id_ex_bus[ppn].value3=0;
            is_forward_ex(ppn,1,1);
            is_forward_ex(ppn,2,2);
            is_forward_ex(ppn,0,3);
            cur_ex_wb_bus[ppn].is_clear_sr_sta=1;
            cur_ex_wb_bus[ppn].ex_result=SR_STA_FLAG_HBSC;
            if (event_list==NULL)
            {
                load_event->event_type=EVENT_STOREM;
                load_event->event_para1=cur_id_ex_bus[ppn].value1>>1<<1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2>>1<<1;
                load_event->event_para3=cur_id_ex_bus[ppn].value3>>1<<1;
                load_event->core_id=ppn;
                load_event->next=NULL;
                //load_event->trigger_clk=clk+STORE_CYCLE(load_event->event_para2);
                load_event->trigger_clk=clk+STORE_CYCLE0;
                event_list=load_event;
                printf("STOREM from %d len %d to %d judged in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->event_para3,load_event->trigger_clk);
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                load_event->event_type=EVENT_STOREM;
                load_event->event_para1=cur_id_ex_bus[ppn].value1>>1<<1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2>>1<<1;
                load_event->event_para3=cur_id_ex_bus[ppn].value3>>1<<1;
                load_event->core_id=ppn;
                load_event->next=NULL;
                //load_event->trigger_clk=clk+STORE_CYCLE(load_event->event_para2);
                load_event->trigger_clk=clk+STORE_CYCLE0;
                previous->next=load_event;
                printf("STOREM from %d len %d to %d judged in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->event_para3,load_event->trigger_clk);
            }
        }
        // else if (strcmp(bus.snd_ins.opname,"STOREM")==0)
        // {
        //     event_node *token=event_list;
        //     event_node *previous=token;
        //     while (token!=NULL)
        //     {
        //         previous=token;
        //         token=token->next;
        //     }
        //     previous->event_para3=bus.value1>>1<<1;
        //     printf("STOREM from %d len %d to %d finished in %d;\n",previous->event_para1,previous->event_para2,previous->event_para3,previous->trigger_clk);
        // }
        else if ((strcmp(bus.snd_ins.opname,"LOADT")==0))
        {
            event_node *query_event=(event_node *)malloc(sizeof(event_node));
            event_node *token=event_list;
            event_node *previous=token;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,2,2);
            is_forward_ex(ppn,1,3);
            cur_ex_wb_bus[ppn].is_clear_sr_sta=1;
            cur_ex_wb_bus[ppn].ex_result=SR_STA_FLAG_TBLC;
            if (event_list==NULL)
            {
                query_event->event_type=EVENT_LOADT;
                query_event->event_para1=cur_id_ex_bus[ppn].value1;
                query_event->event_para2=cur_id_ex_bus[ppn].value2;//ItemID
                query_event->event_para3=cur_id_ex_bus[ppn].value3;
                query_event->core_id=ppn;
                query_event->next=NULL;
                query_event->trigger_clk=clk+LOADT_CYCLE0;
                event_list=query_event;
                printf("LOADT in TableID %d ItemID %d to %d finished in %ld;\n",
                query_event->event_para1,query_event->event_para2,
                query_event->event_para3,query_event->trigger_clk);
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                query_event->event_type=EVENT_LOADT;
                query_event->event_para1=cur_id_ex_bus[ppn].value1;
                query_event->event_para2=cur_id_ex_bus[ppn].value2;//ItemID
                query_event->event_para3=cur_id_ex_bus[ppn].value3;
                query_event->core_id=ppn;
                query_event->next=NULL;
                query_event->trigger_clk=clk+LOADT_CYCLE0;
                previous->next=query_event;
                printf("LOADT in TableID %d ItemID %d to %d finished in %ld;\n",
                query_event->event_para1,query_event->event_para2,
                query_event->event_para3,query_event->trigger_clk);
            }
            
        }
        // else if (strcmp(bus.snd_ins.opname,"LOADT")==0)
        // {
        //     event_node *token=event_list;
        //     event_node *previous=token;
        //     while (token!=NULL)
        //     {
        //         previous=token;
        //         token=token->next;
        //     }
        //     previous->event_para3=cur_id_ex_bus[ppn].value1;
        //     printf("LOADT in TableID %d ItemID %d to %d finished in %d;\n",
        //     previous->event_para1,previous->event_para2,previous->event_para3,previous->trigger_clk);
        // }
        else if (strcmp(bus.snd_ins.opname,"STORET")==0)
        {
            event_node *query_event=(event_node *)malloc(sizeof(event_node));
            event_node *token=event_list;
            event_node *previous=token;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,2);
            cur_ex_wb_bus[ppn].is_clear_sr_sta=1;
            cur_ex_wb_bus[ppn].ex_result=SR_STA_FLAG_TBSC;
            if (event_list==NULL)
            {
                query_event->event_type=EVENT_STORET;
                query_event->event_para1=cur_id_ex_bus[ppn].value1;
                query_event->event_para2=cur_id_ex_bus[ppn].value2;//ItemID+DATA
                query_event->event_para3=bus.snd_ins.imm;//
                query_event->core_id=ppn;
                query_event->next=NULL;
                query_event->trigger_clk=clk+STORET_CYCLE0;
                event_list=query_event;
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                query_event->event_type=EVENT_STORET;
                query_event->event_para1=cur_id_ex_bus[ppn].value1;
                query_event->event_para2=cur_id_ex_bus[ppn].value2;//ItemID+DATA
                query_event->event_para3=bus.snd_ins.imm;//
                query_event->core_id=ppn;
                query_event->next=NULL;
                query_event->trigger_clk=clk+STORET_CYCLE0;
                previous->next=query_event;
            }
            printf("STORET in TableID %d from %d len %d finished in %ld;\n",
            query_event->event_para1,query_event->event_para2,query_event->event_para3,query_event->trigger_clk);
        }
        else if (strcmp(bus.snd_ins.opname,"QUERYT")==0)
        {
            event_node *query_event=(event_node *)malloc(sizeof(event_node));
            event_node *token=event_list;
            event_node *previous=token;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs2))
            // {
            //     cur_id_ex_bus[ppn].value3=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,2);
            is_forward_ex(ppn,2,3);
            cur_ex_wb_bus[ppn].is_clear_sr_sta=1;
            cur_ex_wb_bus[ppn].ex_result=SR_STA_FLAG_QC;
            if (event_list==NULL)
            {
                query_event->event_type=EVENT_QUERYT;
                query_event->event_para1=cur_id_ex_bus[ppn].value1;
                query_event->event_para2=cur_id_ex_bus[ppn].value3;//qtuple
                query_event->event_para3=cur_id_ex_bus[ppn].value2;//rdata
                query_event->core_id=ppn;
                query_event->event_para4=bus.snd_ins.imm;
                query_event->next=NULL;
                query_event->trigger_clk=clk+QUERY_CYCLE0(query_event->event_para4);
                event_list=query_event;
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                query_event->event_type=EVENT_QUERYT;
                query_event->event_para1=cur_id_ex_bus[ppn].value1;
                query_event->event_para2=cur_id_ex_bus[ppn].value3;//qtuple
                query_event->event_para3=cur_id_ex_bus[ppn].value2;//rdata
                query_event->event_para4=bus.snd_ins.imm;
                query_event->core_id=ppn;
                query_event->next=NULL;
                query_event->trigger_clk=clk+QUERY_CYCLE0(query_event->event_para4);
                previous->next=query_event;
            }
            //merge khh: QUERYT printf
            printf("QUERYT in TableID %d from key %d len %d and result from %d will be judged at clk %ld\n",
            query_event->event_para1,query_event->event_para2,query_event->event_para4,query_event->event_para3,query_event->trigger_clk);
        }
        else if (strcmp(bus.snd_ins.opname,"INSERTT")==0)
        {
            event_node *query_event=(event_node *)malloc(sizeof(event_node));
            event_node *token=event_list;
            event_node *previous=token;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_id_ex_bus[ppn].snd_ins.rd==0)
            //     cur_id_ex_bus[ppn].value1=0;
            // else if (cur_id_ex_bus[ppn].snd_ins.rs1==0)
            //     cur_id_ex_bus[ppn].value2=0;
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,2);
            cur_ex_wb_bus[ppn].is_clear_sr_sta=1;
            cur_ex_wb_bus[ppn].ex_result=SR_STA_FLAG_TBIC;
            if (event_list==NULL)
            {
                query_event->event_type=EVENT_INSERTT;
                query_event->event_para1=cur_id_ex_bus[ppn].value1;
                query_event->event_para2=cur_id_ex_bus[ppn].value2;//Insertt s
                query_event->event_para3=bus.snd_ins.imm;//size
                query_event->core_id=ppn;
                query_event->next=NULL;
                query_event->trigger_clk=clk+INSERTT_CYCLE(query_event->event_para3);
                event_list=query_event;
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                query_event->event_type=EVENT_INSERTT;
                query_event->event_para1=cur_id_ex_bus[ppn].value1;
                query_event->event_para2=cur_id_ex_bus[ppn].value2;//Insertt s
                query_event->event_para3=bus.snd_ins.imm;//size
                query_event->core_id=ppn;
                query_event->next=NULL;
                query_event->trigger_clk=clk+INSERTT_CYCLE(query_event->event_para3);
                previous->next=query_event;
            }
            printf("INSERTT in TableID %d from %d len %d finished in %ld;\n",
            query_event->event_para1,query_event->event_para2,query_event->event_para3,query_event->trigger_clk);
        }
        else if (strcmp(bus.snd_ins.opname,"DELETET")==0)
        {
            event_node *query_event=(event_node *)malloc(sizeof(event_node));
            event_node *token=event_list;
            event_node *previous=token;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_id_ex_bus[ppn].snd_ins.rd==0)
            //     cur_id_ex_bus[ppn].value1=0;
            // else if (cur_id_ex_bus[ppn].snd_ins.rs1==0)
            //     cur_id_ex_bus[ppn].value2=0;
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,2);
            if (event_list==NULL)
            {
                query_event->event_type=EVENT_DELETET;
                query_event->event_para1=cur_id_ex_bus[ppn].value1;
                query_event->event_para2=cur_id_ex_bus[ppn].value2;//deletes s
                query_event->event_para3=bus.snd_ins.imm;//size
                query_event->core_id=ppn;
                query_event->next=NULL;
                query_event->trigger_clk=clk+DELETET_CYCLE(query_event->event_para3);
                event_list=query_event;
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                query_event->event_type=EVENT_DELETET;
                query_event->event_para1=cur_id_ex_bus[ppn].value1;
                query_event->event_para2=cur_id_ex_bus[ppn].value2;//Insertt s
                query_event->core_id=ppn;
                query_event->event_para3=bus.snd_ins.imm;//size
                query_event->next=NULL;
                query_event->trigger_clk=clk+DELETET_CYCLE(query_event->event_para3);
                previous->next=query_event;
            }
            printf("DELETET in TableID %d from %d len %d finished in %ld;\n",
            query_event->event_para1,query_event->event_para2,query_event->event_para3,query_event->trigger_clk);
        }
        else if (strcmp(bus.snd_ins.opname,"MALCSMB")==0)
        {
            event_node *load_event=(event_node *)malloc(sizeof(event_node));
            event_node *token=event_list;
            event_node *previous=token;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_id_ex_bus[ppn].snd_ins.rd==31)
            //     cur_id_ex_bus[ppn].value1=0;
            // if (cur_id_ex_bus[ppn].snd_ins.rs1==31)
            //     cur_id_ex_bus[ppn].value2=0;
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,2);
            cur_ex_wb_bus[ppn].is_clear_sr_sta=1;
            cur_ex_wb_bus[ppn].ex_result=SR_STA_FLAG_TLB;
            if (event_list==NULL)
            {
                load_event->event_type=EVENT_LOADTLB;
                load_event->event_para1=cur_id_ex_bus[ppn].value1>>1<<1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2;
                load_event->core_id=ppn;
                load_event->next=NULL;
                load_event->trigger_clk=clk+LOADTLB_CYCLE(load_event->event_para2);
                event_list=load_event;
                printf("LOADTLB to %d len %d finished in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->trigger_clk);
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                load_event->event_type=EVENT_LOADTLB;
                load_event->event_para1=cur_id_ex_bus[ppn].value1>>1<<1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2;
                load_event->core_id=ppn;
                load_event->next=NULL;
                load_event->trigger_clk=clk+LOADTLB_CYCLE(load_event->event_para2);
                previous->next=load_event;
                printf("LOADTLB to %d len %d finished in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->trigger_clk);
            }
        }
        else if (strcmp(bus.snd_ins.opname,"FREESMB")==0)
        {
            event_node *load_event=(event_node *)malloc(sizeof(event_node));
            event_node *token=event_list;
            event_node *previous=token;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_id_ex_bus[ppn].snd_ins.rd==31)
            //     cur_id_ex_bus[ppn].value1=0;
            // if (cur_id_ex_bus[ppn].snd_ins.rs1==31)
            //     cur_id_ex_bus[ppn].value2=0;
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,2);
            cur_ex_wb_bus[ppn].is_clear_sr_sta=1;
            cur_ex_wb_bus[ppn].ex_result=SR_STA_FLAG_TLB;
            if (event_list==NULL)
            {
                load_event->event_type=EVENT_DELETETLB;
                load_event->event_para1=cur_id_ex_bus[ppn].value1>>1<<1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2;
                load_event->core_id=ppn;
                load_event->next=NULL;
                load_event->trigger_clk=clk+DELETETLB_CYCLE(load_event->event_para2);
                event_list=load_event;
                printf("DELETETLB to %d len %d finished in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->trigger_clk);
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                load_event->event_type=EVENT_DELETETLB;
                load_event->event_para1=cur_id_ex_bus[ppn].value1>>1<<1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2;
                load_event->core_id=ppn;
                load_event->next=NULL;
                load_event->trigger_clk=clk+DELETETLB_CYCLE(load_event->event_para2);
                previous->next=load_event;
                printf("DELETETLB to %d len %d finished in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->trigger_clk);
            }
        }
        else if (strcmp(bus.snd_ins.opname,"REFSMB")==0)
        {
            event_node *load_event=(event_node *)malloc(sizeof(event_node));
            event_node *token=event_list;
            event_node *previous=token;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rs1))
            // {
            //     cur_id_ex_bus[ppn].value2=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_id_ex_bus[ppn].snd_ins.rd==31)
            //     cur_id_ex_bus[ppn].value1=0;
            // if (cur_id_ex_bus[ppn].snd_ins.rs1==31)
            //     cur_id_ex_bus[ppn].value2=0;
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,2);
            cur_ex_wb_bus[ppn].is_clear_sr_sta=1;
            cur_ex_wb_bus[ppn].ex_result=SR_STA_FLAG_TLB;
            if (event_list==NULL)
            {
                load_event->event_type=EVENT_REUSETLB;
                load_event->event_para1=cur_id_ex_bus[ppn].value1>>1<<1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2;
                load_event->core_id=ppn;
                load_event->next=NULL;
                load_event->trigger_clk=clk+REUSETLB_CYCLE(load_event->event_para2);
                event_list=load_event;
                printf("REUSETLB to %d len %d finished in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->trigger_clk);
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                load_event->event_type=EVENT_REUSETLB;
                load_event->event_para1=cur_id_ex_bus[ppn].value1>>1<<1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2;
                load_event->core_id=ppn;
                load_event->next=NULL;
                load_event->trigger_clk=clk+REUSETLB_CYCLE(load_event->event_para2);
                previous->next=load_event;
                printf("REUSETLB to %d len %d finished in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->trigger_clk);
            }
        }
        else if (strcmp(bus.snd_ins.opname,"PUSHQ")==0)
        {
            cq_event_node *load_event=(cq_event_node *)malloc(sizeof(cq_event_node));
            cq_event_node *token=cq_queue;
            cq_event_node *previous=token;

            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,2);
            is_forward_ex(ppn,2,3);
            if (cq_queue==NULL)
            {
                load_event->event_type=PUSHQ_EVENT;
                int cn=(cur_id_ex_bus[ppn].value1&0x00FF)>>4;
                if (cn==(ppn>>3))
                {
                    cn=(cur_id_ex_bus[ppn].value1&0x00FF)%16;
                    if (cn==0)
                        cn=16;
                    else if (cn==2)
                        cn=17;
                    else if (cn==4)
                        cn=18;
                    else if (cn==6)
                        cn=19;
                    else
                        cn=20; //error
                }
                load_event->event_para1=cn;
                load_event->event_para2=cur_id_ex_bus[ppn].value2;
                load_event->event_para3=cur_id_ex_bus[ppn].value3;
                load_event->core_id=ppn;
                load_event->next=NULL;
                load_event->trigger_clk=clk+PUSHQ_CYCLE(load_event->event_para3);
                cq_queue=load_event;
                if (load_event->event_para3==cur_id_ex_bus[ppn].value3>64)
                {
                    printf("Error PUSHQ!\n");
                    exit(0);
                }
                printf("PUSHQ to %d mb %d SMBnum %d finished in %ld;\n",
                load_event->event_para1,load_event->event_para2,load_event->event_para3,
                load_event->trigger_clk);
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                load_event->event_type=PUSHQ_EVENT;
                int cn=(cur_id_ex_bus[ppn].value1&0x00FF)>>4;
                if (cn==(ppn>>3))
                {
                    cn=(cur_id_ex_bus[ppn].value1&0x00FF)%16;
                    if (cn==0)
                        cn=16;
                    else if (cn==2)
                        cn=17;
                    else if (cn==4)
                        cn=18;
                    else if (cn==6)
                        cn=19;
                    else
                        cn=20; //error
                }
                load_event->event_para1=cn;
                load_event->event_para2=cur_id_ex_bus[ppn].value2;
                load_event->event_para3=cur_id_ex_bus[ppn].value3;
                load_event->core_id=ppn;
                load_event->next=NULL;
                load_event->trigger_clk=clk+PUSHQ_CYCLE(load_event->event_para3);
                previous->next=load_event;
                if (load_event->event_para3==cur_id_ex_bus[ppn].value3>64)
                {
                    printf("Error PUSHQ!\n");
                    exit(0);
                }
                printf("PUSHQ to %d mb %d SMBnum %d finished in %ld;\n",
                load_event->event_para1,load_event->event_para2,load_event->event_para3,
                load_event->trigger_clk);
            }
        }
        else if (strcmp(bus.snd_ins.opname,"SENDMB")==0)
        {
            event_node *load_event=(event_node *)malloc(sizeof(event_node));
            event_node *token=event_list;
            event_node *previous=token;
            is_forward_ex(ppn,0,1);
            is_forward_ex(ppn,1,2);
            if (event_list==NULL)
            {
                load_event->event_type=EVENT_SENDMB;
                load_event->event_para1=cur_id_ex_bus[ppn].value1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2;
                load_event->core_id=ppn;
                load_event->next=NULL;
                load_event->trigger_clk=clk+SENDMB_CYCLE;
                event_list=load_event;
                printf("SENDMB to %d from %d finished in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->trigger_clk);
            }
            else 
            {
                while (token!=NULL)
                {
                    previous=token;
                    token=token->next;
                }
                load_event->event_type=EVENT_SENDMB;
                load_event->event_para1=cur_id_ex_bus[ppn].value1;
                load_event->event_para2=cur_id_ex_bus[ppn].value2;
                load_event->core_id=ppn;
                load_event->next=NULL;
                load_event->trigger_clk=clk+SENDMB_CYCLE;
                previous->next=load_event;
                printf("SENDMB to %d from %d finished in %ld;\n",
                load_event->event_para1,load_event->event_para2,
                load_event->trigger_clk);
            }
        }
        else if (strcmp(bus.snd_ins.opname,"BEQI")==0)
        {
            if ((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_ZR)!=0)
                cur_ex_wb_bus[ppn].pc_change=1;
            cur_ex_wb_bus[ppn].ex_result=bus.snd_ins.imm;
        }
        else if (strcmp(bus.snd_ins.opname,"BGTI")==0)
        {
            if ((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_ZR)==0 && (sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)==0)
                cur_ex_wb_bus[ppn].pc_change=1;
            cur_ex_wb_bus[ppn].ex_result=bus.snd_ins.imm;
        }
        else if (strcmp(bus.snd_ins.opname,"BI")==0)
        {
            cur_ex_wb_bus[ppn].pc_change=1;
            cur_ex_wb_bus[ppn].ex_result=bus.snd_ins.imm;
        }
        //merge-khh BNE,BNEI,BLT,BLTI,BGT
        else if (strcmp(bus.snd_ins.opname,"BNE")==0)
        {
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd)
            // {    
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,0,1);
            if((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_ZR)==0)
            {    
                cur_ex_wb_bus[ppn].pc_change = 1;
            }
            cur_ex_wb_bus[ppn].ex_result = cur_id_ex_bus[ppn].value1;
        }
        else if (strcmp(bus.snd_ins.opname,"BEQ")==0)
        {
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd)
            // {    
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,0,1);
            if((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_ZR)!=0)
            {    
                cur_ex_wb_bus[ppn].pc_change = 1;
            }
            cur_ex_wb_bus[ppn].ex_result = cur_id_ex_bus[ppn].value1;
        }
        else if (strcmp(bus.snd_ins.opname,"BNEI")==0)
        {
            if((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_ZR)==0)
            {    
                cur_ex_wb_bus[ppn].pc_change = 1;
            }
            cur_ex_wb_bus[ppn].ex_result = bus.snd_ins.imm;
        }
        else if (strcmp(bus.snd_ins.opname,"BLT")==0)
        {
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd)
            // {    
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,0,1);
            if((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)!=0)
            {    
                cur_ex_wb_bus[ppn].pc_change = 1;
            }
            cur_ex_wb_bus[ppn].ex_result = cur_id_ex_bus[ppn].value1;
        }
        else if (strcmp(bus.snd_ins.opname,"BLTI")==0)
        {
            if((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)!=0)
            {    
                cur_ex_wb_bus[ppn].pc_change = 1;
            }
            
            cur_ex_wb_bus[ppn].ex_result = bus.snd_ins.imm;
        }
        else if (strcmp(bus.snd_ins.opname,"BGT")==0)
        {
            // if(cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus)[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd)
            // {    
            //     cur_id_ex_bus[ppn].value1 = cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            is_forward_ex(ppn,0,1);
            if((sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_ZR)==0 && (sr[ppn][SR_STA_FLAG]&SR_STA_FLAG_CY)==0)
            {    
                cur_ex_wb_bus[ppn].pc_change = 1;
            }
            cur_ex_wb_bus[ppn].ex_result = cur_id_ex_bus[ppn].value1;
        }
        else if (strcmp(bus.snd_ins.opname,"B")==0)
        {
            cur_ex_wb_bus[ppn].pc_change=1;
            // if (cur_wb_bus[ppn].is_gpr_des && (cur_wb_bus[ppn].waddr==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata;
            // }
            // else if (cur_wb_bus[ppn].is_mul && (cur_wb_bus[ppn].waddr+1==cur_id_ex_bus[ppn].snd_ins.rd))
            // {
            //     cur_id_ex_bus[ppn].value1=cur_wb_bus[ppn].wdata2;
            // }
            // if (cur_id_ex_bus[ppn].snd_ins.rd==31)
            //     cur_id_ex_bus[ppn].value1=0;
            is_forward_ex(ppn,0,1);
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].value1;
        }
        else if (strcmp(bus.snd_ins.opname,"ISB")==0)
        {
            if ((sr[ppn][SR_STA_FLAG] & bus.snd_ins.imm)==0)
                cur_ex_wb_bus[ppn].pc_halt=1;
            cur_ex_wb_bus[ppn].ex_result=bus.pc>>2;
        }
        else if (strcmp(bus.snd_ins.opname,"ISBC")==0)
        {

            if ((sr[ppn][SR_STA_FLAG] & bus.snd_ins.imm)==0)
                cur_ex_wb_bus[ppn].pc_halt=1;
            else
            {
                cur_ex_wb_bus[ppn].trd_ins.rd=SR_STA_FLAG;
                cur_ex_wb_bus[ppn].is_clear_sr_sta=1;

            }
            cur_ex_wb_bus[ppn].ex_result=cur_id_ex_bus[ppn].snd_ins.imm;
        }
        else if (strcmp(bus.snd_ins.opname,"END")==0)
        {
            cur_ex_wb_bus[ppn].is_gpr_des=0;
            cur_ex_wb_bus[ppn].forward=0;
            cur_ex_wb_bus[ppn].ex_result=0;
            cur_ex_wb_bus[ppn].sr_cy=0;
            cur_ex_wb_bus[ppn].sr_zr=0;
        }
        else
        {
            cur_ex_wb_bus[ppn].is_gpr_des=0;
            cur_ex_wb_bus[ppn].forward=0;
            cur_ex_wb_bus[ppn].ex_result=0;
            cur_ex_wb_bus[ppn].sr_cy=0;
            cur_ex_wb_bus[ppn].sr_zr=0;
        }
    }
    else
    {
        cur_ex_wb_bus[ppn].is_read_hb=0;
        cur_ex_wb_bus[ppn].is_sr_des=0;
        cur_ex_wb_bus[ppn].stage_valid=0;
        cur_ex_wb_bus[ppn].is_sr_alu=0;
        cur_ex_wb_bus[ppn].sr_cy=0;
        cur_ex_wb_bus[ppn].sr_zr=0;
    }
}









/************************************************************************************/


void ppe_sim::ppe_run()
{
     for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            if (core_queues[i].is_used[j] == true)
            {
                int ppenum = i * 8 + j;
                ppe(ppenum);
            }
        }
    }
}

void ppe_sim::ppe(int ppenum)
{
    printf("clk=%ld\n", clk);
    printf("This PPE is ppe%d,cluster:%d\n", ppenum, ppenum >> 3);
    //�жϣ��������Ƿ�ﵽ��������������
    if (strcmp(cur_wb_bus[ppenum].fth_ins.opname, "END") == 0)
    {
        int core_number = ppenum % 8;
        int queue_number = ppenum >> 3;
        core_queues[queue_number].is_used[core_number] = 0;
        memset(cur_wb_bus[ppenum].fth_ins.opname, 0, 10);
        cur_wb_bus[ppenum].wen = 0;
        memset(cur_ex_wb_bus[ppenum].trd_ins.opname, 0, 10);
        cur_ex_wb_bus[ppenum].stage_valid = 0;
        memset(cur_id_ex_bus[ppenum].snd_ins.opname, 0, 10);
        cur_id_ex_bus[ppenum].stage_valid = 0;
        memset(cur_if_id_bus[ppenum].fst_ins.opname, 0, 10);
        printf("END!\n------------------\n");
        return;
    }
    if ((sr[ppenum][SR_STA_FLAG] & SR_STA_FLAG_ON) == 0)
    {
        int core_number = ppenum % 8;
        int queue_number = ppenum >> 3;
        core_queues[queue_number].is_used[core_number] = 0;
        memset(cur_wb_bus[ppenum].fth_ins.opname, 0, 10);
        cur_wb_bus[ppenum].wen = 0;
        memset(cur_ex_wb_bus[ppenum].trd_ins.opname, 0, 10);
        cur_ex_wb_bus[ppenum].stage_valid = 0;
        memset(cur_id_ex_bus[ppenum].snd_ins.opname, 0, 10);
        cur_id_ex_bus[ppenum].stage_valid = 0;
        memset(cur_if_id_bus[ppenum].fst_ins.opname, 0, 10);
        printf("END!\n------------------\n");
        return;
    }
    sr[ppenum][SR_STA_FLAG] |= SR_STA_FLAG_ON;
    //����д�ؼ����������
    sr_ts_count(ppenum);
    if (cur_wb_bus[ppenum].is_hb_des) //��Local Buffer��д������
    {
        hb[ppenum].head_buffer[cur_wb_bus[ppenum].waddr] = cur_wb_bus[ppenum].wdata % 256;
        hb[ppenum].head_buffer[cur_wb_bus[ppenum].waddr + 1] = cur_wb_bus[ppenum].wdata >> 8;
        printf("line:%d,ins:%s,write 0x%x(%hu %hu) into LB %d & %d\n",
               cur_wb_bus[ppenum].fth_ins.line, cur_wb_bus[ppenum].fth_ins.opname, cur_wb_bus[ppenum].wdata,
               hb[ppenum].head_buffer[cur_wb_bus[ppenum].waddr + 1], hb[ppenum].head_buffer[cur_wb_bus[ppenum].waddr],
               cur_wb_bus[ppenum].waddr + 1, cur_wb_bus[ppenum].waddr);
    }
    if (cur_wb_bus[ppenum].is_gpr_des && cur_wb_bus[ppenum].wen && cur_wb_bus[ppenum].waddr != 31)
    {
        gpr[ppenum][cur_wb_bus[ppenum].waddr] = cur_wb_bus[ppenum].wdata;
        printf("line:%d,ins:%s,write %hu into GPR %d\n",
               cur_wb_bus[ppenum].fth_ins.line, cur_wb_bus[ppenum].fth_ins.opname,
               cur_wb_bus[ppenum].wdata, cur_wb_bus[ppenum].waddr);
    }
    //�˷���32bit����
    if (cur_wb_bus[ppenum].is_gpr_des && cur_wb_bus[ppenum].wen && cur_wb_bus[ppenum].is_mul && cur_wb_bus[ppenum].waddr != 30)
    {
        gpr[ppenum][cur_wb_bus[ppenum].waddr + 1] = cur_wb_bus[ppenum].wdata2;
        printf("line%d,ins:%s,write %hu into GPR %d\n",
               cur_wb_bus[ppenum].fth_ins.line, cur_wb_bus[ppenum].fth_ins.opname,
               cur_wb_bus[ppenum].wdata2, cur_wb_bus[ppenum].waddr + 1);
    }
    if (cur_wb_bus[ppenum].is_sr_des && cur_wb_bus[ppenum].wen && cur_wb_bus[ppenum].is_hash)
    {
        sr[ppenum][SR_SAR_HASHH] = cur_wb_bus[ppenum].wdata;
        sr[ppenum][SR_SAR_HASHL] = cur_wb_bus[ppenum].wdata2;
        printf("write HASHH 0x%x,HASHL 0x%x\n", sr[ppenum][SR_SAR_HASHH], sr[ppenum][SR_SAR_HASHL]);
    }
    else if (cur_wb_bus[ppenum].is_sr_des && cur_wb_bus[ppenum].wen && cur_wb_bus[ppenum].waddr == SR_STA_FLAG)
    {
        sr[ppenum][SR_STA_FLAG] = sr[ppenum][SR_STA_FLAG] & (~cur_wb_bus[ppenum].wdata);
        printf("line%d,ins:%s,SR_STA_FLAG set to 0x%x\n",
               cur_wb_bus[ppenum].fth_ins.line, cur_wb_bus[ppenum].fth_ins.opname, sr[ppenum][SR_STA_FLAG]);
    }
    else if (cur_wb_bus[ppenum].is_sr_des && cur_wb_bus[ppenum].wen)
    {
        sr[ppenum][cur_wb_bus[ppenum].waddr] = cur_wb_bus[ppenum].wdata;
        printf("line%d,ins:%s,write %d into sr%d\n",
               cur_wb_bus[ppenum].fth_ins.line, cur_wb_bus[ppenum].fth_ins.opname,
               cur_wb_bus[ppenum].wdata, cur_wb_bus[ppenum].waddr);
    }
    // if (cur_wb_bus[ppenum].is_mr_des && cur_wb_bus[ppenum].wen)
    // {
    //     mr[cur_wb_bus[ppenum].waddr]=cur_wb_bus[ppenum].wdata;
    //     printf("write %d into mr%d\n",cur_wb_bus[ppenum].wdata,cur_wb_bus[ppenum].waddr);
    // }
    wb_stage(ppenum, cur_ex_wb_bus[ppenum]);
    //��ȷ�ϣ���������дSR�������Ϸ�дSR
    set_sr_sta_alu(ppenum, cur_ex_wb_bus[ppenum]);
    clear_sr_sta(ppenum, cur_ex_wb_bus[ppenum]);
    ex_stage(ppenum, cur_id_ex_bus[ppenum]);
    id_stage(ppenum, cur_if_id_bus[ppenum]);
    if_stage(ppenum, sr[ppenum][SR_PC] << 2);
    srpc_change(ppenum);
    event_node *current_event = check_event(ppenum, clk);
    while (current_event != NULL)
    {
        handle_event(current_event);
        free(current_event);
        current_event = check_event(ppenum, clk);
    }
    printf("------------------\n");
}

unsigned int ppe_sim::hash32(unsigned short data_in,unsigned int lfsr_q){
    unsigned int lfsr_c;
    bool lcb[32];
    /*    lfsr_c[0] = lfsr_q[16] ^ lfsr_q[22] ^ lfsr_q[25] ^ lfsr_q[26] ^ lfsr_q[28] ^ 
                      data_in[0] ^ data_in[6] ^ data_in[9] ^ data_in[10] ^ data_in[12];*/
    lcb[0]=bget(lfsr_q,16) ^ bget(lfsr_q,22) ^ bget(lfsr_q,25) ^ bget(lfsr_q,26) ^
           bget(lfsr_q,28) ^ bget(data_in,0) ^ bget(data_in,6) ^ bget(data_in,9) ^
           bget(data_in,10) ^ bget(data_in,12);
    /*    lfsr_c[1] = lfsr_q[16] ^ lfsr_q[17] ^ lfsr_q[22] ^ lfsr_q[23] ^ lfsr_q[25] ^ lfsr_q[27] ^ lfsr_q[28] ^ lfsr_q[29] ^ 
                      data_in[0] ^ data_in[1] ^ data_in[6] ^ data_in[7] ^ data_in[9] ^ data_in[11] ^ data_in[12] ^ data_in[13];*/
    lcb[1]=bget(lfsr_q,16) ^ bget(lfsr_q,17) ^ bget(lfsr_q,22) ^ bget(lfsr_q,23) ^
           bget(lfsr_q,25) ^ bget(lfsr_q,27) ^ bget(lfsr_q,28) ^ bget(lfsr_q,29) ^
           bget(data_in,0) ^ bget(data_in,1) ^ bget(data_in,6) ^ bget(data_in,7) ^ 
           bget(data_in,9) ^ bget(data_in,11) ^ bget(data_in,12) ^ bget(data_in,13);
    /*    lfsr_c[2] = lfsr_q[16] ^ lfsr_q[17] ^ lfsr_q[18] ^ lfsr_q[22] ^ lfsr_q[23] ^ lfsr_q[24] ^ lfsr_q[25] ^ lfsr_q[29] ^ lfsr_q[30] ^
                      data_in[0] ^ data_in[1] ^ data_in[2] ^ data_in[6] ^ data_in[7] ^ data_in[8] ^ data_in[9] ^ data_in[13] ^ data_in[14];*/
    lcb[2]=bget(lfsr_q,16) ^ bget(lfsr_q,17) ^ bget(lfsr_q,18) ^ bget(lfsr_q,22) ^
           bget(lfsr_q,23) ^ bget(lfsr_q,24) ^ bget(lfsr_q,25) ^ bget(lfsr_q,29) ^
           bget(lfsr_q,30) ^ bget(data_in,0) ^ bget(data_in,1) ^ bget(data_in,2) ^ 
           bget(data_in,6) ^ bget(data_in,7) ^ bget(data_in,8) ^ bget(data_in,9) ^
           bget(data_in,13) ^ bget(data_in,14);
    /*    lfsr_c[3] = lfsr_q[17] ^ lfsr_q[18] ^ lfsr_q[19] ^ lfsr_q[23] ^ lfsr_q[24] ^ lfsr_q[25] ^ lfsr_q[26] ^ lfsr_q[30] ^ lfsr_q[31] ^
                      data_in[1] ^ data_in[2] ^ data_in[3] ^ data_in[7] ^ data_in[8] ^ data_in[9] ^ data_in[10] ^ data_in[14] ^ data_in[15];*/
    lcb[3]=bget(lfsr_q,17) ^ bget(lfsr_q,18) ^ bget(lfsr_q,19) ^ bget(lfsr_q,23) ^
           bget(lfsr_q,24) ^ bget(lfsr_q,25) ^ bget(lfsr_q,26) ^ bget(lfsr_q,30) ^
           bget(lfsr_q,31) ^ bget(data_in,1) ^ bget(data_in,2) ^ bget(data_in,3) ^ 
           bget(data_in,7) ^ bget(data_in,8) ^ bget(data_in,9) ^ bget(data_in,10) ^
           bget(data_in,14) ^ bget(data_in,15);
    /*    lfsr_c[4] = lfsr_q[16] ^ lfsr_q[18] ^ lfsr_q[19] ^ lfsr_q[20] ^ lfsr_q[22] ^ lfsr_q[24] ^ lfsr_q[27] ^ lfsr_q[28] ^ lfsr_q[31] ^
                      data_in[0] ^ data_in[2] ^ data_in[3] ^ data_in[4] ^ data_in[6] ^ data_in[8] ^ data_in[11] ^ data_in[12] ^ data_in[15];*/
    lcb[4]=bget(lfsr_q,16) ^ bget(lfsr_q,18) ^ bget(lfsr_q,19) ^ bget(lfsr_q,20) ^
           bget(lfsr_q,22) ^ bget(lfsr_q,24) ^ bget(lfsr_q,27) ^ bget(lfsr_q,28) ^
           bget(lfsr_q,31) ^ bget(data_in,0) ^ bget(data_in,2) ^ bget(data_in,3) ^ 
           bget(data_in,4) ^ bget(data_in,6) ^ bget(data_in,8) ^ bget(data_in,11) ^
           bget(data_in,12) ^ bget(data_in,15);
    /*    lfsr_c[5] = lfsr_q[16] ^ lfsr_q[17] ^ lfsr_q[19] ^ lfsr_q[20] ^ lfsr_q[21] ^ lfsr_q[22] ^ lfsr_q[23] ^ lfsr_q[26] ^ lfsr_q[29] ^
                      data_in[0] ^ data_in[1] ^ data_in[3] ^ data_in[4] ^ data_in[5] ^ data_in[6] ^ data_in[7] ^ data_in[10] ^ data_in[13];*/
    lcb[5]=bget(lfsr_q,16) ^ bget(lfsr_q,17) ^ bget(lfsr_q,19) ^ bget(lfsr_q,20) ^
           bget(lfsr_q,21) ^ bget(lfsr_q,22) ^ bget(lfsr_q,23) ^ bget(lfsr_q,26) ^
           bget(lfsr_q,29) ^ bget(data_in,0) ^ bget(data_in,1) ^ bget(data_in,3) ^ 
           bget(data_in,4) ^ bget(data_in,5) ^ bget(data_in,6) ^ bget(data_in,7) ^
           bget(data_in,10) ^ bget(data_in,13);
    /*    lfsr_c[6] = lfsr_q[17] ^ lfsr_q[18] ^ lfsr_q[20] ^ lfsr_q[21] ^ lfsr_q[22] ^ lfsr_q[23] ^ lfsr_q[24] ^ lfsr_q[27] ^ lfsr_q[30] ^ 
                      data_in[1] ^ data_in[2] ^ data_in[4] ^ data_in[5] ^ data_in[6] ^ data_in[7] ^ data_in[8] ^ data_in[11] ^ data_in[14];*/
    lcb[6]=bget(lfsr_q,17) ^ bget(lfsr_q,18) ^ bget(lfsr_q,20) ^ bget(lfsr_q,21) ^
           bget(lfsr_q,22) ^ bget(lfsr_q,23) ^ bget(lfsr_q,24) ^ bget(lfsr_q,27) ^
           bget(lfsr_q,30) ^ bget(data_in,1) ^ bget(data_in,2) ^ bget(data_in,4) ^ 
           bget(data_in,5) ^ bget(data_in,6) ^ bget(data_in,7) ^ bget(data_in,8) ^
           bget(data_in,11) ^ bget(data_in,14);
    /*lfsr_c[7] = lfsr_q[16] ^ lfsr_q[18] ^ lfsr_q[19] ^ lfsr_q[21] ^ lfsr_q[23] ^ lfsr_q[24] ^ lfsr_q[26] ^ lfsr_q[31] ^
                  data_in[0] ^ data_in[2] ^ data_in[3] ^ data_in[5] ^ data_in[7] ^ data_in[8] ^ data_in[10] ^ data_in[15];*/
    lcb[7]=bget(lfsr_q,16) ^ bget(lfsr_q,18) ^ bget(lfsr_q,19) ^ bget(lfsr_q,21) ^
           bget(lfsr_q,23) ^ bget(lfsr_q,24) ^ bget(lfsr_q,26) ^ bget(lfsr_q,31) ^
           bget(data_in,0) ^ bget(data_in,2) ^ bget(data_in,3) ^ bget(data_in,5) ^ 
           bget(data_in,7) ^ bget(data_in,8) ^ bget(data_in,10) ^ bget(data_in,15);
    /*lfsr_c[8] = lfsr_q[16] ^ lfsr_q[17] ^ lfsr_q[19] ^ lfsr_q[20] ^ lfsr_q[24] ^ lfsr_q[26] ^ lfsr_q[27] ^ lfsr_q[28] ^
                  data_in[0] ^ data_in[1] ^ data_in[3] ^ data_in[4] ^ data_in[8] ^ data_in[10] ^ data_in[11] ^ data_in[12];*/
    lcb[8]=bget(lfsr_q,16) ^ bget(lfsr_q,17) ^ bget(lfsr_q,19) ^ bget(lfsr_q,20) ^
           bget(lfsr_q,24) ^ bget(lfsr_q,26) ^ bget(lfsr_q,27) ^ bget(lfsr_q,28) ^
           bget(data_in,0) ^ bget(data_in,1) ^ bget(data_in,3) ^ bget(data_in,4) ^ 
           bget(data_in,8) ^ bget(data_in,10) ^ bget(data_in,11) ^ bget(data_in,12);
    /*lfsr_c[9] = lfsr_q[17] ^ lfsr_q[18] ^ lfsr_q[20] ^ lfsr_q[21] ^ lfsr_q[25] ^ lfsr_q[27] ^ lfsr_q[28] ^ lfsr_q[29] ^
                  data_in[1] ^ data_in[2] ^ data_in[4] ^ data_in[5] ^ data_in[9] ^ data_in[11] ^ data_in[12] ^ data_in[13];*/
    lcb[9]=bget(lfsr_q,17) ^ bget(lfsr_q,18) ^ bget(lfsr_q,20) ^ bget(lfsr_q,21) ^
           bget(lfsr_q,25) ^ bget(lfsr_q,27) ^ bget(lfsr_q,28) ^ bget(lfsr_q,29) ^
           bget(data_in,1) ^ bget(data_in,2) ^ bget(data_in,4) ^ bget(data_in,5) ^ 
           bget(data_in,9) ^ bget(data_in,11) ^ bget(data_in,12) ^ bget(data_in,13);
    /*lfsr_c[10] = lfsr_q[16] ^ lfsr_q[18] ^ lfsr_q[19] ^ lfsr_q[21] ^ lfsr_q[25] ^ lfsr_q[29] ^ lfsr_q[30] ^
                   data_in[0] ^ data_in[2] ^ data_in[3] ^ data_in[5] ^ data_in[9] ^ data_in[13] ^ data_in[14];*/
    lcb[10]=bget(lfsr_q,16) ^ bget(lfsr_q,18) ^ bget(lfsr_q,19) ^ bget(lfsr_q,21) ^
            bget(lfsr_q,25) ^ bget(lfsr_q,29) ^ bget(lfsr_q,30) ^ bget(data_in,0) ^
            bget(data_in,2) ^ bget(data_in,3) ^ bget(data_in,5) ^ bget(data_in,9) ^ 
            bget(data_in,13) ^ bget(data_in,14);
    /*lfsr_c[11] = lfsr_q[16] ^ lfsr_q[17] ^ lfsr_q[19] ^ lfsr_q[20] ^ lfsr_q[25] ^ lfsr_q[28] ^ lfsr_q[30] ^ lfsr_q[31] ^ 
                   data_in[0] ^ data_in[1] ^ data_in[3] ^ data_in[4] ^ data_in[9] ^ data_in[12] ^ data_in[14] ^ data_in[15];*/
    lcb[11]=bget(lfsr_q,16) ^ bget(lfsr_q,17) ^ bget(lfsr_q,19) ^ bget(lfsr_q,20) ^
            bget(lfsr_q,25) ^ bget(lfsr_q,28) ^ bget(lfsr_q,30) ^ bget(lfsr_q,31) ^
            bget(data_in,0) ^ bget(data_in,1) ^ bget(data_in,3) ^ bget(data_in,4) ^ 
            bget(data_in,9) ^ bget(data_in,12) ^ bget(data_in,14) ^ bget(data_in,15);
    /*lfsr_c[12] = lfsr_q[16] ^ lfsr_q[17] ^ lfsr_q[18] ^ lfsr_q[20] ^ lfsr_q[21] ^ lfsr_q[22] ^ lfsr_q[25] ^ lfsr_q[28] ^ lfsr_q[29] ^ lfsr_q[31] ^
                   data_in[0] ^ data_in[1] ^ data_in[2] ^ data_in[4] ^ data_in[5] ^ data_in[6] ^ data_in[9] ^ data_in[12] ^ data_in[13] ^ data_in[15];*/
    lcb[12]=bget(lfsr_q,16) ^ bget(lfsr_q,17) ^ bget(lfsr_q,18) ^ bget(lfsr_q,20) ^
            bget(lfsr_q,21) ^ bget(lfsr_q,22) ^ bget(lfsr_q,25) ^ bget(lfsr_q,28) ^
            bget(lfsr_q,29) ^ bget(lfsr_q,31) ^ bget(data_in,0) ^ bget(data_in,1) ^ 
            bget(data_in,2) ^ bget(data_in,4) ^ bget(data_in,5) ^ bget(data_in,6) ^
            bget(data_in,9) ^ bget(data_in,12) ^ bget(data_in,13) ^ bget(data_in,15);
    /*lfsr_c[13] = lfsr_q[17] ^ lfsr_q[18] ^ lfsr_q[19] ^ lfsr_q[21] ^ lfsr_q[22] ^ lfsr_q[23] ^ lfsr_q[26] ^ lfsr_q[29] ^ lfsr_q[30] ^
                   data_in[1] ^ data_in[2] ^ data_in[3] ^ data_in[5] ^ data_in[6] ^ data_in[7] ^ data_in[10] ^ data_in[13] ^ data_in[14];*/
    lcb[13]=bget(lfsr_q,17) ^ bget(lfsr_q,18) ^ bget(lfsr_q,19) ^ bget(lfsr_q,21) ^
            bget(lfsr_q,22) ^ bget(lfsr_q,23) ^ bget(lfsr_q,26) ^ bget(lfsr_q,29) ^
            bget(lfsr_q,30) ^ bget(data_in,1) ^ bget(data_in,2) ^ bget(data_in,3) ^ 
            bget(data_in,5) ^ bget(data_in,6) ^ bget(data_in,7) ^ bget(data_in,10) ^
            bget(data_in,13) ^ bget(data_in,14);
    /*lfsr_c[14] = lfsr_q[18] ^ lfsr_q[19] ^ lfsr_q[20] ^ lfsr_q[22] ^ lfsr_q[23] ^ lfsr_q[24] ^ lfsr_q[27] ^ lfsr_q[30] ^ lfsr_q[31] ^
                   data_in[2] ^ data_in[3] ^ data_in[4] ^ data_in[6] ^ data_in[7] ^ data_in[8] ^ data_in[11] ^ data_in[14] ^ data_in[15];*/
    lcb[14]=bget(lfsr_q,18) ^ bget(lfsr_q,19) ^ bget(lfsr_q,20) ^ bget(lfsr_q,22) ^
            bget(lfsr_q,23) ^ bget(lfsr_q,24) ^ bget(lfsr_q,27) ^ bget(lfsr_q,30) ^
            bget(lfsr_q,31) ^ bget(data_in,2) ^ bget(data_in,3) ^ bget(data_in,4) ^ 
            bget(data_in,6) ^ bget(data_in,7) ^ bget(data_in,8) ^ bget(data_in,11) ^
            bget(data_in,14) ^ bget(data_in,15);
    /*lfsr_c[15] = lfsr_q[19] ^ lfsr_q[20] ^ lfsr_q[21] ^ lfsr_q[23] ^ lfsr_q[24] ^ lfsr_q[25] ^ lfsr_q[28] ^ lfsr_q[31] ^
                   data_in[3] ^ data_in[4] ^ data_in[5] ^ data_in[7] ^ data_in[8] ^ data_in[9] ^ data_in[12] ^ data_in[15];*/
    lcb[15]=bget(lfsr_q,19) ^ bget(lfsr_q,20) ^ bget(lfsr_q,21) ^ bget(lfsr_q,23) ^
            bget(lfsr_q,24) ^ bget(lfsr_q,25) ^ bget(lfsr_q,28) ^ bget(lfsr_q,31) ^
            bget(data_in,3) ^ bget(data_in,4) ^ bget(data_in,5) ^ bget(data_in,7) ^ 
            bget(data_in,8) ^ bget(data_in,9) ^ bget(data_in,12) ^ bget(data_in,15);
    /*lfsr_c[16] = lfsr_q[0] ^ lfsr_q[16] ^ lfsr_q[20] ^ lfsr_q[21] ^ lfsr_q[24] ^ lfsr_q[28] ^ lfsr_q[29] ^ 
                      data_in[0] ^ data_in[4] ^ data_in[5] ^ data_in[8] ^ data_in[12] ^ data_in[13];*/
    lcb[16]=bget(lfsr_q,0) ^ bget(lfsr_q,16) ^ bget(lfsr_q,20) ^ bget(lfsr_q,21) ^
            bget(lfsr_q,24) ^ bget(lfsr_q,28) ^ bget(lfsr_q,29) ^ bget(data_in,0) ^
            bget(data_in,4) ^ bget(data_in,5) ^ bget(data_in,8) ^ bget(data_in,12) ^ 
            bget(data_in,13);
    /*lfsr_c[17] = lfsr_q[1] ^ lfsr_q[17] ^ lfsr_q[21] ^ lfsr_q[22] ^ lfsr_q[25] ^ lfsr_q[29] ^ lfsr_q[30] ^
                   data_in[1] ^ data_in[5] ^ data_in[6] ^ data_in[9] ^ data_in[13] ^ data_in[14];*/
    lcb[17]=bget(lfsr_q,1) ^ bget(lfsr_q,17) ^ bget(lfsr_q,21) ^ bget(lfsr_q,22) ^
            bget(lfsr_q,25) ^ bget(lfsr_q,29) ^ bget(lfsr_q,30) ^ bget(data_in,1) ^
            bget(data_in,5) ^ bget(data_in,6) ^ bget(data_in,9) ^ bget(data_in,13) ^ 
            bget(data_in,14);
    /*lfsr_c[18] = lfsr_q[2] ^ lfsr_q[18] ^ lfsr_q[22] ^ lfsr_q[23] ^ lfsr_q[26] ^ lfsr_q[30] ^ lfsr_q[31] ^
                   data_in[2] ^ data_in[6] ^ data_in[7] ^ data_in[10] ^ data_in[14] ^ data_in[15];*/
    lcb[18]=bget(lfsr_q,2) ^ bget(lfsr_q,18) ^ bget(lfsr_q,22) ^ bget(lfsr_q,23) ^
            bget(lfsr_q,26) ^ bget(lfsr_q,30) ^ bget(lfsr_q,31) ^ bget(data_in,2) ^
            bget(data_in,6) ^ bget(data_in,7) ^ bget(data_in,10) ^ bget(data_in,14) ^ 
            bget(data_in,15);
    /*lfsr_c[19] = lfsr_q[3] ^ lfsr_q[19] ^ lfsr_q[23] ^ lfsr_q[24] ^ lfsr_q[27] ^ lfsr_q[31] ^
                   data_in[3] ^ data_in[7] ^ data_in[8] ^ data_in[11] ^ data_in[15];*/
    lcb[19]=bget(lfsr_q,3) ^ bget(lfsr_q,19) ^ bget(lfsr_q,23) ^ bget(lfsr_q,24) ^
            bget(lfsr_q,27) ^ bget(lfsr_q,31) ^ bget(data_in,3) ^ bget(data_in,7) ^
            bget(data_in,8) ^ bget(data_in,11) ^ bget(data_in,15);
    /*lfsr_c[20] = lfsr_q[4] ^ lfsr_q[20] ^ lfsr_q[24] ^ lfsr_q[25] ^ lfsr_q[28] ^
                   data_in[4] ^ data_in[8] ^ data_in[9] ^ data_in[12];*/
    lcb[20]=bget(lfsr_q,4) ^ bget(lfsr_q,20) ^ bget(lfsr_q,24) ^ bget(lfsr_q,25) ^
            bget(lfsr_q,28) ^ bget(data_in,4) ^ bget(data_in,8) ^ bget(data_in,9) ^
            bget(data_in,12);
    /*lfsr_c[21] = lfsr_q[5] ^ lfsr_q[21] ^ lfsr_q[25] ^ lfsr_q[26] ^ lfsr_q[29] ^
                   data_in[5] ^ data_in[9] ^ data_in[10] ^ data_in[13];*/
    lcb[21]=bget(lfsr_q,5) ^ bget(lfsr_q,21) ^ bget(lfsr_q,25) ^ bget(lfsr_q,26) ^
            bget(lfsr_q,29) ^ bget(data_in,5) ^ bget(data_in,9) ^ bget(data_in,10) ^
            bget(data_in,13);
    /*lfsr_c[22] = lfsr_q[6] ^ lfsr_q[16] ^ lfsr_q[25] ^ lfsr_q[27] ^ lfsr_q[28] ^ lfsr_q[30] ^
                      data_in[0] ^ data_in[9] ^ data_in[11] ^ data_in[12] ^ data_in[14];*/
    lcb[22]=bget(lfsr_q,6) ^ bget(lfsr_q,16) ^ bget(lfsr_q,25) ^ bget(lfsr_q,27) ^
            bget(lfsr_q,28) ^ bget(lfsr_q,30) ^ bget(data_in,0) ^ bget(data_in,9) ^
            bget(data_in,11) ^ bget(data_in,12) ^ bget(data_in,14);
    /*lfsr_c[23] = lfsr_q[7] ^ lfsr_q[16] ^ lfsr_q[17] ^ lfsr_q[22] ^ lfsr_q[25] ^ lfsr_q[29] ^ lfsr_q[31] ^
                   data_in[0] ^ data_in[1] ^ data_in[6] ^ data_in[9] ^ data_in[13] ^ data_in[15];*/
    lcb[23]=bget(lfsr_q,7) ^ bget(lfsr_q,16) ^ bget(lfsr_q,17) ^ bget(lfsr_q,22) ^
            bget(lfsr_q,25) ^ bget(lfsr_q,29) ^ bget(lfsr_q,31) ^ bget(data_in,0) ^
            bget(data_in,1) ^ bget(data_in,6) ^ bget(data_in,9) ^ bget(data_in,13) ^
            bget(data_in,15);
    /*lfsr_c[24] = lfsr_q[8] ^ lfsr_q[17] ^ lfsr_q[18] ^ lfsr_q[23] ^ lfsr_q[26] ^ lfsr_q[30] ^
                   data_in[1] ^ data_in[2] ^ data_in[7] ^ data_in[10] ^ data_in[14];*/
    lcb[24]=bget(lfsr_q,8) ^ bget(lfsr_q,17) ^ bget(lfsr_q,18) ^ bget(lfsr_q,23) ^
            bget(lfsr_q,26) ^ bget(lfsr_q,30) ^ bget(data_in,1) ^ bget(data_in,2) ^
            bget(data_in,7) ^ bget(data_in,10) ^ bget(data_in,14);
    /*lfsr_c[25] = lfsr_q[9] ^ lfsr_q[18] ^ lfsr_q[19] ^ lfsr_q[24] ^ lfsr_q[27] ^ lfsr_q[31] ^
                   data_in[2] ^ data_in[3] ^ data_in[8] ^ data_in[11] ^ data_in[15];*/
    lcb[25]=bget(lfsr_q,9) ^ bget(lfsr_q,18) ^ bget(lfsr_q,19) ^ bget(lfsr_q,24) ^
            bget(lfsr_q,27) ^ bget(lfsr_q,31) ^ bget(data_in,2) ^ bget(data_in,3) ^
            bget(data_in,8) ^ bget(data_in,11) ^ bget(data_in,15);
    /*lfsr_c[26] = lfsr_q[10] ^ lfsr_q[16] ^ lfsr_q[19] ^ lfsr_q[20] ^ lfsr_q[22] ^ lfsr_q[26] ^
                   data_in[0] ^ data_in[3] ^ data_in[4] ^ data_in[6] ^ data_in[10];*/
    lcb[26]=bget(lfsr_q,10) ^ bget(lfsr_q,16) ^ bget(lfsr_q,19) ^ bget(lfsr_q,20) ^
            bget(lfsr_q,22) ^ bget(lfsr_q,26) ^ bget(data_in,0) ^ bget(data_in,3) ^
            bget(data_in,4) ^ bget(data_in,6) ^ bget(data_in,10);
    /*lfsr_c[27] = lfsr_q[11] ^ lfsr_q[17] ^ lfsr_q[20] ^ lfsr_q[21] ^ lfsr_q[23] ^ lfsr_q[27] ^
                      data_in[1] ^ data_in[4] ^ data_in[5] ^ data_in[7] ^ data_in[11];*/
    lcb[27]=bget(lfsr_q,11) ^ bget(lfsr_q,17) ^ bget(lfsr_q,20) ^ bget(lfsr_q,21) ^
            bget(lfsr_q,23) ^ bget(lfsr_q,27) ^ bget(data_in,1) ^ bget(data_in,4) ^
            bget(data_in,5) ^ bget(data_in,7) ^ bget(data_in,11);
    /*lfsr_c[28] = lfsr_q[12] ^ lfsr_q[18] ^ lfsr_q[21] ^ lfsr_q[22] ^ lfsr_q[24] ^ lfsr_q[28] ^
                      data_in[2] ^ data_in[5] ^ data_in[6] ^ data_in[8] ^ data_in[12];*/
    lcb[28]=bget(lfsr_q,12) ^ bget(lfsr_q,18) ^ bget(lfsr_q,21) ^ bget(lfsr_q,22) ^
            bget(lfsr_q,24) ^ bget(lfsr_q,28) ^ bget(data_in,2) ^ bget(data_in,5) ^
            bget(data_in,6) ^ bget(data_in,8) ^ bget(data_in,12);
    /*lfsr_c[29] = lfsr_q[13] ^ lfsr_q[19] ^ lfsr_q[22] ^ lfsr_q[23] ^ lfsr_q[25] ^ lfsr_q[29] ^
                      data_in[3] ^ data_in[6] ^ data_in[7] ^ data_in[9] ^ data_in[13];*/
    lcb[29]=bget(lfsr_q,13) ^ bget(lfsr_q,19) ^ bget(lfsr_q,22) ^ bget(lfsr_q,23) ^
            bget(lfsr_q,25) ^ bget(lfsr_q,29) ^ bget(data_in,3) ^ bget(data_in,6) ^
            bget(data_in,7) ^ bget(data_in,9) ^ bget(data_in,13);
    /*lfsr_c[30] = lfsr_q[14] ^ lfsr_q[20] ^ lfsr_q[23] ^ lfsr_q[24] ^ lfsr_q[26] ^ lfsr_q[30] ^
                      data_in[4] ^ data_in[7] ^ data_in[8] ^ data_in[10] ^ data_in[14];*/
    lcb[30]=bget(lfsr_q,14) ^ bget(lfsr_q,20) ^ bget(lfsr_q,23) ^ bget(lfsr_q,24) ^
            bget(lfsr_q,26) ^ bget(lfsr_q,30) ^ bget(data_in,4) ^ bget(data_in,7) ^
            bget(data_in,8) ^ bget(data_in,10) ^ bget(data_in,14);
    /*lfsr_c[31] = lfsr_q[15] ^ lfsr_q[21] ^ lfsr_q[24] ^ lfsr_q[25] ^ lfsr_q[27] ^ lfsr_q[31] ^
                      data_in[5] ^ data_in[8] ^ data_in[9] ^ data_in[11] ^ data_in[15];*/
    lcb[31]=bget(lfsr_q,15) ^ bget(lfsr_q,21) ^ bget(lfsr_q,24) ^ bget(lfsr_q,25) ^
            bget(lfsr_q,27) ^ bget(lfsr_q,31) ^ bget(data_in,5) ^ bget(data_in,8) ^
            bget(data_in,9) ^ bget(data_in,11) ^ bget(data_in,15);
    //merge lcb into lfsr_c;
    for (int i=0;i<32;i++)
    {
        lfsr_c+=(lcb[i]<<i);
    }
    printf("HASH Result: 0x%x\n",lfsr_c);
    return lfsr_c;
}
void ppe_sim::wb_stage(int ppn,ex_wb_bus bus){
    printf("WB:PC=%d, Line %d:%s %d %d %d %d\n",bus.pc,
    bus.trd_ins.line,bus.trd_ins.opname,bus.trd_ins.rd,bus.trd_ins.rs1,bus.trd_ins.rs2,bus.trd_ins.imm);
    //ADD GPR0��GPR1��GPR2    30
    //���߼Ĵ�����Ҫ��0�żĴ���д30
    if (bus.stage_valid)
    {
        cur_wb_bus[ppn].pc=bus.pc;
        cur_wb_bus[ppn].fth_ins=bus.trd_ins;
        cur_wb_bus[ppn].wen=1;
        cur_wb_bus[ppn].is_gpr_des=0;
        cur_wb_bus[ppn].is_sr_des=0;
        cur_wb_bus[ppn].is_mr_des=0;
        cur_wb_bus[ppn].is_hb_des=0;
        cur_wb_bus[ppn].is_hash=0;
        cur_wb_bus[ppn].is_mul=cur_ex_wb_bus[ppn].is_mul;
        if (bus.is_gpr_des==1 && !bus.is_read_hb)
        {
            cur_wb_bus[ppn].is_gpr_des=1;
            cur_wb_bus[ppn].waddr=bus.trd_ins.rd;
            cur_wb_bus[ppn].wdata=bus.ex_result;
            cur_wb_bus[ppn].wdata2=bus.ex_result2;
            // printf("ADDI: %d\n",cur_ex_wb_bus[ppn].ex_result);
        }
        //khh added: for MOVBRLE MOVBRBE
        else if (bus.is_gpr_des && !bus.is_be)
        {
            cur_wb_bus[ppn].is_gpr_des=1;
            cur_wb_bus[ppn].waddr=bus.trd_ins.rd;
            cur_wb_bus[ppn].wdata=(hb[ppn].head_buffer[bus.ex_result])+(hb[ppn].head_buffer[bus.ex_result+1]<<8);
        }
        else if (bus.is_gpr_des)
        {
            cur_wb_bus[ppn].is_gpr_des=1;
            cur_wb_bus[ppn].waddr=bus.trd_ins.rd;
            cur_wb_bus[ppn].wdata=(hb[ppn].head_buffer[bus.ex_result]<<8)+(hb[ppn].head_buffer[bus.ex_result+1]);
        }
        // else if (bus.is_gpr_des ) // else if (bus.is_gpr_des && !bus.is_be)
        // {
        //     cur_wb_bus[ppn].is_gpr_des=1;
        //     cur_wb_bus[ppn].waddr=bus.trd_ins.rd;
        //     cur_wb_bus[ppn].wdata=(hb[ppn].head_buffer[bus.ex_result])+(hb[ppn].head_buffer[bus.ex_result+1]<<8);
        // }
        //else if (bus.is_gpr_des)
        //{
            // cur_wb_bus[ppn].is_gpr_des=1;
            // cur_wb_bus[ppn].waddr=bus.trd_ins.rd;
            // cur_wb_bus[ppn].wdata=(hb[ppn].head_buffer[bus.ex_result]<<8)+(hb[ppn].head_buffer[bus.ex_result+1]);
        //}
        else if (bus.is_sr_des==1 && !bus.is_read_hb)
        {
            cur_wb_bus[ppn].is_sr_des=1;
            cur_wb_bus[ppn].waddr=bus.trd_ins.rd;
            cur_wb_bus[ppn].wdata=bus.ex_result;
        }
        //merge wzy: cksumle,cksumbe
        else if (strcmp(cur_wb_bus[ppn].fth_ins.opname,"CKSUMLE")==0)
        {
            cur_wb_bus[ppn].is_sr_des=1;
            cur_wb_bus[ppn].waddr=SR_SAR_CKS;
            cur_wb_bus[ppn].wdata=((hb[ppn].head_buffer[bus.hb_addr])+(hb[ppn].head_buffer[bus.hb_addr+1]<<8))
                                    +bus.ex_result+cur_wb_bus[ppn].is_sr_cy;
            printf("SR_SAR_CKS:0x%x,hb_high:0x%x,hb_low:0x%x,CY:%d\n",bus.ex_result,
                    hb[ppn].head_buffer[bus.hb_addr+1],hb[ppn].head_buffer[bus.hb_addr],cur_wb_bus[ppn].is_sr_cy);
            if ((cur_wb_bus[ppn].wdata<bus.ex_result || (cur_wb_bus[ppn].wdata==bus.ex_result && cur_wb_bus[ppn].is_sr_cy))
                && cur_ex_wb_bus[ppn].is_mhm>0)
                cur_wb_bus[ppn].is_sr_cy=1;
            else if (cur_wb_bus[ppn].wdata<bus.ex_result || (cur_wb_bus[ppn].wdata==bus.ex_result && cur_wb_bus[ppn].is_sr_cy))
            {
                cur_wb_bus[ppn].is_sr_cy=0;
                cur_wb_bus[ppn].wdata++;
            }
            else
                cur_wb_bus[ppn].is_sr_cy=0;
            if (cur_ex_wb_bus[ppn].is_mhm==0)
                cur_wb_bus[ppn].is_sr_cy=0;
            printf("Result:0x%x,CY:%d\n",cur_wb_bus[ppn].wdata,cur_wb_bus[ppn].is_sr_cy);
        }
        else if (strcmp(cur_wb_bus[ppn].fth_ins.opname,"CKSUMBE")==0)
        {
            cur_wb_bus[ppn].is_sr_des=1;
            cur_wb_bus[ppn].waddr=SR_SAR_CKS;
            cur_wb_bus[ppn].wdata=((hb[ppn].head_buffer[bus.hb_addr]<<8)+(hb[ppn].head_buffer[bus.hb_addr+1]))
                                    +bus.ex_result+cur_wb_bus[ppn].is_sr_cy;
            printf("SR_SAR_CKS:0x%x,hb_high:0x%x,hb_low:0x%x,CY:%d\n",bus.ex_result,
                    hb[ppn].head_buffer[bus.hb_addr+1],hb[ppn].head_buffer[bus.hb_addr],cur_wb_bus[ppn].is_sr_cy);
            if ((cur_wb_bus[ppn].wdata<bus.ex_result || (cur_wb_bus[ppn].wdata==bus.ex_result && cur_wb_bus[ppn].is_sr_cy))
                && cur_ex_wb_bus[ppn].is_mhm>0)
                cur_wb_bus[ppn].is_sr_cy=1;
            else if (cur_wb_bus[ppn].wdata<bus.ex_result || (cur_wb_bus[ppn].wdata==bus.ex_result && cur_wb_bus[ppn].is_sr_cy))
            {
                cur_wb_bus[ppn].is_sr_cy=0;
                cur_wb_bus[ppn].wdata++;
            }
            else
                cur_wb_bus[ppn].is_sr_cy=0;
            if (cur_ex_wb_bus[ppn].is_mhm==0)
                cur_wb_bus[ppn].is_sr_cy=0;
            printf("Result:0x%x,CY:%d\n",cur_wb_bus[ppn].wdata,cur_wb_bus[ppn].is_sr_cy);
        }
        // else if (strcmp(cur_wb_bus[ppn].fth_ins.opname,"CKSUM")==0)
        // {
        //     cur_wb_bus[ppn].is_sr_des=1;
        //     cur_wb_bus[ppn].waddr=SR_SAR_CKS;
        //     cur_wb_bus[ppn].wdata=((hb[ppn].head_buffer[bus.hb_addr])+(hb[ppn].head_buffer[bus.hb_addr+1]<<8))
        //                             +bus.ex_result+cur_wb_bus[ppn].is_sr_cy;
        //     printf("SR_SAR_CKS:0x%x,hb_high:0x%x,hb_low:0x%x,CY:%d\n",bus.ex_result,
        //             hb[ppn].head_buffer[bus.hb_addr+1],hb[ppn].head_buffer[bus.hb_addr],cur_wb_bus[ppn].is_sr_cy);
        //     if ((cur_wb_bus[ppn].wdata<bus.ex_result || (cur_wb_bus[ppn].wdata==bus.ex_result && cur_wb_bus[ppn].is_sr_cy))
        //         && cur_ex_wb_bus[ppn].is_mhm>0)
        //         cur_wb_bus[ppn].is_sr_cy=1;
        //     else if (cur_wb_bus[ppn].wdata<bus.ex_result || (cur_wb_bus[ppn].wdata==bus.ex_result && cur_wb_bus[ppn].is_sr_cy))
        //     {
        //         cur_wb_bus[ppn].is_sr_cy=0;
        //         cur_wb_bus[ppn].wdata++;
        //     }
        //     else
        //         cur_wb_bus[ppn].is_sr_cy=0;
        //     if (cur_ex_wb_bus[ppn].is_mhm==0)
        //         cur_wb_bus[ppn].is_sr_cy=0;
        //     printf("Result:0x%x,CY:%d\n",cur_wb_bus[ppn].wdata,cur_wb_bus[ppn].is_sr_cy);
        // }
        //khh added:HASHLE,HASHBE
        else if (strcmp(cur_wb_bus[ppn].fth_ins.opname,"HASHLE")==0)
        {
            cur_wb_bus[ppn].is_sr_des=1;
            cur_wb_bus[ppn].is_hash=1;
            cur_wb_bus[ppn].waddr=SR_SAR_HASHH;
            unsigned short data_in=(hb[ppn].head_buffer[bus.hb_addr])+(hb[ppn].head_buffer[bus.hb_addr+1]<<8);
            unsigned int lfsr_q=(cur_ex_wb_bus[ppn].ex_result<<16)+(cur_ex_wb_bus[ppn].ex_result2);
            printf("HASH data_in: 0x%x, crc_last: 0x%x\n",data_in,lfsr_q);
            unsigned int crc_out=hash32(data_in,lfsr_q);
            cur_wb_bus[ppn].wdata=crc_out>>16;
            cur_wb_bus[ppn].wdata2=crc_out&0x0000ffff;
        }
        else if (strcmp(cur_wb_bus[ppn].fth_ins.opname,"HASHBE")==0)
        {
            cur_wb_bus[ppn].is_sr_des=1;
            cur_wb_bus[ppn].is_hash=1;
            cur_wb_bus[ppn].waddr=SR_SAR_HASHH;
            unsigned short data_in=(hb[ppn].head_buffer[bus.hb_addr]<<8)+(hb[ppn].head_buffer[bus.hb_addr+1]);
            unsigned int lfsr_q=(cur_ex_wb_bus[ppn].ex_result<<16)+(cur_ex_wb_bus[ppn].ex_result2);
            printf("HASH data_in: 0x%x, crc_last: 0x%x\n",data_in,lfsr_q);
            unsigned int crc_out=hash32(data_in,lfsr_q);
            cur_wb_bus[ppn].wdata=crc_out>>16;
            cur_wb_bus[ppn].wdata2=crc_out&0x0000ffff;
        }
        // else if (strcmp(cur_wb_bus[ppn].fth_ins.opname,"HASH")==0)
        // {
        //     cur_wb_bus[ppn].is_sr_des=1;
        //     cur_wb_bus[ppn].is_hash=1;
        //     cur_wb_bus[ppn].waddr=SR_SAR_HASHH;
        //     unsigned short data_in=(hb[ppn].head_buffer[bus.hb_addr])+(hb[ppn].head_buffer[bus.hb_addr+1]<<8);
        //     unsigned int lfsr_q=(cur_ex_wb_bus[ppn].ex_result<<16)+(cur_ex_wb_bus[ppn].ex_result2);
        //     printf("HASH data_in: 0x%x, crc_last: 0x%x\n",data_in,lfsr_q);
        //     unsigned int crc_out=hash32(data_in,lfsr_q);
        //     cur_wb_bus[ppn].wdata=crc_out>>16;
        //     cur_wb_bus[ppn].wdata2=crc_out&0x0000ffff;
        // }
        else if (bus.is_mr_des==1 && bus.is_read_hb)
        {
            cur_wb_bus[ppn].is_mr_des=1;
            cur_wb_bus[ppn].waddr=bus.trd_ins.rd;
            cur_wb_bus[ppn].wdata=(hb[ppn].head_buffer[bus.ex_result])+(hb[ppn].head_buffer[bus.ex_result+1]<<8);
        }
        else if (bus.is_read_hb && bus.is_hb_des)
        {
            cur_wb_bus[ppn].is_hb_des=1;
            cur_wb_bus[ppn].waddr=bus.hb_addr;
            cur_wb_bus[ppn].wdata=(hb[ppn].head_buffer[bus.ex_result])+(hb[ppn].head_buffer[bus.ex_result+1]<<8);
        }
        else if (bus.is_hb_des==1)
        {
            cur_wb_bus[ppn].is_hb_des=1;
            cur_wb_bus[ppn].waddr=bus.hb_addr;
            cur_wb_bus[ppn].wdata=bus.ex_result;
        }
        else
        {
            cur_wb_bus[ppn].wen=0;
            cur_wb_bus[ppn].is_gpr_des=0;
        }
    }
    else
    {
        cur_wb_bus[ppn].wen=0;
        cur_wb_bus[ppn].is_gpr_des=0;
        cur_wb_bus[ppn].is_sr_des=0;
    }
}

ppeInit::event_node* ppe_sim::check_event(int ppn,long clk)
{
    event_node *token=event_list;
    event_node *previous=token;
    while (token!=NULL)
    {
        if (token->trigger_clk==clk && token->core_id==ppn)
        {
            if (token==event_list)
            {
                event_list=event_list->next;
            }
            else
            {
                previous->next=token->next;
            }
            token->next=NULL;
            return token;
        }
        else
        {
            previous=token;
            token=token->next;
        }     
    }
    return token;
}


void ppe_sim::handle_event(event_node *event)
{
    if (event->event_type==EVENT_LOADM)
    {
        int valid=1;
        for (int i=event->event_para1;i<event->event_para1+event->event_para2;i++)
        {
            int tlb_loc=512+256+(i/256)*2;
            int tlb_valid=hb[event->core_id].head_buffer[tlb_loc+1]>>7;
            if (!tlb_valid){
                valid=0;
                printf("Error SMB_valid in PB[%d]\n",tlb_loc+1);
                break;
            }
        }
        if (valid) 
        {
            
            event_node *load_event=(event_node *)malloc(sizeof(event_node));
            load_event->core_id=event->core_id;
            load_event->event_type=EVENT_LOADMS2;
            load_event->event_para1=event->event_para1;
            load_event->event_para2=event->event_para2;
            load_event->event_para3=event->event_para3;
            load_event->event_para4=event->event_para4;
            load_event->trigger_clk=event->trigger_clk+LOAD_CYCLE(event->event_para2);
            printf("SMB is correct! LOADM finished in %ld\n",load_event->trigger_clk);
            load_event->next=NULL;
            event_node *cur=event_list;
            if (cur==NULL)
                event_list=load_event;
            else
            {
                while (cur->next!=NULL)
                {
                    cur=cur->next;
                }
                cur->next=load_event;
            }
        }
        else
        {
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_HBLC;
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
            sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_HBLE;
        }
    }
    else if (event->event_type==EVENT_LOADMS2)
    {
        int j=event->event_para3;
        for (int i=event->event_para1;i<event->event_para1+event->event_para2;i++)
        {
            int tlb_loc=512+256+(i/256)*2;
            int tlb_index=hb[event->core_id].head_buffer[tlb_loc]+
            ((hb[event->core_id].head_buffer[tlb_loc+1]&0x7f)<<8);
            int iset=i%256;
            hb[event->core_id].head_buffer[j]=tlb[tlb_index].content[iset];
            j++;
        }
        sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_HBLC;
        printf("clk=%ld,LOADM completed!\n",clk);
    }
    else if (event->event_type==EVENT_STOREM)
    {
        int valid=1;
        for (int i=event->event_para3;i<event->event_para3+event->event_para2;i++)
        {
            int tlb_loc=512+256+128+(i/256)*2;
            int tlb_valid=hb[event->core_id].head_buffer[tlb_loc+1]>>7;
            if (!tlb_valid){
                valid=0;
                printf("Error SMB_valid in PB[%d]\n",tlb_loc+1);
                break;
            }
        }
        if (valid) //��ַ������Ч
        {
            //�������¼�
            event_node *load_event=(event_node *)malloc(sizeof(event_node));
            load_event->core_id=event->core_id;
            load_event->event_type=EVENT_STOREMS2;
            load_event->event_para1=event->event_para1;
            load_event->event_para2=event->event_para2;
            load_event->event_para3=event->event_para3;
            load_event->event_para4=event->event_para4;
            load_event->trigger_clk=event->trigger_clk+STORE_CYCLE(event->event_para2);
            printf("SMB is correct! STOREM finished in %ld\n",load_event->trigger_clk);
            load_event->next=NULL;
            event_node *cur=event_list;
            if (cur==NULL)
                event_list=load_event;
            else
            {
                while (cur->next!=NULL)
                {
                    cur=cur->next;
                }
                cur->next=load_event;
            }
        }
        else
        {
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_HBSC;
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
            sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_HBSE;
        }

    }
    else if (event->event_type==EVENT_STOREMS2)
    {
        int j=event->event_para1;
        FILE *fp;
        if (first_stms2[event->core_id>>3]==0)
        {
            char str_stm[25];
            memset(str_stm,0,25);
            memcpy(str_stm,def_stm_path,strlen(def_stm_path));
            char str_clu[7];
            memset(str_clu,0,7);
            sprintf(str_clu,"_%d.txt",event->core_id>>3);
            memcpy(str_stm+13,str_clu,7);
            fp=fopen(str_stm,"w");
            first_stms2[event->core_id>>3]=1;
        }
        else
        {
            char str_stm[25];
            memset(str_stm,0,25);
            memcpy(str_stm,def_stm_path,strlen(def_stm_path));
            char str_clu[7];
            memset(str_clu,0,7);
            sprintf(str_clu,"_%d.txt",event->core_id>>3);
            memcpy(str_stm+13,str_clu,7);
            fp=fopen(str_stm,"a");
        }
        fseek(fp,0,SEEK_END);
        fprintf(fp,"core%d:STOREM\n",event->core_id);
        fprintf(fp,"%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
        event->event_para3,event->event_para3+1,event->event_para3+2,event->event_para3+3);
        int k=0;
        for (int i=event->event_para3;i<event->event_para3+event->event_para2;i++)
        {
            int tlb_loc=512+256+128+(i/256)*2;
            int iset=i%256;
            int tlb_index=hb[event->core_id].head_buffer[tlb_loc]+
            ((hb[event->core_id].head_buffer[tlb_loc+1] & 0x7f)<<8);
            tlb[tlb_index].content[iset]=hb[event->core_id].head_buffer[j];
            fprintf(fp,"0x%x: 0x%x\t",tlb_index*256+iset,tlb[tlb_index].content[iset]);
            if (k==3 && (i+4<event->event_para3+event->event_para2))
            {
                k=0;
                fprintf(fp,"\n");
                fprintf(fp,"%d\t\t\t%d\t\t\t%d\t\t\t%d\n",
                i+1,i+2,i+3,i+4);
            }
            else
                k++;
            j++;
        }
        fprintf(fp,"\n");
        fprintf(fp,"------------------------------\n");
        fclose(fp);
        eb.length+=event->event_para2;
        sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_HBSC;
        printf("clk=%ld,STOREM completed!\n",clk);
    }
    else if (event->event_type==EVENT_LOADT)
    {
        int i=0;
        int fail=0;
        //sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_TBLC;
        while ((g_table_pool[i].TableID!=event->event_para1) && (i<64))
        {
            i++;
        }
        if (i==64) 
        {
            fail=1;
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_TBLC;
            printf("LOADT No Table\n");
            printf("LOADT finish!\n");
        }
        else
        {
            item_list *cur=g_table_pool[i].Table;
            long loadID=(hb[event->core_id].head_buffer[event->event_para2])+(hb[event->core_id].head_buffer[event->event_para2+1]<<8)+
            (hb[event->core_id].head_buffer[event->event_para2+2]<<16)+(hb[event->core_id].head_buffer[event->event_para2+3]<<24);
            while (cur!=NULL)
            {
                int find=1;
                if (loadID!=cur->ItemID) find=0;
                if (find)
                {
                    //�������¼�
                    event_node *load_event=(event_node *)malloc(sizeof(event_node));
                    load_event->core_id=event->core_id;
                    load_event->event_type=EVENT_LOADTS2;
                    load_event->event_para1=event->event_para1;
                    load_event->event_para2=event->event_para2;
                    load_event->event_para3=event->event_para3;
                    load_event->event_para4=event->event_para4;
                    load_event->trigger_clk=event->trigger_clk+LOADT_CYCLE(g_table_pool[i].ItemSize);
                    printf("Item found! LOADT finished in %ld\n",load_event->trigger_clk);
                    load_event->next=NULL;
                    event_node *cur=event_list;
                    if (cur==NULL)
                        event_list=load_event;
                    else
                    {
                        while (cur->next!=NULL)
                        {
                            cur=cur->next;
                        }
                        cur->next=load_event;
                    }
                    break;
                }
                else
                cur=cur->next;
            }
            if (cur==NULL)
            {

                //�������¼�
                event_node *load_event=(event_node *)malloc(sizeof(event_node));
                load_event->core_id=event->core_id;
                load_event->event_type=EVENT_LOADTS2;
                load_event->event_para1=event->event_para1;
                load_event->event_para2=event->event_para2;
                load_event->event_para3=event->event_para3;
                load_event->event_para4=event->event_para4;
                load_event->trigger_clk=event->trigger_clk+LOADT_CYCLE(g_table_pool[i].ItemSize);
                printf("Table found! LOADT finished in %ld\n",load_event->trigger_clk);
                printf("Warning! LOADT no item\n");
                load_event->next=NULL;
                event_node *cur=event_list;
                if (cur==NULL)
                    event_list=load_event;
                else
                {
                    while (cur->next!=NULL)
                    {
                        cur=cur->next;
                    }
                    cur->next=load_event;
                }
            }
        }
        if (fail)
        {
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
            sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_TBLE;
        }
        //printf("LOADT finish!\n");

    }
    else if (event->event_type==EVENT_LOADTS2)
    {
        int i=0;
        int fail=0;
        sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_TBLC;
        while ((g_table_pool[i].TableID!=event->event_para1) && (i<64))
        {
            i++;
        }
        if (i==64) 
        {
            fail=1;
            printf("LOADT No Table\n");
        }
        else
        {
            item_list *cur=g_table_pool[i].Table;
            long loadID=(hb[event->core_id].head_buffer[event->event_para2])+(hb[event->core_id].head_buffer[event->event_para2+1]<<8)+
            (hb[event->core_id].head_buffer[event->event_para2+2]<<16)+(hb[event->core_id].head_buffer[event->event_para2+3]<<24);
            while (cur!=NULL)
            {
                int find=1;
                if (loadID!=cur->ItemID) find=0;
                if (find)
                {
                    for (int j=0;j<(g_table_pool[i].ItemSize);j+=2)
                    {
                        hb[event->core_id].head_buffer[event->event_para3+j]=cur->Item[j];
                        hb[event->core_id].head_buffer[event->event_para3+j+1]=cur->Item[j+1];
                        //mr[((event->event_para3)+(j/2))%64]=
                        //(cur->Item[j+1])+((cur->Item[j])<<8);
                        printf("HB%d set to %d\n",(event->event_para3)+j,hb[event->core_id].head_buffer[(event->event_para3)+j]);
                        printf("HB%d set to %d\n",(event->event_para3)+j+1,hb[event->core_id].head_buffer[(event->event_para3)+j+1]);
                    }
                    break;
                }
                else
                cur=cur->next;
            }
            if (cur==NULL)
            {

                for (int j=0;j<(g_table_pool[i].ItemSize);j+=2)
                    {
                        hb[event->core_id].head_buffer[event->event_para3+j]=0;
                        hb[event->core_id].head_buffer[event->event_para3+j+1]=0;
                        //mr[((event->event_para3)+(j/2))%64]=
                        //(cur->Item[j+1])+((cur->Item[j])<<8);
                        printf("HB%d set to %d\n",(event->event_para3)+j,hb[event->core_id].head_buffer[(event->event_para3)+j]);
                        printf("HB%d set to %d\n",(event->event_para3)+j+1,hb[event->core_id].head_buffer[(event->event_para3)+j+1]);
                    }
            }
        }
        if (fail)
        {
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
            sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_TBLE;
        }
        printf("LOADT finish!\n");
    }
    else if (event->event_type==EVENT_STORET)
    {
        int i=0;
        int fail=0;
        
        while ((g_table_pool[i].TableID!=event->event_para1) && (i<64))
        {
            i++;
        }
        if (i==64) 
        {
            //TODO:
            fail=1;
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_TBSC;
            printf("STORET No Table\n");
            printf("STORET finish!\n");
        }
        else
        {
            item_list *cur=g_table_pool[i].Table;
            item_list *pre=cur;
            //long loadID=(mr[event->event_para2]<<16)+mr[(event->event_para2+1)%64];
            long loadID=(hb[event->core_id].head_buffer[event->event_para2])+(hb[event->core_id].head_buffer[event->event_para2+1]<<8)+
            (hb[event->core_id].head_buffer[event->event_para2+2]<<16)+(hb[event->core_id].head_buffer[event->event_para2+3]<<24);
            int num=0;
            while (cur!=NULL)
            {
                num++;
                int find=1;
                if (loadID!=cur->ItemID) find=0;
                if (find)
                {
                    //�������¼�
                    event_node *load_event=(event_node *)malloc(sizeof(event_node));
                    load_event->core_id=event->core_id;
                    load_event->event_type=EVENT_STORETS2;
                    load_event->event_para1=event->event_para1;
                    load_event->event_para2=event->event_para2;
                    load_event->event_para3=event->event_para3;
                    load_event->event_para4=event->event_para4;
                    load_event->trigger_clk=event->trigger_clk+STORET_CYCLE(g_table_pool[i].ItemSize);
                    printf("Table found! STORET finished in %ld\n",load_event->trigger_clk);
                    load_event->next=NULL;
                    event_node *cur=event_list;
                    if (cur==NULL)
                        event_list=load_event;
                    else
                    {
                        while (cur->next!=NULL)
                        {
                            cur=cur->next;
                        }
                        cur->next=load_event;
                    }
                    // for (int j=0;j<(g_table_pool[i].ItemSize);j+=2)
                    // {
                    //     cur->Item[j]=hb[event->core_id].head_buffer[event->event_para2+4+j];
                    //     cur->Item[j+1]=hb[event->core_id].head_buffer[event->event_para2+4+j+1];
                    //     printf("Item %d set to %d\n",j,cur->Item[j]);
                    //     printf("Item %d set to %d\n",j+1,cur->Item[j+1]);
                    // }
                    break;
                }
                else
                {
                    pre=cur;
                    cur=cur->next;
                }
            }
            if (cur==NULL)
            {
                // item_list * new_item=(item_list *)malloc(sizeof(item_list));
                // new_item->ItemID=loadID;
                // new_item->next=NULL;
                printf("new_Item\n");
                //�������¼�
                event_node *load_event=(event_node *)malloc(sizeof(event_node));
                load_event->core_id=event->core_id;
                load_event->event_type=EVENT_STORETS2;
                load_event->event_para1=event->event_para1;
                load_event->event_para2=event->event_para2;
                load_event->event_para3=event->event_para3;
                load_event->event_para4=event->event_para4;
                load_event->trigger_clk=event->trigger_clk+STORET_CYCLE(g_table_pool[i].ItemSize);
                printf("Table found! STORET finished in %ld\n",load_event->trigger_clk);
                load_event->next=NULL;
                event_node *cur=event_list;
                if (cur==NULL)
                    event_list=load_event;
                else
                    {
                    while (cur->next!=NULL)
                    {
                        cur=cur->next;
                    }
                    cur->next=load_event;
                }
            }
        }
        if (fail)
        {
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
            sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_TBSE;
        }
    }
    else if (event->event_type==EVENT_STORETS2)
    {
        int i=0;
        int fail=0;
        sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_TBSC;
        while ((g_table_pool[i].TableID!=event->event_para1) && (i<64))
        {
            i++;
        }
        if (i==64) 
        {
            //TODO:
            fail=1;
            printf("STORET No Table\n");
        }
        else
        {
            item_list *cur=g_table_pool[i].Table;
            item_list *pre=cur;
            //long loadID=(mr[event->event_para2]<<16)+mr[(event->event_para2+1)%64];
            long loadID=(hb[event->core_id].head_buffer[event->event_para2])+(hb[event->core_id].head_buffer[event->event_para2+1]<<8)+
            (hb[event->core_id].head_buffer[event->event_para2+2]<<16)+(hb[event->core_id].head_buffer[event->event_para2+3]<<24);
            int num=0;
            printf("LoadID : %ld\n",loadID);
            while (cur!=NULL)
            {
                num++;
                int find=1;
                if (loadID!=cur->ItemID) find=0;
                if (find)
                {
                    printf("LoadID : %ld\n",loadID);
                    for (int j=0;j<(g_table_pool[i].ItemSize);j+=2)
                    {
                        cur->Item[j]=hb[event->core_id].head_buffer[event->event_para2+4+j];
                        cur->Item[j+1]=hb[event->core_id].head_buffer[event->event_para2+4+j+1];
                        printf("Item %d set to %d\n",j,cur->Item[j]);
                        printf("Item %d set to %d\n",j+1,cur->Item[j+1]);
                    }
                    break;
                }
                else
                {
                    pre=cur;
                    cur=cur->next;
                }
            }
            if (cur==NULL)
            {
                item_list * new_item=(item_list *)malloc(sizeof(item_list));
                new_item->ItemID=loadID;
                new_item->next=NULL;
                printf("new_Item  LoadID : %ld\n",loadID);
                for (int j=0;j<(g_table_pool[i].ItemSize);j+=2)
                    {
                        new_item->Item[j]=hb[event->core_id].head_buffer[event->event_para2+4+j];
                        new_item->Item[j+1]=hb[event->core_id].head_buffer[event->event_para2+4+j+1];
                        printf("Item %d set to %d\n",j,new_item->Item[j]);
                        printf("Item %d set to %d\n",j+1,new_item->Item[j+1]);
                    }
                if (g_table_pool[i].Table==NULL)
                {
                    g_table_pool[i].Table=new_item;
                }
                else
                {
                    pre->next=new_item;
                }
            }
        }
        if (fail)
        {
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
            sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_TBSE;
        }
        printf("STORET finish!\n");

    }
    else if (event->event_type==EVENT_QUERYT)
    {
        int i=0;
        int fail=0;
        //no table/no item will qc,else wait for qc
        //sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_QC;
        while ((q_table_pool[i].TableID!=event->event_para1) && (i<64))
        {
            i++;
        }
        if (i==64) 
        {
            fail=1;
            printf("Query No Table\n");
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_QC;
            sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_TBQE;
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
            printf("QUERYT finish!\n");
        }
        else
        {   
            item_list *cur=q_table_pool[i].Table;
            for (int j=0;j<event->event_para4;j++)
            {
                printf("LBindex:%-4u KEY:%-4u\n",(event->event_para2)+j*2,hb[event->core_id].head_buffer[(event->event_para2)+j*2]);
                printf("LBindex:%-4u KEY:%-4u\n",(event->event_para2)+j*2+1,hb[event->core_id].head_buffer[(event->event_para2)+j*2+1]);
            }
            while (cur!=NULL)
            {
                //�����
                int find=1;
                for (int j=0;j<event->event_para4;j++)
                {
                    //ƥ��
                    if ((hb[event->core_id].head_buffer[(event->event_para2)+j*2]!=(cur->Item[j*2])) || 
                         (hb[event->core_id].head_buffer[(event->event_para2)+j*2+1]!=(cur->Item[j*2+1])))
                    {
                        find=0;
                        break;
                    }
                    //merge by khh: Query Key Added
                    else
                    {

                    }
                }
                if (find && (event->event_para4*2==q_table_pool[i].QuerySize))
                {
                    printf("Query Hit!\n");
                    //fixed: return data event
                    //�������¼�
                    event_node *load_event=(event_node *)malloc(sizeof(event_node));
                    load_event->core_id=event->core_id;
                    load_event->event_type=EVENT_QUERYTS2;
                    load_event->event_para1=event->event_para1;
                    load_event->event_para2=event->event_para2;
                    load_event->event_para3=event->event_para3;
                    load_event->event_para4=event->event_para4;
                    load_event->trigger_clk=event->trigger_clk+QUERY_CYCLE(q_table_pool[i].ItemSize-q_table_pool[i].QuerySize);
                    printf("QUERYT is correct!will be finished in %ld\n",load_event->trigger_clk);
                    load_event->next=NULL;
                    event_node *ecur=event_list;
                    if (ecur==NULL)
                        event_list=load_event;
                    else
                    {
                        while (ecur->next!=NULL)
                        {
                            ecur=ecur->next;
                        }
                        ecur->next=load_event;
                    }
                    break;
                }
                else
                cur=cur->next;
            }
            if (cur==NULL)
            {
                printf("Query no item\n");
                sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_QC;
                sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_QM;
                fail=1;
                printf("QUERYT finish!\n");
            }
        }
        
    }
    else if (event->event_type==EVENT_QUERYTS2)
    {
        int i=0;
        int fail=0;
        //no table/no item will qc,else wait for qc
        sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_QC;
        while ((q_table_pool[i].TableID!=event->event_para1) && (i<64))
        {
            i++;
        }
        if (i==64) 
        {

        }
        else
        {   
            item_list *cur=q_table_pool[i].Table;
            while (cur!=NULL)
            {
                //�����
                int find=1;
                for (int j=0;j<event->event_para4;j++)
                {
                    //ƥ��
                    if ((hb[event->core_id].head_buffer[(event->event_para2)+j*2]!=(cur->Item[j*2])) || 
                         (hb[event->core_id].head_buffer[(event->event_para2)+j*2+1]!=(cur->Item[j*2+1])))
                    {
                        find=0;
                        break;
                    }
                    //merge by khh: Query Key Added
                    else
                    {
                        //must have found
                        // printf("LBindex:%-4u KEY:%-4u\n",(event->event_para2)+j*2,hb[event->core_id].head_buffer[(event->event_para2)+j*2]);
                        // printf("LBindex:%-4u KEY:%-4u\n",(event->event_para2)+j*2+1,hb[event->core_id].head_buffer[(event->event_para2)+j*2+1]);
                    }
                }
                if (find && (event->event_para4*2==q_table_pool[i].QuerySize))
                {
                    for (int j=0;j<(q_table_pool[i].ItemSize-q_table_pool[i].QuerySize);j+=2)
                    {
                        hb[event->core_id].head_buffer[(event->event_para3)+j]=cur->Item[j+q_table_pool[i].QuerySize];
                        hb[event->core_id].head_buffer[((event->event_para3))+j+1]=cur->Item[j+1+q_table_pool[i].QuerySize];
                        printf("HB%d set to %d\n",(event->event_para3)+j,hb[event->core_id].head_buffer[(event->event_para3)+j]);
                        printf("HB%d set to %d\n",(event->event_para3)+j+1,hb[event->core_id].head_buffer[(event->event_para3)+j+1]);
                    }
                    printf("QUERYT finish!\n");
                    break;
                }
                else
                cur=cur->next;
            }
            if (cur==NULL)
            {
                //must have found
                printf("Query no item\n");
                sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_QC;
                sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_QM;
                fail=1;
                printf("QUERYT finish!\n");
            }
        }
    }
    else if (event->event_type==EVENT_INSERTT)
    {
        int i=0;
        int fail=0;
        sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_TBIC;
        while (((q_table_pool[i].TableID!=event->event_para1) || (q_table_pool[i].ItemSize!=event->event_para3*2))
                && (i<64))
        {
            i++;
        }
        if (i==64) 
        {
            fail=1;
            printf("Insert No Table\n");
        }
        else
        {
            item_list *cur=q_table_pool[i].Table;
            item_list *pre=cur;
            int id=0;
            while (cur!=NULL)
            {
                id++;
                int find=1;
                for (int j=0;j<q_table_pool[i].QuerySize;j++)
                {
                    if ((hb[event->core_id].head_buffer[(event->event_para2)+j]!=(cur->Item[j])))
                    {
                        find=0;
                        break;
                    }
                }
                pre=cur;
                if (find)
                {
                    /*fail=1;*/
                    printf("Find insert item!\n");
                    for (int j=0;j<q_table_pool[i].ItemSize;j+=2)
                    {
                        cur->Item[j]=hb[event->core_id].head_buffer[event->event_para2+j];
                        cur->Item[j+1]=hb[event->core_id].head_buffer[event->event_para2+j+1];
                        printf("Insert Item: %d:%d,%d:%d\n",
                        j,cur->Item[j],j+1,cur->Item[j+1]);
                    }
                    break;
                }
                else
                cur=cur->next;
            }
            if (cur==NULL && id<q_table_pool[i].ItemNumMax)
            {
                item_list *insert_item=(item_list *)malloc(sizeof(item_list));
                insert_item->ItemID=id;
                insert_item->next=NULL;
                for (int j=0;j<q_table_pool[i].ItemSize;j+=2)
                {
                    
                    insert_item->Item[j]=hb[event->core_id].head_buffer[event->event_para2+j];
                    insert_item->Item[j+1]=hb[event->core_id].head_buffer[event->event_para2+j+1];
                    printf("Insert Item: %d:%d,%d:%d\n",
                    j,insert_item->Item[j],j+1,insert_item->Item[j+1]);
                }
                if (q_table_pool[i].Table==NULL)
                    q_table_pool[i].Table=insert_item;
                else
                    pre->next=insert_item;
            }
        }
        if (fail)
        {
            sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_TBLE;
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
        }
        printf("INSERTT finish!\n");
    }
    else if (event->event_type==EVENT_DELETET)
    {
        int i=0;
        int fail=0;
        sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_TBIC;
        while ((q_table_pool[i].TableID!=event->event_para1) && (i<64))
        {
            i++;
        }
        if (i==64) 
        {
            fail=1;
            printf("Delete No Table\n");
        }
        else
        {
            item_list *cur=q_table_pool[i].Table;
            item_list *pre=cur;
            while (cur!=NULL)
            {
                int find=1;
                for (int j=0;j<q_table_pool[i].QuerySize/2;j++)
                {
                    if ((hb[event->core_id].head_buffer[(event->event_para2)+j*2]!=(cur->Item[j*2])) ||
                         (hb[event->core_id].head_buffer[(event->event_para2)+j*2+1]!=(cur->Item[j*2+1])))
                    {
                        find=0;
                        break;
                    }
                }
                if (find)
                {
                    pre->next=cur->next;
                    free(cur);
                    printf("Find and Delete!\n");
                    break;
                }
                else
                {
                    pre=cur;
                    cur=cur->next;
                }
            }
            if (cur==NULL)
            {
                printf("Delete no item\n");
                fail=1;
            }
        }
        if (fail)
        {
            sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
            sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_TBDE;
        }
        printf("QUERYT finish!\n");
    }
    else if (event->event_type==EVENT_LOADTLB)
    {
        sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_TLB;
        
        for (int i=0;i<event->event_para2;i++)
        {
            if (free_tlb_list==NULL) //SMB����
            {
                printf("No Empty SMB!\n");
                sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
                sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_TLBRE;
                break;
            }
            else
            {
                hb[event->core_id].head_buffer[event->event_para1+i*2]=free_tlb_list->tlb_num %256;
                hb[event->core_id].head_buffer[event->event_para1+i*2+1]=free_tlb_list->tlb_num>>8;
                hb[event->core_id].head_buffer[event->event_para1+i*2+1]|=0x80; //��Ϊ��Ч
                printf("set hb%d %d tlbnum 0x%x 0x%x\n",
                event->event_para1+i*2+1,event->event_para1+i*2,
                hb[event->core_id].head_buffer[event->event_para1+i*2+1],
                hb[event->core_id].head_buffer[event->event_para1+i*2]);
                tlb[free_tlb_list->tlb_num].used++;
                tlb_list *cur=free_tlb_list;
                free_tlb_list=free_tlb_list->next_free;
                free(cur);
            }
        }
        printf("LOADTLB finish!\n");
    }
    else if (event->event_type==EVENT_DELETETLB)
    {
        sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_TLB;
        
        for (int i=0;i<event->event_para2;i++)
        {
            hb[event->core_id].head_buffer[event->event_para1+i*2+1]&=0x7f; //�����Чλ
            int num=hb[event->core_id].head_buffer[event->event_para1+i*2]+(hb[event->core_id].head_buffer[event->event_para1+i*2+1]<<8);
            if (tlb[num].used==0)
            {
                printf("ERROR! Has delete!\n");
                sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
                sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_TLBRE;
                break;
            }
            else
            {
                tlb[num].used--;
                printf("delete SMB No.%d\n",num);
                if (tlb[num].used==0)
                {
                    tlb_list *cur=free_tlb_list;
                    tlb_list *token=(tlb_list *)malloc(sizeof(tlb_list));
                    if (cur==NULL)
                    {
                        token->tlb_num=num;
                        token->next_free=NULL;
                        free_tlb_list=token;
                    }
                    else
                    {
                        while (cur->next_free!=NULL)
                        {
                            cur=cur->next_free;
                        }
                        token->tlb_num=num;
                        token->next_free=NULL;
                        cur->next_free=token;
                    }
                }
            }
        }
        printf("DELETETLB finish!\n");
    }
    else if (event->event_type==EVENT_REUSETLB)
    {
        sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_TLB;
        
        for (int i=0;i<event->event_para2;i++)
        {
            int num=hb[event->core_id].head_buffer[event->event_para1+i*2]+
            ((hb[event->core_id].head_buffer[event->event_para1+i*2+1]&0x7f)<<8);
            if (tlb[num].used==0 || (hb[event->core_id].head_buffer[event->event_para1+i*2+1]>>7)==0)
            {
                printf("ERROR! Has not used!\n");
                sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_ERR;
                sr[event->core_id][SR_STA_ERR]|=SR_STA_ERR_TLBRE;
                break;
            }
            else
            {
                tlb[num].used++;
                printf("reuse num %d\n",num);
            }
        }
        printf("REUSETLB finish!\n");
    }
    else if (event->event_type==EVENT_SENDMB)
    {
        sr[event->core_id][SR_STA_FLAG]|=SR_STA_FLAG_MBT;
        for (int i=0;i<8;i++)
        {
            hb[event->event_para1].head_buffer[1048+i]=hb[event->core_id].head_buffer[event->event_para2+i];
        }
        printf("SENDMB to core %d MBR1 Finish!\n",event->event_para1);
        sr[event->event_para1][SR_STA_FLAG]|=SR_STA_FLAG_MBR0;
    }
}

/************************************************************************************/
ppeInit::cq_event_node * ppe_sim::check_cq_event(long clk)
{
    cq_event_node *token = cq_queue;
    cq_event_node *previous = token;
    while (token != NULL)
    {
        if (token->trigger_clk == clk)
        {
            if (token == cq_queue)
            {
                cq_queue = cq_queue->next;
            }
            else
            {
                previous->next = token->next;
            }
            token->next = NULL;
            return token;
        }
        else
        {
            previous = token;
            token = token->next;
        }
    }
    return token;
}

void ppe_sim::handle_cq_event(cq_event_node *cq_event)
{
    if (cq_event->event_type == PUSHQ_EVENT)
    {

        tlb_des_queue *token = (tlb_des_queue *)malloc(sizeof(tlb_des_queue));
        sr[cq_event->core_id][SR_STA_FLAG] |= SR_STA_FLAG_MBT;
        if (cq_event->event_para1 != 20)
        {
            token->next = NULL;
            token->length = cq_event->event_para3;
            for (int i = 0; i < cq_event->event_para3; i++)
            {
                token->tlb_num[i] = hb[cq_event->core_id].head_buffer[896 + i * 2] +
                                    (hb[cq_event->core_id].head_buffer[896 + i * 2 + 1] << 8);
            }
            token->mb_addr = 1024;
            for (int i = 0; i < 8; i++)
            {
                token->mb_content[i] = hb[cq_event->core_id].head_buffer[cq_event->event_para2 + i];
            }
            tlb_des_queue *cur = core_queues[cq_event->event_para1].input_queue;
            tlb_des_queue *pre = cur;
            if (cur == NULL)
            {
                core_queues[cq_event->event_para1].input_queue = token;
            }
            else
            {
                while (cur != NULL)
                {
                    pre = cur;
                    cur = cur->next;
                }
                pre->next = token;
            }
            printf("data from ppe%d,SMBnum %d,input into queue%d\n", cq_event->core_id, cq_event->event_para3, cq_event->event_para1);
        }
        else
        {
            printf("queue error!\n");
        }
    }
}


void ppe_sim::pkt_to_ppe(Packet *pkt)
{
    
    pkt->set_checksum_error(false);
    const char *data = pkt->data();

    printf("packet in size:%d\n",pkt->get_data_size());
}

void ppe_sim::ppe_to_pkt(Packet *pkt)
{
    
    pkt->set_checksum_error(false);
    char *data;
    data = pkt->remove(10);
    

    printf("packet out size:%d\n",pkt->get_data_size());
}

void ppe_sim::export_pkt(Packet *pkt,int *packet_out)
{
    
    pkt->set_checksum_error(false);
    char *data;
    for (int i = 16; i < 20; i++)
    {
        tlb_des_queue *pre = NULL;
        if (core_queues[i].input_queue)
        {
            int slice_num, offset, sys_meta, offset_num, pkt_length, port,slice_size;
            tlb_des_queue *select = core_queues[i].input_queue;
            printf("\n*****************CORE_QUEUE******************\n");
            if (i == 16 || i == 17)
                sys_meta = 32;
            else
                sys_meta = 0;
            while (select)
            {
                pre = select;
                offset = (select->mb_content[2] + sys_meta) % 256;
                offset_num = (select->mb_content[2] + sys_meta) / 256;
                pkt_length = select->mb_content[0] * 256 + select->mb_content[1];
                int remain = pkt_length % 256;
                // compare datasize and change the packet_buffer
                int offset_length =pkt->get_data_size()-pkt_length;
                data = pkt->data();
                if(offset_length>0)
                {
                    data = pkt->prepend(offset_length);
                }
                else if(offset_length<0)
                {
                    data = pkt->remove(-offset_length);
                }
                printf("QUEUE:%d  Length:%d   offset:%d   pkt_len:%d    remain:%d\nMailbox: ", i, select->length, offset,pkt_length,remain);
                for(int mail = 0; mail < 8; mail++)
                {
                    printf("%d  ", select->mb_content[mail]);
                }
                printf("\n");
                for(slice_num = 0; slice_num < select->length; slice_num++)
                {
                    if (tlb[select->tlb_num[slice_num] & 0x7fff].used == 0)
                        continue;
                    int j;
                    if (slice_num == offset_num)
                    {
                        if (slice_num == select->length-1)
                            slice_size = remain;
                        else
                            slice_size = 256 - offset;
                        for (j = 0; j < slice_size; j++)
                        {
                            // egress.ingress_buffer[(slice_num - offset_num) * 256 + j] = tlb[select->tlb_num[slice_num] & 0x7fff].content[j + offset];
                            *(data+ j) = tlb[select->tlb_num[slice_num] & 0x7fff].content[j + offset];
                        }
                    }
                    else if (slice_num > offset_num)
                    {
                        if (slice_num == select->length-1)
                            slice_size = (remain + offset) % 256;
                        else
                            slice_size = 256 - offset;
                        for (j = 0; j < slice_size; j++)
                        {
                            // egress.ingress_buffer[(slice_num - offset_num) * 256 + j - offset] = tlb[select->tlb_num[slice_num] & 0x7fff].content[j];
                            *(data+(slice_num - offset_num) * 256 + j - offset) = tlb[select->tlb_num[slice_num] & 0x7fff].content[j];
                        }
                    }
                    tlb[select->tlb_num[slice_num] & 0x7fff].used = 0;
                    // memset(tlb[select->tlb_num[slice_num]].content,0,256);
                }
                char pkt_name[30];
                sprintf(pkt_name, "egr_%d_%ld_%d.txt", i, clk, ++egr_num);
                FILE *outfile = fopen(pkt_name, "w");
                if (!outfile)
                {
                    printf("egress file not found!\n");
                    exit(0);
                }
                fprintf(outfile, "%-3d\n", pkt_length);
                switch (select->mb_content[4] >> 2)
                {
                case 0:
                    port = 0;
                    break;
                case 1:
                    port = 1;
                    break;
                case 2:
                    port = 2;
                    break;
                case 3:
                    port = 3;
                    break;
                default:
                    printf("egress port error!");
                    exit(0);
                }
                fprintf(outfile, "%-3d\n", port);
                fprintf(outfile, "%-3d\n", port & 0x1);
                for (int index = 0; index < pkt_length; index++)
                {
                    if (index % 4 == 0 && index > 0)
                        fprintf(outfile, "\n");
                    fprintf(outfile, "%-6X", *(data+index));
                }
                select = select->next;
                core_queues[i].input_queue = select;
                free(pre);
                // printf("Core End\n");
                fclose(outfile);
                --*packet_out;
            }
        }
    }
}


void ppe_sim::sim(Packet *pkt)
{
    int packet_out=1;
    while (1)
    {
        if(all_queue_empty() && all_core_end())
        {
            break;
        }
        if(packet_out<=0)
            break;
    // if(all_queue_empty)
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                printf(" %d  ",i,core_queues[i].is_used[j]);
             
            }
            printf("\n");
        }
        pkt_to_tlb(pkt,0,0);

        core_queue_check();
        for (int i = 0; i < 16; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                printf(" %d  ",i,core_queues[i].is_used[j]);
             
            }
            printf("\n");
            
        }
        ppe_run();
        cq_event_node *current_event = check_cq_event(clk);
        while (current_event != NULL)
        {
            handle_cq_event(current_event);
            free(current_event);
            current_event = check_cq_event(clk);
        }
        export_pkt(pkt,&packet_out);
        printf("packet_out:%d\n",packet_out);
        clk++;
    }

    
}


void ppe_sim::transmit(ib_module &&input_buffer)
{
    // output_buffer(std::move(input_buffer));
}
}
