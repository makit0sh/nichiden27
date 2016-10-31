/*
 * Twinkle_North_final.c
 *
 * Created: 2012/11/07 18:57:08
 *  Author: Haruki Nishimura(Nichiden_23)
 *Caution!!! ヒューズは　Lo = E2, Hi= DF, Ex = FF　に設定すること！(DIVCLK8をDisableする)
 */
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*
 *制御用変数の説明
 *twinkle_patterns        :またたきのパターン数。
 *twinkle_rate          :またたき用の乱数値の更新レート。これにnを設定するとn回のタイマ割込に1回の割合で値が更新する。（2324プラネでは12個の乱数更新している）
 *chaos_div            :chaos_gen()で生成される乱数は1~32768の幅であるが、このままではタイマ0割込に使用できないので適当な2の乗数で割る。
 *led_int_max          :LEDの点灯時間の最大値を決定するパラメーター。これはタイマ割込の間隔によって決まっているのでオシロスコープで波形を見ながら調整のこと。
 *min_strong,min_weak      :点灯時間の最小値を決定するパラメーター。これが小さいほどまたたきの度合いが強い。
 *twinkle_shift          :乱数の周期性問題を解決するために、乱数とそれに対応する信号出力ビットをport_shift()で変更している。これにnを設定するとn回の乱数更新に1回の割合で出力がビットシフトする。
 */

#define twinkle_patterns 12
#define twinkle_rate 2
#define chaos_div 256
#define led_int_max 160
#define min_strong 30
#define min_weak 90
#define twinkle_shift 100

unsigned long chaos[12] = {512,844,1020,1024,2020,2048,2424,3624,4824,8224,8884,11024};
unsigned long shift_chaos = 28000;
unsigned int led_int[12] = {160,160,160,160,160,160,160,160,160,160,160,160};
int  bit_num[6] = {0,1,2,3,4,5};
unsigned int tr = twinkle_rate;
unsigned int ts = twinkle_shift;

unsigned int left_shift(unsigned int i, unsigned int a)//6個の数字の並んでいる数列を左にaシフトしたとき、右からi番目に並んでいる数を与える。
{
  i = i + a;
  if(i > 5){
    i = i - 6;
  }
  return i;
}

unsigned long chaos_gen(unsigned long y)//Max == 32768までの整数値を返す疑似乱数（1/fゆらぎ）。
{
  if(y < 1638){
    y = y + ((2 * y * y)/32768) + 1966;
  }else{
    if(y < 16384){
      y = y + ((2 * y * y)/32768);
    }else{
      if(y > 31129){
        y = y - ((2 * (32768 - y) * (32768 - y))/32768) - 1310;
      }else{
        y = y - ((2 * (32768 - y) * (32768 - y))/32768);
      }

    }
  }
  return y;
}

void port_shift(void)//疑似周期性を目立たなくするために乱数値とその対応ビットを変更する。所要時間は6数の数列で100us。
{
  int i;
  shift_chaos = chaos_gen(shift_chaos);
  if(shift_chaos >= 12000){
    for(i = 0;i <= 5;i++){
      bit_num[i] = left_shift(bit_num[i],1);
    }
  }else{
    for(i = 0;i <= 5;i++){
      bit_num[i] = left_shift(bit_num[i],4);
    }
  }
}

void chaos_renew(void)//乱数値を更新する。所要時間は12変数で1ms。
{
  int i;
  for(i = 0;i <= 5;i++){
    chaos[i] = chaos_gen(chaos[i]);
    led_int[i] =(int)(chaos[i]/chaos_div + min_strong);
    if(led_int[i] >= led_int_max){
      led_int[i] = led_int_max;
    }
  }
  for(i = 6;i <= 11;i++){
    chaos[i] = chaos_gen(chaos[i]);
    led_int[i] =(int)(chaos[i]/chaos_div + min_weak);
    if(led_int[i] >= led_int_max){
      led_int[i] = led_int_max;
    }
  }
  return;
}

void setup(void)//初期設定（TIMER0_CTC、TIMER割込、INT0割込）
{
  _delay_ms(100);
  DDRA  =  0xFF;  //PORTA0~7を出力に設定
  DDRB  =  0xFF;
  /*タイマ0
    CTCモード、タイマ0_Compare_A割込*/
  TCCR0A  =  0x01;//CTC0をHに
  TCCR0B  =  0x00
      |  1<<CS02
      |  0<<CS01
      |  0<<CS00;
  OCR0A  =  180;//CTCのMAX値の設定(180、プリスケーラ256の設定でタイマ割込間隔は7.5msec)
  TIMSK  |=  1<<OCIE0A//タイマ0CompA割込有効
      |  0<<TOIE0;
  GIMSK  |=  1<<INT0;//INT0割り込み有効
  MCUCR  |=  0<<ISC01
      |  0<<ISC00;//INTピンのLowで割り込み発生
  PORTA  =  0x00;
  PORTB  =  0x00;
  return;
}

ISR (INT0_vect)//またたきOFF（スイッチで切り替え）
{
  PORTA = 0xFF;
  PORTB = 0xFF;
}

ISR (TIMER0_COMPA_vect)//またたきをつかさどる部分。PORTAがまたたき強い、PORTBがまたたき弱い。
{
  int i;
  unsigned int c_up = 0;
  unsigned int x = twinkle_patterns;
  PORTA = 0xFF;
  PORTB = 0xFF;
  (tr)--;              //乱数更新時期の判定と実行をする。
  if(tr == 0){
    (ts)--;            //ビットシフト更新時期の判定と実行をする。
    if(ts == 0){
      port_shift();
      ts = twinkle_shift;
    }else{
      _delay_us(100);
    }
    chaos_renew();
    tr = twinkle_rate;
  }else{
    _delay_us(1100);      //それらの操作をしない場合でも、同じだけの時間waitして調整する。
  }
  while (x) {            //led_intの値とc_up（カウントアップ）の値を比較し、一致するまではON、一致したらLEDをOFFにする。全部OFFになったらループを抜けてmainに戻る。
    (c_up)++;
    for(i = 0;i <= 5;i++){
      if(led_int[i] == c_up){
        PORTA ^= 1<<bit_num[i];
        x--;
      }
    }
    for(i = 6;i <= 11;i++){
      if(led_int[i] == c_up){
        PORTB ^= 1<<bit_num[i - 6];
        x--;
      }
    }
    _delay_us(7);
  }
  return;
}

int main(void)
{
  setup();
    sei();
  for(;;){}
}
