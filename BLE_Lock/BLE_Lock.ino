#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

#define relay 2

String knownBLEAddresses[] = {"cd:47:45:86:a8:cc", "3c:71:bf:4c:9c:d2"};
int RSSI_THRESHOLD = -100;
bool device_found;
int scanTime = 1; //In seconds
BLEScan* pBLEScan;
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      for (int i = 0; i < (sizeof(knownBLEAddresses) / sizeof(knownBLEAddresses[0])); i++)
      {
        //Uncomment to Enable Debug Information
        //Serial.println("*************Start**************");
        //Serial.println(sizeof(knownBLEAddresses));
        //Serial.println(sizeof(knownBLEAddresses[0]));
        //Serial.println(sizeof(knownBLEAddresses)/sizeof(knownBLEAddresses[0]));
        //Serial.println(advertisedDevice.getAddress().toString().c_str());
        //Serial.println(knownBLEAddresses[i].c_str());
        //Serial.println("*************End**************");
        if (strcmp(advertisedDevice.getAddress().toString().c_str(), knownBLEAddresses[i].c_str()) == 0)
        {
          device_found = true;
          break;
        }
        else
          device_found = false;
      }
      Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());
    }
};
void setup() {
  Serial.begin(115200); //Enable UART on ESP32
  Serial.println("Scanning..."); // Print Scanning
  pinMode(relay, OUTPUT); //make BUILTIN_LED pin as output
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); //create new scan
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks()); //Init Callback Function
  pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
  pBLEScan->setInterval(100); // set Scan interval
  pBLEScan->setWindow(99);  // less or equal setInterval value
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Set PMK key
  esp_now_set_pmk((uint8_t *)PMK_KEY_STR);
}
void loop() 
{
  // put your main code here, to run repeatedly:
  BLEScanResults foundDevices = pBLEScan->start(scanTime, false);
  
  for (int i = 0; i < foundDevices.getCount(); i++)
  {
    BLEAdvertisedDevice device = foundDevices.getDevice(i);
    int rssi = device.getRSSI();
    Serial.print("RSSI: ");
    Serial.println(rssi);
    if (rssi > RSSI_THRESHOLD && device_found == true)
    {

      // Try and connect using ESPNow and get the key.
      // Register the receiver board as peer
    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;
    //Set the receiver device LMK key
    for (uint8_t i = 0; i < 16; i++) {
      peerInfo.lmk[i] = LMK_KEY_STR[i];
    }
    // Set encryption to true
    peerInfo.encrypt = true;
    
    // Add receiver as peer        
    if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
      return;
    }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of transmitted packet
  esp_now_register_send_cb(OnDataSent);
      Serial.println("Door Open");
      digitalWrite(relay, HIGH);
      delay(5000);
      Serial.println("Door Close");
      digitalWrite(relay, LOW);
    }

  }
  pBLEScan->clearResults();   // delete results fromBLEScan buffer to release memory
}
