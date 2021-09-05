#include<SoftwareSerial.h>
#include<math.h>
#include <stdio.h>  
#include <stdlib.h> 
#include <string.h>

#include <SPI.h>
#include <RH_RF95.h>


RH_RF95 rf95;

#define pinTX 5
#define pinRX 3
//MAX485 control pin
#define transmitir  7   //pin2 is used by lora chip
#define transmitir1 4


SoftwareSerial RS485(pinRX, pinTX);
uint8_t data[9];
static byte energy[] = {0x01,0x04,0x01,0x56,0x00,0x02,0x90,0x27};

void setup() {
  // put your setup code here, to run once:
    RS485.begin(9600);
    Serial.begin(9600);
    Serial.println("Starting...");
    pinMode(transmitir, OUTPUT);
    pinMode(transmitir1, OUTPUT);
    if (!rf95.init())
      Serial.println("init failed");
    rf95.setFrequency(868.0);
    delay(2000);
}

void loop() {
  // put your main code here, to run repeatedly:
  enable_transmission();
  for(int i=0;i<8;i++){
    RS485.write(energy[i]);
  } 
  disable_transmission();
  delay(2000);
     while (RS485.available()){
      RS485.readBytes(data, 9);
     } 
    char buf3[9];
    char buf4[9];
    char buf5[9];
    char buf6[9];
    char mantissa[25];
    binaire(data[3],buf3);
    decale(buf3);
    binaire(data[4],buf4);
    decale(buf4);
    binaire(data[5],buf5);
    decale(buf5);
    binaire(data[6],buf6);
    decale(buf6);
    strcpy(mantissa ,buf4);
    strcat(mantissa ,buf5);
    strcat(mantissa ,buf6);
    exponent(buf4[0],buf3);
   int d = pow(2,7)*(buf3[0]-48)+ pow(2,6)*(buf3[1]-48)+pow(2,5)*(buf3[2]-48)+pow(2,4)*(buf3[3]-48) +pow(2,3)*(buf3[4]-48) +pow(2,2)*(buf3[5]-48)+pow(2,1)*(buf3[6]-48)+pow(2,0)*(buf3[7]-48)-127;
   float E = pow(2,d)+pow(2,d-1)*(mantissa[1]-48)+pow(2,d-2)*(mantissa[2]-48)+pow(2,d-3)*(mantissa[3]-48)+pow(2,d-4)*(mantissa[4]-48)+pow(2,d-5)*(mantissa[5]-48)+pow(2,d-6)*(mantissa[6]-48)+pow(2,d-7)*(mantissa[7]-48)+pow(2,d-8)*(mantissa[8]-48)+pow(2,d-9)*(mantissa[9]-48)+pow(2,d-10)*(mantissa[10]-48)+pow(2,d-11)*(mantissa[11]-48)+pow(2,d-12)*(mantissa[12]-48)+pow(2,d-13)*(mantissa[13]-48)+pow(2,d-14)*(mantissa[14]-48)+pow(2,d-15)*(mantissa[15]-48) +pow(2,d-16)*(mantissa[16]-48) +pow(2,d-17)*(mantissa[17]-48)+pow(2,d-18)*(mantissa[18]-48)+pow(2,d-19)*(mantissa[19]-48)+pow(2,d-20)*(mantissa[20]-48) +pow(2,d-21)*(mantissa[21]-48) +pow(2,d-22)*(mantissa[22]-48)+pow(2,d-23)*(mantissa[23]-48);
   Serial.print(" Energy :");
   Serial.print(E);
   Serial.println(" Kwh\n");

   //send data via lora
   String sent_data = String(E);
  Serial.println(E);
  int dataLength = sent_data.length(); dataLength++;
  uint8_t total[dataLength]; //variable for data to send
  sent_data.toCharArray(total, dataLength); //change type data from string ke uint8_t
  //Serial.println(data);
  if(rf95.send(total, dataLength)){
  //if(rf95.send(sent_data, dataLength)){
    Serial.println("data sent successfully"); //send data
  }
  rf95.waitPacketSent();
  delay(1000);

}



void enable_transmission(){        
     digitalWrite(transmitir, HIGH);  //Enable max485 transmission
     digitalWrite(transmitir1, HIGH);  //Enable max485 transmission
     delay(200);
 }
 void disable_transmission(){
     digitalWrite(transmitir,LOW);    //Disable max485 transmission mode
     digitalWrite(transmitir1,LOW);    //Disable max485 transmission mode
     delay(200);

 }

 //////////function decimal to binary/////////////
char* binaire(int n, char* buffer)
{
  char* buf = buffer; int rem ;
  do
  {
    rem = n%2; 
    n /= 2; 
    if (rem>9) rem+=7;
   *buf = 48+(char)rem; 
   
    buf++;
  
  } while (n>0);
  
  *buf='\0';
  strrev(buffer);
  return buffer;
}
                                                                                                                                                                                                                                                                                                                                                                              
///////////function exponent/////////////////
char* exponent (char n,char* buffer){
  
  char* buf = buffer;
  buf[0]=buf[1];
  buf[1]=buf[2]; 
  buf[2]=buf[3];
  buf[3]=buf[4];
  buf[4]=buf[5];
  buf[5]=buf[6];
  buf[6]=buf[7];
  buf[7]=n;
  buf[8]='\0';
  return buffer;
  }
  
//////////function decal//////////////////
char* decale(char* buffer)
{
  size_t a = strlen(buffer);
  char buf[9];
  for(int i=0; i<8-a ;i++) {
  buf[i]= '0';
  }
  buf[8-a]='\0';
  strcat(buf,buffer);
  strcpy(buffer,buf);

  return buffer;
  }
