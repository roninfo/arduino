
#include "PCD8544.h"
#include "SoftwareSerial.h"

SoftwareSerial BT(10,11);

#define echoPin 8
#define trigPin 9
#define LEDPin 13
long duration, cm;
char comandosBT[255];
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
  char dadosDoBT;
  while (BT.available()) {
    dadosDoBT=BT.read();
    comandosBT[i++]=dadosDoBT;
  }
  if (strlen(comandosBT) != 0) {
    Serial.println(comandosBT);
    if(dadosDoBT=='0' || dadosDoBT=='1' || strstr(comandosBT,"auto")!=0 || strstr(comandosBT,"man")!=0)
    {
 
      BT.write("Comando ");
      
      if(strstr(comandosBT,"1")!=0) {
        digitalWrite(13, HIGH);
        Serial.println("Ta ligado! ");
        BT.write("<ligar bomba> ");
      } else {
        digitalWrite(13, LOW);
      }
      if (strstr(comandosBT,"auto")!=0) {
        permiteAcionamentoAuto=true;
      } else if (strstr(comandosBT,"man")!=0) {
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
    comandosBT[cl]=0;
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