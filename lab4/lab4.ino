#include<stdint.h>

#define PORTB_OUT 0x3F
#define PORTB_CLEAR ~PORTB_OUT
#define PORTD_OUT ((1 << PD6) | (1 << PD7)) 
#define PORTD_CLEAR ~PORTD_OUT
#define MEASURE_DELAY_MS 25

uint32_t last_measure_time = 0;
uint16_t rg = 0;

void write8bit(uint8_t data){
  PORTB = (PORTB & PORTB_CLEAR) | (data & PORTB_OUT);
  PORTD = (PORTD & PORTD_CLEAR) | (data & PORTD_OUT);
}

void setup() {
  PORTB = (PORTB & PORTB_CLEAR);
  PORTD = (PORTD & PORTD_CLEAR);
  DDRB |= PORTB_OUT;
  DDRD |= PORTD_OUT;

  ADMUX = (1 << REFS0);
  ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
}

uint16_t read_adc(){
  ADCSRA |= (1 << ADSC);
  while(ADCSRA & (1 << ADSC));
  return (uint16_t) ADC;
}

void loop(){
  if(millis() - last_measure_time >= MEASURE_DELAY_MS){
    rg = read_adc();
    if (rg > 0){
      rg = rg / 128 + 1;
    }

    if (rg >= 8){
      rg = 0xFF;
    } else {
      rg = (1 << rg) - 1;
    }
  
    write8bit(rg);
    last_measure_time = millis();

  }
}
