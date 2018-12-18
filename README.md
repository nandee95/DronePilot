Work in progress

# Dependencies
## Pilot (C++)
[SFML](https://www.sfml-dev.org)\
[escapi](https://github.com/jarikomppa/escapi)

## Drone (Arduino)
[RF24-STM](https://github.com/jaretburkett/RF24-STM)\
[MPU9250](https://github.com/bolderflight/MPU9250)\
[SFML_Packet](https://github.com/nandee95/SFML_Packet)

## Transmitter (Arduino)
[RF24](https://github.com/nRF24/RF24)\
[SFML_Packet](https://github.com/nandee95/SFML_Packet)

# Hardware
## Drone
<table width="100%">
  <thead>
  	<tr><td>Image</td><td>Name</td><td>Specs</td><td>Op. Voltage</td><td>Pinout</td></tr>
  </thead>
  <tbody>
  <tr><td><img src="https://wiki.stm32duino.com/images/thumb/d/db/STM32_Blue_Pill_perspective.jpg/450px-STM32_Blue_Pill_perspective.jpg" width=120></td><td><a href="https://wiki.stm32duino.com/index.php?title=Blue_Pill">STM32 Blue Pill</a></td><td>STM32F103C8<br>Clock: 72Mhz<br>RAM: 20KB<br>Flash: 64/128KB</td><td>5V (USB)</td><td><a href="https://wiki.stm32duino.com/images/a/ae/Bluepillpinout.gif" target="_blank"><img src="https://wiki.stm32duino.com/images/a/ae/Bluepillpinout.gif" width=120></a></td></tr>
  <tr><td><img src="https://lastminuteengineers.com/wp-content/uploads/2018/07/nRF24L01-PA-LNA-External-Antenna-Wireless-Transceiver-Module.png" width=120></td><td><a href="https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/">NRF24L01</a></td><td>Distance: up to 1km<br>Frequency: 2.4Ghz<br>Interface: SPI<br>Amplified</td><td>3.3V</td><td><a href="https://lastminuteengineers.com/wp-content/uploads/2018/07/Pinout-nRF24L01-PA-LNA-External-Antenna-Wireless-Transceiver-Module.png" target="_blank"><img src="https://lastminuteengineers.com/wp-content/uploads/2018/07/Pinout-nRF24L01-PA-LNA-External-Antenna-Wireless-Transceiver-Module.png" width=120></a></td></tr>
  <tr><td align=middle><img src="https://camo.githubusercontent.com/c320f6e5db1aed989b6a1282a2ddb71582c13c60/687474703a2f2f706c617967726f756e642e61726475696e6f2e63632f75706c6f6164732f4d61696e2f6d70752d363035302e6a7067" width=50></td><td><a href="https://camo.githubusercontent.com/c320f6e5db1aed989b6a1282a2ddb71582c13c60/687474703a2f2f706c617967726f756e642e61726475696e6f2e63632f75706c6f6164732f4d61696e2f6d70752d363035302e6a7067">MPU9250</a></td><td>Gyroscope<br>Accelerometer<br>Magnetometer<br>Interface: I<sup>2</sup>C</td><td>5V</td><td>-</td></tr>
   <tr><td align=middle><img src="https://www.makerfabs.com/image/cache/makerfabs/BMP280%20Barometer/BMP280%20Barometer_1-1000x750.jpg" width=80></td><td><a href="https://www.bosch-sensortec.com/bst/products/all_products/bmp280">BMP280</a></td><td>Barometer<br>Thermometer<br>Interface: I<sup>2</sup>C</td><td>3.3V</td><td>-</td></tr>
   <tr><td align=middle><img src="https://www.robotshop.com/media/catalog/product/cache/image/625x625/9df78eab33525d08d6e5fb8d27136e95/h/c/hc-sr04-ultrasonic-range-finder-2.png" width=80></td><td><a href="https://lastminuteengineers.com/arduino-sr04-ultrasonic-sensor-tutorial/">HC-SR04</a></td><td>Range: 5m<br>Resolution: 1cm</td><td>5V</td><td><a href="https://lastminuteengineers.com/wp-content/uploads/2018/06/HC-SR04-Ultrasonic-Distance-Sensor-Pinout.jpg" target="_blank"><img src="https://lastminuteengineers.com/wp-content/uploads/2018/06/HC-SR04-Ultrasonic-Distance-Sensor-Pinout.jpg" width=120></a></td></tr>
   </tbody>
</table>

## Receiver
<table>
  <thead>
  	<tr><td>Image</td><td>Name</td><td>Specs</td><td>Op. Voltage</td><td>Pinout</td></tr>
  </thead>
  <tbody>
  <tr><td><img src="http://wiki.sunfounder.cc/images/0/0c/Nano.png" width=120></td><td><a href="https://wiki.eprolabs.com/index.php?title=Arduino_Nano">Arduino nano</a></td><td>ATmega328<br>Clock: 16Mhz<br>RAM: 2KB<br>Flash: 32KB</td><td>5V (USB)</td><td><a href="https://simba-os.readthedocs.io/en/latest/_images/arduino-nano-pinout.png" target="_blank"><img src="https://simba-os.readthedocs.io/en/latest/_images/arduino-nano-pinout.png" width=120></a></td></tr>
  <tr><td><img src="https://lastminuteengineers.com/wp-content/uploads/2018/07/nRF24L01-PA-LNA-External-Antenna-Wireless-Transceiver-Module.png" width=120></td><td><a href="https://lastminuteengineers.com/nrf24l01-arduino-wireless-communication/">NRF24L01</a></td><td>Distance: up to 1km<br>Frequency: 2.4Ghz<br>Interface: SPI<br>Amplified</td><td>3.3V</td><td><a href="https://lastminuteengineers.com/wp-content/uploads/2018/07/Pinout-nRF24L01-PA-LNA-External-Antenna-Wireless-Transceiver-Module.png" target="_blank"><img src="https://lastminuteengineers.com/wp-content/uploads/2018/07/Pinout-nRF24L01-PA-LNA-External-Antenna-Wireless-Transceiver-Module.png" width=120></a></td></tr>
   </tbody>
</table>



# Future plans
<ul>
	<li>Porting the code for UNIX</li>
	<li>Analog video feed</li>
	<li>Ultrasonic sensor on the bottom</li>
</ul>
