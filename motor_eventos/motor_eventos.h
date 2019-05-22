#ifndef _motor_included
#define _motor_included
 typedef struct {
    unsigned char n0 :4;
    unsigned char n1 :4;
} nibles;
extern nibles Memoria[1024][2];
extern void RunMVM();
#endif
