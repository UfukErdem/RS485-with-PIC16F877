// Ön Ýþlemci makro konfigürasyonlarý //

#ifndef RS485_BUFFER_SIZE
#define RS485_BUFFER_SIZE  5 
   int8 rs485_RxMessage[RS485_BUFFER_SIZE];
#else
   int8 rs485_RxMessage[RS485_BUFFER_SIZE];
#endif

#ifndef RS485_BAUD
#define RS485_BAUD 9600
#endif

#ifndef RS485_PINS_CONFIG
#define RS485_PINS_CONFIG

#define RS485_RX          PIN_B2  
#define RS485_TX          PIN_B3 
#define RS485_ENABLE      PIN_B4

#use rs232(baud=RS485_BAUD, xmit=RS485_TX, rcv=RS485_RX, enable=RS485_ENABLE, bits=8, parity=N, stop=1, stream=RS485)

#endif

void set_Sender()// RS485 Gönderme Moduna geçme.
{
   output_high(RS485_TX); 
   output_low(RS485_RX); 
   output_high(RS485_ENABLE);
}

void set_Listener()// RS485 Alým Moduna geçme.          
{
   output_low(RS485_TX); 
   output_high(RS485_RX); 
   output_low(RS485_ENABLE);
}

void config_ACK_Master()// Master ACK Kontrolü Konfigürasyonu
{
   set_tris_e(0xff);
   output_e(0x00);
}

void config_ACK_Slave()// Slave ACK Kontrolü Konfigürasyonu
{
   set_tris_e(0x00);
   output_e(0x00);
}
//ACK Master/Slave Konfigürasyonu
#ifndef RS485_SLAVE
#define RS485_MASTER
   set_Sender();
   config_ACK_Master();
#else
   set_Listener();
   config_ACK_Slave();
#endif

void sendData(int8 ID, int8 len, int8 data, int8 crc)//Data gönderme Fonksiyonu 
{
  
   if(!input(RS485_RX))
   {
      putc(ID); putc(len); putc(data); putc(crc); 
   }  
}

// RS485 Data Frame Struct.
typedef struct{ 

   int8 ID;
   int8 len;
   int8 data;
   int8 crc;

}rs485_RxMailbox;

extern rs485_RxMailbox rs485Message; // Master/Slave 'de tanýmlanmýþ deðiþken, my_rs485'e taþýndý.
extern volatile int8 error_Kontrol = 0;
void getData() // Data Alma Fonksiyonu
{
   int sayac = 0;
   int32 sayac1 = 0;
   //Data gelmesini bekle.
    while(true)
    {
      if(error_Kontrol == 1) {break;}
      if(sayac >= 4 )   {  sayac = 0; sayac1 = 0; break; output_low(PIN_E0);}//|| sayac1 >= 9000
      if(kbhit())
      {         
         rs485_RxMessage[sayac++] = getc();
         
      }
      else
         sayac1++;
    }
    rs485Message.ID    = rs485_RxMessage[0];
    rs485Message.len   = rs485_RxMessage[1];
    rs485Message.data  = rs485_RxMessage[2];
    rs485Message.crc   = rs485_RxMessage[3];
    
}

int8 check_ID(rs485_RxMailbox gelen,int8 ID) // Message Id kontrolü yapýlýr.
{
   if(gelen.ID == ID)                   {  return 1;   }
   else                                 {  return 0;   }
}   

// Donanýmsal ACK
void send_ACK(int8 cihaz)// Slave'den gönderilen verinin Master'a ulaþýp ulaþmadýðýnýn kontrolü yapýldý.
{
   switch(cihaz)   
   {
      case 1:
         output_high(PIN_E0);        
      case 2:
         output_high(PIN_E0);
      break;      
      case 3:
         output_high(PIN_E0);
      break;   
   }
}
int8 get_ACK(int8 cihaz)// Gönderilen verinin Master kýsmýnda hangi Slave'den geldiðinin kontrolü.
{  
   switch(cihaz)   
   {
      case 1:
         if(input(PIN_E0))    {return 1;}   
         else                 {return 0;} 
      break;
      
      case 2:
         if(input(PIN_E1))    {return 1;}   
         else                 {return 0;} 
      break;
      
      case 3:
         if(input(PIN_E2))    {return 1;}   
         else                 {return 0;} 
      break;   
   } 
}

int8 calc_CRC(int8 ID, int8 len, int8 data)//CRC Hesabý 
{
   int16 toplam = 0;
   int8  kalan  = 0;
   toplam += ID;
   toplam += len;
   toplam += data;  
   kalan = toplam % 13;  
   return kalan;
}
int8 check_CRC(int8 ID, int8 len, int8 data,int8 crc)//CRC Kontrolü 
{
   int16 toplam = 0;
   toplam += ID;
   toplam += len;
   toplam += data;
   toplam -= crc;
   
   if(toplam%13 == 0)
   return 1;
   else
   return 0;
}

