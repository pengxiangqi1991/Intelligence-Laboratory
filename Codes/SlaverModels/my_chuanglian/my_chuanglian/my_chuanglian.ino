#include<string.h>
int pin = 2;
int pin_off = 3;
String comdata="";
String strID = "\"id\":5";//{"syn": 1001,"id":4,"packetType": 1005,"cmd": 1}
const char *id =strID.c_str();
#define DOOR_OPENING_TIME_SEC 5000
#define ON 2
#define OFF 3


int DataProcess(String str);
void OpenDoor(void);
void setup(){
  pinMode(pin,OUTPUT);
  pinMode(pin_off,OUTPUT);
  Serial.begin(115200);
  digitalWrite(pin,HIGH); 
  digitalWrite(pin_off,HIGH);   
}

void loop()
{  
  int cmdType=0;
  while (Serial.available() > 0)  
  {
    comdata += char(Serial.read());
    delay(2);
  }
  if (comdata.length() > 0)
  {
    cmdType=DataProcess(comdata);
    if(cmdType==ON)
    {
      OpenSwitch();
      Serial.println("{\"syn\":1002,\"id\":5,\"packetType\":1005,\"ack\":1,\"cmdType\":1}");
//{"syn":1002,"id":5,"packetType":1005,"cmd":1,"cmdType":1}
    }
    else if(cmdType==OFF)
    {
      CloseSwitch();
      Serial.println("{\"syn\":1002,\"id\":5,\"packetType\":1005,\"ack\":1,\"cmdType\":1}");
      //{"syn":1002,"id":5,"packetType":1005,"cmd":1,"cmdType":2}

    }

    //Serial.println(comdata);
    comdata = "";
  }


}


// //{"syn": 1001,"id":6,"packetType": 1005,"cmd": 1}
int DataProcess(String str)
{
  const char *k;
  k = str.c_str();


  //YuanCheng open door
  //{"syn":1001,"id":6,"packetType":1005,"cmd":1,"cmdType":1}
  if(NULL!=strstr(k,"\"cmdType\":1"))
  {
    if(NULL!=strstr(k,"\"syn\":1001"))
    {
      //Serial.println("\"syn\": 1001");
      if(NULL!=strstr(k,id))
        if(NULL!=strstr(k,"\"packetType\":1005"))	
          if(NULL!=strstr(k,"\"cmd\":1"))//on		
          {
            //Serial.println("check ok");
            return ON;
          }
          else if(NULL!=strstr(k,"\"cmd\":2"))//off
          {
            return OFF;	
          }
    }  
  }
  return 0;

}
void CloseSwitch(void)
{
  digitalWrite(pin_off,LOW);
  delay(100);
  digitalWrite(pin_off,HIGH);
//  Serial.println("DOOR_CLOSED");	
}

void OpenSwitch(void)
{
  digitalWrite(pin,LOW);
  delay(100);
  digitalWrite(pin,HIGH);
//  Serial.println("DOOR_OPEN");

}



