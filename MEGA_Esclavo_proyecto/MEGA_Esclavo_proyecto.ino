#include <Wire.h>
// Incluímos la librería para poder controlar el servo
#include <Servo.h>

// Declaramos la variable para controlar el servo
Servo servoMotor;
//displays
const int pindisplay = 33;
const int display1[7] = { 35, 37, 39, 41, 43, 45, 47 }; 
const int display2[7] = { 34, 36, 38, 40, 42, 44, 46 }; 
const int ledPin = 8, l_bano = 22, l_sala = 24, l_oficina = 26, l_cocina = 28, l_cuarto = 30;
int alarma = 0,evento = 1, buzzer = 31,relevador_foco = 3;
int sensorhumo =0,humo;
bool sensorvalue=false;
const byte numbersDisplayAnode[10] = {
                          0b1000000,          //0
                          0b1111001,          //1
                          0b0100100,          //2
                          0b0110000,          //3
                          0b0011001,          //4
                          0b0010010,          //5
                          0b0000010,          //6
                          0b1111000,          //7
                          0b0000000,          //8
                          0b0010000};         //9
                          

void setup() {
  Serial.begin(9600); /* begin serial for debug */
  Wire.begin(1); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  //Pins
  pinMode(ledPin, OUTPUT);
  pinMode(l_bano, OUTPUT);
  pinMode(l_sala, OUTPUT);
  pinMode(l_oficina, OUTPUT);
  pinMode(l_cocina, OUTPUT);
  pinMode(l_cuarto, OUTPUT);
  pinMode(pindisplay, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(relevador_foco, OUTPUT);
  digitalWrite(relevador_foco, HIGH);

  //Displays 1 y 2
  for(int i = 0; i < 7;i++) {
    pinMode(display1[i], OUTPUT);  
    pinMode(display2[i], OUTPUT);  
  }
  
  for(int i = 0; i < 7;i++) {
    digitalWrite(display1[i], HIGH);  
    digitalWrite(display2[i], HIGH);
    delay(250);
    digitalWrite(display1[i], LOW);
    digitalWrite(display2[i], LOW);  
    delay(250);
  }
  
 //Iniciamos el display en 0
  lightSegments(0,display1);
  lightSegments(0,display2);
  //lightSegments2(0);
  digitalWrite(pindisplay,0b0010000);
  
  // Iniciamos el servo para que empiece a trabajar con el pin 49
  servoMotor.attach(49);
}

void loop(){
  //eventoalarma();
  delay(10);
}

void receiveEvent(int howMany) {
  
  int accion, ledID, estado,estadoalarma;
  char temperatura_c;
  
  // Si hay dos bytes disponibles
  while (Wire.available() > 0){

    accion = Wire.read();
    Serial.print("Evento llamado: ");
    Serial.println(accion);
  //Accion a realizar el arduino mega al comunicarse con su maestro el arduino mcu

  if(accion == 9){
    Serial.println("Alarma activada!!!");
    digitalWrite(buzzer, HIGH);
  }else if(accion == 10){
    digitalWrite(buzzer, LOW);
  }
  switch(accion){
    case 1: //CASE LEDS
    
      ledID = Wire.read();
      estado = Wire.read();
      Serial.println(ledID);
      Serial.print("Estado: ");
      Serial.println(estado);
      switch(ledID){
        case 2:
          if(estado == 0){
            digitalWrite(l_bano, LOW);
          }else{
            digitalWrite(l_bano, HIGH);
          }
        break;
        case 3:
          if(estado == 0){
            digitalWrite(l_sala, LOW);
          }else{
            digitalWrite(l_sala, HIGH);
          }
        break;
        case 4:
          if(estado == 0){
            digitalWrite(l_oficina, LOW);
          }else{
            digitalWrite(l_oficina, HIGH);
          }
        break;
        case 5:
          if(estado == 0){
            digitalWrite(l_cocina, LOW);
          }else{
            digitalWrite(l_cocina, HIGH);
          }
        break;
        case 6:
          if(estado == 0){
            digitalWrite(l_cuarto, LOW);
          }else{
            digitalWrite(l_cuarto, HIGH);
          }
        break;
        case 7: //COCHERA
          Serial.println("Servo activado!!!");
          if(estado == 0){
            // Desplazamos a la posición 90º
            servoMotor.write(90);
            // Esperamos 1 segundo
            
          }else{
            // Desplazamos a la posición 180º
            servoMotor.write(180);
            // Esperamos 1 segundo
            
          }
        break;
        case 8:
          if(estado == 0){
            digitalWrite(relevador_foco, HIGH);
          }else{
            digitalWrite(relevador_foco, LOW);
          }
        break;
        default:
          Serial.println("No se encontro el LedID");
          digitalWrite(ledPin, HIGH);
          delay(1000);
          digitalWrite(ledPin, LOW);
        break;
      }
    break;
    case 2: //CASE TEMPERATURA
      temperatura_c = Wire.read();
      int temperatura =temperatura_c;
      Serial.print("Temperatura: ");
      Serial.print(temperatura);
      Serial.println(" *C ");
      eventoTemperatura(temperatura);
    break;
  }
  
    Serial.println("Accion completada!!");
  }
}


void eventoTemperatura(int temperatura){
  
  int digitostemp = temperatura; 
   
  if(temperatura < 0){
    digitalWrite(pindisplay,0b0000000);
    digitostemp = -digitostemp;
  }else{
    digitalWrite(pindisplay,0b0010000);
  }

  if(temperatura <10){
    lightSegments(0,display1);
  }

  if(temperatura == 0){
    lightSegments(0,display2);
  }
  
  bool displayest = true;
  while(digitostemp){
    
    int digito = digitostemp % 10;
    
    Serial.println("----");
    Serial.println(digito);
    if(displayest){
      switch(digito){
        case 0: lightSegments(0,display2);break;
        case 1: lightSegments(1,display2);break;
        case 2: lightSegments(2,display2);break;
        case 3: lightSegments(3,display2);break;
        case 4: lightSegments(4,display2);break;
        case 5: lightSegments(5,display2);break;
        case 6: lightSegments(6,display2);break;
        case 7: lightSegments(7,display2);break;
        case 8: lightSegments(8,display2);break;
        case 9: lightSegments(9,display2);break;
        default: lightSegments(0,display2);break;
        }
      displayest = false;
    }else{
      switch(digito){
        case 0: lightSegments(0,display1);break;
        case 1: lightSegments(1,display1);break;
        case 2: lightSegments(2,display1);break;
        case 3: lightSegments(3,display1);break;
        case 4: lightSegments(4,display1);break;
        case 5: lightSegments(5,display1);break;
        case 6: lightSegments(6,display1);break;
        case 7: lightSegments(7,display1);break;
        case 8: lightSegments(8,display1);break;
        case 9: lightSegments(9,display1);break;
        default: lightSegments(0,display1);break;
        }
      
    }
    delay(1000);
    digitostemp /= 10;
  } 
}

void lightSegments(int number,int display[]) {
  byte numberBit = numbersDisplayAnode[number];
  for (int i = 0; i < 7; i++)  {
    int bit = bitRead(numberBit, i);
    digitalWrite(display[i], bit);
  }
}

void requestEvent() {
  sensorhumo = analogRead(4);
  
  if(sensorhumo > 330){
    sensorvalue = true;
  }else{
    sensorvalue = false; 
  }
  Serial.print("Sensor de humo : ");
  Serial.println(sensorhumo);
  Serial.print("Value : ");
  Serial.println(sensorvalue);
  Wire.write(sensorvalue);
   /*send string on request */
}


/* void lightSegments2(int number) {
  byte numberBit = numbersDisplayAnode[number];
  for (int i = 0; i < 7; i++)  {
    int bit = bitRead(numberBit, i);
    digitalWrite(display2[i], bit);
  }
} */
