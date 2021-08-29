#include <ESP8266WiFi.h>
#include <Servo.h>
#include <dht11.h>
#include "ThingSpeak.h"

#define DHT_pin  16
#define sol_ileri 14
#define sol_geri 12
#define sol_hiz 5

#define sag_ileri 13
#define sag_geri 15
#define sag_hiz 4

dht11 DHT11;

WiFiClient client;
WiFiServer server(80);

unsigned long Channel_ID_sicaklik = ;// sicaklik kanal id'si  giriniz
unsigned long Channel_ID_nem = ;   // Nem kanal id'si giriniz
const char * API_key_sicaklik = "";  // Thingspeak api keyinizi giriniz
const char * API_key_nem = "";

String tspeak_ip = "http://184.106.153.149";

int hiz;

int slider_hiz;                                // Telefondan gelen hız slider'ının tutuldugu değişken
int slider_pos1;                              //  Telefondan gelen x slider'ının tutuldugu değişken
int slider_pos2;                             //   Telefondan gelen y slider'ının tutuldugu değişken

float sicaklik, nem;

const char* ssid = "";                               //wifi isim girilecek yer
const char* password = "";                 // wifi şifre girilecek yer

String data = "";

Servo servo_1;
Servo servo_2;

void WifiBaglan()
{
  Serial.println("Wifi'ye Baglaniliyor...");
  WiFi.begin(ssid, password);                             // wifi bağlanmak için ssid ve şifrenin eşleştiği yer
  while ((!(WiFi.status() == WL_CONNECTED)))
  {
    delay(300);
    Serial.print("...");
  }
  Serial.println("");
  Serial.println("Wifi'ye Baglandı");
  Serial.println("Nodemcu ip:");
  Serial.print((WiFi.localIP()));                        //"localIP" esp nin wifide'ki ip sini gösterir
}


void setup() {
  pinMode(sol_ileri, OUTPUT);
  pinMode(sag_ileri, OUTPUT);
  pinMode(sol_geri, OUTPUT);
  pinMode(sag_geri, OUTPUT);
  pinMode(sol_hiz, OUTPUT);
  pinMode(sag_hiz, OUTPUT);

  servo_1.attach(2);                                    // X Servosunu Tetikler
  servo_2.attach(0);                                   // Y Servosunu Tetikler

  Serial.begin(115200);
  WifiBaglan();                                      // WifiBaglan() fonksiyonunu çağırır
  server.begin();
  ThingSpeak.begin(client);
}

String Dinle(void)
{
  String request = client.readStringUntil('\r'); /*Gelen degeri parcalamak için önce 0. indexten 5. indexe kadar silme yapılır
                                                   daha sonra gelen verinin uzunluk degeri alınır ve bu değerden 9 değeri düşülerek tekrar gelen istekten veri parçalanır.*/ 
  //Serial.println(request);                  // GET /w HTTP/1.1 degeri doner
  while (!client.available()) delay(1);
  request.remove(0, 5);                     // 0.index-5.index silinir ve "w HTTP/1.1"  oalrak çıktı verir.
  request.remove(request.length() - 9, 9); /*dönen degerin uzunlugu alınır(10) ve bu değerden 9 çıkarılır deger 1. indexten 9. index a kadar silme yapar
                                         sonuç olarak elimizde 0. index kalır. yani göndermek istedigimiz deger "w" */
  return request;
}

void loop()
{
  int chk = DHT11.read(DHT_pin);
  nem = (float)DHT11.humidity;
  sicaklik = (float)DHT11.temperature;
  delay(3);
  ThingSpeak.writeField(Channel_ID_sicaklik, 1, sicaklik,API_key_sicaklik);   // deger gönderir ise 200 degerini döndürür bir değişkene atayıp ekrana yazdırabilirsiniz
  ThingSpeak.writeField(Channel_ID_nem, 1, nem,API_key_nem);
  Serial.println(sicaklik);
  
  client = server.available();
  if (!client) return;
  data = Dinle();
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
  String data_hiz = data.substring(1, data.length());     // string deger döner (1. indexten data'nın uzunluguna kadar veriyi yazar 0-250 ARASI DEGER" örn --> "H125" gibi
  slider_hiz = data_hiz.toInt();  // dönen string degeri integer türüne çevrilir --> örn "H125" --> 125
  hiz = slider_hiz;    // hiz değişkenine dönen deger yazılır
}

void Servo_X() {
//   String data_x = data.substring(1, data.length());     //string deger döner (1. indexten data'nın uzunluguna kadar veriyi yazar 0-180 ARASI DEGER" örn --> "X25" gibi
  slider_pos1 = data_x.toInt();                           // dönen string degeri integer türüne çevrilir --> örn "X25" --> 25
  servo_1.write(slider_pos1);         // dönen deger servo_1 degişkenine yazılır
  delay(3);
}

void Servo_Y() {
  String data_y = data.substring(1, data.length());
  slider_pos2 = data_y.toInt();
  servo_2.write(slider_pos2);
  delay(3);
}
