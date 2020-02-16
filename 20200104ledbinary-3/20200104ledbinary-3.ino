#include "LedControl.h"

/*
  Now we need a LedControl to work with.
  pin 12 is connected to the DataIn
  pin 11 is connected to LOAD(CS)
  pin 10 is connected to the CLK
*/
LedControl lc = LedControl(12, 10, 11, 1);

/* we always wait a bit between updates of the display */
unsigned long delaytime1 = 1000;
unsigned long delaytime2 = 50;

void setup() {
  // put your setup code here, to run once:
  /*
    The MAX72XX is in power-saving mode on startup,
    we have to do a wakeup call
  */
  lc.shutdown(0, false);
  /* Set the brightness to a medium values */
  lc.setIntensity(0, 8);
  /* and clear the display */
  lc.clearDisplay(0);
  Serial.begin(9600); //debugging
}

int const matrixHeight = 5; //number of rows in of LED matrix
int const matrixWidth = 6; //max width is 8, best dimensions are 5*5 or 5*6
byte const widthMask = (1 << matrixWidth) - 1;

byte matrix[matrixHeight];

unsigned long const maxCount = pow(2, (matrixHeight * matrixWidth));

void numToBits(unsigned long value, byte arr[]) {
  //could use some error checking
  int row = 0;
  for (value; value > 0; value >>= matrixWidth) {
    arr[row] = byte(value) & widthMask;
    row++;//array row
  }
}

void resetMatrix(int size, byte arr[]) {
  for (int row = 0; row < size; row++) {
    arr[row] = 0;
  }
}

void fillMatrix(int size, byte arr[]) {
  for (int row = 0; row < size; row++) {
    arr[row] = widthMask;
  }
}

void err(int size, byte arr[]) {
  resetMatrix(size, arr);
  displayBits(size, arr);
  fillMatrix(size, arr);
  displayBits(size, arr);
}

void displayBits(int size, byte arr[]) {
  for (int row = 0; row < size; row++) {
    lc.setRow(0, row, arr[row]);
  }
  delay(delaytime1);
}

void countUp(unsigned long start) {
  for (unsigned long count = start; count <= maxCount; count++) {
    numToBits(count, matrix);
    displayBits(matrixHeight, matrix);
  }
}

void countDown(unsigned long start) {
  if (start > maxCount) {
    err(matrixHeight, matrix); //blink if number is too big
  } 
  else {
    for (unsigned long count = start; count > 0; count--) {
      numToBits(count, matrix);
      displayBits(matrixHeight, matrix);
    }
  }
}

void loop() {
  resetMatrix(matrixHeight, matrix);
  countDown(31556952);
}
