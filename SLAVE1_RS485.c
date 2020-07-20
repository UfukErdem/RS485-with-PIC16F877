#include <16F877a.h>
#device ADC = 8
#fuses HS, NOWDT, NOLVP, NOBROWNOUT, NOPROTECT, PUT
#use delay(clock=20000000)
#include <my_rs485.c>

#define RS485_ID 0x01 // Slave1 Cihazýn Adresi
#define RS485_SLAVE

// Fonksiyon Prototipleri //
void config_ADC(void);

// Deðiþkenler //
rs485_RxMailbox rs485Message;
int8 pot_Val;
int8 crc;
volatile int8 error_Kontrol = 0;
// Sensör Bilgisi Okuma
#int_ad
void ADC_IRQHandler()
{
   set_adc_channel(0);
   delay_us(10);
   pot_Val = read_adc();
   set_Sender();// RS485 Veri Gönderme Modu
   crc = calc_CRC(0x09,0x01,pot_Val); //CRC Hesaplama
   sendData(0x09,0x01, pot_Val, crc); // Master Cihaza Data Gönderme
   set_Listener(); //RS485 Veri Dinleme Modu
}

void main()
{
   config_ADC();
   
   while(TRUE)
   {       
      rs485Message.ID = 0;
      getData(); // Data Alma  
      if(check_ID(rs485Message.ID,RS485_ID) == 1) // Cihaz ID kontrolü
      {      
         if(check_CRC(rs485Message.ID,rs485Message.len,rs485Message.data,rs485Message.crc) == 1)
         {
            send_ACK(1); // ACK Kontrolü            
            set_adc_channel(0);
            delay_us(10);
            pot_Val = read_adc();
            output_high(PIN_D0);
            output_low(PIN_D1);
         }
         else
         {
            send_ACK(1); // ACK Kontrolü 
            delay_ms(30);
            output_low(PIN_D0);
            output_high(PIN_D1);
         }
         output_low(PIN_E0);
      }     
      set_Listener();
      
   }  
}
// Kesme ve Pin Konfigürasyonlarý
void config_ADC(void)
{
   set_tris_a(0xFF);
   set_tris_d(0x00);
   output_d(0x00);
   enable_interrupts(GLOBAL);
   enable_interrupts(INT_AD);
   setup_adc(ADC_CLOCK_DIV_32);
   setup_adc_ports(AN0);  
}

