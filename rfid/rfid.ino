#include <SPI.h>  // Biblioteca do Serial Peripheral Interface (Interface Periférica Serial).
#include <MFRC522.h>  // Biblioteca do módulo RFID-RC522.

// CONFIGURAÇÃO NO ARDUINO MEGA
// MOSI = 51, MISO = 50, SCK = 52, SS = 53, RST = 5

#define SS_PIN 53  // Define o pino 53 como Escravo.
#define RST_PIN 5  // Define o pino 5 como Reset.
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Cria um objeto MFRC522.

void setup() {
 Serial.begin(9600); // Inicializa a comunicação serial com o PC.
 SPI.begin();  // Inicializa o SPI bus.
 mfrc522.PCD_Init(); // Inicializa o cartão MFRC522.
 Serial.println("Scan a MIFARE Classic PICC to demonstrate Value Blocks.");  // Imprime no Serial Monitor a frase entre " ".
}

void loop() {

 // Procura por novos cartões.
 if ( ! mfrc522.PICC_IsNewCardPresent()) {
  return;
 }

 // Seleciona um dos cartões.
 if ( ! mfrc522.PICC_ReadCardSerial()) {
  return;
 }
 // Agora um cartão é selecionado. O UID e SAK estão no mfrc522.unis.

        // Descarrega UID.
 Serial.print("Card UID:");
 for (byte i = 0; i < mfrc522.uid.size; i++) {
  Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
  Serial.print(mfrc522.uid.uidByte[i], HEX);
 } 
 Serial.println();

 // Descarrega PICC.
 byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
 Serial.print("PICC type: ");
 Serial.println(mfrc522.PICC_GetTypeName(piccType));
 if ( piccType != MFRC522::PICC_TYPE_MIFARE_MINI 
  && piccType != MFRC522::PICC_TYPE_MIFARE_1K
  && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
  Serial.println("This sample only works with MIFARE Classic cards.");
  return;
 }

 // Orepara a chave - todas as chaves estão configuradas para FFFFFFFFFFFFh (Padrão de fábrica).
 MFRC522::MIFARE_Key key;
 for (byte i = 0; i < 6; i++) {
  key.keyByte[i] = 0xFF;
 }

 // Nesta amostra nos usamos o segundo setor (bloco 4-7).
 byte sector  = 1;
 byte valueBlockA = 5;
 byte valueBlockB = 6;
 byte trailerBlock = 7;
 
 // Valida usando a chave A.
 Serial.println("Authenticating using key A...");  // Imprime no Serial Monitor.
 byte status;
 status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
 if (status != MFRC522::STATUS_OK) {
  Serial.print("PCD_Authenticate() failed: ");
  Serial.println(mfrc522.GetStatusCodeName(status));
  return;
 }
 
 // Precisamos de um setor que define blocos 5 e 6 com valores de blocos e que permita a chave B. 
 byte trailerBuffer[] = { 255,255,255,255,255,255,  0,0,0,  0,  255,255,255,255,255,255};  // Mantem por teclas padrão.
 // g1=6 => Set block 5 as value block. Must use Key B towrite & increment, A or B can be used for derement.
 // g2=6 => Same thing for block 6.
 // g3=3 => Key B must be used to modify the Sector Trailer. Key B becomes valid. 
 mfrc522.MIFARE_SetAccessBits(&trailerBuffer[6], 0, 6, 6, 3);
 
 // Agora nos lemos o setor e vemos se isto ista como nos queremos que seja.
 Serial.println("Reading sector trailer...");
 byte buffer[18];
 byte size = sizeof(buffer);
 status = mfrc522.MIFARE_Read(trailerBlock, buffer, &size);
 if (status != MFRC522::STATUS_OK) {
  Serial.print("MIFARE_Read() failed: ");
  Serial.println(mfrc522.GetStatusCodeName(status));
  return;
 }
 if ( buffer[6] != trailerBuffer[6]
  && buffer[7] != trailerBuffer[7]
  && buffer[8] != trailerBuffer[8]) {
  Serial.println("Writing new sector trailer...");  // Imprime no Serial Monitor.
  status = mfrc522.MIFARE_Write(trailerBlock, trailerBuffer, 16);
  if (status != MFRC522::STATUS_OK) {
   Serial.print("MIFARE_Write() failed: ");
   Serial.println(mfrc522.GetStatusCodeName(status));
   return;
  }
 }
 
 // Valida usando chave B.
 Serial.println("Authenticating again using key B...");  // Imprime no Serial Monitor.
 status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
 if (status != MFRC522::STATUS_OK) {
  Serial.print("PCD_Authenticate() failed: ");  // Se haver falha na autenticação uma mensagem será exibida no Serial Monitor.
  Serial.println(mfrc522.GetStatusCodeName(status));
  return;
 }
 
 /*OBS.:
                O valor dos blocos tem um valor com sinal de 32 bit armazenado três vezes e um endereço de 8 bits armazenados 4 vezes. 
                Certifique-se de blocos valueBlockA e valueBlockB tem esse formato: formatBlock (valueBlockA); */

 // Adciona 1 no valor de valueBlockA e armazena o resultado em valueBlockA.
 Serial.print("Adding 1 to value of block "); Serial.println(valueBlockA);
 status = mfrc522.MIFARE_Increment(valueBlockA, 1);
 if (status != MFRC522::STATUS_OK) {
  Serial.print("MIFARE_Increment() failed: ");
  Serial.println(mfrc522.GetStatusCodeName(status));
  return;
 }
 status = mfrc522.MIFARE_Transfer(valueBlockA);
 if (status != MFRC522::STATUS_OK) {
  Serial.print("MIFARE_Transfer() failed: ");
  Serial.println(mfrc522.GetStatusCodeName(status));
  return;
 }
 
 // Descarrega o resultado.
 mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
 
 // Pausa PICC
 mfrc522.PICC_HaltA();

 // Para a criptografia em PCD.
 mfrc522.PCD_StopCrypto1();
}

void formatBlock(byte blockAddr) {
 Serial.print("Reading block "); Serial.println(blockAddr);
 byte buffer[18];
 byte size = sizeof(buffer);
 byte status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
 if (status != MFRC522::STATUS_OK) {
  Serial.print("MIFARE_Read() failed: ");
  Serial.println(mfrc522.GetStatusCodeName(status));
  return;
 }

 if ( (buffer[0] == (byte)~buffer[4])
  && (buffer[1] == (byte)~buffer[5])
  && (buffer[2] == (byte)~buffer[6])
  && (buffer[3] == (byte)~buffer[7])
  
  && (buffer[0] == buffer[8])
  && (buffer[1] == buffer[9])
  && (buffer[2] == buffer[10])
  && (buffer[3] == buffer[11])
  
  && (buffer[12] == (byte)~buffer[13])
  && (buffer[12] ==        buffer[14])
  && (buffer[12] == (byte)~buffer[15])) {
  Serial.println("Block has correct Block Value format.");
 }
 else {
  Serial.println("Writing new value block...");
  byte valueBlock[] = { 0,0,0,0,  255,255,255,255,  0,0,0,0,   blockAddr,~blockAddr,blockAddr,~blockAddr };
  status = mfrc522.MIFARE_Write(blockAddr, valueBlock, 16);
  if (status != MFRC522::STATUS_OK) {
   Serial.print("MIFARE_Write() failed: ");
   Serial.println(mfrc522.GetStatusCodeName(status));
  }
 }  
} // Fim do formatBlock()
