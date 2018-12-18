#include <SFML_Packet.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  //Variables to store the result
  sf::Uint8 i8;
  sf::Uint16 i16;
  sf::Uint32 i32;

  //Filling and extracting the packet
  sf::Packet packet;
  packet << (sf::Uint8)123 << (sf::Uint16)12345 << (sf::Uint32)123456789;
  packet >> i8 >> i16 >> i32;

  //Printing the result
  Serial.print(i8);
  Serial.print(" ");
  Serial.print(i16);
  Serial.print(" ");
  Serial.println(i32);

  //Waiting for 1s
  delay(1000);
}
