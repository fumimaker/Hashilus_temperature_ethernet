#include <ArduinoOSC.h>

// Ethernet stuff
const IPAddress ip(192, 168, 100, 65);
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};

// for ArduinoOSC
OscEthernet osc;
const char* host = "192.168.100.7";
const int recv_port = 9999;
const int send_port = 9999;

void setup()
{
    Serial.begin(115200);


  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    } else if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }
    // no point in carrying on, so do nothing forevermore:
    while (true) {
      delay(1);
    }
  }
  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());
    // Ethernet stuff
    Ethernet.begin(mac, ip);

    // ArduinoOSC
    osc.begin(recv_port);
    /*
    osc.subscribe("/test", [](OscMessage& m)
    {
        Serial.print("/test : ");
        Serial.print(m.ip()); Serial.print(" ");
        Serial.print(m.port()); Serial.print(" ");
        Serial.print(m.size()); Serial.print(" ");
        Serial.print(m.address()); Serial.print(" ");
        Serial.print(m.getArgAsInt32(0)); Serial.println();
    });
    */
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
    //osc.send(host, send_port, "/send", 12);
  });

    // TODO: TBD
    // osc.publish(host, send_port, "/value", value);
    // osc.publish(host, send_port, "/millis", &millis);
}

void loop()
{
  float objectTemp = 90.2;
  float ambientTemp = 32.1;
  float i = objectTemp;
  OscMessage msg(host, send_port, "/send");
  msg.push(i);
  osc.send(msg);
  //osc.send(host, send_port, "/objectTemp", objectTemp);// float
  //osc.send(host, send_port, "/switch", switch_state); // boolean
  //osc.send(host, send_port, "/test", 123);
  osc.parse(); // should be called
  delay(100);
}
