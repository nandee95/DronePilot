#include <SFML_Packet.h>

void setup() {
  Serial.begin(9600);
}

void loop() {
  //Creating an array
  sf::Uint8 asd[10];
  for(int i=0;i<10;i++) asd[i]=i*2;

  //Creating a packet
  sf::Packet packet;

  //Appending the array to the packet
  packet.append(&asd,sizeof(asd));

  //Extracting the data from the packet
  sf::Uint8 asd2[10];
  for(int i=0;i<10;i++) packet >> asd2[i];

  //Printing the result
  for(int i=0;i<10;i++)
  {
    Serial.print(asd2[i]);
    Serial.print(" ");
  }
  Serial.println();

  //Waiting for 1s
  delay(1000);
}
