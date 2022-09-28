
// SERVER
#include <RF24.h>
#include <Servo.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

Servo servoLeftArm;
Servo servoRightArm;
Servo servoLeftLeg;
Servo servoRightLeg;
Servo servoHead;

#define DELAY 1

#define PAYLOAD_SIZE 20

// instantiate an object for the nRF24L01 transceiver
RF24 radio(7, 8); // using pin 7 for the CE pin, and pin 8 for the CSN pin

// For this example, we'll be using 6 addresses; 1 for each TX node
// It is very helpful to think of an address as a path instead of as
// an identifying device destination
// Notice that the last byte is the only byte that changes in the last 5
// addresses. This is a limitation of the nRF24L01 transceiver for pipes 2-5
// because they use the same first 4 bytes from pipe 1.
uint64_t address = 0x7878787878LL;
int16_t received = 0;

/*************************************** SETUP *****************************************/
void setup()
{
  Serial.begin(115200);

  // Attach servos
  servoLeftArm.attach(2);
  servoRightArm.attach(3);
  servoLeftLeg.attach(4);
  servoRightLeg.attach(5);
  servoHead.attach(6);

  // initialize the transceiver on the SPI bus
  if (!radio.begin()) {
    Serial.println(F("radio hardware is not responding!!"));
    while(1) {}
  }
    // Set the PA Level low to try preventing power supply related problems
  // because these examples are likely run with nodes in close proximity of
  // each other.

  // radio.setPALevel(RF24_PA_LOW); // RF24_PA_MAX is default.

  // save on transmission time by setting the radio to only transmit the
  // number of bytes we need to transmit a float
  radio.setPayloadSize(PAYLOAD_SIZE);
  // Set the addresses for all pipes to TX nodes
  radio.openReadingPipe(0, address);
  // put radio in RX mode
  radio.startListening(); 

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 flop"));
    while (1) {} // hold in infinite loop
  }
  // setup OLED display
  display.clearDisplay();
  // display.setRotation(2);
  display.setTextSize(1);               
  display.setTextColor(SSD1306_WHITE); 

}

/*************************************** LOOP *****************************************/

void loop()
{
  
  char buffer[PAYLOAD_SIZE];     // Buffer for recieving radio data from TouchDesigner transmitter
  uint8_t pipe;
  if (radio.available(&pipe)) {             // is there a payload? get the pipe number that recieved it
    received++;
    uint8_t length = radio.getPayloadSize(); // get the size of the payload
    radio.read(&buffer, length);            // fetch payload from FIFO
    int a; int b; int c; int d; int e;
    int n = sscanf(buffer, "%d %d %d %d %d", &a, &b, &c, &d, &e);
    int aa; int bb; int cc; int dd; int ee;
    
    aa = map(a, 100, 900, 80, 180);
    bb = map(b, 100, 900, 80, 180);
    cc = map(c, 100, 900, 80, 180);
    dd = map(d, 100, 900, 80, 180);
    ee = map(e, 100, 900, 80, 180);

    servoLeftArm.write(aa);
    servoRightArm.write(bb);
    servoLeftLeg.write(cc); 
    servoRightLeg.write(dd);
    servoHead.write(ee);

    display.clearDisplay();
    display.setCursor(0,0);
    display.println(received);
    display.print(a);
    display.print(" ");
    display.print(b);
    display.print(" ");
    display.print(c);
    display.print(" ");
    display.print(d);
    display.print(" ");
    display.println(e);
    display.print(aa);
    display.print(" ");
    display.print(bb);
    display.print(" ");
    display.print(cc);
    display.print(" ");
    display.print(dd);
    display.print(" ");
    display.println(ee);

  }
  display.display();
  delay(DELAY);
}
