// Arduino Calculator with Base Conversion
#include <Keypad.h>
#include <LiquidCrystal.h>

// LCD pin assignments: RS, Enable, D4, D5, D6, D7
// Using digital pins that are commonly available and not for serial communication.
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Variables for calculator logic
long firstNum = 0;      // Stores the first operand
long secondNum = 0;     // Stores the second operand
double resultValue = 0; // Stores the result of calculations
char currentOperator = ' '; // Stores the active operator (+, -, *, /)

// Flag to determine if we are entering the first or second number
bool enteringFirstNumber = true;

// Variable to control base conversion display mode
// 0: Decimal, 1: Binary, 2: Hexadecimal, 3: Octal
int conversionDisplayMode = 0;

// Character array for displaying converted results on LCD (max 16 chars + null terminator)
char displayBuffer[17];

// Keypad setup
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns

// Define the keypad layout
char keys[ROWS][COLS] = {
  {'1','2','3','+'},
  {'4','5','6','-'},
  {'7','8','9','*'},
  {'C','0','=','/'}
};

// Keypad row pins (connected to Arduino digital pins)
byte rowPins[ROWS] = {10, 9, 8, 7}; // Example: D10, D9, D8, D7

// Keypad column pins (connected to Arduino analog pins, used as digital)
// Avoids conflict with serial pins (0, 1)
byte colPins[COLS] = {A0, A1, A2, A3}; //(Digital pins 14, 15, 16, 17)

// Initialize the Keypad library
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- Base Conversion Arrays ---
// These arrays map integer values to their character representations in different bases.
char hexadecimalDigits[16] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
char octalDigits[8] = {'0','1','2','3','4','5','6','7'};
char binaryDigits[2] = {'0','1'};
char decimalDigits[10] = {'0','1','2','3','4','5','6','7','8','9'};

// --- Conversion Constants ---
// Maximum number of digits for various base representations for an 8-bit number (0-255)
// Binary: 8 bits (e.g., 11111111)
// Hexadecimal: 2 digits (e.g., FF)
// Octal: 3 digits (e.g., 377)
const int BIN_DIGITS = 8;
const int HEX_DIGITS = 2;
const int OCT_DIGITS = 3;

// Converts a number represented as a character array in a given base to its decimal integer value.
int convertToDecimal(char* numArray, int numDigits, char* baseDigits, int base) {
  int accumulator = 0;
  int power = 1; // Represents base^0, base^1, base^2, etc.

  // Iterate from the rightmost digit (least significant) to the leftmost
  for (int i = numDigits - 1; i >= 0; i--) {
    int digitValue = 0;
    // Find the integer value of the current character digit
    for (int s = 0; s < base; s++) {
      if (numArray[i] == baseDigits[s]) {
        digitValue = s;
        break; // Found the digit's value, exit inner loop
      }
    }
    accumulator += digitValue * power; // Add (digit value * base^position) to accumulator
    power *= base; // Increase power for the next digit (e.g., 1, 10, 100 for base 10)
  }
  return accumulator;
}

// Converts a decimal integer value to a string representation in a specified base.
void convertFromDecimal(int decimalValue, char* baseDigits, int numDigits, int base, char* resultBuffer) {
  int tempValue = decimalValue;
  // Iterate from the rightmost position (least significant digit) to the leftmost
  for (int i = numDigits - 1; i >= 0; i--) {
    resultBuffer[i] = baseDigits[tempValue % base]; // Get the remainder and map to character
    tempValue /= base; // Divide by base for the next digit
  }
  resultBuffer[numDigits] = '\0'; // Null-terminate the string
}

// Converts a decimal value to its binary string representation.
void decimalToBinary(int value, char* resultBuffer) {
  convertFromDecimal(value, binaryDigits, BIN_DIGITS, 2, resultBuffer);
}

// Converts a decimal value to its hexadecimal string representation.
void decimalToHexadecimal(int value, char* resultBuffer) {
  convertFromDecimal(value, hexadecimalDigits, HEX_DIGITS, 16, resultBuffer);
}

// Converts a decimal value to its octal string representation.
void decimalToOctal(int value, char* resultBuffer) {
  convertFromDecimal(value, octalDigits, OCT_DIGITS, 8, resultBuffer);
}

// Displays the current result in different bases based on the conversionDisplayMode.
void displayConvertedResult(int val) {
  lcd.clear();
  lcd.setCursor(0, 0);

  switch (conversionDisplayMode) {
    case 0: // Decimal
      lcd.print("Decimal:");
      lcd.setCursor(0, 1);
      lcd.print(val);
      break;
    case 1: // Binary
      lcd.print("Binary:");
      decimalToBinary(val, displayBuffer);
      lcd.setCursor(0, 1);
      lcd.print(displayBuffer);
      break;
    case 2: // Hexadecimal
      lcd.print("Hexadecimal:");
      decimalToHexadecimal(val, displayBuffer);
      lcd.setCursor(0, 1);
      lcd.print(displayBuffer);
      break;
    case 3: // Octal
      lcd.print("Octal:");
      decimalToOctal(val, displayBuffer);
      lcd.setCursor(0, 1);
      lcd.print(displayBuffer);
      break;
  }
}

void setup() {
  lcd.begin(16, 2); // Initialize the LCD with 16 columns and 2 rows
  lcd.print("Arduino Calculator"); // Display a welcome message
  delay(2000); // Wait for 2 seconds
  lcd.clear(); // Clear the LCD screen
} 

void loop() {
  // Get the key pressed from the keypad
  char customKey = customKeypad.getKey();

  // Only process if a key was pressed
  if (customKey != NO_KEY) {
    if (customKey >= '0' && customKey <= '9') {
      // If a number key is pressed
      if (enteringFirstNumber) {
        firstNum = firstNum * 10 + (customKey - '0'); // Build the first number
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(firstNum);
        conversionDisplayMode = 0; // Reset conversion mode when new number starts
      } else {
        secondNum = secondNum * 10 + (customKey - '0'); // Build the second number
        lcd.setCursor(0, 1);
        lcd.print(secondNum);
        // Clear previous operator display on first digit of second number
        if (secondNum == (customKey - '0')) { // Check if this is the very first digit
             lcd.setCursor(15, 0);
             lcd.print(currentOperator);
        }
      }
    } else if (customKey == '+' || customKey == '-' || customKey == '*' || customKey == '/') {
      // If an operator key is pressed
      currentOperator = customKey; // Store the operator
      enteringFirstNumber = false; // Switch to entering the second number
      secondNum = 0; // Reset second number for new input
      lcd.setCursor(15, 0); // Display operator on the first line, far right
      lcd.print(currentOperator);
    } else if (customKey == '=') {
      // If the equals key is pressed
      if (enteringFirstNumber && conversionDisplayMode != 0) {
        // If '=' is pressed after displaying a converted result, cycle conversion mode
        conversionDisplayMode = (conversionDisplayMode + 1) % 4; // Cycle 0->1->2->3->0
        displayConvertedResult((int)resultValue);
      } else if (!enteringFirstNumber && currentOperator != ' ') {
        // Perform calculation if second number has been entered
        lcd.clear();
        lcd.setCursor(0, 0); // Display result on the first line

        // Perform the arithmetic operation
        switch (currentOperator) {
          case '+':
            resultValue = firstNum + secondNum;
            break;
          case '-':
            resultValue = firstNum - secondNum;
            break;
          case '*':
            resultValue = firstNum * secondNum;
            break;
          case '/':
            // Handle division by zero
            if (secondNum != 0) {
              resultValue = (double)firstNum / secondNum;
            } else {
              lcd.print("Error: Div by 0");
              delay(1500); // Display error for a short period
              resultValue = 0; // Reset result
            }
            break;
        }
        lcd.print(resultValue); // Display the calculated result
        firstNum = resultValue; // Set result as the first number for chained operations
        secondNum = 0; // Reset second number
        enteringFirstNumber = true; // Go back to entering the first number (for next operation)
        currentOperator = ' '; // Clear the operator
        conversionDisplayMode = 0; // Reset to decimal display
      } else {
        // If '=' is pressed without a complete operation, treat as a conversion cycle
        conversionDisplayMode = (conversionDisplayMode + 1) % 4; // Cycle 0->1->2->3->0
        displayConvertedResult((int)resultValue);
      }
    } else if (customKey == 'C') {
      // If 'C' (Clear) key is pressed
      firstNum = 0;      // Reset first number
      secondNum = 0;     // Reset second number
      resultValue = 0;   // Reset total result
      currentOperator = ' '; // Clear operator
      enteringFirstNumber = true; // Go back to entering the first number
      conversionDisplayMode = 0; // Reset display mode to decimal
      lcd.clear();       // Clear LCD
      lcd.print("Cleared!"); // Show clear message
      delay(500);
      lcd.clear();
    }
  }
}
