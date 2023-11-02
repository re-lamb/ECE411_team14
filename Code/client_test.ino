#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define BUTTON 21

static BLEUUID testServUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
static BLEUUID testCharUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

static boolean doConnect = false;
static boolean connected = false;

static BLEAddress *pServerAddress;
static BLERemoteCharacteristic *testCharacteristic;

int scanTime = 0; // In seconds
int value = 0;    // a counter

BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
      if (advertisedDevice.isAdvertisingService(testServUUID))
      {
        Serial.printf("Found our service!\n");
        pServerAddress = new BLEAddress(advertisedDevice.getAddress());
        doConnect = true;

        // stop scanning
        pBLEScan->stop();
        Serial.println("Scanning stopped");
      }
    }
};

//Connect to the BLE Server that has the name, Service, and Characteristics
bool connectToServer(BLEAddress pAddress) {
   BLEClient* pClient = BLEDevice::createClient();
 
  // Connect to the remove BLE Server.
  pClient->connect(pAddress);
 
  // Obtain a reference to the service we are after in the remote BLE server.
  BLERemoteService* pRemoteService = pClient->getService(testServUUID);
  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(testServUUID.toString().c_str());
    return (false);
  }

   // Obtain a reference to the characteristics in the service of the remote BLE server.
  testCharacteristic = pRemoteService->getCharacteristic(testCharUUID);
  if (testCharacteristic == nullptr)
  {
    Serial.print("Failed to find our characteristic UUID");
    return false;
  }

  return true;
}


void setup() {
  Serial.begin(115200);
  Serial.println("Setting up client...");

  pinMode(BUTTON, INPUT_PULLUP);

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);  // less or equal setInterval value
  
  Serial.println("Scanning...");
  pBLEScan->start(0, false);
}

void loop() {
  // put your main code here, to run repeatedly:
 
  bool buttPressed;
  String toSend;

  if (doConnect == true) 
  {
    if (connected)
    {
      while (connected)
      {
        // check button (active low)
        buttPressed = digitalRead(BUTTON);

        if (!buttPressed) {
          value++;
      
          toSend = String(value);
          Serial.println("toSend is " + toSend);

          // send the message
          testCharacteristic->writeValue(toSend.c_str(), toSend.length());
        }

        delay(100);
      }
    }
    else
    {
      // try to connect
      if (connectToServer(*pServerAddress))
      {
        connected = true;
        Serial.println("Connected to server!");
      }
      else
      {
        // try again
        Serial.println("Connection failed!");
        connected = false;
        doConnect = false;

        Serial.println("Rescanning...");
        pBLEScan->start(0, false);
      }
    }
  }

  delay(1000);
}