/*****************************************************************************************
*                                      Twinkle.ino
* Created: 2012/11/07 18:57:08
* Author: Haruki Nishimura(Nichiden_23)
* Edited: 2016/10/31 12:52:48 by K_Ito(Nichiden_27)
*
* Caution!!! ヒューズはLo = E2, Hi= DF, Ex = FFに設定すること！(DIVCLK8をDisableする)
* K_Ito追記: Arduino IDEに以下のjsonを読み込ませるとボードマネージャからAVRが追加できる。
*   http://drazzy.com/package_drazzy.com_index.json
*
* 制御用変数の説明
*   twinkle_patterns:またたきのパターン数。
*   twinkle_rate:またたき用の乱数値の更新レート。これにnを設定するとn回のタイマ割込に1回の割合で値が更新する。（2324プラネでは12個の乱数更新している）
*   chaos_div:chaos_gen()で生成される乱数は1~32768の幅であるが、このままではタイマ0割込に使用できないので適当な2の乗数で割る。
*   on_duration_max:LEDの点灯時間の最大値を決定するパラメーター。これはタイマ割込の間隔によって決まっているのでオシロスコープで波形を見ながら調整のこと。
*   min_str_countong,min_weak:点灯時間の最小値を決定するパラメーター。これが小さいほどまたたきの度合いが強い。
*   twinkle_shift:乱数の周期性問題を解決するために、乱数とそれに対応する信号出力ビットをport_shift()で変更している。これにnを設定するとn回の乱数更新に1回の割合で出力がビットシフトする。
*****************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

#define F_CPU 8000000UL
#define twinkle_patterns 12
#define twinkle_rate 2
#define chaos_div 256
#define on_duration_max 160
#define min_strong 30
#define min_weak 90
#define twinkle_shift 100

#define FOR(i,k,n) for(int (i)=(k);(i)<(n);(i)++)
#define REP(i,n) FOR(i,0,n)

class Twinkle {
  public:
    int bit_num[6] = {0,1,2,3,4,5};
    unsigned int on_duration[12] = {160,160,160,160,160,160,160,160,160,160,160,160};
    void count(void);
  private:
    unsigned long shift_chaos = 28000;
    unsigned long chaos[12] = {512,844,1020,1024,2020,2048,2424,3624,4824,8224,8884,11024};
    unsigned int tr_count = twinkle_rate;
    unsigned int ts_count = twinkle_shift;
    void port_shift(void);
    void refresh(void);
    unsigned long chaos_gen(unsigned long y);
};
Twinkle twinkle;

void Twinkle::port_shift(void){ // 疑似周期性を目立たなくするために乱数値とその対応ビットを変更する。所要時間は6数の数列で100us
  shift_chaos = chaos_gen(shift_chaos);
  REP(i,6) bit_num[i] = (bit_num[i] + (shift_chaos >= 12000 ? 1 : 4)) % 6; // 6要素の数列を左に回転シフト
}

void Twinkle::refresh(void){ // 乱数値を更新する。所要時間は12変数で1ms
  REP(i,12){
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
  tr_count--; //乱数更新時期の判定と実行をする
  if(!tr_count){
    ts_count--; //ビットシフト更新時期の判定と実行をする
    if(!ts_count){
      port_shift();
      ts_count = twinkle_shift;
    }else _delay_us(100);
    refresh();
    tr_count = twinkle_rate;
  }else _delay_us(1100); //それらの操作をしない場合でも、同じだけの時間waitして調整する
}

ISR(INT0_vect){ //またたきOFF（スイッチで切り替え）
  PORTA = 0xFF;
  PORTB = 0xFF;
}

ISR(TIMER0_COMPA_vect){ //またたきをつかさどる部分。PORTAがまたたき強い、PORTBがまたたき弱い。
  unsigned int c_up = 0;
  unsigned int pattern_count = twinkle_patterns;
  PORTA = 0xFF;
  PORTB = 0xFF;
  twinkle.count();
  while(pattern_count){ //on_durationの値とc_up（カウントアップ）の値を比較し、一致するまではON、一致したらLEDをOFFにする。全部OFFになったらループを抜けてmainに戻る。
    c_up++;
    REP(i,12){
      if(twinkle.on_duration[i] == c_up){
        if(i < 6) PORTA ^= 1 << twinkle.bit_num[i];
        else PORTB ^= 1 << twinkle.bit_num[i-6];
        pattern_count--;
      }
    }
    _delay_us(7);
  }
}

int main(void){
  /*** 初期設定(TIMER0_CTC、TIMER割込、INT0割込) ***/
  _delay_ms(100);
  DDRA = 0xFF; //PORTA0~7を出力に設定
  DDRB = 0xFF;
  /*タイマ0 CTCモード、タイマ0_Compare_A割込*/
  TCCR0A = 0x01; //CTC0をHに
  TCCR0B = 0x00 | 1<<CS02 | 0<<CS01 | 0<<CS00;
  OCR0A  = 180; //CTCのMAX値の設定(180、プリスケーラ256の設定でタイマ割込間隔は7.5msec)
  TIMSK |= 1<<OCIE0A | 0<<TOIE0; //タイマ0CompA割込有効
  GIMSK |= 1<<INT0; //INT0割り込み有効
  MCUCR |= 0<<ISC01 | 0<<ISC00; //INTピンのLowで割り込み発生
  PORTA  = 0x00;
  PORTB  = 0x00;
  /*************************************************/
  sei();
  for(;;){}
}
