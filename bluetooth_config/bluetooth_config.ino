#include <SoftwareSerial.h>  // Biblioteca responsável pela comunicação serial.
  
  
SoftwareSerial mySerial(14,15); // RX, TX 
String command = "";  // Responsável por reconhecer os comandos no AT.
  
void setup()  
{ 
   
  Serial.begin(115200);  // Abre a porta de comunicação serial para reconhecer os comandos AT.  
  Serial.println("Type AT commands!");   
  mySerial.begin(9600);   // Taxa de transferência na "Porta COM X" a 9600bps.
}  
  
void loop() 
{ 
  // Lê o dispositivo de saída disponível. 
  if (mySerial.available())
  { 
     while(mySerial.available())
       { // Enquanto haver mais para ser lido, mantenha lendo.
       command += (char)mySerial.read(); 
     } 
   Serial.println(command); 
   command = "";  
  } 
 
  // Lê a entrada o usuário se disponível. 
  if (Serial.available())
  { 
    delay(10); // Pequeno delay. 
    mySerial.write(Serial.read()); 
  } 
  
} // Fim do loop.
