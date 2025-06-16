// Arduino Elevator Simulation with NeoPixels
#include <Adafruit_NeoPixel.h> // Include the NeoPixel library

// Define the NeoPixel pin and number of pixels
#define NEOPIXEL_PIN 2     // Digital pin connected to the NeoPixel data input
#define NUMPIXELS 10       // Total number of NeoPixels in your strip (0-9 for floors)

// Create a NeoPixel object
// Arguments: number of pixels, pin number, pixel type (NEO_GRB + NEO_KHZ800 for most strips)
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// --- Global Variables ---
// Elevator state definitions
enum ElevatorState {
  ELEVATOR_IDLE,      // Elevator is waiting for input
  ELEVATOR_MOVING_UP, // Elevator is moving upwards
  ELEVATOR_MOVING_DOWN, // Elevator is moving downwards
  ELEVATOR_ARRIVED    // Elevator has reached its destination
};

ElevatorState elevatorState = ELEVATOR_IDLE; // Initial state: idle

int delayval = 400; // Delay for each step of elevator movement (milliseconds)
int currentFloor = 0; // The floor the elevator is currently on (starts at ground floor)
int desiredFloor = -1; // The floor the user wants to go to (-1 indicates no destination set)

// Door LED control
int doorLedPin = 3;   // Digital pin connected to the door indicator LED
bool doorsOpen = false; // true if doors are open, false if closed

// Variables for random NeoPixel colors
int redColor = 0;
int greenColor = 0;
int blueColor = 0;

// --- Function Prototypes ---
void setRandomColor();
void updateFloorLED(int floorIndex);
void controlDoors();
void printFloorStatus();

// --- Setup Function ---
void setup() {
  Serial.begin(9600); // Initialize serial communication for debugging and input
  while (!Serial); // Wait for serial port to connect (useful for some boards)

  pixels.begin(); // Initialize the NeoPixel library
  pixels.clear(); // Turn off all pixels initially
  pixels.show();  // Send the clear command to the strip

  pinMode(doorLedPin, OUTPUT); // Set the door LED pin as an output

  Serial.println("Arduino Elevator Ready!");
  Serial.println("Enter a floor number (0-9) in the Serial Monitor.");

  // Set the initial elevator position (ground floor)
  currentFloor = 0;
  updateFloorLED(currentFloor); // Light up the LED for the ground floor
  printFloorStatus(); // Print initial status
}

// --- Loop Function ---
void loop() {
  // --- Input Handling ---
  if (Serial.available() > 0 && elevatorState == ELEVATOR_IDLE) {
    // Read the input from the Serial Monitor
    String inputString = Serial.readStringUntil('\n');
    int inputFloor = inputString.toInt(); // Convert string to integer

    // Validate the input floor
    if (inputFloor >= 0 && inputFloor < NUMPIXELS) {
      if (inputFloor == currentFloor) {
        Serial.print("You are already on floor ");
        Serial.print(currentFloor);
        Serial.println(". Please enter a different floor.");
      } else {
        desiredFloor = inputFloor;
        if (desiredFloor > currentFloor) {
          elevatorState = ELEVATOR_MOVING_UP;
          Serial.print("Moving up to floor ");
          Serial.println(desiredFloor);
        } else {
          elevatorState = ELEVATOR_MOVING_DOWN;
          Serial.print("Moving down to floor ");
          Serial.println(desiredFloor);
        }
      }
    } else {
      Serial.println("Invalid floor. Please enter a number between 0 and 9.");
    }
  }

  // --- Elevator State Machine ---
  switch (elevatorState) {
    case ELEVATOR_IDLE:
      // Do nothing, just wait for input - The current floor LED is already lit
      break;

    case ELEVATOR_MOVING_UP:
      if (currentFloor < desiredFloor) {
        // Move up one floor
        currentFloor++;
        updateFloorLED(currentFloor); // Light up the new current floor's LED
        printFloorStatus();
        delay(delayval); // Simulate travel time
      } else {
        // Reached destination
        elevatorState = ELEVATOR_ARRIVED;
      }
      break;

    case ELEVATOR_MOVING_DOWN:
      if (currentFloor > desiredFloor) {
        // Move down one floor
        currentFloor--;
        updateFloorLED(currentFloor); // Light up the new current floor's LED
        printFloorStatus();
        delay(delayval); // Simulate travel time
      } else {
        // Reached destination
        elevatorState = ELEVATOR_ARRIVED;
      }
      break;

    case ELEVATOR_ARRIVED:
      Serial.print("Arrived at floor ");
      Serial.print(currentFloor);
      Serial.println(". Doors opening...");
      doorsOpen = true;
      controlDoors(); // Open doors LED
      delay(4000); // Doors open for 4 seconds

      Serial.println("Doors closing...");
      doorsOpen = false;
      controlDoors(); // Close doors LED
      delay(1000); // Small delay after closing doors

      desiredFloor = -1; // Reset desired floor
      elevatorState = ELEVATOR_IDLE; // Go back to idle state, waiting for next input
      Serial.println("Enter a floor number (0-9) in the Serial Monitor.");
      break;
  }
}

// --- Helper Functions ---

// Sets a random color for the NeoPixel.
void setRandomColor() {
  redColor = random(0, 256);   // 0-255 for RGB values
  greenColor = random(0, 256);
  blueColor = random(0, 256);
}

//Updates the NeoPixel display to show the current floor.
// Turns off all pixels then lights up the specified one.
void updateFloorLED(int floorIndex) {
  if (floorIndex >= 0 && floorIndex < NUMPIXELS) {
    pixels.clear(); // Turn off all pixels
    setRandomColor(); // Get a new random color for the active pixel
    // Set the specified pixel to the random color
    pixels.setPixelColor(floorIndex, pixels.Color(redColor, greenColor, blueColor));
    pixels.show(); // Send the updated pixel colors to the hardware
  }
}

// Controls the state of the door indicator LED.
void controlDoors() {
  if (doorsOpen) {
    digitalWrite(doorLedPin, HIGH); // Turn on LED for open doors
  } else {
    digitalWrite(doorLedPin, LOW);  // Turn off LED for closed doors
  }
}

// Prints the current floor status to the Serial Monitor.
void printFloorStatus() {
  Serial.print("Current Floor: ");
  // Special case for ground floor
  if (currentFloor == 0) {
    Serial.println("T (Ground)");
  } else {
    Serial.println(currentFloor);
  }
}
