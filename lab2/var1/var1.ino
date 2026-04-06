#include<stdint.h>

#define SERIAL_BPS 9600
#define VOLTAGE_WITH_CONV 5000
#define MAX_VALUE 1024
#define MEASURE_DELAY 500
#define ALPHA 0.2
#define NALPHA (1.0 - ALPHA)
#define MEASURES 30

uint16_t raw_data = 0;
int16_t temp_x10 = 0;
uint32_t last_measure_time = 0;

uint8_t sample_index = 0;
int16_t samples[MEASURES];
float filtered_temp = 0;
bool is_first_measure = true;

void setup() {
  Serial.begin(SERIAL_BPS);
  ADMUX = (1 << REFS0);
  ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);
}

void loop(){
  if (sample_index < MEASURES){
    if(millis() - last_measure_time >= MEASURE_DELAY){
      last_measure_time = millis();
      ADCSRA |= (1 << ADSC);
      while(ADCSRA & (1 << ADSC));
      raw_data = ADC;
      temp_x10 = ((uint32_t)raw_data * VOLTAGE_WITH_CONV) / MAX_VALUE;
      
      if(is_first_measure){
        filtered_temp = temp_x10;
        samples[sample_index] = (int16_t) filtered_temp;
        sample_index++;
        is_first_measure = false;
      } else {
        filtered_temp = (NALPHA * filtered_temp) + (ALPHA * temp_x10);
        samples[sample_index] = (int16_t)filtered_temp;
        sample_index++;
      }
    }
  } else {
    Serial.println(F("START_BATCH"));
    for(uint16_t i = 0; i < MEASURES; i++){
      Serial.print(samples[i] / 10);
      Serial.print('.');
      Serial.print(samples[i] % 10);
      Serial.print('\n');
    }

    Serial.println(F("END_BATCH"));
    
    for(uint8_t i = 20; i > 0; i--){
      Serial.print(F("RESTART IN "));
      Serial.println(i);
      delay(1000);
    }

    sample_index = 0;
    is_first_measure = true;
  }
}
