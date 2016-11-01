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
*   TWINKLE_RATE:またたき用の乱数値の更新レート。これにnを設定するとn回のタイマ割込に1回の割合で値が更新する。（2324プラネでは12個の乱数更新している）
*   TWINKLE_SHIFT:乱数の周期性問題を解決するために、乱数とそれに対応する信号出力ビットをport_shift()で変更している。これにnを設定するとn回の乱数更新に1回の割合で出力がビットシフトする。
*   TWINKLE_PATTERNS:またたきのパターン数。
*   chaos_div:chaos_gen()で生成される乱数は1~32768の幅であるが、このままではタイマ0割込に使用できないので適当な2の乗数で割る。
*   on_duration_max:LEDの点灯時間の最大値を決定するパラメーター。これはタイマ割込の間隔によって決まっているのでオシロスコープで波形を見ながら調整のこと。
*   min_strong,min_weak:点灯時間の最小値を決定するパラメーター。これが小さいほどまたたきの度合いが強い。
*****************************************************************************************/

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "Twinkle.h"

#define F_CPU 8000000UL
#define TWINKLE_RATE 2
#define TWINKLE_SHIFT 100
#define TWINKLE_PATTERNS 12

Twinkle twinkle(TWINKLE_RATE, TWINKLE_SHIFT);

ISR(INT0_vect){ //またたきOFF（スイッチで切り替え）
  PORTA = 0xFF;
  PORTB = 0xFF;
}

ISR(TIMER0_COMPA_vect){ //またたきをつかさどる部分。PORTAがまたたき強い、PORTBがまたたき弱い。
  unsigned int c_up = 0;
  unsigned int pattern_count = TWINKLE_PATTERNS;
  PORTA = 0xFF;
  PORTB = 0xFF;
  twinkle.count();
  while(pattern_count){ //on_durationの値とc_up（カウントアップ）の値を比較し、一致するまではON、一致したらLEDをOFFにする。全部OFFになったらループを抜けてmainに戻る。
    c_up++;
    for(int i=0;i<12;i++){
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
