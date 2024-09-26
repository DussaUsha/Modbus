#include <ModbusRTU.h>
#include <SoftwareSerial.h>

SoftwareSerial S(18, 17);  // RS485 communication

ModbusRTU mb;
uint16_t slave_response = 0;  // Data read from the slave
uint16_t user_input = 0;      // User input to send to the slave
#define RESPONSE_REG 10    // Register to hold the response sent to the master
#define INPUT_REG 11       // Register to hold the input from the master

bool cb(Modbus::ResultCode event, uint16_t transactionId, void* data) {
  if (event != Modbus::EX_SUCCESS) {
    Serial.print("Modbus error: 0x");
    Serial.println(event, HEX);
  }
  return true;
}

void setup() {
  Serial.begin(9600);         // Serial for debugging
  S.begin(9600, SWSERIAL_8N1);
  mb.begin(&S, 8);            // RE/DE connected to pin 8
  mb.master();                // Set as Modbus master

  Serial.println("Master starting...");
}

void loop() {
  // Read random data from the slave
  mb.readHreg(1, RESPONSE_REG, &slave_response, 1, cb);  // Read to registor of 10 of slave data  
  // Wait until the transaction is complete
  while (mb.slave()) {
    mb.task();
  }

  // Print the values received from the slave
  Serial.print("Master read random data from slave: ");
  Serial.println(slave_response);

  // Check for user input from Serial monitor
  if (Serial.available() > 0) {
    user_input = Serial.parseInt();  // Get user input
    Serial.print("Sending value to slave: ");
    Serial.println(user_input);

    // Write user input to the slave
    mb.writeHreg(1, INPUT_REG, user_input, cb);  // Write to register 11 of slave (slave ID 1)

    // Wait until the transaction is complete
    while (mb.slave()) {
      mb.task();
    }
  }

  delay(1000);  // Delay for readability
}
