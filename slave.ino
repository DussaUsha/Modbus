#include <ModbusRTU.h>
#include <SoftwareSerial.h>

SoftwareSerial S(18, 17);  // RS485 communication

#define RESPONSE_REG 10  // Register to hold the response sent to the master
#define INPUT_REG 11     // Register to hold the input from the master
// #define RESPONSE_REG1 11
#define SLAVE_ID 1  // Slave ID

ModbusRTU mb;
uint16_t random_value = 0;  // Value to hold the random data
float random1 = 1.50;
uint16_t master_input = 0;  // Value to hold the input received from the master

bool cb(Modbus::ResultCode event, uint16_t transactionId, void* data) {
  if (event != Modbus::EX_SUCCESS) {
    Serial.print("Modbus error: 0x");
    Serial.println(event, HEX);
  }
  return true;
}

void setup() {
  Serial.begin(9600);  // Serial for debugging
  S.begin(9600, SWSERIAL_8N1);
  mb.begin(&S, 8);           // RE/DE connected to pin 8
  mb.slave(SLAVE_ID);        // Set as Modbus slave
  mb.addHreg(RESPONSE_REG);  // Add holding register for response to master
  mb.addHreg(INPUT_REG);     // Add holding register for input from master
}

void loop() {
  // Process Modbus requests
  mb.task();

  // Generate and send random data to master
  random_value = random(0,100);  // Generate a random number between 0 and 99
  // random1 = random(0, 99.9);
  mb.Hreg(RESPONSE_REG, random_value);  // Update holding register with random value on 10th address cell
  // mb.Hreg(RESPONSE_REG1, random1);
  // Check if there's an input from the master
  master_input = mb.Hreg(INPUT_REG);  // Read input from master from 11th address cell
  Serial.print("Received input from master: ");
  Serial.println(master_input);

  Serial.print("Sent random data to master: ");
  Serial.println(random_value);
  delay(1000);  // Delay to control how often data is sent
}
