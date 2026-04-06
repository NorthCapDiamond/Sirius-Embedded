#include<stdint.h>
#include<math.h>

#define SERIAL_BPS 9600
#define BUF_SIZE 10
#define DEBOUNCE_DELAY_MS 50
#define BTN_MASK (1 << PD2)

uint16_t buffer[BUF_SIZE];
uint8_t counter = 0;
uint8_t idx = 0;

uint32_t last_measure_time = 0;
uint8_t last_btn_state = BTN_MASK;
uint8_t stable_btn_state = BTN_MASK;
uint32_t last_debounce_time = 0;

void setup() {
  Serial.begin(SERIAL_BPS);
  ADMUX = (1 << REFS0);
  ADCSRA = (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2);

  DDRD &= ~BTN_MASK; 
  PORTD |= BTN_MASK;
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
        save_to_buf(read_adc());
      }
    }
  }


  if(Serial.available() > 0){
    if(Serial.read() == 'p'){
      present_results();
    }
  }
}

uint16_t read_adc(){
  ADCSRA |= (1 << ADSC);
  while(ADCSRA & (1 << ADSC));
  return (uint16_t) ADC;
}

void save_to_buf(uint16_t value){
  buffer[idx] = value;
  idx = (idx+1) % BUF_SIZE;
  if (counter < BUF_SIZE){ counter++; }
  Serial.print(F("SAVED: "));
  Serial.println(value);
}

void present_results(){
  Serial.println(F("STATISTIC REPORT:"));
  
  if (!counter){
    Serial.println(F("NO DATA TO SHOW"));
    return;
  }

  float sum = 0;

  for(uint8_t i = 0; i < counter; i++){
    sum += buffer[i];
  }

  float avg = sum / counter;
  float variance_sum = 0;

  for(uint8_t i = 0; i < counter; i++){
    variance_sum += pow(buffer[i] - avg, 2);
  }

  float sko = sqrt(variance_sum / counter);

  Serial.print(F("AVERAGE: "));
  Serial.println(avg);

  Serial.print(F("SKO: "));
  Serial.println(sko);

  Serial.println(F("VALUES:"));

  for(uint8_t i = 0; i < counter; i++){
    if (i && i % 5 == 0){
      Serial.println();
    }

    Serial.print(buffer[i]);
    Serial.print(" ");
  }

  Serial.println();
  Serial.println(F("REPORT FINISHED"));
}
