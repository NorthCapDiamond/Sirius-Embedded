#include <stdint.h>

#define BTNS_MASK 0x3C
#define DEBOUNCE_INTERVAL_MS 50

uint8_t last_btns_state = BTNS_MASK;
uint32_t last_debounce_time = 0;
uint8_t stable_btns_state = BTNS_MASK;

void setup() {
  Serial.begin(9600);
  DDRD &= ~BTNS_MASK;
  PORTD |= BTNS_MASK;

  ADMUX = (1 << REFS0);
  ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
}

void send_number(uint8_t button){
  Serial.write(button);
}

void loop(){
  uint8_t current_btns_state = PIND & BTNS_MASK; 
  
  if(last_btns_state != current_btns_state){
    last_debounce_time = millis();
    last_btns_state = current_btns_state;
  }
  
  if (millis() - last_debounce_time >= DEBOUNCE_INTERVAL_MS) {
    if (current_btns_state != stable_btns_state) {    
      for (uint8_t i = 0; i < 4; i++) {
        uint8_t pin_bit = (1 << (i + 2)); 
        if (!(current_btns_state & pin_bit) && (stable_btns_state & pin_bit)) {
          send_number(i);
        }
      }
      stable_btns_state = current_btns_state;
    }
  }
}
