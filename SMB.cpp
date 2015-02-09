#include <Arduino.h>
extern "C" {
#include <i2cmaster.h>
}
// SMBus Hacker v1.0 by FalconFour
//  with huge credit to liudr (http://liudr.wordpress.com/) for the awesome work on phi_prompt
//
// insert standard block of legalese and disclaimers here; by relation of the phi_prompt license,
// this code is only licensed for personal and educational use. don't blow yourself up.
// don't sell it. don't blame me if you blow yourself up. and don't sue me if you need more
// money, because i don't have any! =P
//
// The battery SMBus connection always connects to analog pins 5 and 6. To say that SCL is analog
//  5 and SDA is analog 4 probably wouldn't do much good since they don't usually mark that stuff
//  on batteries. First, start with a multimeter and a length of wire, checking the outer pins
//  first. Those are usually + and -. You need "-", unplug + when you find it. Poke around with
//  the "+" wire you just removed, and ground still plugged in. You should NOT see over 5 volts on
//  any pins. Avoid a pin if you see more than 5 volts. Use the SMBus Test function to test your
//  wiring. It won't respond to anything but its own address, so you might want to use a SMBus
//  Scan when you're not having luck but have secure wires.
//
//                          --- THIS IS WHERE I GET YOUR ATTENTION! ---
//    These are the parts you need to modify to your own setup. Add buttons as needed (minimum of
//    4 to navigate without resetting every mis-step), change the LCD pins, mode, etc.
//

byte deviceAddress = B0001011; // most battery controllers seem to use this

// -- Okay, you can hit Upload now.
//
#define bufferLen 32
char i2cBuffer[bufferLen];
uint8_t serialCommand;
unsigned int serialData;
int global_style=125; // This is the style of the phi_prompt menus

#define BATT_MAH 0
#define BATT_MA 1
#define BATT_MV 2
#define BATT_MINUTES 3
#define BATT_PERCENT 4
#define BATT_TENTH_K 5
#define BATT_BITFIELD 6
#define BATT_DEC 7
#define BATT_HEX 8
#define BATT_STRING 16

#define Cmd_ManufacturerAccess     0x00
#define Cmd_RemainingCapacityAlarm 0x01
#define Cmd_RemainingTimeAlarm     0x02
#define Cmd_BatteryMode            0x03
#define Cmd_AtRate                 0x04
#define Cmd_AtRateTimeToFull       0x05
#define Cmd_AtRateTimeToEmpty      0x06
#define Cmd_AtRateOK               0x07
#define Cmd_Temperature            0x08
#define Cmd_Voltage                0x09
#define Cmd_Current                0x0A
#define Cmd_AverageCurrent         0x0B
#define Cmd_MaxError               0x0C
#define Cmd_RelativeStateOfCharge  0x0D
#define Cmd_AbsoluteStateOfCharge  0x0E
#define Cmd_RemainingCapacity      0x0F
#define Cmd_FullChargeCapacity     0x10
#define Cmd_RunTimeToEmpty         0x11
#define Cmd_AverageTimeToEmpty     0x12
#define Cmd_AverageTimeToFull      0x13
#define Cmd_ChargingCurrent        0x14
#define Cmd_ChargingVoltage        0x15
#define Cmd_BatteryStatus          0x16
#define Cmd_CycleCount             0x17
#define Cmd_DesignCapacity         0x18
#define Cmd_DesignVoltage          0x19
#define Cmd_SpecificationInfo      0x1A
#define Cmd_ManufactureDate        0x1B
#define Cmd_SerialNumber           0x1C
#define Cmd_ManufacturerName       0x1D
#define Cmd_DeviceName             0x1E
#define Cmd_DeviceChemistry        0x1F
#define Cmd_ManufacturerData       0x20
#define Cmd_Flags                  0x21
#define Cmd_EDV1                   0x22
#define Cmd_EDVF                   0x23

// table of ALL global command labels used in the command sets; if you want to add a new command, tack it onto the bottom.
prog_char cmdLabel_ManufacturerAccess[] PROGMEM = "ManufacturerAccess";
prog_char cmdLabel_RemainingCapacityAlarm[] PROGMEM = "RemainingCapacityAlarm";
prog_char cmdLabel_RemainingTimeAlarm[] PROGMEM = "RemainingTimeAlarm";
prog_char cmdLabel_BatteryMode[] PROGMEM = "BatteryMode";
prog_char cmdLabel_AtRate[] PROGMEM = "AtRate";
prog_char cmdLabel_AtRateTimeToFull[] PROGMEM = "AtRateTimeToFull";
prog_char cmdLabel_AtRateTimeToEmpty[] PROGMEM = "AtRateTimeToEmpty";
prog_char cmdLabel_AtRateOK[] PROGMEM = "AtRateOK";
prog_char cmdLabel_Temperature[] PROGMEM = "Temperature";
prog_char cmdLabel_Voltage[] PROGMEM = "Voltage";
prog_char cmdLabel_Current[] PROGMEM = "Current";
prog_char cmdLabel_AverageCurrent[] PROGMEM = "AverageCurrent";
prog_char cmdLabel_MaxError[] PROGMEM = "MaxError";
prog_char cmdLabel_RelativeStateOfCharge[] PROGMEM = "RelativeStateOfCharge";
prog_char cmdLabel_AbsoluteStateOfCharge[] PROGMEM = "AbsoluteStateOfCharge";
prog_char cmdLabel_RemainingCapacity[] PROGMEM = "RemainingCapacity";
prog_char cmdLabel_FullChargeCapacity[] PROGMEM = "FullChargeCapacity";
prog_char cmdLabel_RunTimeToEmpty[] PROGMEM = "RunTimeToEmpty";
prog_char cmdLabel_AverageTimeToEmpty[] PROGMEM = "AverageTimeToEmpty";
prog_char cmdLabel_AverageTimeToFull[] PROGMEM = "AverageTimeToFull";
prog_char cmdLabel_ChargingCurrent[] PROGMEM = "ChargingCurrent";
prog_char cmdLabel_ChargingVoltage[] PROGMEM = "ChargingVoltage";
prog_char cmdLabel_BatteryStatus[] PROGMEM = "BatteryStatus";
prog_char cmdLabel_CycleCount[] PROGMEM = "CycleCount";
prog_char cmdLabel_DesignCapacity[] PROGMEM = "DesignCapacity";
prog_char cmdLabel_DesignVoltage[] PROGMEM = "DesignVoltage";
prog_char cmdLabel_SpecificationInfo[] PROGMEM = "SpecificationInfo";
prog_char cmdLabel_ManufactureDate[] PROGMEM = "ManufactureDate";
prog_char cmdLabel_SerialNumber[] PROGMEM = "SerialNumber";
prog_char cmdLabel_ManufacturerName[] PROGMEM = "ManufacturerName";
prog_char cmdLabel_DeviceName[] PROGMEM = "DeviceName";
prog_char cmdLabel_DeviceChemistry[] PROGMEM = "DeviceChemistry";
prog_char cmdLabel_ManufacturerData[] PROGMEM = "ManufacturerData";
prog_char cmdLabel_Flags[] PROGMEM = "Flags";
prog_char cmdLabel_EDV3[] PROGMEM = "EDV3";
prog_char cmdLabel_EDV2[] PROGMEM = "EDV2";
prog_char cmdLabel_EDV1[] PROGMEM = "EDV1";
prog_char cmdLabel_EDVF[] PROGMEM = "EDVF";

// this is set up in the same fashion as phi_prompt menus, because it's also used as one in the selector. String the names of your commands together IN ORDER with the commands definition below this.
const char *bq2040Labels[] PROGMEM =        { cmdLabel_ManufacturerAccess, cmdLabel_RemainingCapacityAlarm, cmdLabel_RemainingTimeAlarm, cmdLabel_BatteryMode, cmdLabel_AtRate, cmdLabel_AtRateTimeToFull, cmdLabel_AtRateTimeToEmpty, cmdLabel_AtRateOK, cmdLabel_Temperature, cmdLabel_Voltage, cmdLabel_Current, cmdLabel_AverageCurrent, cmdLabel_MaxError, cmdLabel_RelativeStateOfCharge, cmdLabel_AbsoluteStateOfCharge, cmdLabel_RemainingCapacity, cmdLabel_FullChargeCapacity, cmdLabel_RunTimeToEmpty, cmdLabel_AverageTimeToEmpty, cmdLabel_AverageTimeToFull, cmdLabel_ChargingCurrent, cmdLabel_ChargingVoltage, cmdLabel_BatteryStatus, cmdLabel_CycleCount, cmdLabel_DesignCapacity, cmdLabel_DesignVoltage, cmdLabel_SpecificationInfo, cmdLabel_ManufactureDate, cmdLabel_SerialNumber, cmdLabel_ManufacturerName, cmdLabel_DeviceName, cmdLabel_DeviceChemistry, cmdLabel_ManufacturerData, cmdLabel_Flags, cmdLabel_EDV3, cmdLabel_EDV2, cmdLabel_EDV1, cmdLabel_EDVF };
// here, a two-dimension array. first byte is the command code itself (hex). second byte is the type of result it's expected to return (all except the strings come through as words.
const uint8_t bq2040Commands[][2] PROGMEM = {      { 0x00, BATT_HEX },          { 0x01, BATT_MAH },            { 0x02, BATT_MINUTES }, { 0x03, BATT_BITFIELD },{ 0x04, BATT_MA }, { 0x05, BATT_MINUTES },    { 0x06, BATT_MINUTES },   {0x07, BATT_HEX},  {0x08, BATT_TENTH_K}, {0x09, BATT_MV}, { 0x0A, BATT_MA },   { 0x0B, BATT_MA },   {0x0C, BATT_PERCENT},    { 0x0D, BATT_PERCENT },         { 0x0E, BATT_PERCENT },         { 0x0F, BATT_MAH },         { 0x10, BATT_MAH },      { 0x11, BATT_MINUTES },     { 0x12, BATT_MINUTES },     { 0x13, BATT_MINUTES },      { 0x14, BATT_MA },        { 0x15, BATT_MV },    { 0x16, BATT_BITFIELD }, { 0x17, BATT_DEC },    { 0x18, BATT_MAH },      { 0x19, BATT_MV },     { 0x1A, BATT_BITFIELD },   { 0x1B, BATT_BITFIELD },   { 0x1C, BATT_DEC },    { 0x20, BATT_STRING },    {0x21, BATT_STRING},  { 0x22, BATT_STRING },     { 0x23, BATT_STRING },{0x2F, BATT_BITFIELD},{0x3C, BATT_MV},{0x3D, BATT_MV},{0x3E, BATT_MV},{0x3F, BATT_MV} };

// this is what selects the command set itself. there's only one listing here, so just add a new entry (char cmdset_item01[] PROGMEM), then add that entry to the cmdset_items() list below.
char cmdset_item00[] PROGMEM = "bq2040";
const char *cmdset_items[] PROGMEM = {cmdset_item00};

// update each of these commands with your custom command set, just add a new "case" correlating to the cmdset_items[] position above.
uint8_t cmd_getCode(uint8_t command) {
      // bq2040 commands
      return pgm_read_byte(&bq2040Commands[command][0]); // command is first parameter
}
uint8_t cmd_getType(uint8_t command) {
      // bq2040 commands
      return pgm_read_byte(&bq2040Commands[command][1]); // type is second parameter
}
void cmd_getLabel(uint8_t command, char* destBuffer) {
      // bq2040 commands
      strlcpy_P(destBuffer,(char*)pgm_read_word(&bq2040Labels[command]),bufferLen-1);
}
char** cmd_getPtr() {
      return (char**)&bq2040Labels;
}
uint8_t cmd_getLength() {
      return (sizeof(bq2040Labels) / sizeof(&bq2040Labels));
}

void setup() {
  byte x,y;
  Serial.begin(57600);

  i2c_init();
  //PORTC = (1 << PORTC4) | (1 << PORTC5); //enable pullups
  Serial.print("BattMon 1.0 ");
  //i2c_stop();
}

void top_menu();
void DisplaySetupMenu();
void DisplayReadMenu();
void DisplayControlMenu(); // Display submenu, will return here upon exiting menu
void DetectSMBus();
void ScanI2C();
void EnterI2C();
void DisplayIDData();
void DisplayChargeData();
void DisplayStatistics();
void DisplaySingleCommand();
void DisplayAllInfo();
//void DisplayControlWriteWord();
//void DisplayControlReadWord();
//void DisplayControlReadBlock();
uint8_t i2c_detect_device ( uint8_t addr );
void i2c_smbus_write_word ( uint8_t command, unsigned int data );
unsigned int i2c_smbus_read_word ( uint8_t command );
uint8_t i2c_smbus_read_block ( uint8_t command, char* blockBuffer, uint8_t blockBufferLen );
void lcdPadBinary (uint8_t value, uint8_t bits);
void fmtDouble(double val, byte precision, char *buf, unsigned bufLen);
int wait_on_escape(int time) {
	return 0;
}

void loop() {
  // dummy loop() statement since we're sorta abstracting C into an object-oriented layout here... just enters into a menu loop and stays there, this never loops.
  top_menu();
  // ... I could probably do something really nasty here like erase the flash, and it'd never know.
}

void top_menu() {
  Serial.println("Scan      - 1");
  Serial.println("Read Info - 2");
  Serial.println("Reset     - 3");
  Serial.println("calibrate      - 4");
  Serial.println("stop calibrate     - 5");
  Serial.println("write remaining     - 6");
  Serial.println("charge cync     - 7");
  while(!Serial.available()) { delay(10); }
  switch ((int)Serial.read()-'0') {
      case 0:
      DisplaySetupMenu();
      break;
      case 1:
      ScanI2C();
      break;
      case 2:
      DisplayAllInfo();
      break;
      case 3:
/*	  i2c_start((deviceAddress<<1) + I2C_WRITE);
	  i2c_write(0x4f);
	  i2c_write(0x5a);
	  i2c_write(0xff);
	  i2c_write(0x7d);
	  i2c_write(0x00);
	  i2c_write(0x00);
	  i2c_write(0x7d);
	  i2c_write(0x80);
	  i2c_write(0x00);
	  i2c_stop();
*/	i2c_smbus_write_word ( 0x4f, 0xff5a );
      //	delay(50);
	i2c_smbus_write_word ( 0x7d, 0x0000 );
      	//delay(50);
	i2c_smbus_write_word ( 0x7d, 0x0080 );
      	//delay(50);
      break;
      case 4:
	i2c_smbus_write_word ( 0x00, 0x0653 );
      break;
      case 5:
	i2c_smbus_write_word ( 0x00, 0x0660 );
	      break;
      case 6:
	i2c_smbus_write_word ( 0x26, 0x1130 - 0x05FF);
	      break;
      case 7:
	i2c_smbus_write_word ( 0x00, 0x064d );
	      break;
      default:
      break;
    }
  while(Serial.available()) { Serial.read();}
 
}

void DisplaySetupMenu() {
  Serial.println("Test SMBus");
  Serial.println("Scan SMBus");
  Serial.println("Enter Address");
  Serial.println("Main Menu");
  while(1) {
    while(!Serial.available()) { delay(10); }
    switch ((int)Serial.read()-'0') {
      case 0:
      DetectSMBus();
      break;
      case 1:
      ScanI2C();
      break;
      case 2:
      EnterI2C();
      break;
      case 3:
      return;
      break;
      default:
      break;
    }
    delay(500);
  }
}

void DisplayReadMenu() {
  Serial.println("Battery ID");
  Serial.println("Charge data");
  Serial.println("Statistics");
  Serial.println("Main Menu");
  while(1) {
    while(!Serial.available()) { delay(10); }
    switch ((int)Serial.read()-'0') {
      case 0:
      DisplayIDData();
      break;
      case 1:
      DisplayChargeData();
      break;
      case 2:
      DisplayStatistics();
      break;
      case 3:
      return;
      break;
      default:
      break;
    }
    delay(500);
  }
}

void DisplayControlMenu() {
  Serial.println("Single Command");
  Serial.println("Write word");
  Serial.println("Read word");
  Serial.println("Read block");
  Serial.println("Main menu");
  while(1) {
    while(!Serial.available()) { delay(10); }
    switch ((int)Serial.read()-'0') {
      case 0:
      DisplaySingleCommand();
      break;
      case 1:
//      DisplayControlWriteWord();
      break;
      case 2:
//      DisplayControlReadWord();
      break;
      case 3:
//      DisplayControlReadBlock();
      break;
      case 4:
      return; // back to main menu
      break;
      default:
      break;
    }
    delay(500);
  }
}

void DisplayAllInfo() {
  int wordBuffer;
  double valueBuffer;
  int command;
  for(command=0;command< cmd_getLength();command++) {
    Serial.print("<< ");
    Serial.print(cmd_getCode(command));
    Serial.print(" ");
    memset(i2cBuffer,0,bufferLen);
    if (cmd_getType(command) < BATT_STRING) {
      wordBuffer = i2c_smbus_read_word(cmd_getCode(command));
      Serial.print(wordBuffer,HEX);
      Serial.print(" ");
    } else if (cmd_getType(command) == BATT_STRING) {
      wordBuffer=i2c_smbus_read_block(cmd_getCode(command),i2cBuffer,bufferLen);
      Serial.print(wordBuffer,DEC);
      Serial.print(" ");
      int x;
      for(x=0;x<wordBuffer;x++) {
	      if(i2cBuffer[x]==0) {break; }
              if((int)i2cBuffer[x]>=32 && (int)i2cBuffer[x]<=127) {
	      	Serial.print(i2cBuffer[x]);
	      } else {
		      Serial.print("ž");
	      }
      } 
      Serial.println("");
    } else return;
    switch (cmd_getType(command)) {
      case BATT_MAH:
        valueBuffer = wordBuffer/1000;
//        Serial.print("bu<");
//        Serial.print(i2cBuffer);
//        Serial.print("><");
        fmtDouble(valueBuffer,6,i2cBuffer,bufferLen);
//        Serial.print(i2cBuffer);
//        Serial.print("><");
        strcpy(i2cBuffer+strcspn(i2cBuffer,0)," Ah");
//        Serial.print(i2cBuffer);
//        Serial.print(">");
        break;
      case BATT_MA:
        valueBuffer = (double)wordBuffer/1000.0;
        fmtDouble(valueBuffer,6,i2cBuffer,bufferLen);
        strcpy(i2cBuffer+strcspn(i2cBuffer,0)," Amps");
        break;
      case BATT_MV:
        valueBuffer = (double)wordBuffer/1000.0;
        fmtDouble(valueBuffer,6,i2cBuffer,bufferLen);
        strcpy(i2cBuffer+strcspn(i2cBuffer,0)," Volts");
        break;
      case BATT_MINUTES:
        itoa(wordBuffer,i2cBuffer,10);
        strcpy(i2cBuffer+strcspn(i2cBuffer,0)," Minutes");
        break;
      case BATT_PERCENT:
        itoa(wordBuffer,i2cBuffer,10);
        strcpy(i2cBuffer+strcspn(i2cBuffer,0),"%");
        break;
      case BATT_TENTH_K:
        valueBuffer = (double)((float)wordBuffer/10.0 - 273.15);
        fmtDouble(valueBuffer,6,i2cBuffer,bufferLen);
        strcpy(i2cBuffer+strcspn(i2cBuffer,0)," C");
        break;
      case BATT_BITFIELD:
        itoa(wordBuffer,i2cBuffer,2);
        break;
      case BATT_DEC:
        itoa(wordBuffer,i2cBuffer,10);
        break;
      case BATT_HEX:
        strcpy(i2cBuffer,"0x");
        itoa(wordBuffer,i2cBuffer+2,16);
        break;
    }
    Serial.print(i2cBuffer);
    Serial.print(" ");
    //lcd.setCursor(0,0);
    cmd_getLabel(command,i2cBuffer);
    Serial.print(i2cBuffer);
    //lcd.setCursor(12,1);
    Serial.println(">OK<");
  }
    return;
}

void DisplaySingleCommand() {
  int wordBuffer;
  double valueBuffer;
  Serial.println(PSTR("Enter command"));
  while(1) {
    while(!Serial.available()) { delay(10); }
    int command=((int)Serial.read()-'0');
    Serial.println(PSTR(">> Reading... <<"));
    Serial.println(PSTR("Hung? Check con."));
    if (cmd_getType(command) < BATT_STRING) {
      wordBuffer = i2c_smbus_read_word(cmd_getCode(command));
    } else if (cmd_getType(command) == BATT_STRING) {
      i2c_smbus_read_block(cmd_getCode(command),i2cBuffer,bufferLen);
    } else return;
    switch (cmd_getType(command)) {
      case BATT_MAH:
        valueBuffer = wordBuffer/1000;
        fmtDouble(valueBuffer,6,i2cBuffer,bufferLen);
        strcpy_P(i2cBuffer+strcspn(i2cBuffer,0),PSTR(" Ah"));
        break;
      case BATT_MA:
        valueBuffer = wordBuffer/1000;
        fmtDouble(valueBuffer,6,i2cBuffer,bufferLen);
        strcpy_P(i2cBuffer+strcspn(i2cBuffer,0),PSTR(" Amps"));
        break;
      case BATT_MV:
        valueBuffer = wordBuffer/1000;
        fmtDouble(valueBuffer,6,i2cBuffer,bufferLen);
        strcpy_P(i2cBuffer+strcspn(i2cBuffer,0),PSTR(" Volts"));
        break;
      case BATT_MINUTES:
        itoa(wordBuffer,i2cBuffer,10);
        strcpy_P(i2cBuffer+strcspn(i2cBuffer,0),PSTR(" Minutes"));
        break;
      case BATT_PERCENT:
        itoa(wordBuffer,i2cBuffer,10);
        strcpy_P(i2cBuffer+strcspn(i2cBuffer,0),PSTR("%"));
        break;
      case BATT_TENTH_K:
        valueBuffer = ((float)wordBuffer/10 - 273.15) * 1.8 + 32;
        fmtDouble(valueBuffer,6,i2cBuffer,bufferLen);
        strcpy_P(i2cBuffer+strcspn(i2cBuffer,0),PSTR(" F"));
        break;
      case BATT_BITFIELD:
        itoa(wordBuffer,i2cBuffer,2);
        break;
      case BATT_DEC:
        itoa(wordBuffer,i2cBuffer,10);
        break;
      case BATT_HEX:
        strcpy_P(i2cBuffer,PSTR("0x"));
        itoa(wordBuffer,i2cBuffer+2,16);
        break;
    }
    Serial.println(i2cBuffer);
    //lcd.setCursor(0,0);
    cmd_getLabel(command,i2cBuffer);
    Serial.println(i2cBuffer);
    //lcd.setCursor(12,1);
    Serial.println(PSTR(">OK<"));
    while (wait_on_escape(500) == 0) ; // wait for button press
    while (wait_on_escape(25)) ; // wait for release
    return;
  }
}

void DetectSMBus() {
}

void ScanI2C() {
  int x = 0;
  Serial.println("....Checking....");

  for(x=1;x<127;x++) {
    Serial.print(x<<1,HEX);
    if(i2c_detect_device(x)) {
	Serial.print(" Got");
     }	
    Serial.println("");
  }
}

void DisplayIDData() {
  //lcd.clear();
  Serial.println(PSTR("Mfg   Dev   Chem"));
  //lcd.setCursor(0,1);
  Serial.println(PSTR("... Reading ... "));
  do {
    i2c_smbus_read_block(cmd_getCode(Cmd_ManufacturerName),i2cBuffer,bufferLen);
    //lcd.setCursor(0,1);
    Serial.println(PSTR("                "));
    //lcd.setCursor(0,1);
    Serial.println(i2cBuffer);
    //lcd.setCursor(6,1);
    i2c_smbus_read_block(cmd_getCode(Cmd_DeviceName),i2cBuffer,bufferLen);
    Serial.println(i2cBuffer);
    //lcd.setCursor(12,1);
    i2c_smbus_read_block(cmd_getCode(Cmd_DeviceChemistry),i2cBuffer,bufferLen);
    Serial.println(i2cBuffer);
  } while (wait_on_escape(500) == 0);
}

void DisplayChargeData() {
  int currVoltage, currAmps, estPercent, currTemp;
  int lastVoltage, lastAmps, lastPercent, lastTemp;
  int lowVoltage, lowAmps, lowTemp, highVoltage, highAmps, highTemp;
  byte x,y;
  //lcd.clear();
  // initialize the custom char buffer & clear characters
  //for (x=0; x<=7; x++) {
  //  for (y=0; y<=7; y++) lcdCustomCharBuffer[x][y] = 0;
  //  lcd.createChar(x,lcdCustomCharBuffer[x]);
  //}
  //lcd.setCursor(0,0);
  Serial.println(PSTR("V    A    % "));
  Serial.print(0xDF); // ok, seriously, why is this symbol all the way in the far corner of the LCD charmap? With the Chinese gibberish?
  Serial.print('F');
  //lcd.setCursor(2,0);
  Serial.print(0); Serial.print(1);
  //lcd.setCursor(7,0);
  Serial.print(2); Serial.print(3);
  //lcd.setCursor(14,0);
  Serial.print(4); Serial.print(5);
  //lcd.setCursor(0,1);
  Serial.println(PSTR("... Reading ... "));
  while (wait_on_escape(25)) ;
  lowVoltage = i2c_smbus_read_word(cmd_getCode(Cmd_EDVF));
  highVoltage = i2c_smbus_read_word(cmd_getCode(Cmd_ChargingVoltage)) + 300;
  lowAmps = 32767;
  highAmps = 0;
  lowTemp = 32767;
  highTemp = 0;
  do {
    currVoltage = i2c_smbus_read_word(cmd_getCode(Cmd_Voltage));
    currAmps = i2c_smbus_read_word(cmd_getCode(Cmd_Current));
    estPercent = i2c_smbus_read_word(cmd_getCode(Cmd_RelativeStateOfCharge));
    currTemp = i2c_smbus_read_word(cmd_getCode(Cmd_Temperature));
    if (currVoltage != lastVoltage) {
      lastVoltage = currVoltage;
      //lcdClearSpace(0,1,5);
      Serial.println((float)currVoltage / 1000, 1);
      //lcdCharShiftLeft(0,1);
      //y = map(currVoltage,lowVoltage,highVoltage,8,0);
      //for (x=0;x<=7;x++) if (x >= y) bitSet(lcdCustomCharBuffer[1][x],0);
      //lcd.createChar(1,lcdCustomCharBuffer[1]);
    }
    if (currAmps != lastAmps) {
      lastAmps = currAmps;
      //lcdClearSpace(5,1,4);
      Serial.println((float)currAmps / 1000, 1);
      highAmps = max(highAmps,currAmps+100); // +/- 0.1 amps
      lowAmps = min(lowAmps,currAmps-100);
      //lcdCharShiftLeft(2,3);
      //y = map(currAmps,lowAmps,highAmps,8,0);
      //for (x=0;x<=7;x++) if (x >= y) bitSet(lcdCustomCharBuffer[3][x],0);
      //lcd.createChar(3,lcdCustomCharBuffer[3]);
    }
    if (estPercent != lastPercent) {
      lastPercent = estPercent;
      //lcdClearSpace(9,1,3);
      Serial.println(estPercent,DEC);
    }
    if (currTemp != lastTemp) {
      lastTemp = currTemp;
      //lcdClearSpace(12,1,4);
      Serial.println(((float)currTemp/10 - 273.15) * 1.8 + 32, 1);
      highTemp = max(highTemp,currTemp+10); // +/- 0.1 deg K
      lowTemp = min(lowTemp,currTemp-10);
      //lcdCharShiftLeft(4,5);
      //y = map(currTemp,lowTemp,highTemp,8,0);
      //for (x=0;x<=7;x++) if (x >= y) bitSet(lcdCustomCharBuffer[5][x],0);
      //lcd.createChar(5,lcdCustomCharBuffer[5]);
    }
  } while (wait_on_escape(500) == 0);
  //lcd_reinit_phi();
}

void DisplayStatistics() {
  //lcd.clear();
  //lcd.setCursor(0,0);
  Serial.println(PSTR("Date       #Cyc"));
  //lcd.setCursor(0,1);
  Serial.println(PSTR("... Reading ... "));
  serialData = i2c_smbus_read_word(cmd_getCode(Cmd_ManufactureDate));
  ////lcdClearSpace(0,1,16);
  Serial.println((uint8_t)(serialData >> 5) & B00001111,DEC);
  Serial.println('/');
  Serial.println((uint8_t)serialData & B00011111,DEC);
  Serial.println('/');
  Serial.println((serialData >> 9) + 1980,DEC);
  //lcd.setCursor(11,1);
  Serial.println(i2c_smbus_read_word(cmd_getCode(Cmd_CycleCount)),DEC);
  while (wait_on_escape(500) == 0) ;
}

void EnterI2C() {
  //lcd.clear(); // Clear the lcd
  Serial.println(PSTR("Enter 7 bits:   ")); // Prompt user for input

  char textAddress[8] = {'0' + bitRead(deviceAddress,6), '0' + bitRead(deviceAddress,5), '0' + bitRead(deviceAddress,4), '0' + bitRead(deviceAddress,3), '0' + bitRead(deviceAddress,2), '0' + bitRead(deviceAddress,1), '0' + bitRead(deviceAddress,0)}; // This is the buffer that will store the content of the text panel.
    while(!Serial.available()) { delay(10); }
    textAddress[0]= ((int)Serial.read()-'0');
    textAddress[1]= ((int)Serial.read()-'0');
    textAddress[2]= ((int)Serial.read()-'0');
    textAddress[3]= ((int)Serial.read()-'0');
    textAddress[4]= ((int)Serial.read()-'0');
    textAddress[5]= ((int)Serial.read()-'0');
    textAddress[6]= ((int)Serial.read()-'0');
    textAddress[7]= ((int)Serial.read()-'0');
      deviceAddress = strtoul(textAddress, NULL, 2);
      Serial.println(deviceAddress,BIN);
      Serial.println(PSTR(" >OK<"));
  while (wait_on_escape(25)) ; // wait for buttons to be up, may have residual press from menu
  while (wait_on_escape(500) == 0) ; // wait for button press
}

/*void DisplayControlWriteWord() {
  char textAddress[3]="00";  // This is the buffer that will store the content of the text panel. 
  char textValue[5]="0000"; 
  int menuSelection;

  //lcd.clear(); // Clear the lcd
  Serial.println(PSTR("Adr -WRITE- Val ")); // Prompt user for input
  //lcd.setCursor(0,1);
  Serial.println(PSTR("0x..      0x0000"));
  // common parameters
  inputHex.low.c='A'; // Text panel valid input starts with character 'A'.
  inputHex.high.c='F'; // Text panel valid input ends with character 'Z'.
  inputHex.row=1; // Display input panel at row 1
  inputHex.option=1; // Option 1 incluess 0-9 as valid characters.

  while (true) { // this way we can go back and forth, come back to it when we loop back from value field
    inputHex.ptr.msg=textAddress; // Assign the text buffer address
    inputHex.width=2; // Length of the input panel is 2 characters.
    inputHex.col=2; // Display input panel at column 2 (at "0x__")
    switch (input_panel(&inputHex)) {
      case -4: // right (at LSB, go right to the value)
      case 1:  // enter (confirm address, go to the value)
        serialCommand = strtoul(textAddress, NULL, 16);
        ////lcdClearSpace(2,1,2);
        Serial.println(serialCommand,HEX);
        inputHex.ptr.msg=textValue; // Assign the text buffer address
        inputHex.width=4; // Length of the input panel is 4 characters.
        inputHex.col=12; // Display input panel at column 12 (right side of screen, at "0x____")
        while (wait_on_escape(25)) ; // wait for buttons to be up, may have residual press from menu
        menuSelection = input_panel(&inputHex);
        while (wait_on_escape(25)) ; // wait for buttons to be up, may have residual press from menu
        serialData = strtoul(textValue, NULL, 16);
        //lcdClearSpace(12,1,4);
        Serial.println(serialData,HEX);
        switch (menuSelection) {
          case 1: // enter (confirm all, perform write)
            i2c_smbus_write_word(serialCommand,serialData); // write value with command (value converted from string in default above; command converted before we got here)
            //lcd.setCursor(5,1);
            Serial.println(PSTR("-OK-"));
            while (wait_on_escape(500) == 0) ; // wait for button press
          case -1: // escape (return to menu)
            return;
            break; // dummy break, won't reach this
          case -3: // left (at MSB, go back to address)
            break; // this is the only case where this is actually reached... haha
        }
        break;
      case -3: // left (at MSB, go back to menu since we don't want to be here)
      case -1: // escape (back to menu)
        return;
        break;
      default: // invalid?
        break;
    }
  }
}

void DisplayControlReadWord() {
  //lcd.clear(); // Clear the lcd
  Serial.println(PSTR("Addr  -READ- Val")); // Prompt user for input
  //lcd.setCursor(0,1);
  Serial.println(PSTR("0x..      0x...."));

  char textAddress[3]="00"; // This is the buffer that will store the content of the text panel. 
  inputHex.ptr.msg=textAddress; // Assign the text buffer address
  inputHex.low.c='A'; // Text panel valid input starts with character 'A'.
  inputHex.high.c='F'; // Text panel valid input ends with character 'F'.
  inputHex.width=2; // Length of the input panel is 2 characters.
  inputHex.col=2; // Display input panel at column 2
  inputHex.row=1; // Display input panel at row 1
  inputHex.option=1; // Option 1 incluess 0-9 as valid characters.

  switch (input_panel(&inputHex)) {
    case -1: // escape (back to menu)
    case -3: // left (at MSB, back to menu)
      return;
      break;
    case 1:
      serialCommand = strtoul(textAddress, NULL, 16);
      serialData = i2c_smbus_read_word(serialCommand);
      //lcdClearSpace(12,1,4);
      Serial.println(serialData,HEX);
      break;
    default:
      break;
  }
  while (wait_on_escape(25)) ; // wait for buttons to be up, may have residual press from menu
  while (wait_on_escape(500) == 0) ; // wait for button press
}

void DisplayControlReadBlock() {
  byte bytesReceived;
  //lcd.clear(); // Clear the lcd
  Serial.println(PSTR("Block Read: Addr")); // Prompt user for input

  char textAddress[3]="00"; // This is the buffer that will store the content of the text panel.
  inputHex.ptr.msg=textAddress; // Assign the text buffer address
  inputHex.low.c='A'; // Text panel valid input starts with character 'A'.
  inputHex.high.c='F'; // Text panel valid input ends with character 'Z'.
  inputHex.width=3; // Length of the input panel is 3 characters.
  inputHex.col=7; // Display input panel at column 7
  inputHex.row=1; // Display input panel at row 1
  inputHex.option=1; // Option 1 incluess 0-9 as valid characters. Option 0, default, option 1 include 0-9 as valid inputs.

  if (input_panel(&inputHex) == 1) { // only one case here, we want ENTER. Everything else escapes back to menu.
    serialCommand = strtoul(textAddress, NULL, 16);
    //lcd.clear();
    bytesReceived = i2c_smbus_read_block(serialCommand,i2cBuffer,bufferLen);
    Serial.println(PSTR("Cmd "));
    Serial.println(serialCommand,HEX);
    Serial.println(PSTR(": "));
    Serial.println(bytesReceived,DEC);
    Serial.println(PSTR(" Bytes"));
    //lcd.setCursor(0,1);
    if (bytesReceived > 0 && bytesReceived <= 16) // more than 0 bytes, less than 16.
      Serial.println(i2cBuffer);
    else
      Serial.println(PSTR("ERR:Invalid Data"));
    while (wait_on_escape(25)) ; // wait for buttons to be up, may have residual press from menu
    while (wait_on_escape(500) == 0) ; // wait for any button to continue
  }
  while (wait_on_escape(25)) ; // wait for no button press
}
*/
uint8_t i2c_detect_device ( uint8_t addr ) {
  addr <<= 1; // shift to make room for read/write bit
  if (i2c_start(addr + I2C_WRITE)) {
    // device inaccessible
    i2c_stop();
    delay(100);
    return false;
  } else {
    // device accessible
    i2c_write(0x00);
    i2c_rep_start(addr + I2C_READ);
    i2c_readAck();
    i2c_readNak();
    i2c_stop();
    delay(100);
    return true;
  }
}

void i2c_smbus_write_word ( uint8_t command, unsigned int data ) {
  i2c_start((deviceAddress<<1) + I2C_WRITE);
  i2c_write(command);
  i2c_write((uint8_t)data);
  i2c_write((uint8_t)(data>>8));
  i2c_stop();
  return;
}

unsigned int i2c_smbus_read_word ( uint8_t command ) {
  unsigned int buffer = 0;
  i2c_start((deviceAddress<<1) + I2C_WRITE);
  i2c_write(command);
  i2c_rep_start((deviceAddress<<1) + I2C_READ);
  buffer = i2c_readAck();
  buffer += i2c_readNak() << 8;
  i2c_stop();
  return buffer;
}

uint8_t i2c_smbus_read_block ( uint8_t command, char* blockBuffer, uint8_t blockBufferLen ) {
  uint8_t x, num_bytes;
  i2c_start((deviceAddress<<1) + I2C_WRITE);
  i2c_write(command);
  i2c_rep_start((deviceAddress<<1) + I2C_READ);
  num_bytes = i2c_readAck(); // num of bytes; 1 byte will be index 0
  num_bytes = constrain(num_bytes,0,blockBufferLen-2); // room for null at the end
  for (x=0; x<num_bytes-1; x++) { // -1 because x=num_bytes-1 if x<y; last byte needs to be "nack"'d, x<y-1
    blockBuffer[x] = i2c_readAck();
  }
  blockBuffer[x++] = i2c_readNak(); // this will nack the last byte and store it in x's num_bytes-1 address.
  blockBuffer[x] = 0; // and null it at last_byte+1 for LCD printing
  i2c_stop();
  return num_bytes;
}

void lcdPadBinary (uint8_t value, uint8_t bits) {
  if (bits > 8) {
 	 Serial.print('E');
	  return ;
  }
  byte x = 7;
  do {
    if (x > bits-1) continue;
    Serial.print('0' + bitRead(value,x));
  } while (x--);
}

void fmtDouble(double val, byte precision, char *buf, unsigned bufLen = 0xffff);
unsigned fmtUnsigned(unsigned long val, char *buf, unsigned bufLen = 0xffff, byte width = 0);

//
// Produce a formatted string in a buffer corresponding to the value provided.
// If the 'width' parameter is non-zero, the value will be padded with leading
// zeroes to achieve the specified width.  The number of characters added to
// the buffer (not including the null termination) is returned.
//
unsigned fmtUnsigned(unsigned long val, char *buf, unsigned bufLen, byte width) {
  if (!buf || !bufLen) return(0);

  // produce the digit string (backwards in the digit buffer)
  char dbuf[10];
  unsigned idx = 0;
  while (idx < sizeof(dbuf)) {
    dbuf[idx++] = (val % 10) + '0';
    if ((val /= 10) == 0) break;
  }

  // copy the optional leading zeroes and digits to the target buffer
  unsigned len = 0;
  byte padding = (width > idx) ? width - idx : 0;
  char c = '0';
  while ((--bufLen > 0) && (idx || padding)) {
    if (padding) padding--;
    else c = dbuf[--idx];
    *buf++ = c;
    len++;
  }

  // add the null termination
  *buf = '\0';
  return(len);
}

//
// Format a floating point value with number of decimal places.
// The 'precision' parameter is a number from 0 to 6 indicating the desired decimal places.
// The 'buf' parameter points to a buffer to receive the formatted string.  This must be
// sufficiently large to contain the resulting string.  The buffer's length may be
// optionally specified.  If it is given, the maximum length of the generated string
// will be one less than the specified value.
//
// example: fmtDouble(3.1415, 2, buf); // produces 3.14 (two decimal places)
//
void fmtDouble(double val, byte precision, char *buf, unsigned bufLen) {
  if (!buf || !bufLen) return;
  // limit the precision to the maximum allowed value
  const byte maxPrecision = 6;
  if (precision > maxPrecision)
    precision = maxPrecision;

  if (--bufLen > 0)
  {
    // check for a negative value
    if (val < 0.0) {
      val = -val;
      *buf = '-';
      bufLen--;
    }

    // compute the rounding factor and fractional multiplier
    double roundingFactor = 0.5;
    unsigned long mult = 1;
    for (byte i = 0; i < precision; i++) {
      roundingFactor /= 10.0;
      mult *= 10;
    }
    if (bufLen > 0) {
      // apply the rounding factor
      val += roundingFactor;

      // add the integral portion to the buffer
      unsigned len = fmtUnsigned((unsigned long)val, buf, bufLen);
      buf += len;
      bufLen -= len;
    }

    // handle the fractional portion
    if ((precision > 0) && (bufLen > 0)) {
      *buf++ = '.';
      if (--bufLen > 0) buf += fmtUnsigned((unsigned long)((val - (unsigned long)val) * mult), buf, bufLen, precision);
    }
  }

  // null-terminate the string
  *buf = '\0';
}

int main(void)
{
	init();

#if defined(USBCON)
	USBDevice.attach();
#endif
	
	setup();
		
	for (;;) {
		loop();
		if (serialEventRun) serialEventRun();
	}
				
	return 0;
}

