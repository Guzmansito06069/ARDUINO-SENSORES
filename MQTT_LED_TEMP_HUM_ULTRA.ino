#include <DHT.h>
#include <DHT_U.h>


/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//#define WLAN_SSID "INFINITUM4381_2.4"
//#define WLAN_PASS ""

#define WLAN_SSID       "Lab Mac"                              //Totalplay-799F      //Lab Mac
#define WLAN_PASS       "Orim@r174,+"  //3AA5FDC0kDGF77Yb        //799FDFC5A49KWN3u   //Orim@r174,+

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "EdgarGuzman0607"
#define AIO_KEY         "aio_SVdH28wFFH46rdX9SjXPUPR63nMA"
#define AIO_GROUP       ""

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.  
WiFiClient client;
// or... use WiFiFlientSecure f 2 or SSL
//WiFiClientSecure client;

//Agarro los datos del servidor para ponerlos y que agarre los datos
// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Configure un feed llamado 'potValue' para su publicación.
// Observe que las rutas MQTT para AIO siguen el formulario: <username>/feeds/<feedname>
//Adafruit_MQTT_Publish potValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/potValue");

Adafruit_MQTT_Publish tempValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"Temperatura1");
Adafruit_MQTT_Publish humValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"Humedad");
Adafruit_MQTT_Publish distValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"Ultrasonico");

// Setup a feed called 'ledBrightness' for subscribing to changes.
Adafruit_MQTT_Subscribe ledBrightness = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/ledBrightness");
//Adafruit_MQTT_Subscribe ledBrightness = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"otro");

/*************************** Sketch Code ************************************/

 
// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

uint8_t ledPin = D4;
#define DHTPIN D6 //Data
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE);

uint16_t potAdcValue = 0;
float humVarValue=0;
float tempVarValue=0;
uint16_t distVarValue = 0;

uint16_t ledBrightValue = 0;
uint16_t contador=0;
long tiempoUltimaLectura=0;

const int Trigger = D2;   //Pin digital 2 para el Trigger del sensor
const int Echo = D1;   //Pin digital 3 para el Echo del sensor


void setup() {
  Serial.begin(9600);
  delay(10);   //10
  dht.begin();

  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0

  
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,HIGH);

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin,HIGH);
    delay(2000);//200
    Serial.print(".");
    digitalWrite(ledPin,LOW);
    delay(3000);//300
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
  tempVarValue = dht.readTemperature();
  humVarValue = dht.readHumidity();
  tempValue.publish(tempVarValue);
  humValue.publish(humVarValue);
  Serial.println(tempVarValue);
  Serial.println(humVarValue);

  // Setup MQTT subscription for ledBrightness feed.
  // Setup MQTT subscription for ledBrightness feed.
  mqtt.subscribe(&ledBrightness);
  
  }

void loop() {
  //Asegúrese de que la conexión al servidor MQTT esté activa (esto hará que la primera
   // conexión y reconectar automáticamente cuando se desconecta). Ver el MQTT_connect
   MQTT_connect ();

   // este es nuestro subbucle ocupado 'esperar paquetes de suscripción entrantes'
   // intenta pasar tu tiempo aquí 
   
 Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(200))) {
    //if (subscription == &ledBrightness) {
      Serial.print(F("Got LED Brightness : "));
      ledBrightValue = atoi((char *)ledBrightness.lastread);
      Serial.println((char *)ledBrightness.lastread);
      //analogWrite(ledPin, ledBrightValue);
      if (strcmp((char *)ledBrightness.lastread, "ON") == 0) {
        digitalWrite(ledPin, HIGH); 
        Serial.println("ON");
        Serial.println(strcmp((char *)ledBrightness.lastread, "OFF"));
        delay(2000);
      }else
      if (strcmp((char *)ledBrightness.lastread, "OFF") == 0) {
        digitalWrite(ledPin, LOW); 
        Serial.println("OFF");
        Serial.println(strcmp((char *)ledBrightness.lastread, "OFF"));
        delay(2000);
      }
            
    //}
  }

  // Now we can publish stuff!
 
    //Serial.println(millis()-tiempoUltimaLectura);
    if(millis()-tiempoUltimaLectura>30000){ //30000

        tempVarValue = dht.readTemperature();
        humVarValue = dht.readHumidity();
        distVarValue=distancia(); 
  
        if (! distValue.publish(distVarValue)) {
          Serial.println(F("Failed Distancia"));
        } else {
          Serial.println(F("OK! Distancia"));
          Serial.print("distVarValue:");
          Serial.println(distVarValue);
        }

        
        if (! tempValue.publish(tempVarValue)) {
          Serial.println(F("Failed Temperatura"));
        } else {
          Serial.println(F("OK! Temperatura"));
          Serial.print("tempVarValue:");
          Serial.println(tempVarValue);
        }
    
    
        if (! humValue.publish(humVarValue)) {
          Serial.println(F("Failed Humedad"));
        } else {
          Serial.println(F("OK! Humedad"));
          Serial.print("humVarValue:");
          Serial.println(humVarValue);
        }
        contador=0;
       tiempoUltimaLectura=millis();
    }

  
  // ping al servidor para mantener viva la conexión mqtt
   // NO es obligatorio si publica una vez cada KEEPALIVE segundos
   //Un keepalive es un mensaje enviado por un dispositivo a otro para verificar que el enlace entre los dos está funcionando, o para evitar que el enlace se rompa.
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
  delay(100);
}

// Función para conectarse y volver a conectarse según sea necesario al servidor MQTT.
// Debería llamarse en la función de bucle y se encargará de realizar la conexión.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
} 

long distancia(){
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  return d;  
}
