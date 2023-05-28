#include <esp_now.h>
#include <WiFi.h>

// lockAddress stores the MAC Address of the lock module.
uint8_t lockAddress[] = {0x8C, 0xAA,  0xB5, 0x8B, 0x7E, 0x94};

// Defina a datatype struct_message that will be used for communication between the two modules.
typedef struct struct_message {
    String Password;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// We populate the password field of the myData struct by "THIS IS A CHAR" (Lack of Imagination!!).
myData.Password = String("THIS IS A CHAR");

// Define a variable of the type esp_now_peer_info_t that will store the connection information for a session.
esp_now_peer_info_t peerInfo;

// This is a callback function. i.e., it is a function which will be executed every single time some data is recieved by the lock.
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}
 
void setup() {
  // Initialise Serial Monitor.
  Serial.begin(115200);
 
  // Set device as a Wi-Fi Station.
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW.
  // Check if ESP-NOW was initialised properly, if it wasn't, log it to the Serial monitor and reset.
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // We define all the parameters for the connection (reciever address, encryption status and communication channel)
  memcpy(peerInfo.peer_addr, lockAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // We add the device defined by the above parameters as a peer.
  // Check if the connection was established and if it wasn't we reset and start over.
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {  
  // Send a message to the connected peer that contains the password on a loop.
  esp_err_t result = esp_now_send(lockAddress, (uint8_t *) &myData, sizeof(myData));

  // We check if the message was sent successfully and log it to the Serial Monitor.
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }

  // We wait for 2 seconds before broadcasting again.
  delay(2000);
}
