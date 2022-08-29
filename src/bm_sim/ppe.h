#ifndef __PPE_H_
#define __PPE_H_
#include<vector>
#include<string>
#include<ppeInit.h>
// #include <functional>
#include <memory>
// #include <chrono>
// #include <thread>

class ppe_sim:ppeInit
{
public:
    
    
public:
    ib_module fun(ib_module input_buffer);

    void cq_init()override;   
    bool all_queue_empty()override;
    bool all_core_end()override;;
    void core_queue_check()override;
    void tlb_init()override;
    bool pack_to_tlb(ib_module *ib, int fcs, int port);

    ppe_sim();

    // ib_module &operator=(ib_module &&buffer)=default;

    void transmit(ib_module &&input_buffer);
private:
    //ppe_sim constructor funtion
    
    //input_size
    // const int input_size=10;
    //input_buffer
    ib_module input_buffer;
    //output_buffer
    ib_module output_buffer;
    //
    const int fcs;

    const int port;
    
    
};



#endif
