/*--- EXO </Army> . Version 1.0 Rev 280 */
/*--- ABV EDUCATION 2016 -- */

//---ECHO : PROGRAMAR CONTINGENCIA PARA PONER MUÑECA EN FUNCIONAMIENTO ---//


//#include <SoftwareSerial.h> (Sin uso)
#include <Servo.h> //Libreria utilizada para el movimiento de los Servomotores

#define LED 13 //Se declara el pin digital 13 como LED
#define BAUDRATE 57600 //Se establecen los Baudios a 57600
#define DEBUGOUTPUT 0

#define EEG_POWER_BANDS 8
#define MAX_PACKET_LENGTH 64 //Se declara como 64 el largo máximo de paquetes a recibir.

#define powercontrol 10
uint32_t eegPower[EEG_POWER_BANDS];
uint8_t packetData[MAX_PACKET_LENGTH];

//Bloque dedicado para el CheckSum de variables
byte generatedChecksum = 0;
byte checksum = 0;
int payloadLength = 0;
byte payloadData[64] = {0};
byte poorQuality = 0;
byte attention = 0;
byte meditation = 0;
byte blink = 0;
byte ASIC_EEG_POWER = 0;
byte EEG_POWER = 0;
byte BAT = 0;
//Fin del bloque de CheckSum

//Variables del sistema
long lastReceivedPacket = 0;
boolean bigPacket = false;

bool flag = true;
int dato = 0;
int dato2 = 0;
int delta = 0;

int pro;

// 11,10,9,6,5,3 pwm

//Se le asigna a cada dedo un Servomotor
Servo pulgar; //10
Servo indice; //11
Servo mayor; //9
Servo anular; //6
Servo menique; //5
Servo muneca; //Muñeca (3) Se utilizará en la version 1.0 CDC

int pos = 0;
int statusLed = 12;

int enviado; //Se envía el numero completo
int num; //Numero del servo
int posicion; //Posicion del servo
int test; //Chequeo


// Arranque de Exo Army
void setup() {

  Serial.begin(9600); //Se inicializa el Serial para Arduino
  Serial1.begin(57600);//Se inicializa el Serial para la Bincha
  Serial.print("EXO ARMY - INICIANDO...\n");
  indice.attach(11);
  pulgar.attach(10);
  mayor.attach(9);
  anular.attach(6);
  menique.attach(5);
  muneca.attach(3); //Pin reservado para el motor de la muñeca. Se aplicará en la versión 2.0
  Serial.print("Servos cargados correctamente. Reiniciando posicion, por favor espere...\n");
  //Bloque destinado a reiniciar la posición de los Servomotores a 0.
  pulgar.write(pos);
  indice.write(pos);
  mayor.write(pos);
  anular.write(pos);
  menique.write(pos);
  muneca.write(pos);
  Serial.print("Servos Reiniciados correctamente.\n Inicializando Interfaz con Mindwave...\n");
  //Fin del bloque

  //mindWave.setup(); Sin uso
  //mindWave.setDebug(true); Sin uso
  pinMode(statusLed, OUTPUT);
  digitalWrite(statusLed, HIGH);
  delay(500);
  digitalWrite(statusLed, LOW);
  Serial.print("Interfaz Mindwave inicializada con exito.\n Exo Army en funcionamiento.\n");

}

////////////////////////////////
// Lee los datos desde la Serial UART //
////////////////////////////////
byte ReadOneByte()

{
  int ByteRead;
  while (!Serial1.available());
  ByteRead = Serial1.read();

#if DEBUGOUTPUT
  Serial.print((char)ByteRead);   //ECHO: el mismo byte fuera del serial USB (Solo para debug)
#endif
  return ByteRead;
}

//Decompila el paquete recibido por ReadOneByte, y muestra en serial:
//Ondas Alpha, Theha, Gamma, Omega, Beta, Meditacion, Atencion y Blink
void MindWave() {
  // Look for sync bytes
  if (ReadOneByte() == 170)
  {
    if (ReadOneByte() == 170)
    {
      payloadLength = ReadOneByte();

      if (payloadLength > 169)         //Payload length can not be greater than 169
        return;
      generatedChecksum = 0;
      for (int i = 0; i < payloadLength; i++)
      {
        payloadData[i] = ReadOneByte();            //Read payload into memory
        generatedChecksum += payloadData[i];
      }

      checksum = ReadOneByte();         //Read checksum byte from stream
      generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum

      if (checksum == generatedChecksum)
      {
        poorQuality = 200;
        attention = 0;
        meditation = 0;


        for (int i = 0; i < payloadLength; i++)
        { // Parse the payload
          switch (payloadData[i])
          {
            case 2:
              i++;
              poorQuality = payloadData[i];
              bigPacket = true;
              break;
            case 4:
              i++;
              attention = payloadData[i];
              break;
            case 5:
              i++;
              meditation = payloadData[i];
              break;
            case 0x80:
              i = i + 3;
              EEG_POWER = payloadData[i];
              break;
            case 0x83:
              i++;
              for (int j = 0; j < EEG_POWER_BANDS; j++) {
                eegPower[j] = ((uint32_t)payloadData[++i] << 16) | ((uint32_t)payloadData[++i] << 8) | (uint32_t)payloadData[++i];
              }
              break;
            case 16:  // blink detected: blink strength
              //  toggleTinyLed();
              blink = payloadData[i];
              break;
            case 0x01:
              i++;
              BAT = payloadData[i];
              break;
            default:
              break;
          } // switch
        } // for loop

#if !DEBUGOUTPUT

        // *** Add your code here ***

        if (bigPacket)
        {
          if (poorQuality <= 80)
            digitalWrite(LED, HIGH);
          else
            digitalWrite(LED, LOW);
          Serial.print("PoorQuality: ");
          Serial.print(poorQuality, DEC);
          Serial.print(" Attention: ");
          Serial.print(attention, DEC);
          Serial.print(" Time since last packet: ");
          Serial.print(millis() - lastReceivedPacket, DEC);
          lastReceivedPacket = millis();
          Serial.print("\n");
          Serial.print("Meditation");
          Serial.print("\n");
          Serial.print(meditation, DEC);
          ////TESTEOOO/////
          Serial.print("\n");
          Serial.print("Blink");
          Serial.print("\n");
          Serial.print(blink);
          ///TESTEOOOO/////
          Serial.print("\n");
          Serial.print("DELTA");
          Serial.print("\n");
          Serial.print(eegPower[0], DEC);
          Serial.print("\n");
          Serial.print("THETA");
          Serial.print("\n");
          Serial.print(eegPower[1], DEC);
          Serial.print("\n");
          Serial.print("LOW ALPHA");
          Serial.print("\n");
          Serial.print(eegPower[2], DEC);
          Serial.print("\n");
          Serial.print("HIGH ALPHA");
          Serial.print("\n");
          Serial.print(eegPower[3], DEC);
          Serial.print("\n");
          Serial.print("LOW BETA");
          Serial.print("\n");
          Serial.print(eegPower[4], DEC);
          Serial.print("\n");
          Serial.print("HIGH BETA");
          Serial.print("\n");
          Serial.print(eegPower[5], DEC);
          Serial.print("\n");
          Serial.print("LOW GAMMA");
          Serial.print("\n");
          Serial.print(eegPower[6], DEC);
          Serial.print("\n");
          Serial.print("MID GAMMA");
          Serial.print("\n");
          Serial.print(eegPower[7], DEC);
          Serial.print("\n");
          Serial.print("EEG_POWER");
          Serial.print("\n");
          Serial.print(EEG_POWER);
          Serial.print("\n");

          Serial.print("Bateria de Neurosky (TESTEO)");
          Serial.print("\n");
          Serial.print(0x1E);
          Serial.print("\n");

          int dives = 0;

          for (int i = 0; i <= 10; i++) {
            dives++;
            pro = pro + eegPower[4];
            pro = pro / dives;
            i++;

          }

          Serial.print("PROMEDIO BETA BAJA");
          Serial.print("\n");
          Serial.print(pro, DEC);
          Serial.print("\n");

          if (flag == true)  {
            dato = eegPower[4];
            flag = false;
          }
          else
            (dato = dato2);
          dato2 = eegPower[4];

          delta = abs(dato2 - dato);
          Serial.println("VALOR ABSOLUTO DE BETA BAJA: \n");
          Serial.println(delta);
          Serial. print("\n");

          switch (attention / 10)
          {
              //ECHO
          }
        }
#endif
        bigPacket = false;
      }
      else {
        // Checksum Error
      }  // end if else for checksum
    } // end if read 0xAA byte
  } // end if read 0xAA byte
}

////////ZONA DE TESTEO DE CODIGO/////////////

void toggleTinyLed() {
}

////////ZONA DE TESTEO DE CODIGO/////////////

void Antietica() {
  pulgar.write(180);
  indice.write(180);
  mayor.write(180);
  anular.write(180);
  menique.write(180);
  delay(5000);
  pulgar.write(0);
  indice.write(0);
  mayor.write(0);
  anular.write(0);
  menique.write(0);
  delay(5000);
}

void metalero() {
  pulgar.write(180);
  indice.write(0);
  mayor.write(180);
  anular.write(180);
  menique.write(0);
  delay(3000);
  pulgar.write(0);
  indice.write(0);
  mayor.write(0);
  anular.write(0);
  menique.write(0);
  delay(3000);
}

void punio() {
  pulgar.write(180);
  indice.write(180);
  mayor.write(180);
  anular.write(180);
  menique.write(180);
  delay(1000);
  pulgar.write(0);
  indice.write(0);
  mayor.write(0);
  anular.write(0);
  menique.write(0);
  delay(1000);
}

void check() {
  menique.write(180);
  delay(500);
  menique.write(0);
  delay(500);
  anular.write(180);
  delay(500);
  anular.write(40);
  delay(500);
  mayor.write(180);
  delay(500);
  mayor.write(0);
  delay(500);
  indice.write(180);
  delay(500);
  indice.write(0);
  delay(500);
  pulgar.write(180);
  delay(500);
  pulgar.write(0);
  delay(500);
}

void oka() {
  menique.write(30);
  anular.write(50);
  mayor.write(80);
  indice.write(150);
  pulgar.write(180);
  delay(8000);
  menique.write(0);
  anular.write(0);
  mayor.write(0);
  indice.write(0);
  pulgar.write(0);
  delay(8000);
}

void contingencia() {
  int x = attention;
  if (x > 0 && x < 25) {

    menique.write(20);
    anular.write(20);
    mayor.write(20);
    indice.write(20);
    pulgar.write(0);
  }
  if (x > 25 && x < 50) {

    menique.write(45);
    anular.write(45);
    mayor.write(45);
    indice.write(45);
    pulgar.write(45);
  }
  if (x > 50 && x < 75) {

    menique.write(100);
    anular.write(100);
    mayor.write(100);
    indice.write(100);
    pulgar.write(100);
  }
  if (x > 75 && x <= 100) {

    menique.write(140);
    anular.write(140);
    mayor.write(140);
    indice.write(140);
    pulgar.write(140);
  }
}
void contigenciav2() {   //CONTINGENCIAV2

  int x = attention; //varaible de atencion

  //condicionales para el movimiento
  if (x > 0 && x <= 18) {
    menique.write(140); //menique
    anular.write(0);
    mayor.write(0);
    indice.write(0);
    pulgar.write(0);
  }
  if (x > 19 && x <= 36) { //anular
    menique.write(0);
    anular.write(140);
    mayor.write(0);
    indice.write(0);
    pulgar.write(0);
  }
  if (x > 37 && x <= 54) { //mayor
    menique.write(0);
    anular.write(0);
    mayor.write(140);
    indice.write(0);
    pulgar.write(0);
  }
  if (x > 55 && x <= 72) {   //indice
    menique.write(0);
    anular.write(0);
    mayor.write(0);
    indice.write(140);
    pulgar.write(0);
  }
  if (x > 73 && x <= 90) { //pulgar
    menique.write(0);
    anular.write(0);
    mayor.write(0);
    indice.write(0);
    pulgar.write(140);
  }
  if (x > 91 && x <= 100) {  //todos los dedos
    menique.write(140);
    anular.write(140);
    mayor.write(140);
    indice.write(140);
    pulgar.write(140);
  }
} //fin de contingencia

void contingenciaM() {
  int x = meditation;
  if (x > 0 && x < 25) {

    menique.write(20);
    anular.write(20);
    mayor.write(20);
    indice.write(20);
    pulgar.write(20);
  }
  if (x > 25 && x < 50) {

    menique.write(45);
    anular.write(45);
    mayor.write(45);
    indice.write(45);
    pulgar.write(45);
  }
  if (x > 50 && x < 75) {

    menique.write(100);
    anular.write(100);
    mayor.write(100);
    indice.write(100);
    pulgar.write(100);
  }
  if (x > 75 && x <= 100) {

    menique.write(140);
    anular.write(140);
    mayor.write(140);
    indice.write(140);
    pulgar.write(140);
  }
}

void Apreton() {
  int x = meditation;
  if (x > 0 && x < 75) {

    menique.write(30);
    anular.write(30);
    mayor.write(30);
    indice.write(30);
    pulgar.write(140);
  }
}

void Muneca() {
  int x = meditation;
  if (x > 0 && x <= 25 ) {
    muneca.write(90); //test :OJO CON LA BARRIDA
  }
}

void ExoSystem() {

  if (Serial.available() >= 1)
  {
    /*
      1- Leer un numero entero por serial
      2- Calculamos su modulo por 10 (sera el numero del motor)
      3- Dividir el entero inicial por 10
      4- Lo que quede, sera la posicion del motor
    */
    enviado = Serial.parseInt();
    num = enviado % 10;
    enviado = enviado / 10;
    posicion = enviado;

    //Hora de mover los servos!
    if (num == 1)
    {
      indice.write(posicion);
    }
    else if (num == 2)
    {
      mayor.write(posicion);
    }
    else if (num == 3)
    {
      anular.write(posicion);
    }
    else if (num == 4)
    {
      menique.write(posicion);
    }
    else if (num == 5)
    {
      pulgar.write(posicion);
    }
    else if (num == 6) {
      muneca.write(posicion);
    }
  }
}

void loop() {
  MindWave();
  ExoSystem();
  contigenciav2();
  Apreton();
  Muneca();
  // contingencia();
  //check();
}
