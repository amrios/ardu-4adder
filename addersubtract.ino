#include "IRremote.h"

int data=8;   //74HC595  pin 8 DS
int latch=9;  //74HC595  pin 9 STCP
int clock=10; //74HC595  pin 10 SHCP
int receiver = 11; // Signal Pin of IR receiver to Arduino Digital Pin 11
int dig1 = 2;
int dig2 = 5;
int dig3 = 4;
int dig4 = 3;

int digit[] = {2, 3, 4, 5}; //Mapping for digits on BCD
int a[4] = {0, 0 , 0 , 0}; // Group A
boolean aEntry = false; // For entry checking

int b[4] = {0}; // Group B
boolean bEntry = false; // For entry checking

int *c; // Stored as pointer to allow checking if it is null
int cSplit[4] = {0}; // Stores our overlapping digit.

unsigned long startMillis;


boolean solve = false; // For solve mode
int digit_place = 0;
int *displayer; // Our ptr for displayer gets updated through the program, which allows bitDisplay() to display the physical numbers.

/* IR related*/
IRrecv irrecv(receiver);
decode_results results; 

/* index = number */
unsigned char table[]=
{0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x77,0x7c
,0x39,0x5e,0x79,0x71,0x00};

/* Turns off all segment in display */
void disp_off() {
  digitalWrite(dig1, HIGH);
  digitalWrite(dig2, HIGH);
  digitalWrite(dig3, HIGH);
  digitalWrite(dig4, HIGH);
}


// Performs addition. complement() depends
int* addition(int aEntry[], int bEntry[]) {
  int *solution = malloc(4 * sizeof(int)); //add solution to heap
  int carry = 0;
  for (int i=3; i >= 0; i--) {
    solution[i] = (aEntry[i] + bEntry[i] + carry) % 2;
    carry = (aEntry[i] + bEntry[i] + carry) / 2;
  }
  cSplit[3] = carry;
  return solution;
}

// Complement w/ offset +1
int* complement(int entry[]) {
  int offset[4] = {0, 0, 0, 1};
  int *comple = malloc(4 * sizeof(int)); //add complement to heap
  for (int i=0; i < 4; i++) {
    comple[i] = (entry[i] + 1) % 2;
    Serial.println(comple[i]);
  }
  comple = addition(comple, offset);
  return comple;
}

void setup() {
// Display initialization
  pinMode(latch,OUTPUT);
  pinMode(clock,OUTPUT);
  pinMode(data,OUTPUT);
  pinMode(dig1, OUTPUT);
  pinMode(dig2, OUTPUT);
  pinMode(dig3, OUTPUT);
  pinMode(dig4, OUTPUT);

// IR initialization
  Serial.begin(9600);
  Serial.println("IR Receiver Button Decodee"); 
  irrecv.enableIRIn();

// disp_off() is probably unnecessary, setting up ptr for what array to display
  disp_off();
  aEntry = true;
  displayer = a; //set reference to A by default 

  // Remove this, test to see coomplement is implemented correctly.
  int *complem = complement(test);
  for (int i=0; i < 4; i++) {
    Serial.print(complem[i]);
  }
}


unsigned int translateIR() {

  switch(results.value)

  {
  case 0xFFA25D: Serial.println("POWER"); break;
  case 0xFFE21D: Serial.println("FUNC/STOP"); break;
  case 0xFF629D: return 2;  // ADDITION, UP
  case 0xFF22DD: Serial.println("FAST BACK");    break;
  case 0xFF02FD: Serial.println("PAUSE");    break;
  case 0xFFC23D: Serial.println("FAST FORWARD");   break;
  //case 0xFFE01F: return 20; //DOWN
  case 0xFFA857: return 3; // SUBTRACT, DOWN
  //case 0xFF906F: return 10; //UP
  case 0xFF9867: Serial.println("EQ");    break;
  case 0xFFB04F: Serial.println("ST/REPT");    break;
  case 0xFF6897: return 1; // THIS IS 1
  case 0xFF30CF: return 1;
  case 0xFF18E7: Serial.println("2");    break;
  case 0xFF7A85: Serial.println("3");    break;
  case 0xFF10EF: Serial.println("4");    break;
  case 0xFF38C7: Serial.println("5");    break;
  case 0xFF5AA5: Serial.println("6");    break;
  case 0xFF42BD: Serial.println("7");    break;
  case 0xFF4AB5: return 0;   break; //8
  case 0xFF52AD: Serial.println("9");    break;
  case 0xFFFFFFFF: return 1;  //REPEAT

  default: 
    Serial.println(" other button   ");

  }// End Case
  return 9;
  delay(500); // Do not get immediate repeat


} //END translateIR

unsigned char bcd_translate(int entry) {
  switch (entry)
  {
    case 0: return 0x3f;
    case 1: return 0x06;

    default: Serial.println("This shouldn't happen.");
  }
}
/*
 * Our displayer supports two implementations
 * 1. if - switch between 
 * Allows switching between two groups of numbers.
 * 2. else - no switch
 * Numbers sent do not switch; numbers remain static.
 */
void bitDisplay(unsigned int nums[]) {

  for (int i=0; i < 4; i++) {
      unsigned char bcd_input = bcd_translate(nums[i]);
      digitalWrite(latch,HIGH);
      shiftOut(data,clock,MSBFIRST, bcd_input);
      digitalWrite(latch,LOW);
      digitalWrite(digit[i],LOW);
      delay(2);
      disp_off();
    
  }
}

void loop() {

  bitDisplay(displayer);
  
  if (irrecv.decode(&results)) { // get input from remote
    delay(100);
    unsigned int input = translateIR();
    Serial.print("\nIR Remote recieved input: ");
    Serial.print(input);
    Serial.print("\nCurrent digit place: ");
    //Serial.print(digit_place);

    if (solve) {
      if (input == 2) { //ADDITION, UP
        c = addition(a, b);
        Serial.println("Performing addition...");
      }
      if (input == 3) { //SUBTRACTION, DOWN
        c = addition(a, complement(b));
        cSplit[3] = 0;
        Serial.println("Performing subtraction...");
      }
      displayer = c;
      //cSplit[3] = c[0];
      Serial.println("Final result should be displayed.");
      
    }

    if (bEntry) {  // if currently entering 'B'
      Serial.println("Appending input to B");
      b[digit_place] = input;
      digit_place++;
      if (digit_place == 4) {
        Serial.println("Now entering solve mode...");
        solve = true;
        bEntry = false;
        aEntry = false;
      }
    }
    
    if (aEntry) {  // if currently entering 'A'
      Serial.println("\nAppending input to A");
      a[digit_place] = input;
      digit_place++;
      if (digit_place == 4) { // reset digit place to 0 and inverse current bools
        for(int i=0; i<15;i++){
          bitDisplay(displayer);
          delay(50);
          disp_off();
          delay(50);
          }
        digit_place = 0;
        aEntry = !aEntry;
        bEntry = !bEntry;
        if (bEntry == true) {
        displayer = b;
          }
      Serial.print("Now resetting counter");
      }
    }

/*
    if (digit_place == 4) { // reset digit place to 0 and inverse current bools
      digit_place = 0;
      aEntry = !aEntry;
      bEntry = !bEntry;
      if (bEntry == true) {
        displayer = b;
      }
      Serial.print("Now resetting counter");
    }

    */
    for (int i = 0; i < 4; i++){
      if(aEntry){
      //Serial.print(a[i]);
      }
      else{
        //Serial.print(b[i]);
      }
    }
    irrecv.resume();
  }
  
  if ((solve == true) && (c != NULL) && (cSplit[3] != 0)) {
    if (startMillis == 0) {
      startMillis = millis();
    }
    if ((millis() - startMillis) < (unsigned long)2000) {
      displayer = cSplit;
    }
    else if ((millis() - startMillis) <  (unsigned long)4000) {
      displayer = c;
    }
    if ((millis() - startMillis) > (unsigned long)4000) {
      startMillis = 0;
    }
  }
}
 

  /*
  Display(2);
  delay(500);
  Display(3);
  delay(500);
  Display(4);
  delay(500);
  Display(5);
  delay(500);
  Display(6);
  delay(500);
  Display(7);
  delay(500);
  Display(8);
  delay(500);
  Display(9);
  delay(500);
  Display(10);
  delay(500);
  Display(11);
  delay(500);
  Display(12);
  delay(500);
  Display(13);
  delay(500);
  Display(14);
  delay(500);
  Display(15);
  delay(500);
  */
