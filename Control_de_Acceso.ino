//------------------------------------------------------------------------------------------------------
//                                       Variables y funciones TECLADO
#include <SPI.h>//https://www.arduino.cc/en/reference/SPI
#include <MFRC522.h>//https://github.com/miguelbalboa/rfid
#include <LiquidCrystal_I2C.h> 
byte pinesFilas[] = {32,33,25,26}; 
byte pinesColumnas[] = {27,14,12}; 
char teclas[4][4] = {{'1','2','3'}, 
                     {'4','5','6'}, 
                     {'7','8','9'}, 
                     {'0','0','#'}};
char CODIGO[5];
byte codi;
char CONTRASENA[5]= "1234"; 
#define SS_PIN 5
#define RST_PIN 17     
const int ipaddress[4] = {103, 97, 67, 25}; 
byte nuidPICC[4] = {0, 0, 0, 0};
MFRC522::MIFARE_Key key;
MFRC522 rfid = MFRC522(SS_PIN, RST_PIN);
int nL; //filas del teclado
int nC; //columnas del teclado
int counter; //contador auxilar para el registro de la contraseña
int counter2; //cuenta el numero de veces que se ha puesto la contraseña de manera incorrecta
int CERROJO = 15; //pin del cerrojo de la puerta
int ALARMA = 2; //pin de la alarma
int dada;
bool leer_NFC = 0;
int LED = 13;
int trig_pin_Mitte = 4;     //Sensor middle
int echo_pin_Mitte = 16;
int distance_Mitte;

LiquidCrystal_I2C lcd(0x27,16,2);                               //LCD - Dimensiones(tamaño, Columnas, Filas) 
                                    
void setup() {
  Serial.begin(9600);
  Serial.println("LABEL,hora,codigo");
  SPI.begin();
  rfid.PCD_Init();
  rfid.PCD_DumpVersionToSerial();
  lcd.init();                                                   //LCD - Inicializar el LCD
  lcd.backlight();
  pinMode(CERROJO, OUTPUT);
  digitalWrite(CERROJO, LOW); //pestillo cerrado
  pinMode(ALARMA, OUTPUT);
  digitalWrite(ALARMA, LOW); //alarma apagada
  counter2=0;
  counter=0;
  pinMode(trig_pin_Mitte,OUTPUT);
  pinMode(echo_pin_Mitte,INPUT);
  pinMode(LED,OUTPUT);
  
 
//configuración teclado
  
  for (int nL = 0; nL <= 3; nL++) {
     pinMode(pinesFilas[nL], OUTPUT);
     digitalWrite(pinesFilas[nL], HIGH);
  }
  for (int nC = 0; nC <= 2; nC++) {
     pinMode(pinesColumnas[nC], INPUT_PULLUP);
  } 
  lcd.clear();
}
 
void loop()
{
  digitalWrite(trig_pin_Mitte,HIGH);                //We send an order to the sensor to send a signal into the air to check for obstacles. 
  delayMicroseconds(10);
  digitalWrite(trig_pin_Mitte,LOW);   
  distance_Mitte=pulseIn(echo_pin_Mitte,HIGH);   //We take the duration the signal takes to come back to the sensor.
  Serial.println(distance_Mitte);
  digitalWrite(LED, LOW);
  if (distance_Mitte <3000  or distance_Mitte >40000) {

  digitalWrite(LED, HIGH);
readRFID();
    //TECLADO
    //Barrido por las filas
    lcd.setCursor(0, 0);
    lcd.print("INGRESE CODIGO");
    lcd.setCursor(counter, 1);
    for (int nL = 0; nL <= 3; nL++)
    {
      digitalWrite(pinesFilas[nL], LOW);
      
      //Barrido en columnas buscando un LOW
      for (int nC = 0; nC <= 2; nC++) {
        if (digitalRead(pinesColumnas[nC]) == LOW)
        {
          CODIGO[counter]=teclas[nL][nC];                 //guarda los digitos presionados para compararlos con la contraseña
          lcd.print(teclas[nL][nC]);
          digitalWrite(ALARMA, HIGH);
          delay(35);
          digitalWrite(ALARMA, LOW); 
          counter=counter+1;
          while(digitalRead(pinesColumnas[nC]) == LOW){}
        }
      }
      digitalWrite(pinesFilas[nL], HIGH);
    }

    if ((String(CODIGO)==String(CONTRASENA) and counter==4) or String(codi) == "202" or String(codi) == "252"){
      for (int i= 0; i < 4; i++){
      digitalWrite(ALARMA, HIGH);
      delay(100);
      digitalWrite(ALARMA, LOW);
      delay(50); }
      delay(3000);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CLAVE CORRECTA");
      digitalWrite(CERROJO, HIGH);
      delay(3000);
      if (String(codi) == "252"){lcd.print("BIENVENIDO CARLOS");
      delay(3000);
      }
      else if (String(codi) == "202"){
        lcd.clear();
        lcd.print("BIENVENIDO TOMAS");
      delay(3000);
      lcd.clear();
      }

      
      digitalWrite(CERROJO, LOW);
      if ((String(CODIGO)==String(CONTRASENA) and counter==4)){
        Serial.print("DATA,TIME,");
        Serial.println(CODIGO);
      }
      else if (String(codi) == "202"){
        Serial.print("DATA,TIME,");
        Serial.println(codi);
      }
      counter2=0;
      counter=0;
      codi = 0;
      
      lcd.clear();
      
 
    }
    else if ((counter==4) or (leer_NFC==1 and (String(codi) != "202" or String(codi) != "252"))) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CLAVE INCORRECTA");
      lcd.setCursor(0, 1);
      lcd.print("INTENTE OTRA VEZ");
      if (counter==4){
        Serial.print("DATA,TIME,");
        Serial.println(CODIGO);
      }
      else if (leer_NFC==1 and String(codi) != "202"){
        Serial.print("DATA,TIME,");
        Serial.println(codi);
      }
      counter2=counter2+1;
      counter=0;
      leer_NFC=0;
      delay(3000);
      lcd.clear();
      
      
      if (counter2==3) {
      digitalWrite(ALARMA, HIGH); //se activa la alarma
      delay(2000);
      digitalWrite(ALARMA, LOW);
      counter2=0;
     }     
    }
 
   
   delay(10);


}}

void readRFID(void ) { /* function readRFID */
 ////Read RFID card
 for (byte i = 0; i < 6; i++) {
   key.keyByte[i] = 0xFF;
 }
 // Look for new 1 cards
 if ( ! rfid.PICC_IsNewCardPresent())
   return;
 // Verify if the NUID has been readed
 if (  !rfid.PICC_ReadCardSerial())
   return;
 // Store NUID into nuidPICC array
 for (byte i = 0; i < 4; i++) {
   nuidPICC[i] = rfid.uid.uidByte[i];
 }
 codi = rfid.uid.uidByte[1];
 leer_NFC=1;
 rfid.PICC_HaltA();
 rfid.PCD_StopCrypto1();
}
