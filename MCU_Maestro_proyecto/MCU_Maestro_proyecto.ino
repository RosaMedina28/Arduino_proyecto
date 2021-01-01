#include <Wire.h> //I2C
#include <DHT.h>  //TemperaturaYHumedad

/***************************************************
  Ejemplo de Adafruit MQTT Library ESP8266
  Adafruit invierte tiempo y recursos en proporcionar este código fuente abierto,
  apoye Adafruit y el hardware de código abierto comprando
  productos de Adafruit!
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"


#define WLAN_SSID       "15a1dc"//"drop.bsq"
#define WLAN_PASS       "274139715"//"projectc:/YQR9gTywyPl4@a7.zero"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "JuanVazquez"
#define AIO_KEY         "aio_FWjh67kZiHF3LesTRPoWWwx1ORbY"
#define AIO_GROUP       "proyectofinal."  // En caso de que creastes grupo si no solo se deja vacio ""

/************ Estado global (¡no necesitas cambiar esto!) ******************/

// Cree una clase ESP8266 WiFiClient para conectarse al servidor MQTT.
WiFiClient client;

// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Configure la clase de cliente MQTT pasando el cliente WiFi y el servidor MQTT y los detalles de inicio de sesión.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/
// Configure un feed llamado 'potValue' para su publicación.
// Observe que las rutas MQTT para AIO siguen el formulario: <username>/feeds/<feedname>
Adafruit_MQTT_Publish distValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"distvalue");
Adafruit_MQTT_Publish temperaturaValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"temperaturavalue");
Adafruit_MQTT_Publish humedadValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"humedadvalue");
Adafruit_MQTT_Publish alarmahumo = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"alarmahumo");
Adafruit_MQTT_Publish focoexterno_publish = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"focoexterno");



// Configure un feed llamado 'Brillo led' para suscribirse a los cambios.
Adafruit_MQTT_Subscribe led_bano = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"led-bano"); //le asignamos el led a un feed en especifico 
Adafruit_MQTT_Subscribe led_sala = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"led-sala");
Adafruit_MQTT_Subscribe led_oficina = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"led-oficina");
Adafruit_MQTT_Subscribe led_cocina = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"led-cocina");
Adafruit_MQTT_Subscribe led_cuarto = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"led-cuarto");
Adafruit_MQTT_Subscribe cochera = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"cochera");
Adafruit_MQTT_Subscribe focoexterno = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"focoexterno");
/*************************** Sketch Code ************************************/

// solución de error para Arduino 1.6.6, parece necesitar una declaración de función
// por alguna razón (solo afecta a ESP8266, probablemente un error de arduino-builder).
void MQTT_connect();

#define Echo D5 
#define Trigger D6
// Definimos el pin digital donde se conecta el sensor
#define DHTPIN D7
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11
// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

uint8_t ledWifi = D3,sensorPIR = D0;
uint16_t contador=0;
uint16_t led_bano_Value = 0;
uint16_t led_sala_Value = 0;
uint16_t led_oficina_Value = 0;
uint16_t led_cocina_Value = 0;
uint16_t led_cuarto_Value = 0;
uint16_t cochera_Value = 0;
uint16_t focoexterno_Value = 0;
long tiempoUltimaLectura=0;
uint16_t ledID;
int accion, luminosidad;



/* Configuracion Inicial del NODEMCU*/
void setup() {
  Serial.begin(9600);
  
  /* Configuracion de I2C */
  Wire.begin();               /* unirse al bus i2c con SDA = D1 y SCL = D2 de NodeMCU */
  Wire.beginTransmission(1);  /* comenzar con la dirección del dispositivo 8 */
  Wire.write("Hello Arduino Esclavo");  /* sends hello string */
  Wire.endTransmission();     /* stop transmitting */

  delay(10);
  
  pinMode(ledWifi,OUTPUT);
  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  pinMode(sensorPIR, INPUT);
    delay(1000);
    Serial.print("Calibrando sensor PIR ");
    for(int i = 0;i < 30;i++){
    delay(1000);
    Serial.print(".");
    }
    Serial.println("Sensor PIR Calibrado!!!");
  

  // Comenzamos el sensor DHT
  dht.begin();

  Serial.println(F("Bienvenido a Adafruit MQTT"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledWifi,HIGH);
    delay(200);
  
    Serial.print(".");
    digitalWrite(ledWifi,LOW);
    digitalWrite(Trigger, LOW);
   
    delay(300);
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());
  
  //saveDistancia();
  
  // Configure la suscripción MQTT para la alimentación ledBrightness..
  mqtt.subscribe(&led_bano);
  mqtt.subscribe(&led_sala);
  mqtt.subscribe(&led_oficina);
  mqtt.subscribe(&led_cocina);
  mqtt.subscribe(&led_cuarto);
  mqtt.subscribe(&cochera);
  mqtt.subscribe(&focoexterno);
  
  }



void loop() {
  // Asegúrese de que la conexión al servidor MQTT esté activa (esto hará que la primera
  // conexión y reconectar automáticamente cuando se desconecta). Ver el MQTT_connect
  // definición de función más abajo.
  MQTT_connect();
  Serial.println("-------------------------------------------LEDS");
  Led(); //Leds y Cochera
  //Serial.println("-------------------------------------------DISTANCIA");
  //saveDistancia(); 
  Serial.println("-------------------------------------------TEMPERATURA");
  saveTemperatura();
  Serial.println("-------------------------------------------ALARMA");
  saveAlarma();
  Serial.println("-------------------------------------------FOCO EXTERNO");
  FocoExterno();
  // hacer ping al servidor para mantener viva la conexión mqtt
  /*
  if(! mqtt.ping()) {
    mqtt.disconnect();
  }
  */
  

 
  
  delay(100);
}



/*Regresamos el valor de la distancia leido del arduino UNO */
/*int requestI2C(){
    int req=1;
    Wire.requestFrom(8, 5); /* request & read data of size 13 from slave */
    /*Leer la cadena que envia el Arduino Uno
    char Str4[6] = "00000";
    int cont=0;
    Serial.println("**********");
    while(Wire.available()){
      char c = Wire.read();
      Str4[cont]=c; 
     //Serial.print(c);
      cont++;
    }
   //Serial.println();
    String str((char*)Str4);
    req=str.toInt();
    Serial.println(req);
    return req;
}*/

/*Leer el estado del LED en ADAFRUIT */
void Led(){
  float distancia; //distancia en centimetros
  float tiempo;
  
  Adafruit_MQTT_Subscribe *subscription;

  while ((subscription = mqtt.readSubscription(200))) {

      Serial.println("----------------LedsValue------------------");
      Serial.print(F("Estado LED-bano : "));
      led_bano_Value = atoi((char *)led_bano.lastread);
      Serial.println((char *)led_bano.lastread);

      Serial.print(F("Estado LED-sala : "));
      led_sala_Value = atoi((char *)led_sala.lastread);
      Serial.println((char *)led_sala.lastread);

      Serial.print(F("Estado LED-oficina : "));
      led_oficina_Value = atoi((char *)led_oficina.lastread);
      Serial.println((char *)led_oficina.lastread);

      Serial.print(F("Estado LED-cocina : "));
      led_cocina_Value = atoi((char *)led_cocina.lastread);
      Serial.println((char *)led_cocina.lastread);

      Serial.print(F("Estado LED-cuarto : "));
      led_cuarto_Value = atoi((char *)led_cuarto.lastread);
      Serial.println((char *)led_cuarto.lastread);
      
      Serial.print(F("Estado Foco-Externo : "));
      focoexterno_Value = atoi((char *)focoexterno.lastread);
      Serial.println((char *)focoexterno.lastread);
      
      Serial.println("-------------------------------------------");
      
      Serial.println("----------------CocheraValue------------------");
      Serial.print(F("Estado Cochera : "));
      cochera_Value = atoi((char *)cochera.lastread);
      Serial.println((char *)cochera.lastread);
      Serial.println("-------------------------------------------");


          

      Wire.beginTransmission(1);  /* begin with device address 1 */
      accion = 1; //accion de los leds es 1
      
      if(strcmp((char *)led_bano.lastread, "ON") == 0) {
        Wire.write(accion);
        ledID = 2;
        Wire.write(ledID);
        Wire.write(1);
        Serial.println("ON");
        Serial.println(strcmp((char *)led_bano.lastread, "ON"));
        delay(50);
      }else
      if(strcmp((char *)led_bano.lastread, "OFF") == 0) {
        Wire.write(accion);
        ledID = 2;
        Wire.write(ledID);
        Wire.write(0);
        Serial.println(strcmp((char *)led_bano.lastread, "OFF"));
        delay(50);
      }

      if(strcmp((char *)led_sala.lastread, "ON") == 0) {
        Wire.write(accion);
        ledID = 3;
        Wire.write(ledID);
        Wire.write(1);
        Serial.println(strcmp((char *)led_sala.lastread, "ON"));
        delay(50);
      }else
      if(strcmp((char *)led_sala.lastread, "OFF") == 0) {
        Wire.write(accion);
        ledID = 3;
        Wire.write(ledID);
        Wire.write(0);
        Serial.println(strcmp((char *)led_sala.lastread, "OFF"));
        delay(50);
      }

      if(strcmp((char *)led_oficina.lastread, "ON") == 0) {
        Wire.write(accion);
        ledID = 4;
        Wire.write(ledID);
        Wire.write(1);
        Serial.println(strcmp((char *)led_oficina.lastread, "ON"));
        delay(50);
      }else
      if(strcmp((char *)led_oficina.lastread, "OFF") == 0) {
        Wire.write(accion);
        ledID = 4;
        Wire.write(ledID);
        Wire.write(0);
        Serial.println(strcmp((char *)led_oficina.lastread, "OFF"));
        delay(100);
      }

       if(strcmp((char *)led_cocina.lastread, "ON") == 0) {
        Wire.write(accion);
        ledID = 5;
        Wire.write(ledID);
        Wire.write(1);
        Serial.println(strcmp((char *)led_cocina.lastread, "ON"));
        delay(100);
      }else
      if(strcmp((char *)led_cocina.lastread, "OFF") == 0) {
        Wire.write(accion);
        ledID = 5;
        Wire.write(ledID);
        Wire.write(0);
        Serial.println(strcmp((char *)led_cocina.lastread, "OFF"));
        delay(100);
      }

       if(strcmp((char *)led_cuarto.lastread, "ON") == 0) {
        Wire.write(accion);
        ledID = 6;
        Wire.write(ledID);
        Wire.write(1);
        Serial.println(strcmp((char *)led_cuarto.lastread, "ON"));
        delay(50);
      }else
      if(strcmp((char *)led_cuarto.lastread, "OFF") == 0) {
        Wire.write(accion);
        ledID = 6;
        Wire.write(ledID);
        Wire.write(0);
        Serial.println(strcmp((char *)led_cuarto.lastread, "OFF"));
        delay(50);
      }

      //COCHERA
      if(strcmp((char *)cochera.lastread, "ON") == 0) {
        Wire.write(accion);
        ledID = 7;        //le asignamos un case a la cochera en el switch(ledID) en el arduino mega
        Wire.write(ledID); //ledID = cochera
        Wire.write(1);     //Estado Cochera Encendido
        Serial.println(strcmp((char *)cochera.lastread, "ON"));
        delay(50);
      }else{
        //Empiezo a medir la distancia
        digitalWrite(Trigger, OUTPUT);
        delayMicroseconds(4); 
      
        digitalWrite(Trigger, HIGH);
        delayMicroseconds(4);
        digitalWrite(Trigger, LOW);
      
        tiempo = pulseIn(Echo, HIGH);
        distancia = tiempo / 58.3;
        Serial.println(distancia);
        if(distancia > 15){
          Serial.println("Cerrando cochera !!!!!");
          if(strcmp((char *)cochera.lastread, "OFF") == 0) {
          Wire.write(accion);
          ledID = 7;          //le asignamos un case a la cochera en el switch(ledID) en el arduino mega
          Wire.write(ledID);  //ledID = cochera
          Wire.write(0);    //Estado Cochera Apagado
          Serial.println(strcmp((char *)cochera.lastread, "OFF"));
          delay(50);
          }
        }else{
          Serial.println("Objecto obtruyendo cochera !!!!!");
        }
      }

      if(strcmp((char *)focoexterno.lastread, "ON") == 0) {
        Wire.write(accion);
        ledID = 8;
        Wire.write(ledID);
        Wire.write(1);
        Serial.println(strcmp((char *)focoexterno.lastread, "ON"));
        delay(50);
      }else
      if(strcmp((char *)focoexterno.lastread, "OFF") == 0) {
        Wire.write(accion);
        ledID = 8;
        Wire.write(ledID);
        Wire.write(0);
        Serial.println(strcmp((char *)focoexterno.lastread, "OFF"));
        delay(50);
      }
      
      Wire.endTransmission();

      //digitalWrite(ledPin,ledBrightValue);
    //}
  }
}

/* GUARDA LA DISTANCIA LEIDO DEL ARDUINO UNO*/
void saveDistancia(){
  float distancia; //distancia en centimetros
  float tiempo; 
  //Empiezo a medir la distancia
  digitalWrite(Trigger, OUTPUT);
  delayMicroseconds(4);  //d=requestI2C();

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(4);
  digitalWrite(Trigger, LOW);

  tiempo = pulseIn(Echo, HIGH);
  distancia = tiempo / 58.3;
  Serial.print("Distancia: ");
  Serial.println(distancia);
  /*Envia el valor a adafruit cada 15 Segundos
    if(millis()-tiempoUltimaLectura>15000){
        // Envia el valor a adafruit
        if (! distValue.publish(distancia)) {
          Serial.println(F("Failed Distancia"));
        } else {
          Serial.println(F("Actualizando Distancia"));
          Serial.println(distancia);
        }    

        contador=0;
        tiempoUltimaLectura=millis();
    }*/
}

/* GUARDA LA TEMPERATURA Y HUMEDAD LEIDO DEL ARDUINO UNO*/
void saveTemperatura(){
  int temperatura, humedad; 

  temperatura = dht.readTemperature();
  humedad = dht.readHumidity();
  
  Serial.print("Temperatura: ");
  Serial.print(temperatura);
  Serial.println(" *C ");

  Serial.print("Humedad: ");
  Serial.print(humedad);
  Serial.println(" %\t");
  
  /*Envia el valor a adafruit cada 15 Segundos*/
    if(millis()-tiempoUltimaLectura>15000){
          //Envia el valor a adafruit
        if (! temperaturaValue.publish(temperatura)) {
          Serial.println(F("Failed Temperatura:("));
        } else {
          Wire.beginTransmission(1);
          accion = 2;
          Wire.write(accion);
          Wire.write(temperatura);
          Wire.endTransmission();
          Serial.println(F("Actualizando Temperatura!!"));
          Serial.println(temperatura);
        }

        if (! humedadValue.publish(humedad)) {
          Serial.println(F("Failed Humedad:("));
        } else {
          Serial.println(F("Actualizando Humedad!!"));
          Serial.println(humedad);
        }

        contador=0;
        tiempoUltimaLectura=millis();
    }
   
}

void saveAlarma(){
  int alarma = 0,estadoalarma = 0; 
  bool sensorvalue = false;

  Wire.requestFrom(1,1);
  while(Wire.available()){
      sensorvalue = Wire.read();
    }

  
  Serial.print("!!!!!!!!!!!!Valor de humo: ");
  Serial.println(sensorvalue);

  if(millis()-tiempoUltimaLectura>15000){

         if(sensorvalue == true){
          Serial.println("Alarma activada!!!!");
          alarma = 1;
          
          if (! alarmahumo.publish(alarma)) {
                Serial.println(F("Failed Alarma:("));
              } else {
                Wire.beginTransmission(1);
                accion = 9;
                Wire.write(accion);
                Wire.endTransmission();
                Serial.println(F("Actualizando Alarma!!"));
                Serial.println(alarma);
              }
        }else{ 
            alarma = 0;
            if (! alarmahumo.publish(alarma)) {
                Serial.println(F("Failed Alarma:("));
              } else {
                Wire.beginTransmission(1);
                accion = 10;
                Wire.write(accion);
                Wire.endTransmission();
                Serial.println(F("Actualizando Alarma!!"));
                Serial.println(alarma);
              }
        }
        contador=0;
        tiempoUltimaLectura=millis();
    }

  
}

void FocoExterno(){
  int luminosidad = 0;int movimiento = 0;
      luminosidad = analogRead(A0);
      Serial.print("Luminosidad: ");
      Serial.println(luminosidad);
  
  
   
      if(luminosidad < 100){
 
          
           movimiento = digitalRead(sensorPIR);//leemos el movimiento
           Serial.print("Movimiento: ");
           Serial.println(movimiento);
           
           if(millis()-tiempoUltimaLectura>15000){
            Serial.println("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%");
              Wire.beginTransmission(1);
                accion = 1;
                if(movimiento == HIGH){ //vemos si hay movimiento
        
                   if (! focoexterno_publish.publish("ON")) {
                    Serial.println(F("Failed Foco externo:("));
                  } else {
                      Wire.write(accion);
                      ledID = 8;
                      Wire.write(ledID);
                      Wire.write(1);
                    Serial.println(F("Actualizando Foco externo a ON!!"));
                    
                  }
               
                }else{
                   if (! focoexterno_publish.publish("OFF")) {
                    Serial.println(F("Failed Foco externo:("));
                  } else {
                      Wire.write(accion);
                      ledID = 8;
                      Wire.write(ledID);
                      Wire.write(1);
                    Serial.println(F("Actualizando Foco externo a OFF!!"));
        
                  }
                }
              Wire.endTransmission();
            contador=0;
            tiempoUltimaLectura=millis();
            }

           
          
        }   
        
}



// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
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
