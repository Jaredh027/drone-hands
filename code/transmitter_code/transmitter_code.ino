#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
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

// Initialize MPU6050 instances
Adafruit_MPU6050 mpu1;
Adafruit_MPU6050 mpu2; 

// MAC address of the drone ESP32 (receiver)
uint8_t droneMAC[] = {0xC8, 0x2E, 0x12, 0x21, 0xBC, 0x24};


void setup() {
  Serial.begin(115200);  // Debugging output
  while (!Serial) delay(10);

   // Initialize I²C communication with the first MPU6050 (GY-521)
  if (!mpu1.begin(0x68)) { // Address for GY-521
    Serial.println("Failed to find GY-521 MPU6050 chip at address 0x68!");
    while (1) delay(10);
  }
  Serial.println("GY-521 MPU6050 Found at 0x68!");

  // Initialize I²C communication with the second MPU6050 (HW-123)
  if (!mpu2.begin(0x69)) { // Address for HW-123
    Serial.println("Failed to find HW-123 MPU6050 chip at address 0x69!");
    while (1) delay(10);
  }
  Serial.println("HW-123 MPU6050 Found at 0x69!");

  // Initialize Wi-Fi in STA mode for ESP-NOW
  WiFi.begin();
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register peer (receiver ESP32)
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, droneMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  // Configure MPU6050 settings
  mpu1.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu1.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu1.setFilterBandwidth(MPU6050_BAND_21_HZ);

  mpu2.setAccelerometerRange(MPU6050_RANGE_8_G);
  mpu2.setGyroRange(MPU6050_RANGE_500_DEG);
  mpu2.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  // Read accelerometer and gyroscope data from both sensors
  sensors_event_t a1, g1, temp1, a2, g2, temp2;
  mpu1.getEvent(&a1, &g1, &temp1);
  mpu2.getEvent(&a2, &g2, &temp2); 


  // Sensor 1: Process for pitch and roll
  float pitch = a1.acceleration.x;  // Pitch (accelerometer X-axis)
  float roll = a1.acceleration.y;   // Roll (accelerometer Y-axis)

  // Sensor 2: Process for thrust (accelerometer X) and yaw (gyro Z)
  float thrust = a2.acceleration.x;  // Thrust (accelerometer X-axis)
  float yaw = a2.acceleration.y;     // Yaw (accelerometer Y-axis)


  // --- Pitch Transformations ---
  if (pitch >= -1.0 && pitch <= 2.0) {
    pitch = 0; // Buffer zone for pitch
  } else if (pitch >= 2.0 && pitch <= 8.0) {
    pitch -= 2;
  } else if (pitch > 8.0) {
    pitch = 6;
  } else if (pitch >= -7.0 && pitch <= -1.0) {
    pitch += 1;
  } else if (pitch < -7.0) {
    pitch = -6;
  }

  // --- Roll Transformations ---
  if (roll >= 1.0 && roll <= 5.0) {
    roll -= 1;
  } else if (roll > 5.0) {
    roll = 4;
  } else if (roll >= -5.0 && roll <= -1.0) {
    roll += 1;
  } else if (roll < -5.0) {
    roll = -4;
  }

   // --- Yaw Transformations ---
  if (yaw >= 1.0 && yaw <= 5.0) {
    yaw -= 1;  
  } else if (yaw > 5.0) {
    yaw = 4;  
  } else if (yaw >= -5.0 && yaw <= -1.0) {
    yaw += 1;  
  } else if (yaw < -5.0) {
    yaw = -4;  
  }


  // --- Thrust Transformations ---
  if (thrust >= -1.0 && thrust <= 2.0) {
    thrust = 0; // Buffer zone, set thrust to 0
  } else if (thrust >= 2.0 && thrust <= 8.0) {
    thrust -= 2; 
  } else if (thrust > 8.0) {
    thrust = 6; 
  } else if (thrust >= -7.0 && thrust <= -1.0) {
    thrust += 1; 
  } else if (thrust < -7.0) {
    thrust = -6; 
  }

  thrust = map(thrust, -6,6,3277,6553);
  pitch = map(pitch, -6,6, 3277,6553);
  roll = map(roll, -5,5,3277,6553);
  yaw = map(yaw, -5,5,3277,6553);

  // Output transformed values for pitch, thrust, roll, and yaw separately
  // Serial.print("Pitch: ");
  // Serial.print(pitch);  // Display pitch 
  // Serial.print(" | Thrust: ");
  // Serial.print(thrust); // Display thrust  
  // Serial.print(" | Roll: ");
  // Serial.print(roll);   // Display roll 
  // Serial.print(" | Yaw: ");
  // Serial.println(yaw);  // Display yaw 

  controlData.throttle = thrust;
  controlData.pitch = pitch;
  controlData.roll = roll;
  controlData.yaw = yaw;

  // Send data to the drone ESP32
  esp_now_send(droneMAC, (uint8_t *)&controlData, sizeof(controlData));
}
