#include <16F887.h>
#fuses NOWDT, HS, NOPUT, NOPROTECT
#use delay(clock=20MHz)

#define DHT11_PIN PIN_C2
#define BUTTON_MODE PIN_A1                  
#define BUTTON_INC  PIN_A2                    
#define BUTTON_GIAM  PIN_A3                    
#define BUTTON_TANG  PIN_A4                      
#define BUTTON_REST PIN_E3   
#define MOTOR_PIN PIN_C0
#define BUZZER_PIN PIN_C1

unsigned int8 humidity_int = 0;
unsigned int8 chuc, donvi;
unsigned int8 chuc_set, donvi_set;
unsigned int8 mode = 0;    
unsigned int8 humidity_set = 70; 
int1 motor_state = 0;

const unsigned int8 ma7doan[10] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99, 0x92, 0x82, 0xF8, 0x80, 0x90};

void dht11_start() {
   output_low(DHT11_PIN);
   delay_ms(18);
   output_high(DHT11_PIN);
   delay_us(30);
   input(DHT11_PIN);
}

unsigned int8 dht11_read_byte() {
   unsigned int8 i, data = 0;
   for (i = 0; i < 8; i++) {
      while (!input(DHT11_PIN)); 
      delay_us(30);
      if (input(DHT11_PIN)) {
         data |= (1 << (7 - i));
         while (input(DHT11_PIN)); 
      }
   }
   return data;
}

int1 dht11_read() {
   unsigned int8 humidity_dec, temp_int, temp_dec, checksum;
   dht11_start();

   if (!input(DHT11_PIN)) {
      delay_us(100);
      if (input(DHT11_PIN)) {
         delay_us(100);
         humidity_int = dht11_read_byte();
         humidity_dec = dht11_read_byte();
         temp_int = dht11_read_byte();
         temp_dec = dht11_read_byte();
         checksum = dht11_read_byte();

         if (checksum == (humidity_int + humidity_dec + temp_int + temp_dec)) {
            return 1;
         }
      }
   }
   return 0;
}

void giaima() {
   chuc = ma7doan[(humidity_int / 10) % 10];
   donvi = ma7doan[humidity_int % 10];
   chuc_set = ma7doan[(humidity_set / 10) % 10];
   donvi_set = ma7doan[humidity_set % 10];
}

void hienthi() {
   int i;
   for (i = 0; i < 20; i++) { 
      output_b(donvi); output_low(PIN_D3);
      delay_ms(1); output_high(PIN_D3);
      output_b(chuc); output_low(PIN_D2);
      delay_ms(1); output_high(PIN_D2);
      output_b(donvi_set); output_low(PIN_D1);
      delay_ms(1); output_high(PIN_D1);
      output_b(chuc_set); output_low(PIN_D0);
      delay_ms(1); output_high(PIN_D0);
   }
}

void handle_buzzer() { 
if (humidity_int < 25) { 
output_high(BUZZER_PIN);  
delay_ms(1000);           
output_low(BUZZER_PIN);   
} else { 
output_low(BUZZER_PIN);   
} 
} 

void kiem_tra_nut_nhan() {
   static int1 mode_pressed = 0;
   static int1 inc_pressed = 0;
   static int1 giam_pressed = 0;
   static int1 tang_pressed = 0;
   
   if (!input(BUTTON_MODE) && !mode_pressed) {
      delay_ms(50);
      if (!input(BUTTON_MODE)) {
         mode = (mode + 1) % 3;
         mode_pressed = 1;
      }
   }
   if (input(BUTTON_MODE)) mode_pressed = 0;
   
   if (!input(BUTTON_INC) && !inc_pressed) {
      delay_ms(50);
      if (!input(BUTTON_INC)) {
       if (mode == 1) { 
         motor_state = !motor_state;  
         output_bit(MOTOR_PIN, motor_state );       
        inc_pressed = 1;
      }
   }
   }
   if (input(BUTTON_INC)) inc_pressed = 0;
   

   if (mode == 2) {
      if (!input(BUTTON_GIAM) && !giam_pressed) {
         delay_ms(50);
         if (!input(BUTTON_GIAM)) {
            if (humidity_set > 0) humidity_set--;
            giam_pressed = 1;
         }
      }
      if (input(BUTTON_GIAM)) giam_pressed = 0;

      if (!input(BUTTON_TANG) && !tang_pressed) {
         delay_ms(50);
         if (!input(BUTTON_TANG)) {
            if (humidity_set < 99) humidity_set++;
            tang_pressed = 1;
         }
      }
      if (input(BUTTON_TANG)) tang_pressed = 0;
   }
}

void dieu_khien_bom() {
   if (mode == 0) { 
      if (humidity_int < humidity_set) output_high(MOTOR_PIN);
      else output_low(MOTOR_PIN);
   } 

}

void main() {
   set_tris_b(0x00);
   set_tris_d(0x00);
   set_tris_a(0x0F);
   set_tris_c(0x00);
   output_high(DHT11_PIN);
   output_low(MOTOR_PIN);

   while (true) {
      kiem_tra_nut_nhan();
      if (dht11_read()) giaima();
      handle_buzzer();
      dieu_khien_bom();
      hienthi();
   }
}

