/* Twinkle.h */
#ifndef __TWINKLE_H__
#define __TWINKLE_H__

class Twinkle {
  public:
    int bit_num[6] = {0,1,2,3,4,5};
    unsigned int on_duration[12] = {160,160,160,160,160,160,160,160,160,160,160,160};
    void count(void);
    Twinkle(int rr, int sr);
  private:
    unsigned long shift_chaos = 28000;
    unsigned long chaos[12] = {512,844,1020,1024,2020,2048,2424,3624,4824,8224,8884,11024};
    unsigned int refresh_rate;
    unsigned int rr_count;
    unsigned int shift_rate;
    unsigned int sr_count;
    void port_shift(void);
    void refresh(void);
    unsigned long chaos_gen(unsigned long y);
};

#endif // __TWINKLE_H__
