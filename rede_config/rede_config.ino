#include <SPI.h>  // Protocolo utilizado para fazer a comunicação com o Arduino e Shield Ethernet
#include <Ethernet.h>  // Biblioteca utilizada para funções internas do shield

// A linha abaixo permite definir o endereço físico (MAC ADDRESS) da placa de rede.

byte mac[] = { 0xAB, 0xCD, 0x12, 0x34, 0xFF, 0xCA };

//Os valores abaixo definem o endereço IP, gateway e máscara.

IPAddress ip(192,168,0,99);  // Define o endereço IP.
IPAddress gateway(192,168,0,254);  // Define o gateway.
IPAddress subnet(255, 255, 255, 0);  // Define a máscara de rede.

void setup()
{
  Ethernet.begin(mac, ip);  // Inicializa a placa com os dados fornecidos
}

void loop() {  // Nada a declarar na função de loop.
 
}
