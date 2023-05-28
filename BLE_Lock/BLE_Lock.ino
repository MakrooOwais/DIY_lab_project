#include <esp_now.h>
#include <WiFi.h>

// Define and initialize an int relay to 26. GPIO26 will be used to control the relay.
int relay = 26;

// Defina a datatype struct_message that will be used for communication between the two modules.
typedef struct struct_message {
    String Password;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// This is a callback function. i.e., it is a function which will be executed every single time some data is recieved by the lock.
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {

  // Copy the incoming data into myData.
  memcpy(&myData, incomingData, sizeof(myData));

  // Print logs to the Serial monitor.
  Serial.println("Key Recieved.");
  Serial.println(myData.Password);
  
  // Compare the recieved key with the onboard key and unlock if they match.
  if(myData.Password.equals(String("THIS IS A CHAR"))){
    
    // Unlock the soleniod lock.
    digitalWrite(relay, LOW);
    
    // Print logs to the Serial monitor.
    Serial.println("Lock Unlocked");
  }

  // Wait for 2 seconds and momentarily close the lock before we check if the key is still in the proximity.
  delay(2000);
  digitalWrite(relay, HIGH);
}
 
void setup() {
  // Initialise Serial Monitor.
  Serial.begin(115200);
  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
  
  // Set device as a Wi-Fi Station.
  WiFi.mode(WIFI_STA);

  // Initialise ESP-NOW.
  // Check if ESP-NOW was initialised properly, if it wasn't, log it to the Serial monitor and reset.
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully initialised, we define OnDataRecv as our callback function.
  // get recv packer info
  Serial.println("Waiting!!");
  esp_now_register_recv_cb(OnDataRecv);
  
}
 
void loop() {

}
