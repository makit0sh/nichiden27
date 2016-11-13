#include <util/delay.h>
#include <math.h>
#include "Twinkle.h"

#define CHAOS_DIV 256 // chaos_gen()で生成される乱数は1~32768の幅であるが、このままではタイマ0割込に使用できないので適当な2の乗数で割る
#define ON_DURATION_MAX 160 // LEDの点灯時間の最大値を決定するパラメーター(タイマ割込の間隔によって決まっているのでオシロスコープで波形を見ながら調整のこと)
#define MIN_STRONG 30 // 点灯時間の最小値を決定するパラメーター。小さいほどまたたきの度合いが強い
#define MIN_WEAK 90
#define TWINKLE_RATE 2 // またたき用の乱数値の更新レート、nを設定するとn回のタイマ割込に1回の割合で値が更新する
#define TWINKLE_SHIFT 100 // 乱数の周期性問題を解決するために、乱数とそれに対応する信号出力ビットをport_shift()で変更している。nを設定するとn回の乱数更新に1回の割合で出力がビットシフトする

#define min(x, y) ((x)<(y)?(x):(y))

Twinkle::Twinkle():refresh_rate(TWINKLE_RATE), rr_count(TWINKLE_RATE), shift_rate(TWINKLE_SHIFT), sr_count(TWINKLE_SHIFT){};

void Twinkle::port_shift(void){ // 疑似周期性を目立たなくするために乱数値とその対応ビットを変更する。所要時間は6数の数列で100us
  shift_chaos = chaos_gen(shift_chaos);
  for(int i=0;i<6;i++) bit_num[i] = (bit_num[i] + (shift_chaos >= 12000 ? 1 : 4)) % 6; // 6要素の数列を左に回転シフト
}

void Twinkle::refresh(void){ // 乱数値を更新する。所要時間は12変数で1ms
  for(int i=0;i<12;i++){
    chaos[i] = chaos_gen(chaos[i]);
    on_duration[i] = min((int)(chaos[i] / CHAOS_DIV + (i<6 ? MIN_STRONG : MIN_WEAK)), ON_DURATION_MAX);
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
