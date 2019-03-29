#include <ArduinoOSC.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
//#include <Ethernet.h>
//#include <SPI.h>
//#include <MsTimer2.h>


// Ethernet stuff
const IPAddress ip(192, 168, 100, 65);
byte mac[] = {0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02};

// for ArduinoOSC
OscEthernet osc;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const char* host = "192.168.100.7";
//const char* host = "192.168.100.155";
const int recv_port = 9999;
const int send_port = 9999;
int32_t maxTemp = 50, maxTime = 5;
int counter = 0, tempCounter = 0;
boolean SSR_status = false;

uint32_t delta_time = 0;

void sendTemperature(void){
  
}

void subscribeTrigger(void){
  osc.subscribe("/need/reply", [](OscMessage &m) {
    Serial.print("/need/reply: ");
    Serial.print(m.ip());
    Serial.print(" ");
    Serial.print(m.port());
    Serial.print(" ");
    Serial.print(m.size());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");
    Serial.print(m.getArgAsInt32(0));
    Serial.println();
    Serial.println();
    maxTime = m.getArgAsInt32(0);
    Serial.println(maxTime);
  });

  osc.subscribe("/set/maxTemp", [](OscMessage &m) {
    Serial.print("/set/maxTemp: ");
    Serial.print(m.ip());
    Serial.print(" ");
    Serial.print(m.port());
    Serial.print(" ");
    Serial.print(m.size());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");
    Serial.print(m.getArgAsInt32(0));
    Serial.println();
    Serial.println();
    maxTemp = m.getArgAsInt32(0);
    Serial.println(maxTemp);
  });

  osc.subscribe("/set/maxTime", [](OscMessage &m) {
    Serial.print("/set/maxTime: ");
    Serial.print(m.ip());
    Serial.print(" ");
    Serial.print(m.port());
    Serial.print(" ");
    Serial.print(m.size());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");
    int32_t tmp = m.getArgAsInt32(0);
    Serial.print("Set max temperature: ");
    Serial.print(tmp);
    Serial.println();
    Serial.println();
    maxTime = tmp;
  });

  osc.subscribe("/set/reset", [](OscMessage &m) {
    
    Serial.print("/set/maxTemp: ");
    Serial.print(m.ip());
    Serial.print(" ");
    Serial.print(m.port());
    Serial.print(" ");
    Serial.print(m.size());
    Serial.print(" ");
    Serial.print(m.address());
    Serial.print(" ");
    
    int32_t tmp = m.getArgAsInt32(0);
    
    Serial.print("Set max time: ");
    Serial.print(tmp);
    Serial.println();
    Serial.println();
    
    if(tmp>0){
      SSR_status = true;
    }
    else{
      SSR_status = false;
    }
  });
}

void setup(){
    pinMode(2, OUTPUT);
    digitalWrite(2, LOW);
    
    Serial.begin(115200);
    
    Serial.println("Initialize Ethernet with DHCP:");
    if (Ethernet.begin(mac) == 0){
      Serial.println("Failed to configure Ethernet using DHCP");
      if (Ethernet.hardwareStatus() == EthernetNoHardware){
        Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      }
      else if (Ethernet.linkStatus() == LinkOFF){
        Serial.println("Ethernet cable is not connected.");
      }
      while (true){
        delay(1);
      }
    }
    Serial.print("My IP address: ");
    Serial.println(Ethernet.localIP());
    Ethernet.begin(mac, ip);
    subscribeTrigger();
    osc.begin(recv_port);
    mlx.begin();
}
    

void loop(){
  if((millis() - delta_time) > 1000){
    Serial.println(millis() - delta_time);
    float objectTemp = mlx.readObjectTempC();
    float ambientTemp = mlx.readAmbientTempC();
    Serial.println(objectTemp);
    
    OscMessage msg(host, send_port, "/temp");
    msg.push(objectTemp).push(SSR_status);
    osc.send(msg);
    
    
    if(objectTemp > maxTemp){
      tempCounter++;
      Serial.print("Over temperature ++ ");
      Serial.println(tempCounter);
    }
    if(tempCounter > maxTime){
      SSR_status = false;
      tempCounter = 0;
      Serial.print("tempCounter > maxTime: ");
      Serial.println(SSR_status);
    }
    
    digitalWrite(2, SSR_status);
    delta_time = millis();
  }
  
  //sendTemperature();
  
  osc.parse();
  //Serial.println(millis());
}
