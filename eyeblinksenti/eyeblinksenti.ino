//                                                         //
//    Program       : Mindwave with Arduino                //
//    Interfacing   : HC-05 Bluetooth Module               //
//    Output        : Eye Blink Control LED                //
#include <SoftwareSerial.h>
SoftwareSerial bt(2, 3);
#define   BAUDRATE           57600
#define   LED                8
#define   Theshold_Eyeblink  110
#define   EEG_AVG            70

long payloadDataS[5] = {0};
long payloadDataB[32] = {0};
byte checksum=0,generatedchecksum=0;
unsigned int Raw_data,Poorquality,Plength,Eye_Enable=0,On_Flag=0,Off_Flag=1 ;
unsigned int j,n=0;
long Temp,Avg_Raw,Temp_Avg;

 void setup() 
 {
   Serial.begin(BAUDRATE);           // USB
   bt.begin(BAUDRATE);
   pinMode(LED, OUTPUT);
 }

 byte ReadOneByte()           // One Byte Read Function
 {
   int ByteRead;
   while(!bt.available());
  ByteRead = bt.read();
   return ByteRead;
 }

 void loop()                     // Main Function
 {
   if(ReadOneByte() == 170)        // AA 1 st Sync data
   {
     if(ReadOneByte() == 170)      // AA 2 st Sync data
     {
       Plength = ReadOneByte();
       if(Plength == 4)   // Small Packet
       { 
         Small_Packet ();
       }
       else if(Plength == 32)   // Big Packet
       { 
         Big_Packet ();
       }
     }
   }         
 }
 
 void Small_Packet ()
 {
   generatedchecksum = 0;
   for(int i = 0; i < Plength; i++) 
   {  
     payloadDataS[i]     = ReadOneByte();      //Read payload into memory
     generatedchecksum  += payloadDataS[i] ;
   }
   generatedchecksum = 255 - generatedchecksum;
   checksum  = ReadOneByte();
   if(checksum == generatedchecksum)        // Varify Checksum
   {  
     if (j<512)
     {
        Raw_data  = ((payloadDataS[2] <<8)| payloadDataS[3]);
       if(Raw_data&0xF000)
       { 
         Raw_data = (((~Raw_data)&0xFFF)+1); 
       }
       else
       { 
         Raw_data = (Raw_data&0xFFF); 
       }
       Temp += Raw_data;
       j++; 
     }
     else 
     { 
       Onesec_Rawval_Fun ();
     }
   }
 }
 
 void Big_Packet()
 {
   generatedchecksum = 0;
   for(int i = 0; i < Plength; i++) 
   {  
     payloadDataB[i]     = ReadOneByte();      //Read payload into memory
     generatedchecksum  += payloadDataB[i] ;
   }
   generatedchecksum = 255 - generatedchecksum;
   checksum  = ReadOneByte();
   if(checksum == generatedchecksum)        // Varify Checksum
   {
     Poorquality = payloadDataB[1];
     if (Poorquality==0 )
     {
       Eye_Enable = 1; 
     }
     else
     { 
       Eye_Enable = 0;
       Serial.println("POOR QUALITY");
     }
   }
 }
 
 void Onesec_Rawval_Fun ()
 {
   Avg_Raw = Temp/512;
   if (On_Flag==0 && Off_Flag==1)
   {
     if (n<3)
     {
       Temp_Avg += Avg_Raw;
       n++;
     }
     else
     {
       Temp_Avg = Temp_Avg/3;
       if (Temp_Avg<EEG_AVG) 
       {
         On_Flag=1;Off_Flag=0;
       }
       n=0;Temp_Avg=0;
     }  
   }              
   Eye_Blink ();
   j=0;
   Temp=0; 
 }
 
 
 void Eye_Blink ()
 {
   if (Eye_Enable)          
   {
     if (On_Flag==1 && Off_Flag==0) 
     { 
       if ((Avg_Raw>Theshold_Eyeblink) && (Avg_Raw<350))
       { 
         digitalWrite(LED,HIGH); 
         Serial.println("EYE BLINKED");
       }
       else
       {
         if (Avg_Raw>350)
         {
           On_Flag==0;Off_Flag==1;
         }
         digitalWrite(LED,LOW); 
       }
     }
     else
     { 
       digitalWrite(LED,LOW); 
     }
   }        
   else
   { 
     digitalWrite(LED,LOW); 
   }
 }
