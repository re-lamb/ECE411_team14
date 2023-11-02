/**************************************************************************
 This is an example for our Monochrome OLEDs based on SSD1306 drivers

 Pick one up today in the adafruit shop!
 ------> http://www.adafruit.com/category/63_98

 This example is for a 128x64 pixel display using SPI to communicate
 4 or 5 pins are required to interface.

 Adafruit invests time and resources providing this open
 source code, please support Adafruit and open-source
 hardware by purchasing products from Adafruit!

 Written by Limor Fried/Ladyada for Adafruit Industries,
 with contributions from the open source community.
 BSD license, check license.txt for more information
 All text above, and the splash screen below must be
 included in any redistribution.
 **************************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI  18
#define OLED_CLK   19
#define OLED_DC    16
#define OLED_CS    17
#define OLED_RESET 21
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

// create a subclass to handle read/write callbacks
class MyCallbacks: public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic);
  void onWrite(BLECharacteristic *pCharacteristic);
};

void MyCallbacks::onRead(BLECharacteristic *pCharacteristic)
{
    Serial.println("Client read " + pCharacteristic->getValue().c_str());

    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Client read %s", pCharacteristic->getValue().c_str());
    display.display();
}
  
void MyCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    Serial.println("Client wrote " + pCharacteristic->getValue().c_str());
    
    display.clearDisplay();
    display.setCursor(0,0);
    display.printf("Client wrote %s", pCharacteristic->getValue().c_str());
    display.display();
}

void setup() {
  Serial.begin(115200);

  Serial.printf("\nCHIP MAC: %012llx\n", ESP.getEfuseMac());
  //Serial.printf("\nCHIP MAC: %012llx\n", ESP.getChipId());

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.printf("\nMAC: %012llx\n", ESP.getEfuseMac());
  display.display();

  display.println("Starting BLE");
  display.display();

  BLEDevice::init("test server");
  BLEServer *pServer = BLEDevice::createServer();
  Serial.println("Server created");

  BLEService *pService = pServer->createService(SERVICE_UUID);
  Serial.println("Service created");

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  Serial.println("Characteristic created");

  pCharacteristic->setCallbacks(new MyCallbacks());
  Serial.println("Callbacks attached!");

  pCharacteristic->setValue("0");
  Serial.println("Characteristic inited!");

  pService->start();
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);

  Serial.println("About to start advertising");
  BLEDevice::startAdvertising();
}

void loop()
{
  Serial.println("Main loop entered");


  for(;;)
  {
    delay(1000);
  }
}
  