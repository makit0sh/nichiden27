#include <util/delay.h>
#include <math.h>
#include "Twinkle.h"

#define chaos_div 256
#define on_duration_max 160
#define min_strong 30
#define min_weak 90
#define min(x, y) ((x)<(y)?(x):(y))

Twinkle::Twinkle(int rr, int sr):refresh_rate(rr), rr_count(rr), shift_rate(sr), sr_count(sr){};

void Twinkle::port_shift(void){ // 疑似周期性を目立たなくするために乱数値とその対応ビットを変更する。所要時間は6数の数列で100us
  shift_chaos = chaos_gen(shift_chaos);
  for(int i=0;i<6;i++) bit_num[i] = (bit_num[i] + (shift_chaos >= 12000 ? 1 : 4)) % 6; // 6要素の数列を左に回転シフト
}

void Twinkle::refresh(void){ // 乱数値を更新する。所要時間は12変数で1ms
  for(int i=0;i<12;i++){
    chaos[i] = chaos_gen(chaos[i]);
    on_duration[i] = min((int)(chaos[i] / chaos_div + i<6 ? min_strong : min_weak), on_duration_max);
  }
}

unsigned long Twinkle::chaos_gen(unsigned long y){ // Max == 32768までの整数値を返す疑似乱数(1/fゆらぎ)
  if(y < 1638) y += 2 * pow(y, 2) / 32768 + 1966;
  else if(y < 16384) y += 2 * pow(y, 2) / 32768;
  else if(y > 31129) y -= 2 * pow(32768 - y, 2) / 32768 + 1310;
  else y -= 2 * pow(32768 - y, 2) / 32768;
  return y;
}

void Twinkle::count(void){
  rr_count--; //乱数更新時期の判定と実行をする
  if(!rr_count){
    sr_count--; //ビットシフト更新時期の判定と実行をする
    if(!sr_count){
      port_shift();
      sr_count = shift_rate;
    }else _delay_us(100);
    refresh();
    rr_count = refresh_rate;
  }else _delay_us(1100); //それらの操作をしない場合でも、同じだけの時間waitして調整する
}
