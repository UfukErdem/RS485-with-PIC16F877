#include <16F877a.h>
#fuses HS, NOWDT, NOLVP, NOBROWNOUT, NOPROTECT, PUT
#use delay(clock=20000000)
#include <my_rs485.c>

#define RS485_ID 0x03 // Slave3 Cihazýn Adresi
#define RS485_SLAVE


// Fonksiyon Prototipleri //
void config_PWM(void);
int8 crc; 

// Deðiþkenler //
rs485_RxMailbox rs485Message;
volatile int8 error_Kontrol = 0;

void main()
{  
   config_PWM();
   while(TRUE)
   {       
      rs485Message.ID = 0;
      getData();  // Data Alma
      if(check_ID(rs485Message.ID,RS485_ID) == 1)// Cihaz ID kontrolü
      {         
         if(check_CRC(rs485Message.ID,rs485Message.len,rs485Message.data,rs485Message.crc) == 1)
         {   
            send_ACK(3);// ACK Kontrolü
            set_pwm1_duty(rs485Message.data);
            set_Sender();
            crc = calc_CRC(0x09,0x01,rs485Message.data);
            sendData(0x09,0x01, rs485Message.data, crc);    
            output_high(PIN_D0);
            output_low(PIN_D1);
            output_high(PIN_D2);
            output_low(PIN_D3);
         }
         else
         {
            send_ACK(3);// ACK Kontrolü
            output_low(PIN_D0);
            output_high(PIN_D1);
            output_low(PIN_D2);
            output_high(PIN_D3);
         }
         output_low(PIN_E0);
      }
      set_Listener();
      
   }  
}
// PWM ve Pin Konfigürasyonlarý
void config_PWM(void)
{
   setup_timer_2(T2_DIV_BY_16,254,1);   // pwm periyodunu 4.08ms ( 250 Hz ) olarak ayarlandý.
   setup_CCP1(CCP_PWM); 
   set_pwm1_duty(0);
   set_tris_d(0x00);
   output_d(0x00);
   output_high(PIN_D0);
   output_low(PIN_D1);  
}
