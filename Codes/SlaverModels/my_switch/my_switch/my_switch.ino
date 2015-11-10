#include<string.h>
int pin = 3;
String comdata="";
String strID = "\"id\":6";//{"syn": 1001,"id":4,"packetType": 1006,"cmd": 1}
const char *id =strID.c_str();
#define DOOR_OPENING_TIME_SEC 5000


int DataProcess(String str);
void OpenDoor(void);
void setup(){
  pinMode(pin,OUTPUT);
  Serial.begin(115200);
  digitalWrite(pin,HIGH); 

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
    if(cmdType==1)
    {
      OpenSwitch();
      Serial.println("{\"syn\":1002,\"id\":6,\"packetType\":1006,\"ack\":1,\"cmdType\":1}");

    }
    else if(cmdType==2)
    {
      Serial.println("{\"syn\":1002,\"id\":6,\"packetType\":1006,\"ack\":1,\"cmdType\":1}");
      CloseSwitch();
      //{"syn":1001,"id":6,"packetType":1006,"cmd":1,"cmdType":1}

    }

    //Serial.println(comdata);
    comdata = "";
  }


}


// //{"syn": 1001,"id":4,"packetType": 1006,"cmd": 1}
int DataProcess(String str)
{
  const char *k;
  k = str.c_str();


  //YuanCheng open door
  if(NULL!=strstr(k,"\"cmdType\":1"))
  {
    if(NULL!=strstr(k,"\"syn\":1001"))
    {
      //Serial.println("\"syn\": 1001");
      if(NULL!=strstr(k,id))
        if(NULL!=strstr(k,"\"packetType\":1006"))	
          if(NULL!=strstr(k,"\"cmd\":1"))//on		
          {
            //Serial.println("check ok");
            return 1;
          }
          else if(NULL!=strstr(k,"\"cmd\":2"))//off
          {
            return 2;	
          }
    }  
  }



  return 0;

}
void CloseSwitch(void)
{
  digitalWrite(pin,HIGH);

  //	Serial.println("DOOR_CLOSED");	
}

void OpenSwitch(void)
{
  digitalWrite(pin,LOW);
  //  Serial.println("DOOR_OPEN");

}



