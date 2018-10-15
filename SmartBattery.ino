#include "Arduino.h"
#include <avr/sleep.h>
#include <avr/power.h>
#include "debugConf.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>


#define LED_PIN 8
#define TRANSISTOR_PIN 9
#define INTERRUPT_PIN 2
#define OLED_RESET 4

Adafruit_INA219 ina219;
Adafruit_SSD1306 display(OLED_RESET);

void pin2Interrupt(void);
void enterSleep(void);
void drawLines();
void printVoltage(float v, float mw, float ma);

void setup() {
	Serial.begin(9600);
	pinMode(LED_PIN, OUTPUT);
	pinMode(TRANSISTOR_PIN, OUTPUT);
	pinMode(INTERRUPT_PIN, INPUT_PULLUP);

	digitalWrite(TRANSISTOR_PIN, HIGH);
	digitalWrite(LED_PIN, HIGH);

//	display.clearDisplay();

	display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	ina219.begin();
	#ifdef DEBUG
	Serial.println("Setup: ok");
	#endif
}

void loop() {

//	float shuntvoltage = 0;
	float busvoltage = 0;
	float current_mA = 0;
//	float loadvoltage = 0;
	float power_mW = 0;

//	shuntvoltage = ina219.getShuntVoltage_mV();
	busvoltage = ina219.getBusVoltage_V();
	current_mA = ina219.getCurrent_mA();
	power_mW = ina219.getPower_mW();
//	loadvoltage = busvoltage + (shuntvoltage / 1000);

	printVoltage(busvoltage, power_mW, current_mA);
	delay(1000);
	//display.clearDisplay();
	enterSleep();
}
void printVoltage(float v, float mw, float ma){
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
  display.println("Volt");
  display.setCursor(35,0);
  display.println(v);

  //*************
  display.setCursor(0,12);
  display.println("mW");
  display.setCursor(35,12);
  display.println(mw);

  //*************
  display.setCursor(0,24);
  display.println("mA");
  display.setCursor(30,24);
  display.println(ma);

  display.display();
  delay(100);
 /*
  display.startscrollright(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrollleft(0x00, 0x0F);
  delay(2000);
  display.stopscroll();
  delay(1000);
  display.startscrolldiagright(0x00, 0x07);
  delay(2000);
  display.startscrolldiagleft(0x00, 0x07);
  delay(2000);
  display.stopscroll();
*/
}
void drawLines(){
  for (int16_t i=0; i<display.height()/2; i+=2) {
    display.drawRect(i, i, display.width()-2*i, display.height()-2*i, WHITE);
    display.display();
    delay(1000);
  }
}

void pin2Interrupt(void) {
  detachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN));
  digitalWrite(LED_PIN, HIGH);
}

void enterSleep() {

	bool a = digitalRead(INTERRUPT_PIN);
	Serial.print("Interrupt ");
	Serial.println(a);
	attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), pin2Interrupt, LOW);
	delay(100);

	if (!a) {
	  powerDownAllPins();
	  set_sleep_mode(SLEEP_MODE_PWR_DOWN);

	  sleep_enable();
	  sleep_mode();
	}

	//TODO :wakeup ?

  sleep_disable();
}

void powerDownAllPins() {
	//Save Power by writing all Digital IO LOW
	for (int i = 0; i < 20; i++) {
		if (i != 2) //esclude pin 2 for interrupt
			pinMode(i, OUTPUT);
			digitalWrite(i, LOW);
	}
}


// TODO:
/*
void setup() {
	pinMode(LED_PIN, OUTPUT);

	// putt low all pin to save energy
	puttLowAll();
	attachInterrupt(0, digitalInterrupt, FALLING); //interrupt for waking up

	//SETUP WATCHDOG TIMER
	WDTCSR = (24); //change enable and WDE - also resets
	WDTCSR = (33); //prescalers only - get rid of the WDE and WDCE bit
	WDTCSR |= (1 << 6); //enable interrupt mode

	//Disable ADC - don't forget to flip back after waking up if using ADC in your application ADCSRA |= (1 << 7);
	ADCSRA &= ~(1 << 7);

	//ENABLE SLEEP - this enables the sleep mode
	SMCR |= (1 << 2); //power down mode
	SMCR |= 1; //enable sleep
}

void loop() {

	digitalWrite(LED_PIN, HIGH);
	delay(10000);
	digitalWrite(LED_PIN, LOW);

	//BOD DISABLE - this must be called right before the __asm__ sleep instruction
	MCUCR |= (3 << 5); //set both BODS and BODSE at the same time
	MCUCR = (MCUCR & ~(1 << 5)) | (1 << 6); //then set the BODS bit and clear the BODSE bit at the same time
	__asm__ __volatile__("sleep");
	//in line assembler to go to sleep
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
}

void digitalInterrupt(void) {
	//needed for the digital input interrupt
}

ISR(WDT_vect) {
	//DON'T FORGET THIS!  Needed for the watch dog timer.  This is called after a watch dog timer timeout - this is the interrupt function called after waking up
}// watchdog interrupt


void puttLowAll() {
	//Save Power by writing all Digital IO LOW - note that pins just need to be tied one way or another, do not damage devices!
	for (int i = 0; i < 20; i++) {
		if (i != 2) //just because the button is hooked up to digital pin 2
			pinMode(i, OUTPUT);
	}
}
*/
