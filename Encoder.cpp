/*    Encoder Class
 *   
 *    Manager to handle the roatary encoder postion and button status
 *  
 */

#include "Arduino.h"
#include "Encoder.h"

// Initiate pin for sensor
Encoder::Encoder(int clk, int dt, int sw)
{
    // INPUT_PULLUP allows us to use ground on the button by using the onboard pullup resistor,
    pinMode (clk,INPUT);
    pinMode (dt,INPUT);
    pinMode(sw, INPUT_PULLUP);
    _sw = sw;
    _clk = clk;
    _dt = dt;
    button_value = 0;
    clk_last = digitalRead(clk);
    debounce_delay = 50; 
}

void Encoder::check_button(){
    int sensor_value;
    // read sensor
    int reading = digitalRead(_sw);
    // INPUT_PULLUP inverses reading
    if(reading == HIGH){
        sensor_value = 0;
    }else{
        sensor_value = 1;
    }
    // if value does not match last state reset debounce timer
    if(sensor_value != last_button){ 
        debounce = millis(); 
   }
   // if reading is stable past debounce timer we can set it
   if ((millis() - debounce) > debounce_delay) {
      if(sensor_value != button_value){
          button_value = sensor_value;
          Serial.print("Encoder Button: ");
          Serial.println(button_value);
      }
   }
    // Save sensor for last state debounce
    last_button = sensor_value;
}


void Encoder::check(){
  check_button();
  int clk_val;
  clk_val = digitalRead(_clk);
   if (clk_val != clk_last){ // Means the knob is rotating
     // if the knob is rotating, we need to determine direction
     // We do that by reading pin B.
     if (digitalRead(_dt) != clk_val) {  // Means pin A Changed first - We're Rotating Clockwise
       pos ++;
       bCW = true;
     } else {// Otherwise B changed first and we're moving CCW
       bCW = false;
       pos--;
     }
     Serial.print ("Rotated: ");
     if (bCW){
       Serial.println ("clockwise");
     }else{
       Serial.println("counterclockwise");
     }
     Serial.print("Encoder Position: ");
     Serial.println(pos);
     
   } 
   clk_last = clk_val;
}




 
