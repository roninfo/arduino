
#include "IRremote.h"
#include "IRremoteInt.h"

  // Biblioteca para conunicação com o Arduino.

int RECV_PIN = 6; // Pino utilizado para fazer a leitura dos valores dos botões do controle remoto.

IRrecv irrecv(RECV_PIN);

decode_results results;  // Decodifica os dados do controle remoto.

void setup()
{
  Serial.begin(9600);  // Inicia o Serial Monitor.
  irrecv.enableIRIn(); // Inicia o infrared receptor.
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
}
String valor = "";
void loop() {
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);  // Imprime na tela do Serial Monitor o valor dos botões do controle remoto em hexatecimal.
    Serial.println(results.value);
    valor = (String)results.value;
    irrecv.resume(); // Aguarda para receber o novo valor quando for pressionado um outro botão.
  }
  if (valor == "16753245" || valor == "551489775") {
  	if (digitalRead(7) == 1) {
		digitalWrite(7, LOW);
		digitalWrite(8, LOW);
		digitalWrite(9, LOW);
  	} else {
  		digitalWrite(7, HIGH);
  	}
  } else if (valor == "16724175") {
  	digitalWrite(8, HIGH);
  } else if (valor == "16718055") {
  	digitalWrite(9, HIGH);
  }
  valor = "";
  delay(500);  // Dá um tempo de 100 ms entre um valor e outro.
}