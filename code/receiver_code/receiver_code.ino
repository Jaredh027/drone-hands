#include <esp_now.h>
#include <WiFi.h>

// Struct to hold control data
typedef struct {
  uint16_t throttle;
  uint16_t pitch;
  uint16_t roll;
  uint16_t yaw;
} ControlData;

ControlData controlData;
    
const int throttlePin = 2; 
const int pitchPin = 4; 
const int rollPin = 5; 
const int yawPin = 18; 
const int flightModePin = 19;

const uint16_t mode1 = 4096;
const uint16_t mode2 = 4915;
const uint16_t mode3 = 5734;

uint16_t currFlightMode = mode1;

const int pwmFreq = 50;       // PWM frequency for servo (50 Hz = 20 ms period)
const int pwmResolution = 16; // PWM resolution (0-65535)

// Callback for ESP-NOW data reception
void onDataRecv(const esp_now_recv_info* recv_info, const uint8_t* data, int len) {
  // Receive control data from the sender

  // Copy received data into controlData struct
  memcpy(&controlData, data, sizeof(ControlData));

  // Uncomment to print the received values for testing
  // Serial.print("Throttle: ");
  // Serial.println(controlData.throttle);
  // Serial.print("Pitch: ");
  // Serial.println(controlData.pitch);
  // Serial.print("Roll: ");
  // Serial.println(controlData.roll);
  // Serial.print("Yaw: ");
  // Serial.println(controlData.yaw);
  // Serial.println();
}

void setup() {  

  Serial.begin(115200);   

  pinMode(throttlePin, OUTPUT);
  pinMode(pitchPin, OUTPUT);
  pinMode(rollPin, OUTPUT);
  pinMode(yawPin, OUTPUT);
  pinMode(flightModePin, OUTPUT);

  ledcAttach(throttlePin, pwmFreq, pwmResolution);
  ledcAttach(pitchPin, pwmFreq, pwmResolution);
  ledcAttach(rollPin, pwmFreq, pwmResolution);
  ledcAttach(yawPin, pwmFreq, pwmResolution);
  ledcAttach(flightModePin, pwmFreq, pwmResolution);

  // Initialize Wi-Fi in STA mode for ESP-NOW
  WiFi.begin();
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register callback for received data
  esp_now_register_recv_cb(onDataRecv);

  Serial.println("ESP32 Receiver Ready");
}

void loop() {
  ledcWrite(throttlePin, controlData.throttle);
  ledcWrite(pitchPin, controlData.pitch);
  ledcWrite(rollPin, controlData.roll);
  ledcWrite(yawPin, controlData.yaw);
  
  if(currFlightMode == mode1){
    ledcWrite(flightModePin, mode2);
    currFlightMode = mode2;
  }
  else if(currFlightMode == mode2){
    ledcWrite(flightModePin, mode3);
    currFlightMode = mode3;
  }
  else {
    ledcWrite(flightModePin, mode1);
    currFlightMode = mode1;
  }
  

}
