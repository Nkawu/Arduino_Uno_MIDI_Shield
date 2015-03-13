//**************************************************************
//  Name    : MIDIPedalBoard
//  Author  : Nkawu
//  Date    : 12 March, 2015
//  Version : 1.0
//  Notes   : Read switches connected to CD4021B Shift Register
//          : then send MIDI out via hiduino firmware
//****************************************************************

#include <MIDI.h>

// #define DEBUG

// General settings
#define NUM_INPUTS 32  // 32 switches for AGO Pedalboard C2-G4
#define BTN_DEBOUNCE 75  // Delay to prevent switch bouncing

// MIDI Settings
#define MIDI_CHANNEL 1
#define MIDI_NOTE_OFFSET 36  // MIDI note 36 = C2
#define MIDI_NOTE_ON_VELOCITY 127

// Arduino Pins
#define CLOCKPIN 9  // To CD4012 pin 10 (CLOCK)
#define LATCHPIN 8  // To CD4012 pin 9 (P/S C)
#define DATAPIN 7   // To CD4012 pin 3 (Q8)

#define NUM_REGISTERS (NUM_INPUTS / 8)

byte midiState[NUM_REGISTERS];    // Hold last register state HIGH=OFF
byte switchState[NUM_REGISTERS];  // Hold shift register state HIGH=OFF
unsigned long lastSwitch = 0;     // Holds last switch read time for debouncing switches

#if ! defined(DEBUG)
MIDI_CREATE_DEFAULT_INSTANCE();
#endif

void setup() {
	// Initialize last switch state array
	for (byte i = 0; i < NUM_REGISTERS; i++) {
		midiState[i] = 255;  // 11111111
	}

	#if defined(DEBUG)
	Serial.begin(38400);
	#else	
	// Initialize MIDI
	MIDI.begin(0);
	#endif

	// Define pin modes
	pinMode(CLOCKPIN, OUTPUT); 
	pinMode(LATCHPIN, OUTPUT);
	pinMode(DATAPIN, INPUT);

}


void loop() {

	readShiftRegisters();  // Read current switch positions
	processSwitches();  // Process switches & send MIDI

	#if defined(DEBUG)
	// Add delay to keep up with print statements
	delay(500);
	#endif
}


// Read the shift registers
void readShiftRegisters() {

	#if defined(DEBUG)
	// Serial.println("-------------------");
	// Serial.println("Reading registers");
	#endif

	// Pulse the latch pin: set it to 1 to collect parallel data, then wait
	digitalWrite(LATCHPIN, 1);
	delayMicroseconds(20);

	// Set latch pin to 0 to transmit data serially  
	digitalWrite(LATCHPIN, 0);

	// While the shift register is in serial mode collect each shift register 
	// into a byte. NOTE: the register attached to the chip comes in first.
	for (byte i = 0; i < NUM_REGISTERS; i++) {
		switchState[i] = shiftIn(DATAPIN, CLOCKPIN);
		#if defined(DEBUG)
		// Serial.print("Register #");
		// Serial.print(i);
		// Serial.print(": ");
		// Serial.print(switchState[i]);
		// Serial.print("-");
		// Serial.println(switchState[i], BIN);
		#endif
	}
}


void processSwitches() {

	if (micros() - lastSwitch > BTN_DEBOUNCE) {  // Ignore if we're in the debounce time
		lastSwitch = micros();

		// Process all registers
		for (byte i = 0; i < NUM_REGISTERS; i++) {
			if (switchState[i] != midiState[i]) {  // Don't process if no switches changed
				#if defined(DEBUG)
				// Serial.print("Register #");
				// Serial.print(i);
				// Serial.print(": ");
				// Serial.print(switchState[i]);
				// Serial.print("-");
				// Serial.println(midiState[i]);
				#endif
				for (byte j = 0; j < 8; j++ ) {
					// Read old & new switch states
					byte switchStateValue = bitRead(switchState[i], j);
					byte midiStateValue   = bitRead(midiState[i], j);
					
					#if defined(DEBUG)
					// Serial.print("  Byte #");
					// Serial.print(j);
					// Serial.print(": ");
					// Serial.print(switchStateValue);
					// Serial.print("-");
					// Serial.println(midiStateValue);
					#endif

					// Switch has changed, send MIDI command
					if (switchStateValue != midiStateValue) {
						byte note = (i * 8) + j + MIDI_NOTE_OFFSET;  // MIDI note for current switch
						if (switchStateValue == 1) {  // HIGH = switch open
							#if defined(DEBUG)
							Serial.print("Note OFF ");
							#else
							MIDI.sendNoteOff(note, 0, MIDI_CHANNEL);
							#endif
						} else {  // LOW = switch closed
							#if defined(DEBUG)
							Serial.print("Note ON ");
							#else
							MIDI.sendNoteOn(note, MIDI_NOTE_ON_VELOCITY, MIDI_CHANNEL);
							#endif
						}
						#if defined(DEBUG)
						Serial.print(i);
						Serial.print("-");
						Serial.print(j);
						Serial.print(": ");
						Serial.println(note);
						#endif
					}
				}
				midiState[i] = switchState[i];  // Save current switch state
			}
		}
	}
}


// Returns a byte with each bit in the byte corresponding
// to a pin on the shift register. leftBit 7 = Pin 7 / Bit 0= Pin 0
// From: http://arduino.cc/en/Tutorial/ShftIn21
byte shiftIn(int myDataPin, int myClockPin) { 
	int i;
	int temp = 0;
	int pinState;
	byte myDataIn = 0;

	pinMode(myClockPin, OUTPUT);
	pinMode(myDataPin, INPUT);

	// Hold the clock pin high 8 times (0,..,7) at the end of each time through
	// the for loop. At the beginning of each loop when the clock is set low, 
	// it will be doing the necessary low to high drop to cause the shift
	// register's DataPin to change state based on the value of the next bit in 
	// its serial information flow. The register transmits the information about 
	// the pins from pin 7 to pin 0 so that is why our function counts down.
	for (i=7; i>=0; i--) {
		digitalWrite(myClockPin, 0);  // Set clock low
		delayMicroseconds(2);
		temp = digitalRead(myDataPin);
		if (temp) {
			pinState = 1;
			// Set the bit to 0 no matter what
			myDataIn = myDataIn | (1 << i);
		} else {
			// Turn it off -- only necessary for debuging
			// print statement since myDataIn starts as 0
			pinState = 0;
		}

		digitalWrite(myClockPin, 1);
	}
	return myDataIn;
}
