#include<stdint.h>

#define PORTB_OUT 0x3F
#define PORTB_CLEAR ~PORTB_OUT
#define PLUS_BTN_MASK 0x01
#define MINUS_BTN_MASK 0x02
#define READ_BTNS_MASK 0x03

static uint8_t start_num = 5;
static uint8_t last_btns_state = 0x3;

void setup() {
  PORTB = (PORTB & PORTB_CLEAR) | (start_num & PORTB_OUT);
  DDRB |= PORTB_OUT;
  DDRD &= ~((1 << PD2) | (1 << PD3));
  PORTD |= ((1 << PD2) | (1 << PD3));
}

void loop(){
  uint8_t current_btns_state = (PIND >> 2) & READ_BTNS_MASK;

  if((last_btns_state & PLUS_BTN_MASK) && !(current_btns_state & PLUS_BTN_MASK)){
    if(start_num < 63){
      start_num++;
    }
  }

  if((last_btns_state & MINUS_BTN_MASK) && !(current_btns_state & MINUS_BTN_MASK)){
    if(start_num > 0){
      start_num--;
    }
  }

  last_btns_state = current_btns_state;

  PORTB = (PORTB & PORTB_CLEAR) | (start_num & PORTB_OUT);

  delay(20);
}

