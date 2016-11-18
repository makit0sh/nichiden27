/* Twinkle.h */
#ifndef __TWINKLE_H__
#define __TWINKLE_H__

#define NORTH_HEMISPHERE
//#define SOUTH_HEMISPHERE

class Twinkle {
  public:
    int bit_num[6] = {0,1,2,3,4,5};
#ifdef NORTH_HEMISPHERE
    unsigned int on[12] = {1,1,1,1,1,0,1,1,1,1,1,1}; // またたかせるかどうか
#endif
#ifdef SOUTH_HEMISPHERE
    unsigned int on[12] = {1,1,1,0,0,0,1,1,1,1,1,1};
#endif
    unsigned int on_duration[12] = {160,160,160,160,160,160,160,160,160,160,160,160};
    void generate(void);
    Twinkle();
  private:
    unsigned long shift_chaos = 28000;
#ifdef NORTH_HEMISPHERE
    unsigned long chaos[12] = {512,844,1020,1024,2020,2048,2424,3624,4824,8224,8884,11024};
#endif
#ifdef SOUTH_HEMISPHERE
    unsigned long chaos[12] = {12404,14884,18200,18402,20442,21488,24242,28004,28824,30224,32008,32024};
#endif
    /* 点灯時間の最小値を決定するパラメーター。小さいほどまたたきの度合いが強い */
#ifdef NORTH_HEMISPHERE
    unsigned int min_duration[12] = {30, 30, 30, 30, 30, 0, 70, 70, 70, 70, 70, 70};
#endif
#ifdef SOUTH_HEMISPHERE
    unsigned int min_duration[12] = {30, 30, 30, 0, 0, 0, 70, 70, 70, 70, 30, 30};
#endif
    unsigned int refresh_rate;
    unsigned int rr_count;
    unsigned int shift_rate;
    unsigned int sr_count;
    void port_shift(void);
    void refresh(void);
    unsigned long chaos_gen(unsigned long y);
};

#endif // __TWINKLE_H__
