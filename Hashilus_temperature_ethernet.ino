#include <ArduinoOSC.h>
#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <MsTimer2.h>

// Ethernet stuff
const IPAddress ip(192, 168, 100, 65);
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

// for ArduinoOSC
OscEthernet osc;
Adafruit_MLX90614 mlx = Adafruit_MLX90614();

const char* host = "192.168.100.7";
const int recv_port = 9999;
const int send_port = 9999;

void sendTemperature(void){
  float objectTemp = mlx.readObjectTempC();
  float ambientTemp = mlx.readAmbientTempC();
  Serial.println(objectTemp);
  OscMessage msg(host, send_port, "/send");
  msg.push(objectTemp);
  osc.send(msg);
}

void ethernetInit (void){
  // start the Ethernet connection:
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
}

void setup(){
    pinMode(2, OUTPUT);
    pinMode(13, OUTPUT);
    digitalWrite(2, LOW);
    digitalWrite(13, LOW);

    Serial.begin(115200);
    mlx.begin();
    MsTimer2::set(1000, sendTemperature);
    MsTimer2::start();
    ethernetInit();
    osc.begin(recv_port);

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
    Serial.print(m.getArgAsInt32(0)); Serial.println();
    Serial.println();
  });
}

void loop(){
  osc.parse();
}