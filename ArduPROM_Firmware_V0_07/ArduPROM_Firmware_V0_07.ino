//########--- MEMO ---########
// The purpose of this sketch is to retreive data from an EPROM.
// I am trying to reverse engineer code from a ROM used by an 80C31 MPU,
//  so that I can flash a new EEPROM with updated or replaced code.
// Besides, it's always fun sniffing out the data from old EPROMS. 
// I've seen interesting stuff in there, and I happen to have quite a few of them sitting in a jar.
//#################################################################################
enum pins { 
  shiftResetPin = 13,  //Shift Register Master Reset (Resets on LOW)
  latchPin = 3,       //Pin connected to ST_CP of 74HC595                     
  clockPin = 2,      //Pin connected to SH_CP of 74HC595
  dataPin = 12,     //Pin connected to DS of 74HC595 
  Q0 = 4
};
int incomingByte = 0;
//#################################################################################
void setup() {
  DDRD &= B00000011;
  DDRB = B11100000;
  Serial.begin(230400);
  pinMode(shiftResetPin, OUTPUT);
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  digitalWrite(shiftResetPin, HIGH);
  for (int c=1; c<40; c++) { Serial.println(); }  // Clean up my serial terminal a little before start
}
//#################################################################################
void loop() {
  welcomeMessage();
 while(1) { 
  while ( Serial.available() > 0 )  
  incomingByte = Serial.read(); // read the incoming byte and convert to char:
  if (incomingByte == 114) { Serial.println("Okay, reading data... Here it comes!"); dumpPROM(); goto done; }
  if (incomingByte == 116) { return; }
 }
done:  
  Serial.println(); Serial.println("#################"); 
  Serial.println("(E)EPROM dump has completed. Would you like another? (y/n): "); 
  while (1) { delay(1000); } //is there a better way to stop the loop?
} 
//#################################################################################
void dumpPROM() {

   digitalWrite(shiftResetPin, LOW); delay(500);
   digitalWrite(shiftResetPin, HIGH); delay(1000); 
    
   for ( int x = 0; x < 256; x++) {                // Index counter for HIGH address bits A8-A15
     for (int y = 0; y < 256; y++) {               // Index counter for  LOW address bits A0-A7
       digitalWrite(latchPin, LOW);                // Ground latchPin; hold low for entire transmission
       shiftOut(dataPin, clockPin, MSBFIRST, x);   // MSBFIRST for this circuit
       shiftOut(dataPin, clockPin, MSBFIRST, y);
       digitalWrite(latchPin, HIGH);               //Return latchPin high; signals chip that it no longer needs to listen for info 
       //uint8_t b = readByte();
       Serial.write( readByte() );

    }
  }   

}
//#################################################################################
uint8_t readByte() {
  uint8_t data = 0;
  uint8_t mask = 0x1;
  for (int i = Q0; i < Q0+8; i++) {
    if (digitalRead(i) == HIGH) {
      data |= mask;
    }
    mask = mask << 1;
  }
  return data;
}
//#################################################################################
void displayBinary(const uint8_t &b) {
  for (uint8_t bit = 0x80; bit != 0; bit >>= 1)
    Serial.write(b & bit ? '1' : '0');
}
//#################################################################################
void welcomeMessage() {
   Serial.println("__________________________________________________");
   Serial.println("| ####   ###  #  #   # ####  ####   ###  ###.### |");
   Serial.println("|#    # #     #  #   # #   # #  #  #   # #  #  # |");
   Serial.println("|###### #  ####  #   # ####  ####  #   # #  #  # |");
   Serial.println("|#    # #  #  #  #   # #     #  #  #   # #     # |");
   Serial.println("|#    # #  ####   ###  #     #   #  ###  #     # |");
   Serial.println("|------------------------------------------------|");  
   Serial.println("| Welcome to the Ardunio Uno (E)EPROM Shield!    |");
   Serial.println("| Please select an option:                       |");
   Serial.println("|- (R)ead Device    |----------------------------|");
   Serial.println("  | (W)rite Device  |");
   Serial.println("  |-----------------|");
}
//#################################################################################
//Code Snippits
// Output should look something like this: "$05CF: 5F" Leading zeros had to be added.
// If theres a better way to get leading zeros, it'd be nice to know.
// Serial.print("$"); if (x<16) {Serial.print("0");} Serial.print(x, HEX); if (y<16) {Serial.print("0");} Serial.print(y, HEX); Serial.print(": "); Serial.println(b, HEX);

