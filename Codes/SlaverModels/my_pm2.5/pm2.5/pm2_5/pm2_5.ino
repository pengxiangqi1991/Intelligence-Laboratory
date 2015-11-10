/*粉尘头的代码
                    Pro mini~~~GP2Y1010AU0F：
                    5V-150欧-（V-LED）
                    5V-150欧—Vcc
                    5V-150欧-220uF-GND
                    GND-（LED-GND）
                    GND-（S-GND）
                    Pin2-LED
                    A0-Vo
 */
 int dustPin=0;//数字口p2控制模拟口A0读取粉尘电压
 int dustVal=0;
 int ledPower=2;
 int delayTime=280;
 int delayTime2=40;
 float offTime=9680;

 //定义一个只有2个元素的数组
 int mySensVals [2];
 //定义2个布尔变量，通过i的变化让粉尘电压值先进先出循环填入上数组，实现相邻采样平均
 boolean i=0,j=0;

 //引入millis()来控制循环
 unsigned long time1=millis();


 //pm2.5的数值、AQI指数、灰尘数，相邻采样平均电压
 float dustValPM25,AQI,dustnumber,averagedustVal;

 void setup(){
   Serial.begin(115200);

   pinMode(ledPower,OUTPUT);//开启粉尘的控制端口

 }

 void loop(){

   //最初的3秒内获得mySensVals [0]，第3秒的时候就可以计算averagedustVal
   if(!j){
     /*粉尘头控制代码*/
     digitalWrite(ledPower,LOW); //开灯
     delayMicroseconds(delayTime);
     dustVal=analogRead(dustPin); //读电压
     delayMicroseconds(delayTime2);
     digitalWrite(ledPower,HIGH); //关灯
     delayMicroseconds(offTime);

     mySensVals [i]=dustVal;//把检测到的第一次电压塞给mySensVals [0]
     i=!i;//i值反转
     j=1;//不再执行这个程序

   }


   /*粉尘头取得的粉尘对应电压dustVal,每3秒循环一次*/

   if (millis() - time1 > 3000){
     time1=millis();

     /*粉尘头控制代码*/
     digitalWrite(ledPower,LOW); //开灯
     delayMicroseconds(delayTime);
     dustVal=analogRead(dustPin); //读电压
     delayMicroseconds(delayTime2);
     digitalWrite(ledPower,HIGH); //关灯

     /*邻采样平均averagedustVal的计算*/
     mySensVals [i]=dustVal;//把检测到的第一次电压塞给mySensVals [1]
     i=!i;//i值反转
     averagedustVal=(mySensVals [0]+mySensVals [1])*0.5;//计算平均电压



     /*计算0.01 cubic feet的颗粒数：（v-0.0356）X120000
      对于电压低于8个单位的用线性，搞定值以下颗粒数为负数的情况*/

     if(averagedustVal>8)
       dustnumber=(averagedustVal*5/1024-0.0356)*120000;
     else
       dustnumber=averagedustVal*52;


     /*计算pm2.5的数值 电压阀值以上用：(v*0.172-0.0999)*1000，阀值以下用线性，参考国外帖子，认为输出最低17，对应粉尘5000*/

     //0.9V【184.5】线性pm：电压单位*0.298373984进行计算，依据为手册上无灰尘也有0.9V的电压
     //0.78V【161】线性pm：电压单位*0.21931677行计算，颗粒数降低数/粉尘降低数值最大值
     //0.69V【142】线性pm：电压单位*0.136338028行计算，颗粒数/粉尘值增速加快速上升
     //((averagedustVal*5/1024)*0.172-0.0999)*1000公式实质是0.83984375*averagedustVal-99.9

     if(averagedustVal>184.5)
       dustValPM25=0.83984375*averagedustVal-99.9;
     else
       dustValPM25=averagedustVal*0.298373984;


     /*AQI的计算，美国标准*/

     if(dustValPM25<15.4)
       AQI=(50-0)/(15.4-0)*(dustValPM25-0)+0;
     else if(dustValPM25<40.4)
       AQI=(100-51)/(40.4-15.5)*(dustValPM25-15.5)+51;
     else if(dustValPM25<65.4)
       AQI=(150-101)/(65.4-40.5)*(dustValPM25-40.5)+101;
     else if(dustValPM25<150.4)
       AQI=(200-151)/(150.4-65.5)*(dustValPM25-65.5)+151;
     else if(dustValPM25<250.4)
       AQI=(300-201)/(250.4-150.5)*(dustValPM25-150.5)+201;
     else if(dustValPM25<350.4)
       AQI=(400-301)/(350.4-250.5)*(dustValPM25-250.5)+301;
     else
       AQI=(500-401)/(500.4-350.5)*(dustValPM25-350.5)+401;

   }

   //做一个保险,对应millis()溢出归零后,millis()<time1,则3秒检测一次循环终止的情况
   if (millis() < time1){
     time1=millis();
     i=0;//重置i的值
     j=0;//重置j的值
   }

   Serial.print("{\"syn\":1000,\"id\":3,\"packetType\":1004,\"PM2.5\":\"");
  Serial.print(dustValPM25);

  Serial.print("\"}\n");
   delay(3000);

 }                                                                                                         
