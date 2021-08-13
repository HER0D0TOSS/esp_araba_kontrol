#include <Servo.h>
#include <ESP8266WiFi.h>

#define led_pin  16
#define sol_ileri 14
#define sol_geri 12
#define sol_hiz 5

#define sag_ileri 13
#define sag_geri 15
#define sag_hiz 4

WiFiClient client;
WiFiServer server(80);

int hiz = 55;
int slider_pos1;
int slider_pos2;

const char* ssid = "";
const char* password = "";



String data = "";

Servo servo_1;
Servo servo_2;



void WifiBaglan()
{
  Serial.println("Wifi'ye Baglaniliyor...");
  WiFi.begin(ssid, password);
  while ((!(WiFi.status()==WL_CONNECTED)))
  {
    delay(300);
    Serial.print("...");
  }
  Serial.println("");
  Serial.println("Wifi'ye Baglandı");
  Serial.println("Nodemcu ip:");
  Serial.print((WiFi.localIP()));
}


void setup(){
  pinMode(sol_ileri, OUTPUT);
  pinMode(sag_ileri, OUTPUT);
  pinMode(sol_geri, OUTPUT);
  pinMode(sag_geri, OUTPUT);
  pinMode(sol_hiz,OUTPUT);
  pinMode(sag_hiz,OUTPUT);
  pinMode(led_pin,OUTPUT);

  servo_1.attach(0);
  servo_2.attach(2);
  
  Serial.begin(115200);
  WifiBaglan();
  server.begin();

}

String checkClient(void)
{
  String request = client.readStringUntil('\r');
  while(!client.available()) delay(1);
    
  request.remove(0, 21);
  return request;
}

void loop()
{
  digitalWrite(led_pin,HIGH);
  client = server.available();
  if (!client) return;
  data = checkClient();
  

  if (data == "ileri"){
    Ileri();}
  else if (data == "geri"){
    Geri();}
   else if (data == "sag"){
    Sag();}
   else if (data == "sol"){
    Sol();}
   else if (data == "dur"){
    Dur();}
   else if (data.startsWith("X")){
    Servo_X();
   }
   else if (data.startsWith("Y")){
    Servo_Y();
    }
 }


void Ileri(){
    digitalWrite(sol_ileri,HIGH);
    digitalWrite(sol_geri,LOW);
    analogWrite(sol_hiz,hiz);

    digitalWrite(sag_ileri,HIGH);
    digitalWrite(sag_geri,LOW);
    analogWrite(sag_hiz,hiz); 
}


void Geri (){
    digitalWrite(sol_ileri,LOW);
    digitalWrite(sol_geri,HIGH);
    analogWrite(sol_hiz,hiz);

    digitalWrite(sag_ileri,LOW);
    digitalWrite(sag_geri,HIGH);
    analogWrite(sag_hiz,hiz);
}
void Sag(){
    digitalWrite(sol_ileri,LOW);
    digitalWrite(sol_geri,HIGH);
    analogWrite(sol_hiz,hiz);

    digitalWrite(sag_ileri,HIGH);
    digitalWrite(sag_geri,LOW);
    analogWrite(sag_hiz,hiz);
}
void Sol(){
    digitalWrite(sol_ileri,HIGH);
    digitalWrite(sol_geri,LOW);
    analogWrite(sol_hiz,hiz);

    digitalWrite(sag_ileri,LOW);
    digitalWrite(sag_geri,HIGH);
    analogWrite(sag_hiz,hiz);
  
}

void Dur(){
    digitalWrite(sol_ileri,LOW);
    digitalWrite(sol_geri,LOW);
    analogWrite(sol_hiz,0);

    digitalWrite(sag_ileri,LOW);
    digitalWrite(sag_geri,LOW);
    analogWrite(sag_hiz,0);
}

void Servo_X(){
  String dataIns = data.substring(2, data.length());
  slider_pos1 = dataIns.toInt();  

  servo_1.write(slider_pos1);
  delay(3);
}

void Servo_Y(){
  String dataIns = data.substring(2, data.length());
  slider_pos2 = dataIns.toInt();  

  servo_2.write(slider_pos2);
  delay(3);
}
