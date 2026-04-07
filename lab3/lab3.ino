#include<stdint.h>

#define SERIAL_BPS 9600
#define DEBOUNCE_DELAY_MS 50
#define BTN_MASK (1 << PD2)
#define POLYNOMIAL 0x07

uint8_t last_btn_state = BTN_MASK;
uint8_t stable_btn_state = BTN_MASK;
uint32_t last_debounce_time = 0;
uint8_t crc8 = 0x00;

void setup() {
  Serial.begin(SERIAL_BPS);
  ADMUX = (1 << REFS0);
  ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

  DDRD &= ~BTN_MASK; 
  PORTD |= BTN_MASK;

  Serial.println("ENTER DATA TO UPDATE CRC-8 OR CLICK BUTTON TO GET CURRENT VALUE");
  delay(100);
}

void update_crc8(uint8_t data){
  crc8 ^= data;
  
  for(uint8_t i = 0; i < sizeof(crc8); i++){
    if(crc8 & 0x80){
      crc8 = (crc8 << 1) ^ POLYNOMIAL; 
    } else {
      crc8 <<= 1;
    }
  }
}

void loop(){
  uint8_t current_btn_state = PIND & BTN_MASK; 
  if(last_btn_state != current_btn_state){
    last_debounce_time = millis();
    last_btn_state = current_btn_state;
  }

  if (millis() - last_debounce_time >= DEBOUNCE_DELAY_MS){
    if(current_btn_state != stable_btn_state){
      stable_btn_state = current_btn_state;

      if(!current_btn_state){
        print_crc8();
      }
    }
  }


  if(Serial.available() > 0){
    update_crc8((uint8_t) Serial.read());
  }
}

void print_crc8(){
  Serial.print(F("Current CRC-8: "));
  Serial.println(crc8);
}
