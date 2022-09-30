
#include <esp_now.h>
#include <Wifi.h>



// SETUP VARIABLES
// ------------------------------
// Reciever MAC Address
uint8_t receiverAddress[] = {0xEC, 0x62, 0x60, 0x84, 0x4B, 0x10};
// Button PIN
#define BUTTON_PIN 21
// POLLING RATE
#define POLLRATE 1
// BUTTON STATES
int currentState;
int lastState = LOW;
// ------------------------------
// Message Format
typedef struct struct_message {
    bool state;
} struct_message;

struct_message myData;

esp_now_peer_info_t peerInfo;
// Callback When Data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("\r\nLast Packet Send Status:\t");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup(){
    // Start serial monitor
    Serial.begin(115200);

    // Setup Button PIN
    pinMode(BUTTON_PIN, INPUT_PULLUP);

    // Set wifi station
    WiFi.mode(WIFI_STA);

    // Init ESP NOW

    if (esp_now_init() != ESP_OK) {
        Serial.println("Error ESP NOW Init");
        return;
    }
    
    esp_now_register_send_cb(OnDataSent);

    // Register Peer
    memcpy(peerInfo.peer_addr, receiverAddress, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;

    // add Peer
    if(esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }
}

void loop() {
    // Set current state by reading digital pin
    lastState = currentState;
    currentState = digitalRead(BUTTON_PIN);

    // Sending Packets
    if (currentState == LOW && lastState == HIGH) {
        // print to serial (debug)
        Serial.println("OFF");
        // set data packet state to off
        myData.state = false;
        // send via ESP NOW
        esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));
        // print success code to serial
        if (result == ESP_OK) {
            Serial.println("Sent Success");
        } else {
            Serial.println("Error sending");
        }
        // delay to next poll
        delay(POLLRATE);
        } else if (currentState == HIGH && lastState == LOW) {
        // print to serial (debug)
        Serial.println("ON");
        // set data packet state to off
        myData.state = true;
        // send via ESP NOW
        esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));
        // print success code to serial
        if (result == ESP_OK) {
            Serial.println("Sent Success");
        } else {
            Serial.println("Error sending");
        }
    }
}