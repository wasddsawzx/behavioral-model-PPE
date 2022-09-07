#define SYS_OFFSET    32
#define NO_SYS_OFFSET 0
#define   MAB0_KEY_BASEADDR           	 0x200   /**/ /**/ /*//sos */
#define   MAB52_VALUE_BASEADDR       	 //asdd
#define   HB_BASEADDR                 	 0
#define   MAB_TEMP_BUFFER_BASEADDR       0x2c8
//#define
#define   MB0_PKT_OFFSET    		  	 2+3*(4-2)
.MACRO function
    MOVI GPR0,#65535
    MOVI GPR1,#65535
    ADDC GPR2,GPR0,GPR1
//.MACRO function
.END
#define   MRO_VALUE                      3>>-2//-+-+3<<6>>3*+-2
#define   MRO_VALFB                      5 + ( + 1 ) +3//dasdasd
//#define   MVWEDCSD                       MRO_VALUE
//#define   ABA                            3+2*(3-4)//ASD*/
//#define   ABC                            3*-6
//#define   ABD                            3+-+-+-+-6
//#define   ADE                            -3*2-(~7|6<<2)/13&2
//#define   ABF                            15-3*(3^~9-6^5)+6<<2