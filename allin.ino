  /*
alibahrololoumi
*/
//include//

#include "DHT.h"
#include "TimerOne.h"
#include <SoftwareSerial.h>
#define OptimalPlantTemperature 25
///definition///
#define rxPin 5
#define txPin 2
SoftwareSerial serial (rxPin, txPin);
#define RelayWaterMotor 4
#define RelayArtifitialLight 6
#define RelayFanHeater 7
#define SensorPinSoilMoisture A0 
#define analogInPinwaterlevel A1
#define analogInPinLight A2
#define DHTPIN 8     // Digital pin connected to DHT11
#define DHTTYPE DHT11   // DHT 11
DHT dht(DHTPIN, DHTTYPE);
 float sensorValue_soilMoisture = 0; 
 float Light_Pin_out=0;
 uint32_t sensorValue_water_level = 0;
 uint32_t WaterLevelPercentage;
 bool IsWater = 0;
 uint32_t Timer_counter=0;
 uint32_t Timer_Motor=0;
 uint32_t Timer_Motor_manually=0;
 uint32_t Timer_fan=0;
 uint32_t Timer_irrigation=0;
 uint32_t Timer_Artifitial_Lamp=0;
 uint32_t Timer_Artifitial_System=0;
 uint8_t TimerOfIrrigation;
 long TimerOfAL;
 uint8_t TimerLamp=0;

 //uint8_t Second = 0;
 //uint8_t Minute = 0;
 //uint8_t hour = 0;
 bool FlagWaterMotor = 0;
 bool FlagWaterMotorManually = 0;
 bool FlagIrrigationManually = 0;
 bool FlagFan = 0;
 bool FlagIrrigation = 0;
 bool FlagAL = 0;
 bool FlagALsystem = 0;
String message;
int water_time_status = 0;
int water_volume_status = 0;
int light_time_status=0;
int light_duration_status=0;
int lowest_temp_status= 0;
int manualcommand_status=0;
int water_time_Action = 0;
int water_volume_Action = 0;
int light_time_Action=0;
int light_duration_Action=0;
int lowest_temp_Action= 0;
int manualcommand_Action=0;
String send_to_8266_monitoring="";
String soil="";
String water="";
String light="";
String temp="";
String hum="";
float soil_Pin_voltage;
float h,t,f;
float hic;
////
 
void setup() { 
  Serial.begin(9600); 
  serial.setTimeout(100);
  Serial.println("Initializing...");
  dht.begin();
  Timer1.initialize(1000000);         
  Timer1.attachInterrupt(callback);  
  //Serial.println("Please wait...");
  pinMode(RelayWaterMotor,OUTPUT);  
  pinMode(RelayArtifitialLight,OUTPUT);   
  pinMode(RelayFanHeater,OUTPUT); 
  digitalWrite(RelayWaterMotor, 1);  
  digitalWrite(RelayArtifitialLight, 1);
  digitalWrite(RelayFanHeater, 1);
  Serial.println("***Arduino Uno Setup done***");
  serial.begin(9600);
  serial.setTimeout(100);
}
/*
 if (serial.available()) {
    String msg;
    msg = serial.readString();
    Serial.print(msg);
    msg = "";
  }
  serial.print(msg);
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// 

void loop() { 
  monitoring();
  recievefromesp8266();
  Action();
  //SoilMoisture();
  
  //fanheater(5,25.70);
  //WaterMotor(5);
  //IrrigationSystem(0,0,5,5);
  //ALlamp(1);
  //ArtificialLightSystem(0,0,10,0.2);
  //Serial.println(WaterLevelPercentage);
  //delay(500);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//functiones   vvvvvvv

void SoilMoisture(){
  for (int i = 0; i < 400; i++){ 
    sensorValue_soilMoisture = (unsigned long)sensorValue_soilMoisture + analogRead(SensorPinSoilMoisture); 
    //delay(1); 
  } 
  sensorValue_soilMoisture = (unsigned long)sensorValue_soilMoisture/400.00;
  soil_Pin_voltage = (unsigned long)sensorValue_soilMoisture *100.00 / (unsigned long)1024.00;  //calculate sensor output voltage
  //soil_Pin_voltage=100.00 - soil_Pin_voltage;
  //soil_Pin_voltage= soil_Pin_voltage *100.00 / 42.00;
  
//  if(soil_Pin_voltage<=0){
//    soil_Pin_voltage=0.00;
//  }
//  if(soil_Pin_voltage>=100){
//    soil_Pin_voltage=99.99;
//  }
//  Serial.print("sensorValue_soilMoisture");
//  Serial.println(soil_Pin_voltage);
  
  //soil_Pin_voltage = 100 - soil_Pin_voltage;   // baraks mikone vali khoob nis
//  Serial.print(" So_Mo_Pe: ");
//  Serial.print(soil_Pin_voltage);       //display sensor output voltage
//  Serial.println(" %");
  //delay(1000); 
}

/////////////////

void WaterLevel(){
  sensorValue_water_level = analogRead(analogInPinwaterlevel); 
  WaterLevelPercentage=(unsigned long)sensorValue_water_level*(unsigned long)100/1024;
  if(WaterLevelPercentage >= 40){
    IsWater=1;
  }
  else{
    IsWater=0;
  }
//  if(WaterLevelPercentage<10){
//    WaterLevelPercentage=10;
//  }
//  if(WaterLevelPercentage>=100){
//    WaterLevelPercentage=99;
//  }
//  Serial.print("W_L_P = " ); 
//  Serial.print(WaterLevelPercentage); 
//  Serial.print("%");
//  Serial.print("     is water : ");
//  Serial.println(IsWater); 
 //delay(1000); 
}

////////////

void TemperatureHumidity(){
  h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  // Compute heat index in Celsius (isFahreheit = false)
  hic = dht.computeHeatIndex(t, h, false);
//  Serial.print(F("Humidity: "));
//  Serial.print(h);
//  Serial.print("%");
//  Serial.print(F("%  Temperature: "));
//  Serial.print("       ");
//  Serial.print(t);
//  Serial.print(F("°C "));
//  Serial.println(" ");
  //delay(2000);
}

/////

void LightSensor(){
  Light_Pin_out = analogRead(analogInPinLight) * 100.00 / 1023.0;
  Light_Pin_out=100.00 -Light_Pin_out;
  Light_Pin_out+=10.00;
//  Serial.print("The Proportion of Light:");
//  Serial.print(Light_Pin_out);
//  Serial.println(" %");
}

////

void callback(){
  Timer_counter+=1;  
}

/////

void WaterMotor(int OnTime_Motor_sec){
  // OnTime_Motor_sec motor roshan mishe agar ab bashe
  if(FlagWaterMotor==0){
    Timer_Motor=Timer_counter;
    WaterLevel();
    Serial.print("WaterLevelPercentage: %");
    Serial.println(WaterLevelPercentage);
    if(IsWater==1){
      digitalWrite(RelayWaterMotor, 0);
      Serial.println("The plant is being watered...");
      FlagWaterMotor=1;
    }
    else{
      Serial.println("!!!! Water Level error");
      Serial.println("!!!! The motor cann't be started");
      //send message to broker
    }   
  }
  else if(Timer_counter - Timer_Motor >= OnTime_Motor_sec){
    digitalWrite(RelayWaterMotor, 1);
    Serial.println("Motor is off");
    FlagWaterMotor=0;
    //delay(1000);
  }
}


/////////////

void WaterMotorManually(int OnTime_Motor_sec){
  // OnTime_Motor_sec motor roshan mishe agar ab bashe
  if(FlagWaterMotorManually==0){
    Timer_Motor_manually=Timer_counter;
    WaterLevel();
    Serial.print("WaterLevelPercentage: %");
    Serial.println(WaterLevelPercentage);
    if(IsWater==1){
      digitalWrite(RelayWaterMotor, 0);
      Serial.println("The plant is being watered...");
      FlagWaterMotorManually=1;
    }
    else{
      Serial.println("!!!! Water Level error");
      Serial.println("!!!! The motor cann't be started");
      //send message to broker
    }   
  }
  else if(Timer_counter - Timer_Motor_manually >= OnTime_Motor_sec){
    digitalWrite(RelayWaterMotor, 1);
    Serial.println("Motor is off");
    FlagWaterMotorManually=0;
    FlagIrrigationManually=1;
    //delay(1000);
  }
}


/////////////
void IrrigationSystem(float Period_of_irrigation_hour ,float Period_of_irrigation_minute,float Period_of_irrigation_sec, int Period_of_water_motor_sec){
  
  TimerOfIrrigation = ((unsigned long)Period_of_irrigation_hour * (unsigned long)3600.00) +((unsigned long)Period_of_irrigation_minute * (unsigned long)60.00) + ((unsigned long)Period_of_irrigation_sec);
  if(FlagIrrigation==0 && FlagWaterMotor==0){
    Serial.println("**irrigation timer is on...**");
    Serial.print("And The plant will be watered after ===> ");
    Serial.print(Period_of_irrigation_hour);
    Serial.print(": ");
    Serial.print(Period_of_irrigation_minute);
    Serial.print("':");
    Serial.print(Period_of_irrigation_sec);
    Serial.println("''");
    Serial.print("For about: ");
    Serial.print(Period_of_water_motor_sec);
    Serial.println("  seconds");
    Timer_irrigation=Timer_counter;    
    FlagIrrigation=1; 
  }
  else if(Timer_counter - Timer_irrigation >= TimerOfIrrigation){
    WaterMotor(Period_of_water_motor_sec);
    
    if(FlagWaterMotor==0){
      if(IsWater==1){
        Serial.println("irrigation is done");
      }
      else{
        Serial.println("!!!! irrigation failed(water level Error!)");
      }
      FlagIrrigation=0;
    }
    //delay(1000);
  }
}

//////////////////

void ALlamp(float OnTime_Lamp_min){
  TimerLamp= (OnTime_Lamp_min *60);
  if(FlagAL==0){
     Timer_Artifitial_Lamp=Timer_counter;
     FlagAL=1;
     digitalWrite(RelayArtifitialLight, 0);
     Serial.println("Lamp is ON");  
  }
  else if(Timer_counter - Timer_Artifitial_Lamp >= TimerLamp){
    digitalWrite(RelayArtifitialLight, 1);
    Serial.println("Lamp is OFF");
    FlagAL=0;
  }
  
}

////////////////////////

void ArtificialLightSystem(int Period_of_AL_0ff_hour ,int Period_of_AL_0ff_minute, int Period_of_AL_0ff_sec,float Period_of_AL_0n_minute){
  
  TimerOfAL=((unsigned long)Period_of_AL_0ff_hour * (unsigned long)3600)+((unsigned long)Period_of_AL_0ff_minute * (unsigned long)60)+(unsigned long)Period_of_AL_0ff_sec;
  
  
  Serial.print("timer:");
  Serial.println(TimerOfAL);
  
  
  LightSensor();
  if(FlagALsystem==0 & FlagAL==0){
     Timer_Artifitial_System=Timer_counter;
     FlagALsystem=1;
     Serial.print("Proportion Of Light: %");  
     Serial.println(Light_Pin_out);    
     Serial.println("**Artifitial light timer is on...**");
    Serial.print("And The Light will Turn on after ===> ");
    Serial.print(Period_of_AL_0ff_hour);
    Serial.print(": ");
    Serial.print(Period_of_AL_0ff_minute);
    Serial.print("':");
    Serial.print(Period_of_AL_0ff_sec);
    Serial.println("''");
    Serial.print("For about: ");
    Serial.print(Period_of_AL_0n_minute);
    Serial.println("  Minutes");   
  }
  else if(Timer_counter - Timer_Artifitial_System >= TimerOfAL){
    ALlamp(Period_of_AL_0n_minute);
    if(FlagAL==0){
      FlagALsystem=0;
      Serial.println("Artifitial Lighting Done");
    }
  }
    
}

/////////////

void recievefromesp8266(){
  
  if (serial.available()) {
    message = serial.readString();
    //Serial.print(message);  
  }
  delay(10);
  if(message=="1" || message=="2" || message=="3"|| message=="4" || message=="5" || message=="6"){
     if(message=="1"){
      water_time_status=1;
     }
     else if(message=="2"){
      water_time_status=2;
     }
     else if(message=="3"){
      water_time_status=3;
     }
     else if(message=="4"){
      water_time_status=4;
     }
     else if(message=="5"){
      water_time_status=5;
     }
     else{
      water_time_status=6;
     }
  }
  else if(message=="7" || message=="8" || message=="9"|| message=="10" || message=="11" || message=="11"){
     if(message=="7"){
      water_volume_status=7;
     }
     else if(message=="8"){
      water_volume_status=8;
     }
     else if(message=="9"){
      water_volume_status=9;
     }
     else if(message=="10"){
      water_volume_status=10;
     }
     else{
      water_volume_status=11;
     }
  }
  else if(message=="12" || message=="13" || message=="14"|| message=="15" || message=="16" || message=="17"){
     if(message=="12"){
      light_time_status=12;
     }
     else if(message=="13"){
      light_time_status=13;
     }
     else if(message=="14"){
      light_time_status=14;
     }
     else if(message=="15"){
      light_time_status=15;
     }
     else if(message=="16"){
      light_time_status=16;
     }
     else{
      light_time_status=17;
     }
  }
  else if(message=="18" || message=="19" || message=="20"|| message=="21" || message=="22" || message=="23"){
     if(message=="18"){
      light_duration_status=18;
     }
     else if(message=="19"){
      light_duration_status=19;
     }
     else if(message=="20"){
      light_duration_status=20;
     }
     else if(message=="21"){
      light_duration_status=21;
     }
     else if(message=="22"){
      light_duration_status=22;
     }
     else{
      light_duration_status=23;
     }
  }
  else if(message=="24" || message=="25" || message=="26"|| message=="27" || message=="28" || message=="29"){
     if(message=="24"){
      lowest_temp_status=24;
     }
     else if(message=="25"){
      lowest_temp_status=25;
     }
     else if(message=="26"){
      lowest_temp_status=26;
     }
     else if(message=="27"){
      lowest_temp_status=27;
     }
     else if(message=="28"){
      lowest_temp_status=28;
     }
     else{
      lowest_temp_status=29;
     }
  }
  else if(message=="30" || message=="31" || message=="32"|| message=="33" || message=="34" || message=="35"){
     if(message=="30"){
      manualcommand_status=30;
     }
     else if(message=="31"){
      manualcommand_status=31;
     }
     else if(message=="32"){
      manualcommand_status=32;
     }
     else if(message=="33"){
      manualcommand_status=33;
     }
     else if(message=="34"){
      manualcommand_status=34;
     }
     else{
      manualcommand_status=35;
     }
  }
  message = "";
  Serial.print("w_t: ");
  Serial.print(water_time_status);
  Serial.print(" w_v: ");
  Serial.print(water_volume_status);
  Serial.print(" l_t: ");
  Serial.print(light_time_status);
  Serial.print(" l_d: ");
  Serial.print(light_duration_status);
  Serial.print(" l_tem: ");
  Serial.print(lowest_temp_status);
  Serial.print(" man: ");
  Serial.println(manualcommand_status);
  delay(500);
    
}


//////////
void monitoring(){
  SoilMoisture();
  WaterLevel();
  TemperatureHumidity();
  LightSensor();

  soil=(String)soil_Pin_voltage;
  water=(String)WaterLevelPercentage;
  light=(String)Light_Pin_out;
  temp=(String)t;
  hum=(String)h;
  for(int i=0;i<=4;i++){
    send_to_8266_monitoring+=soil[i];        
  }
  for(int i=0;i<=1;i++){
    send_to_8266_monitoring+=water[i];        
  }
  for(int i=0;i<=4;i++){
    send_to_8266_monitoring+=light[i];        
  }
  for(int i=0;i<=4;i++){
    send_to_8266_monitoring+=temp[i];        
  }
  for(int i=0;i<=4;i++){
    send_to_8266_monitoring+=hum[i];        
  }
  Serial.println(send_to_8266_monitoring);
  serial.print(send_to_8266_monitoring);
  delay(20);
  send_to_8266_monitoring="";
  
 }

 /////////
 
void fanheater(int OnTime_fan_min ,float desiredTemp){
  // OnTime_fan_sec roshan mishe agar dama kamtar as desired bashad
  int time_on_fan_sec = (unsigned long)OnTime_fan_min * 60;
  if(FlagFan==0){
    Timer_fan=Timer_counter;
    TemperatureHumidity();
    Serial.print("tempreture: %");
    Serial.println(t);
    if(desiredTemp < t){
      digitalWrite(RelayFanHeater, 0);
      Serial.println("The plant is being heated...");
      FlagFan=1;
    }
    else{
      Serial.println("The temp is more than desire");
      //send message to broker
    }   
  }
  else if(Timer_counter - Timer_fan >= time_on_fan_sec){
    digitalWrite(RelayFanHeater, 1);
    Serial.println("fan and heater is off");
    FlagFan=0;
    //delay(1000);
  }
}

//////////
void Action(){

  if(manualcommand_status != 0){
    //do some thing
    if(manualcommand_Action != 31){
      if(manualcommand_status==31){
        manualcommand_Action= manualcommand_status;
        FlagIrrigationManually=0;
      }
    }
    else if(manualcommand_Action != 30){
      if(manualcommand_status==30){
        manualcommand_Action= manualcommand_status;
        FlagIrrigationManually=0;
      }
    }
    if(manualcommand_status==32 || manualcommand_status==33 || manualcommand_status==34 || manualcommand_status==35 ){
      manualcommand_Action= manualcommand_status;
    }
   //manualcommand_status = 0;   
  }
  else if((water_time_status!= 0) && (water_volume_status != 0)){
    //do some thing
    water_time_Action = water_time_status;
    water_volume_Action= water_volume_status;
    
    water_time_status=0;
    water_volume_status=0;
  }
  else if((light_time_status != 0) && (light_duration_status != 0)){
    //do some thing
    light_time_Action=light_time_status;
    light_duration_Action=light_duration_status;
    light_time_status=0;
    light_duration_status=0;
  }
  else if(lowest_temp_status != 0){
    //do some thing
    lowest_temp_Action = lowest_temp_status;
  }

  if(water_time_Action!=0 && water_volume_Action!=0){
    if(water_time_Action ==1){
      IrrigationSystem(24,0,0,(water_volume_Action-3));
    }
    else if(water_time_Action ==2){
      IrrigationSystem(12,0,0,(water_volume_Action-3));
    }
    else if(water_time_Action ==3){
      IrrigationSystem(8,0,0,(water_volume_Action-3));
    }
    else if(water_time_Action ==4){
      IrrigationSystem(168,0,0,(water_volume_Action-3));
    }
    else if(water_time_Action ==5){
      IrrigationSystem(84,0,0,(water_volume_Action-3));
    }
    else if(water_time_Action ==6){
      IrrigationSystem(56,0,0,(water_volume_Action-3));
    }
  }

  if(light_time_Action!=0 && light_duration_Action!=0){
    if(light_time_Action ==12){
      ArtificialLightSystem(24,0,0,((light_duration_Action-17)*5));
    }
    else if(light_time_Action ==13){
      ArtificialLightSystem(12,0,0,((light_duration_Action-17)*5));
    }
    else if(light_time_Action ==14){
      ArtificialLightSystem(8,0,0,((light_duration_Action-17)*5));
    }
    else if(light_time_Action ==15){
      ArtificialLightSystem(168,0,0,((light_duration_Action-17)*5));
    }
    else if(light_time_Action ==16){
      ArtificialLightSystem(84,0,0,((light_duration_Action-17)*5));
    }
    else if(light_time_Action ==17){
      ArtificialLightSystem(56,0,0,((light_duration_Action-17)*5));
    }
  }
  
  if(lowest_temp_Action!=0){
    fanheater(5,((lowest_temp_Action -24.00 )* 5));
  }
  if(manualcommand_Action !=0){
    if(manualcommand_Action==30 && FlagIrrigationManually==0){
      WaterMotorManually(3);
    }
    if(manualcommand_Action==31 && FlagIrrigationManually==0){
      WaterMotorManually(5);
       
    }
    if(manualcommand_Action==32){
      digitalWrite(RelayArtifitialLight, 0);
      Serial.println("RelayArtifitialLight turnes ON manually");  
    }
    if(manualcommand_Action==33){
      digitalWrite(RelayArtifitialLight, 1);
      Serial.println("RelayArtifitialLight turnes OFF manually");
      manualcommand_Action=0;  
    }
    if(manualcommand_Action==34){
//      TemperatureHumidity();
//      if(OptimalPlantTemperature > t){
        digitalWrite(RelayFanHeater, 0);
        Serial.println("The plant is being heated manually...");
//      }
//      else{
//        Serial.println("The temp is more than definition");
//        digitalWrite(RelayFanHeater, 1);
//        manualcommand_Action=0;
//        //send message to broker
//      }  
    }
    if(manualcommand_Action==35){
       digitalWrite(RelayFanHeater, 1);
       Serial.println("fan and heater turn OFF manually...");
       manualcommand_Action=0; 
    }
    
  }
}
