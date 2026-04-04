#include<stdint.h>

#define PORTB_OUT 0x3F
#define PORTB_CLEAR ~PORTB_OUT
#define PLUS_BTN_MASK 0x01
#define MINUS_BTN_MASK 0x02
#define READ_BTNS_MASK 0x03
#define MIN_DELAY 50
#define MAX_DELAY 1000
#define DELAY_STEP 50
#define DEBOUNCE_INTERVAL 20

static uint8_t last_btns_state = 0x3;
static uint16_t current_delay = 500;
static uint16_t current_step = 1;
static uint32_t last_debounce_time = 0;
static uint32_t last_update_time = 0;
void setup() {
  PORTB = PORTB & PORTB_CLEAR;
  DDRB |= PORTB_OUT;
  DDRD &= ~((1 << PD2) | (1 << PD3));
  PORTD |= ((1 << PD2) | (1 << PD3));
}

int switch_led(uint8_t curr){
  if (curr == 0){
    return 1;
  }
  if (curr >= 32){
    return 0;
  } 
  
  return curr*2;
}

void loop(){
  uint32_t current_time = millis();
  uint8_t current_btns_state = (PIND >> 2) & READ_BTNS_MASK;

  if((last_btns_state & PLUS_BTN_MASK) && !(current_btns_state & PLUS_BTN_MASK) && current_time - last_debounce_time >= DEBOUNCE_INTERVAL){
    if(current_delay < MAX_DELAY){
      current_delay += DELAY_STEP;
      last_debounce_time = current_time;
    }
  }

  if((last_btns_state & MINUS_BTN_MASK) && !(current_btns_state & MINUS_BTN_MASK) && current_time - last_debounce_time >= DEBOUNCE_INTERVAL){
    if(current_delay > MIN_DELAY){
      current_delay -= DELAY_STEP;
      last_debounce_time = current_time;
    }
  }

  last_btns_state = current_btns_state;


  if (current_time - last_update_time >= current_delay){
    PORTB = (PORTB & PORTB_CLEAR) | (current_step & PORTB_OUT);
    current_step = switch_led(current_step);
    last_update_time = current_time;
  }
}

