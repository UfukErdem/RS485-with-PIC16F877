#include <16F877a.h>
#fuses HS, NOWDT, NOLVP, NOBROWNOUT, NOPROTECT, PUT
#use delay(clock=20000000)
#include <my_rs485.c>

#define RS485_ID 0x02 // Slave2 Cihazýn Adresi
#define RS485_SLAVE



// Fonksiyon Prototipleri //
void config_Ports(void);
int8 get_Coils(void);
int8 crc;


// Deðiþkenler //
rs485_RxMailbox rs485Message;
int8 coils_Data;
volatile int8 error_Kontrol = 0;

void main()
{
   config_Ports();
   
   while(TRUE)
   {       
      rs485Message.ID = 0;
      getData();  // Data Alma
      if(check_ID(rs485Message.ID,RS485_ID) == 1)// Cihaz ID kontrolü
      {  
         if(check_CRC(rs485Message.ID,rs485Message.len,rs485Message.data,rs485Message.crc) == 1)
         {    
            send_ACK(2);// ACK Kontrolü
            output_d(rs485Message.data);
            coils_Data = get_Coils();
            set_Sender();
            crc = calc_CRC(0x09,0x01,coils_Data);
            sendData(0x09,0x01, coils_Data, crc); 
            output_high(PIN_A0);
            output_low(PIN_A1);
         }
         else
         {
            send_ACK(2);
            output_low(PIN_A0);
            output_high(PIN_A1);
         }
          output_low(PIN_E0);
      }
      set_Listener();
     
   }  
}
// Pin Konfigürasyonlarý
void config_Ports(void)
{
   set_tris_d(0x00);
   output_d(0x00);
   set_tris_a(0x00);
   output_a(0x00);
   set_tris_c(0xFF);
   output_c(0x00);
}
// Coil Çýkýþlarýnýn Hesabý
int8 get_Coils(void)
{
   int8 coils_Data = 0;  
   if(input(PIN_C0))
      coils_Data += 1;
   if(input(PIN_C1))
      coils_Data += 2; 
   if(input(PIN_C2))
      coils_Data += 4;    
   if(input(PIN_C3))
      coils_Data += 8; 
   if(input(PIN_C4))
      coils_Data += 16;  
   if(input(PIN_C5))
      coils_Data += 32; 
   if(input(PIN_C6))
      coils_Data += 64;
   if(input(PIN_C7))
      coils_Data += 128;  
      
   return coils_Data;
}
