
#include "PCD8544.h"
#include "SoftwareSerial.h"

SoftwareSerial BT(10,11);

#define echoPin 8
#define trigPin 9
#define LEDPin 13
long duration, cm;
char cadena[255];
int i=0;
long alturaCaixa=130;
boolean dispositivoLigado = false;
boolean permiteAcionamentoAuto = false;
// A custom glyph (a smiley)...
static const byte glyph[] = { B00010000, B00110100, B00110000, B00110100, B00010000 };
 
static PCD8544 lcd;

// The pins used by this library...
#define PCD8544_SCLK  7  /* clock       (display pin 2) */
#define PCD8544_SDIN  6  /* data-in     (display pin 3) */
#define PCD8544_DC    5  /* data select (display pin 4) */
#define PCD8544_RESET 4  /* reset       (display pin 8) */
#define PCD8544_SCE   3  /* enable      (display pin 5) */

void setup(){
  BT.begin(57600);
  BT.println("Conectado ao controle de nivel de agua");
  BT.listen();
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(LEDPin, OUTPUT);
  pinMode(13, OUTPUT);
  lcd.begin(84,48);
  lcd.createChar(0, glyph);
}
 
void loop(){
  char dato;
  while (BT.available()) {
    dato=BT.read();
    cadena[i++]=dato;
  }
  if (strlen(cadena) != 0) {
     //char dato=BT.read(); //Guarda los datos carácter a carácter en la variable "dato"
    //cadena[i++]=dato; //Vamos colocando cada carácter recibido en el array "cadena"
    
    //Cuando reciba una nueva línea (al pulsar enter en la app) entra en la función
    Serial.println(cadena);
    if(dato=='0' || dato=='1' || strstr(cadena,"auto")!=0 || strstr(cadena,"man")!=0)
    {
 
      BT.write("Comando ");
      //GREEN LED
      if(strstr(cadena,"1")!=0) {
        digitalWrite(13, HIGH);
        Serial.println("Ta ligado! ");
        BT.write("<ligar bomba> ");
      } else {
        digitalWrite(13, LOW);
      }
      if (strstr(cadena,"auto")!=0) {
        permiteAcionamentoAuto=true;
      } else if (strstr(cadena,"man")!=0) {
        permiteAcionamentoAuto=false;
      }
      
      BT.write("executado com sucesso."); //Enviamos un retorno de carro de la app. La app ya crea una línea nueva
      clean(); //Ejecutamos la función clean() para limpiar el array
    }
  }

  calculaDistancia();
 
  drawDisplay();

  drawDispositivo();

  delay(5000);
}
 
long microsecondsToCentimeters(long microseconds){
  // La velocidad del sonido a 20º de temperatura es 340 m/s o
  // 29 microsegundos por centrimetro.
  // La señal tiene que ir y volver por lo que la distancia a 
  // la que se encuentra el objeto es la mitad de la recorrida.
  return microseconds / 29 /2 ;
}

//Limpia el array
void clean()
{
  for (int cl=0; cl<=i; cl++)
  {
    cadena[cl]=0;
  }
  i=0;
}

void drawDisplay() {
    // Write a piece of text on the first line...
  lcd.setCursor(0, 0);
  lcd.print("   Controle   ");
  lcd.print("nivel de agua");
 
  // Write the counter on the second line...
  lcd.setCursor(0, 3);
  lcd.print(cm, DEC);
  lcd.print(" cm");
  lcd.write(' ');
  lcd.setCursor(0,5);
  lcd.write(0);  // write the smiley
}

void drawDispositivo() {
  int x = alturaCaixa - cm;
  if (permiteAcionamentoAuto==true) {
    if (x <= 100) {
        digitalWrite(13, HIGH);
        BT.println("acionamento automatico.");
        dispositivoLigado = true;
    } else if (dispositivoLigado==true){
        digitalWrite(13, LOW);
        BT.println("desligamento automatica.");
    }
  }
  
  BT.write("volume de agua: ");
  BT.print(x);
  BT.println(" litros");
}

void calculaDistancia() {
  digitalWrite(trigPin, LOW); 
  delayMicroseconds(2); 
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5); 
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
 
  cm = microsecondsToCentimeters(duration);
}