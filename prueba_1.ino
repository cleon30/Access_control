//------------------------------------------------------------------------------------------------------
//                                       Variables y funciones TECLADO
#include <LiquidCrystal_I2C.h> 
byte pinesFilas[] = {32,33,25,26}; 
byte pinesColumnas[] = {27,14,12}; 
char teclas[4][4] = {{'1','2','3'}, 
                     {'4','5','6'}, 
                     {'7','8','9'}, 
                     {'0','0','#'}};
char CODIGO[5];
char CONTRASENA[5]= "1234";       
int nL; //filas del teclado
int nC; //columnas del teclado
int counter; //contador auxilar para el registro de la contraseña
int counter2; //cuenta el numero de veces que se ha puesto la contraseña de manera incorrecta
int CERROJO = 13; //pin del cerrojo de la puerta
int ALARMA = 2; //pin de la alarma
int dada;
bool autentificacion = 0;

LiquidCrystal_I2C lcd(0x27,16,2);                               //LCD - Dimensiones(tamaño, Columnas, Filas) 
                                    
void setup() {
  lcd.init();                                                   //LCD - Inicializar el LCD
  lcd.backlight();
  pinMode(CERROJO, OUTPUT);
  digitalWrite(CERROJO, LOW); //pestillo cerrado
  pinMode(ALARMA, OUTPUT);
  digitalWrite(ALARMA, LOW); //alarma apagada
  counter2=0;
  counter=0;
//configuración teclado
  
  for (int nL = 0; nL <= 3; nL++) {
     pinMode(pinesFilas[nL], OUTPUT);
     digitalWrite(pinesFilas[nL], HIGH);
  }
  for (int nC = 0; nC <= 2; nC++) {
     pinMode(pinesColumnas[nC], INPUT_PULLUP);
  } 
   
  Serial.begin(9600);
  //Serial.println("Teclado 4x4");
  Serial.println();
  lcd.clear();
}
 
void loop()
{
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
          CODIGO[counter]=teclas[nL][nC]; //guarda los digitos presionados para compararlos con la contraseña
          /*Serial.print("Tecla: ");
          Serial.println(teclas[nL][nC]);
          Serial.print("CODIGO: ");
          Serial.println(CODIGO);
          Serial.println(CONTRASENA);*/
          lcd.print(teclas[nL][nC]); 
          counter=counter+1;
          while(digitalRead(pinesColumnas[nC]) == LOW){}
        }
      }
      digitalWrite(pinesFilas[nL], HIGH);
    }

    if (String(CODIGO)==String(CONTRASENA) and counter==4){
      Serial.print("SISTEMA DESBLOQUEADO");
      for (int i= 0; i < 4; i++){
      digitalWrite(ALARMA, HIGH);
      delay(100);
      digitalWrite(ALARMA, LOW);
      delay(50); }
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CLAVE CORRECTA");
      counter2=0;
      counter=0;
      delay(3000);
      lcd.clear();
      autentificacion=1;
 
    }
    else if (counter==4) {
      counter2=counter2+1;
      counter=0;
      Serial.println("CLAVE INCORRECTA");
      Serial.print("INTENTE OTRA VEZ"); 
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("CLAVE INCORRECTA");
      lcd.setCursor(0, 1);
      lcd.print("INTENTE OTRA VEZ");
      delay(3000);
      lcd.clear();
      //Serial.print(CODIGO);
      //Serial.println(CONTRASENA);
      
      if (counter2==3) {
      digitalWrite(ALARMA, HIGH); //se activa la alarma
      counter2=0;
     }     
    }

     if (Serial.available()) {
      dada = Serial.read();
      if (dada=='1')  {
        digitalWrite(ALARMA,HIGH);
      }
      if (dada=='0') {
        digitalWrite(ALARMA,LOW);
      }
    }

    if (autentificacion==1){
      digitalWrite(CERROJO, HIGH);
      autentificacion=0; 
      }


      
   
   delay(10);


}
