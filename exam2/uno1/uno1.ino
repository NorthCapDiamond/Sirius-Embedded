#include <stdint.h>

#define LED_MASK 0x0F
#define BTN_FAIL 0x03
#define BTN_WIN  0x0C

uint8_t sequence[50]; 
uint8_t level = 1;
uint8_t current = 0xFF;
bool is_waiting = false;
uint8_t step = 0;

void setup() {
  Serial.begin(9600);
  DDRB |= LED_MASK;
  PORTB &= ~LED_MASK;
  
  randomSeed(analogRead(0));
  level = 1;
  sequence[0] = random(0, 4); 
}

void new_game() {
  is_waiting = false;
  level = 1;
  step = 0;
  current = 0xFF;
  sequence[0] = random(0, 4); 
  delay(1000);
}

void lose(){
  PORTB |= BTN_FAIL;
  PORTB &= ~LED_MASK;
  new_game();
  is_waiting = false;
  step = 0;
}

void win(){
  step = 0;
  current = 0xFF;
  sequence[level] = random(0,4);
  level++;

  PORTB |= BTN_WIN;
  delay(500);
  PORTB &= ~LED_MASK;
  is_waiting = false;
}

void loop() {
  if(!is_waiting){
    for(uint8_t i = 0; i < level; i++){
      delay(400);
      PORTB |= (1 << sequence[i]);
      delay(400);
      PORTB &= ~LED_MASK;
    }

    while(Serial.available()) Serial.read();
    is_waiting = true;
    current = 0xFF;
    step = 0;
  }

  if(is_waiting){
    if(Serial.available()){
      current = (uint8_t)Serial.read();
      if (current == sequence[step]){
        step++;
        current = 0xFF;
        if(step == level){
          win();
        }
      } else {
        lose();
      }
    }
  }
}
