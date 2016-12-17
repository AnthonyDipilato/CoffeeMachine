/*    Coffee Machine
 *    Written By Anthony DiPilato
 *    
 *    This project is for a coffee maching I built.
 *    You can see the  build log and details for the project at [url]
 *    
 *    The arduino control all functions for the coffee machine.
 *    Inputs include 3 buttons, switch, and rotary encoder with push button, thermocontroller
 *    Outputs include 3 lights (one controlled by arduino), 20x4 LCD module, 4 relays
 *    
 *    Brew functions are controlled in Brew class. 
 *    
 *    
 *    USE AT YOUR ON RISK.
 *    I am posting this project for EDUCATIONAL USE ONLY.
 *    This project involves electricity, high heat, and water/steam.
 *    I will not be held liable damages and/or injuries resulting from the use of this code or from copying this project.
 *    
 *    Required libraries
 *    LiquidCrstal_I2C, there are a few versions floating around a couple have an issue with only printing the first character of a string.
 *    Adafruit Max31855 library
 *    
 */
 
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

// Configurations
#include "Configuration.h"

// Buttons class will handle checking and debouncing the buttons and switch states
#include "Buttons.h"
#include "Encoder.h"
#include "Brew.h"

// Setup lcd
LiquidCrystal_I2C lcd(0x27,20,4);  // set the LCD address to 0x27 for a 16 chars and 2 line display


// Pins for rotary encoder
// (clk, dt, sw)
Encoder rotary(A1, A0, A2);

// Pins for buttons and switches
Buttons button_top(10);
Buttons button_mid(9);
Buttons button_bot(8);
Buttons brew_switch(7);

// Used for printing custom characters from bytes
#if defined(ARDUINO) && ARDUINO >= 100
    #define printByte(args)  write(args);
#else
    #define printByte(args)  print(args,BYTE);
#endif
// Define custom characters
uint8_t arrow[8] = {0x00,0x04,0x06,0x1f,0x1f,0x06,0x04,0x00};
uint8_t degree[8]  = {140,146,146,140,128,128,128,128};
// custom characters to display a progress bar
uint8_t leftBox[8] = {0x1f,0x10,0x10,0x10,0x10,0x10,0x10,0x1f};
uint8_t midBox[8] = {0x1f,0x00,0x00,0x00,0x00,0x00,0x00,0x1f};
uint8_t rightBox[8] = {0x1f,0x01,0x01,0x01,0x01,0x01,0x01,0x1f};
uint8_t fullBox[8] = {0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f,0x1f};
uint8_t halfBox[8] = {0x1f,0x1c,0x1c,0x1c,0x1c,0x1c,0x1c,0x1f};

// thermo variables
double temp;

// Pump and light states
int pump_state = 0;
int light_state = 0;
// related variables
long pump_time;
int brew_button_check = 0; // so we only read button once per press
// Display Variables
int last_display = 0;
long last_refresh;
long last_clear;
int refresh_delay = 250;
int last_temp;
// prime initial state
int prime_status = 0;
long prime_start = 0;

// Setup Brew Class
Brew brew(0);

void setup()
{
  
    // Serial output for debugging
    Serial.begin(9600);
    // Pin mode for relays
    pinMode(PUMP, OUTPUT);       
    pinMode(VALVES, OUTPUT);
    pinMode(HEAT, OUTPUT);
    pinMode(LIGHT, OUTPUT);
    // Initialize relays (HIGH is off)
    digitalWrite(PUMP, HIGH);
    digitalWrite(VALVES, HIGH);  
    digitalWrite(HEAT, HIGH);
    digitalWrite(LIGHT, HIGH);
    lcd.init();                      // initialize the lcd 
    lcd.backlight();   
    // Add custom characters             
    lcd.createChar(0, degree);
    lcd.createChar(1, leftBox);
    lcd.createChar(2, midBox);
    lcd.createChar(3, rightBox);
    lcd.createChar(4, fullBox);
    lcd.createChar(5, halfBox);
    lcd.clear();  
    // Allow everything to ready up before starting loop
    delay(500);
    // Prime pump
    prime_start = millis();
    prime();
}


// Toggle pump
// Used for manually priming. Pump has minimum run time so it doesn't immediately shut off if the button is triggered
void toggle_pump(int state){
    if(state == pump_state){
        return;
    }
    if(!state){
        if ((millis() - pump_time) > PUMP_MIN_RUN) { // check minimum run time
            // Turn off pump 
            digitalWrite(PUMP, HIGH); // High is off
            pump_state = state;
        }
    }else{
        // Turn on pump
        digitalWrite(PUMP, LOW); // LOW is on
        pump_state = state;
        pump_time = millis(); // set start for min run time
    }
    return;
}

// Toggle Brew
void toggle_brew(){
    // If cancel brew if active
    if(brew.state){
        brew.stop_();
    }else{
        brew.start(brew_switch.value); // start brew with brew size
    }
}

// Check button states
void check_buttons(){
    // Check encoder
    rotary.check();
    // Check buttons
    button_top.check();
    button_mid.check();
    button_bot.check();
    brew_switch.check();
    //  Pump Button 
    //  Only works while machine is idle also does not trigger valves
    if(!brew.state && button_mid.value){
        toggle_pump(1);
    }else if(!brew.state && !button_mid.value && prime_status)
    {
        toggle_pump(0);
    }
    // Brew Button
    // brew_button_check to make sure we only read it once per press
    if(button_bot.value && !brew_button_check && prime_status){
        toggle_brew();
        brew_button_check = 1;
    }
    else if (!button_bot.value && brew_button_check){
        brew_button_check = 0;
    }
    // TODO Menu button
}

// Status indicator light for brewing
void status_light(){
  // if brewing and light is off, turn it on
  if(brew.state && !light_state){
      digitalWrite(LIGHT, LOW); // LOW is on
      light_state = 1;
  }
  // if not brewing and light is on, turn it off
  else if(!brew.state && light_state){
      digitalWrite(LIGHT, HIGH); // HIGH is off
      light_state = 0;
  }
}

// Main display when not brewing or in menu
void display_ready(){
    // if last_display was not ready screen clear it
    if(last_display != 0){
        lcd.clear();
        last_display = 0;
    }
    // Total Brewed loaded from EEPROM
    lcd.setCursor(0,0);
    lcd.print("Cups Brewed: ");
    lcd.print(brew.total_cups);
    // Temperature
    lcd.setCursor(0,1);
    lcd.print("Boiler Temp: ");
    // if temp changed we will clear it first
    if(brew.temp != last_temp){
        lcd.setCursor(13,1);
        lcd.print("    "); 
        last_temp = brew.temp;
    }
    lcd.setCursor(13,1);
    lcd.print(brew.temp);
    lcd.printByte(0);
    // Batch Size
    lcd.setCursor(0,2);
    lcd.print("Batch Size:  ");
    if(brew_switch.value == 0){
        lcd.print(LARGE_POT);
    }else{
        lcd.print(SMALL_POT);
        lcd.print(" ");  
    }
    lcd.print(" cups");
    // Ready message
    if(pump_state){
        lcd.setCursor(3,3);
        lcd.print("   Priming   "); // Using spaces to clear message in case "Ready to brew" was displayed. lcd.clear() is slow and causes the screen to flicker.
    }else{
        lcd.setCursor(3,3);
        lcd.print("Ready to brew");
    }
}

void display_brewing(){
    // if last_display was not brewing screen clear it
    if(last_display != 1){
        lcd.clear();
        last_display = 1;
    }
    // Status
    lcd.setCursor(2,0);
    if(brew.prime_status){
        lcd.print("Brewing ");
        lcd.print(brew.brew_cups);
        lcd.print(" cups");
    }else{
        lcd.print("Priming");
    }
    // Temperature
    lcd.setCursor(2,1);
    lcd.print("Temp: ");
    // if temp changed we will clear it first
    if(brew.temp != last_temp){
        lcd.setCursor(8,1);
        lcd.print("    "); 
        last_temp = brew.temp;
    }
    lcd.setCursor(8,1);
    lcd.print(brew.temp);
    lcd.printByte(0);
    // Brewed Volume
    lcd.setCursor(2,2);
    lcd.print(brew.brewed);
    lcd.print(" cups ");
    lcd.print(brew.volume);
    lcd.print("ml");

    // Progress Bar
    display_progress_bar();
    lcd.setCursor(14,3);
    lcd.print(brew.percent);
    lcd.print("%");
}

// Build a progress bar for brewing percent
// Uses custom characters to draw bar
void display_progress_bar(){
    int cols = 0; // column count
    // Custom Characters
    // leftBox: 1, midBox: 2, rightBox: 3, fullBox: 4, halfBox: 5
    lcd.setCursor(2,3);
    // First block if < 10%
    if(brew.percent < 5){
        lcd.printByte(1); // left box
        cols++;
    }else if(brew.percent >= 5 && brew.percent < 10){
        lcd.printByte(5); // half box
        cols++;
    }
    // Next full rows
    int full_cols = brew.percent / 10;
    for(int i=0; i < full_cols; i++){
        lcd.printByte(4);
        cols++;  
    }
    // Add half box
    if((brew.percent - (full_cols * 10)) >= 5 && cols > 1 && cols < 9){
        lcd.printByte(5);
        cols++; 
    }
    // Fill empty rows
    int empty = 9 - cols;
    for(int i=0; i < empty; i++){
        lcd.printByte(2);
        cols++;  
    }
    // Last row
    if(cols < 10){
        lcd.printByte(3);  
    }
    
}

// Update Display based on state
// too avoid flicker we will only clear on display change
// last_display: 0 ready, 1 brewing, 2 menu (handled separately)
void update_display(){
    // we dont need to update display on every loop
    if ((millis() - last_refresh) < refresh_delay) {
      return;
    }
    // if not brewing display ready screen
    if(!brew.state){
        display_ready();
    }else{
        display_brewing();
    }
    // update refresh time
    last_refresh = millis();
}

// prime on startup
// if the heater has air in it, it causes temperature spikes and sputtering
void prime(){
    if(millis() < (prime_start + PRIME_TIME)){
        toggle_pump(1);
    }else{
        toggle_pump(0);
        prime_status = 1;
    }
}

// Main loop
void loop()
{
    // If we haven't primed already
    if(!prime_status){
        prime();
    }
    // Check states of buttons and switches
    check_buttons();
    // Brew Class Loop
    brew.loop_();
    // Indicator Light, may move it to brew class
    status_light();
    // Display Loop
    update_display();
}
