// 
//   FILE:  dht11_test1.pde
// PURPOSE: DHT11 library test sketch for Arduino
//
#include <dht11.h>
#include <Wire.h>
#include <math.h>
#define DHT11_PIN 3

dht11 DHT;
int BH1750address = 0x23;//BH1750 I2C地址
byte buff[2];

void setup(){
  Wire.begin();
  Serial.begin(115200);
//  Serial.println("DHT TEST PROGRAM ");
//  Serial.print("LIBRARY VERSION: ");
//  Serial.println(DHT11LIB_VERSION);
//  Serial.println();
//  Serial.println("Type,\tstatus,\tHumidity (%),\tTemperature (C)");
}

void loop(){
  int chk;
//  Serial.print("DHT11, \t");
  chk = DHT.read(DHT11_PIN);    // READ DATA
 /*  switch (chk){
    case DHTLIB_OK:  
                Serial.print("OK,\t"); 
                break;
    case DHTLIB_ERROR_CHECKSUM: 
                Serial.print("Checksum error,\t"); 
                break;
    case DHTLIB_ERROR_TIMEOUT: 
                Serial.print("Time out error,\t"); 
                break;
    default: 
                Serial.print("Unknown error,\t"); 
                break;
  }
 // DISPLAT DATA
*/
 
  //{"syn": 1000,"packetType": 1001,"humidity": "75%"}
  Serial.print("{\"syn\": 1000,\"id\":2,\"packetType\": 1001,\"humidity\": \"");

  Serial.print(DHT.humidity,1);
  Serial.print("\"}\n");
  
  delay(60000);
  

  
  
  int sensorReading = readLightSensor();
  Serial.print("{\"syn\": 1000,\"id\":2,\"packetType\": 1007,\"lightsensor\": \"");
  Serial.print(sensorReading,1);
  Serial.print("\"}\n");


  delay(60000);
  //{"syn": 1000,"packetType": 1002,"temperature": "24.5"}
  Serial.print("{\"syn\": 1000,\"id\":2,\"packetType\": 1002,\"temperature\": \"");
  Serial.print(DHT.temperature,1);
  Serial.print("\"}\n");
  
  delay(60000);

}

///////////////////////////////////////////////////////////////////////////
// get data from light sensor
// you can replace this code for your sensor
int readLightSensor()
{
  uint16_t val=0;
  BH1750_Init(BH1750address);
  delay(200);
  if(2==BH1750_Read(BH1750address))
  {
    val=((buff[0]<<8)|buff[1])/1.2;
  }

//  Serial.print("Sensor value is: ");
//  Serial.println((int)val);

  return val;
}

int BH1750_Read(int address) //
{
  int i=0;
  Wire.beginTransmission(address);
  Wire.requestFrom(address, 2);
  while(Wire.available()) //
  {
    buff[i] = Wire.read();  // receive one byte
    i++;
  }
  Wire.endTransmission();
  return i;
}



void BH1750_Init(int address)
{
  Wire.beginTransmission(address);
  Wire.write(0x10);//1lx reolution 120ms
  Wire.endTransmission();
}




