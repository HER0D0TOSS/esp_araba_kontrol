#include <DHT.h>
#include <DHT_U.h>

#include <ESP8266WiFi.h>
#include <Servo.h>

#define DHT_pin  16 //D0
#define sol_ileri 14
#define sol_geri 12
#define sol_hiz 5

#define sag_ileri 13
#define sag_geri 15
#define sag_hiz 4



WiFiClient client;
WiFiServer server(80);

String apiKey = "";     //  Enter your Write API key from ThingSpeak
const char* server = "api.thingspeak.com";

int hiz;

int slider_hiz;
int slider_pos1;
int slider_pos2;

const char* ssid = "";//wifi isim
const char* password = ""; // wifi şifre

String data = "";

Servo servo_1;
Servo servo_2;

void WifiBaglan()
{
  Serial.println("Wifi'ye Baglaniliyor...");
  WiFi.begin(ssid, password);
  while ((!(WiFi.status() == WL_CONNECTED)))
  {
    delay(300);
    Serial.print("...");
  }
  Serial.println("");
  Serial.println("Wifi'ye Baglandı");
  Serial.println("Nodemcu ip:");
  Serial.print((WiFi.localIP()));
}


void setup() {
  pinMode(sol_ileri, OUTPUT);
  pinMode(sag_ileri, OUTPUT);
  pinMode(sol_geri, OUTPUT);
  pinMode(sag_geri, OUTPUT);
  pinMode(sol_hiz, OUTPUT);
  pinMode(sag_hiz, OUTPUT);
  pinMode(DHT_pin, INPUT);

  servo_1.attach(2); //x
  servo_2.attach(0); //y

  Serial.begin(115200);
  WifiBaglan();
  server.begin();
}

String checkClient(void)
{
  String request = client.readStringUntil('\r');
  /*Gelen degeri parcalamak için önce 0. indexten 5. indexe kadar silme yapılır
    daha sonra gelen verinin uzunluk degeri alınır ve bu değerden 9 değeri düşülerek tekrar gelen istekten veri parçalanır.
  */
  //Serial.println(request); // GET /w HTTP/1.1 degeri doner
  while (!client.available()) delay(1);
  request.remove(0, 5); // 0.index-5.index silinir ve "w HTTP/1.1"  oalrak çıktı verir.
  request.remove(request.length() - 9, 9); /*dönen degerin uzunlugu alınır(10) ve bu değerden 9 çıkarılır deger 1. indexten 9. index a kadar silme yapar
                                         sonuç olarak elimizde 0. index kalır. yani göndermek istedigimiz deger "w" */
  return request;
}

void loop()
{
  client = server.available();
  if (!client) return;
  data = checkClient();
  Serial.println(data);

  //int uzunluk = data.length();
  if (data == "w") {
    Ileri();
  }
  else if (data == "s") {
    Geri();
  }
  else if (data == "d") {
    Sag();
  }
  else if (data == "a") {
    Sol();
  }
  else if (data == "o") {
    Dur();
  }
  else if (data.startsWith("H")) {
    Hiz();
  }
  else if (data.startsWith("X")) {
    Servo_X();
  }
  else if (data.startsWith("Y")) {
    Servo_Y();
  }
  float nem = dht.readHumidity();
  float Sicaklik = dht.readTemperature();
  String postStr = apiKey;
  postStr += "&field1=";
  postStr += String(t);
  postStr += "&field2=";
  postStr += String(h);
  postStr += "\r\n\r\n";

  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(postStr.length());
  client.print("\n\n");
  client.print(postStr);

  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" degrees Celcius, Humidity: ");
  Serial.print(h);
  Serial.println("%. Send to Thingspeak.");
}

void Ileri() {
  digitalWrite(sol_ileri, HIGH);
  digitalWrite(sol_geri, LOW);
  analogWrite(sol_hiz, hiz);
  Serial.println("HIZ:" );
  Serial.print(hiz);
  digitalWrite(sag_ileri, HIGH);
  digitalWrite(sag_geri, LOW);
  analogWrite(sag_hiz, hiz);
}


void Geri () {
  digitalWrite(sol_ileri, LOW);
  digitalWrite(sol_geri, HIGH);
  analogWrite(sol_hiz, hiz);
  Serial.println("HIZ:" );
  Serial.print(hiz);
  digitalWrite(sag_ileri, LOW);
  digitalWrite(sag_geri, HIGH);
  analogWrite(sag_hiz, hiz);
}
void Sag() {
  digitalWrite(sol_ileri, LOW);
  digitalWrite(sol_geri, HIGH);
  analogWrite(sol_hiz, hiz);
  Serial.println("HIZ:" );
  Serial.print(hiz);
  digitalWrite(sag_ileri, HIGH);
  digitalWrite(sag_geri, LOW);
  analogWrite(sag_hiz, hiz);
}
void Sol() {
  digitalWrite(sol_ileri, HIGH);
  digitalWrite(sol_geri, LOW);
  analogWrite(sol_hiz, hiz);
  Serial.println("HIZ:" );
  Serial.print(hiz);
  digitalWrite(sag_ileri, LOW);
  digitalWrite(sag_geri, HIGH);
  analogWrite(sag_hiz, hiz);

}

void Dur() {
  digitalWrite(sol_ileri, LOW);
  digitalWrite(sol_geri, LOW);
  analogWrite(sol_hiz, hiz);
  Serial.println("HIZ:" );
  Serial.print(hiz);
  digitalWrite(sag_ileri, LOW);
  digitalWrite(sag_geri, LOW);
  analogWrite(sag_hiz, hiz);

}

void Hiz() {
  String data_hiz = data.substring(1, data.length());
  slider_hiz = data_hiz.toInt();
  hiz = slider_hiz;
}

void Servo_X() {
  String data_x = data.substring(1, data.length());
  slider_pos1 = data_x.toInt();

  servo_1.write(slider_pos1);
  delay(3);
}

void Servo_Y() {
  String data_y = data.substring(1, data.length());
  slider_pos2 = data_y.toInt();

  servo_2.write(slider_pos2);
  delay(3);
}
