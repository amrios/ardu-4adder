# ardu-4adder

A 4-bit adder/subtractor w/ Arduino Uno board from a Lafvin kit. Made as extra credit for a class.

Takes input via IR remote and performs calculation using binary addition and subtraction.

## IR remote and entering addends

The IR remote has a numerical 0-9 keypad. Since we are only dealing with a binary system, only the 0 and 1 buttons work. Only 4-bit terms are accepted. Once a group is completed, the display will briefly flicker with the final term, then reset back to all zeroes, indicating that you must enter the second addend.

Input accepted through the remote is often unreliable due to the cheap reciever included in the kit. If the display suddenly dims, the board is stuck in an infinite loop and the reset button must be pressed. You can avoid this by using the remote at a distance greater than 6 feet.

After you are done entering the terms in for A and B, you select UP for addition, and DOWN for subtraction. After this, the result is displayed.

## Displaying results

Since our segment display is only limited to four digits, the fifth bit gets displayed by alternating between the 4th and last digit. To obtain the actual result, first get the value with the trailing zeros (e.g. 0001). After it switches, append the number that comes up with the former (e.g. 0101 when it switched, so 00010101 is our final result).

## Logic

The program does use binary addition and subtraction. It does not convert number back to decimal to cheat its way through. 'A', 'B', and the solution gets stored in an array. Subtraction has some logic to remove the carry at the beginning, although it isn't perfect and has some edge cases.

## Schematic

![Schematic](schematic.png)

## Issues

* Subtraction might be incorrect since the solution to ignore the leftover carry was to set the last digit to 1. Needs a proper implementation.

## Video

TBA
