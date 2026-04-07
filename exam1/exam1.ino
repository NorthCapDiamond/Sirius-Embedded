#include<stdint.h>

#define SERIAL_BPS 9600
#define TRIGGER_MASK (1 << PB1)
#define ECHO_MASK (1 << PB0)
#define TRIGGER_CLEAR ~TRIGGER_MASK
#define MEASURE_DELAY_MS 25
#define DISTANCE_FACTOR 0.01715f
#define MAX_WAIT_TIME_MICROS 40000

void setup() {
  Serial.begin(SERIAL_BPS);
  DDRB |= TRIGGER_MASK;
  DDRB &= ~ECHO_MASK;

  ADMUX = (1 << REFS0);
  ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
}

float perform_measure(){
    PORTB &= TRIGGER_CLEAR;
    delayMicroseconds(10);
    PORTB |= TRIGGER_MASK;
    delayMicroseconds(100);
    PORTB &= TRIGGER_CLEAR;

    uint32_t wait_start = micros();
    while (!(PINB & ECHO_MASK)){
      if (micros() - wait_start > MAX_WAIT_TIME_MICROS) return -1.0f;
    }

    uint32_t startTime = micros();

    wait_start = micros();
    while (PINB & ECHO_MASK){
      if (micros() - wait_start > MAX_WAIT_TIME_MICROS) return -1.0f;
    }
    uint32_t duration = micros() - startTime;

    float distance = (float)duration * DISTANCE_FACTOR;

    return distance;
}

void print_distance(float distance){
  Serial.print(F("DISTANCE: "));
  Serial.println(distance);
}

void loop(){
  if(Serial.available() > 0){
    if(Serial.read() == 'm'){
      Serial.println(F("WAIT..."));
      print_distance(perform_measure());
      delay(50);
    }
  }
}


