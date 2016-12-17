/*    Brew Class
 *   
 *    Class manages brewing while active
 *  
 */

#include "Arduino.h"
#include "Brew.h"
#include "Configuration.h"
#include <EEPROM.h>
#include "EEPROMAnything.h"
#include <max6675.h>



struct config_t
{
    int total_cups;
} configuration;

// Set pins and initialize thermocouple (CLK, CS, SO)
MAX6675 thermocouple(THERMO_CLK, THERMO_CS, THERMO_SO);

// Initiate brew class
Brew::Brew(int test) // for some reason  arduino doesn't recognize the class unless we pass an arg
{
    // Initial brew variables
    temp_delay = 250; // the max6675 is limited by read speed
    state = 0;
    valve_state = 0;
    boiler_state = 0;
    // Load total cups brewed from eeprom
    EEPROM_readAnything(0, configuration);
    total_cups = configuration.total_cups;
}

void Brew::save(){
  configuration.total_cups = total_cups;
  EEPROM_writeAnything(0, configuration);
}

void Brew::start(int brew_switch){
   // Large batch = 0
   if(brew_switch == 0){
      brew_size = ONE_CUP * LARGE_POT;
      brew_cups = LARGE_POT;
   }else{
      brew_size = ONE_CUP * SMALL_POT;
      brew_cups = SMALL_POT;
   }
   // Brew Start Settings
   brew_time = 0;
   cycles = 0;
   volume = 0;
   percent = 0;
   state = 1; // brewing active
   brewed = 0; // cups currently brewed
   pump_state = 0;
   // Prime pump
   prime_start = millis();
   prime();
}

// Toggle pump
// Used for manually priming. Pump has minimum run time so it doesn't immediately shut off if the button is triggered
void Brew::toggle_pump(int action){
    if(action == pump_state){
        return;
    }
    if(!action){
        if ((millis() - pump_time) > PUMP_MIN_RUN) {
            // Turn off pump 
            digitalWrite(PUMP, HIGH); // High is off
            digitalWrite(VALVES, HIGH);
            pump_state = action;
        }
    }else{
        // Turn on pump
        digitalWrite(PUMP, LOW);
        digitalWrite(VALVES, HIGH);
        pump_state = action;
        pump_time = millis();
    }
    return;
}



void Brew::prime(){
    if(millis() < (prime_start + PRIME_BREW)){
        toggle_pump(1);
    }else{
        toggle_pump(0);
        prime_status = 1;
    }
}


void Brew::stop_(){
    state = 0;
}


void Brew::toggle_boiler(int action){
    if(action){
      digitalWrite(HEAT, LOW);
    }else{
      digitalWrite(HEAT, HIGH);
    }
    boiler_state = action;
}

void Brew::boiler(){
  int target;
  // if brewing  and not complete
  if(state && volume < brew_size){
      target = BOILER_BREW;
  // if idle
  }else{
      target = BOILER_IDLE;
  }
  // heater on and over threshold, turn off heat
  if(temp >= (target + TEMP_THRESHOLD)){
      toggle_boiler(0);
  }
  // heater off and under threshold, turn on heat
  if(temp < (target - TEMP_THRESHOLD)){
      toggle_boiler(1);
  }
  // Boiler is over max shut off
  if(temp >= BOILER_MAX){
    Serial.println("Boiler Max");
    toggle_boiler(0);
  }
}

void Brew::toggle_cycle(int action){
    // start cycle
    if(action){
        // we want to start each cycle with a short prime
        // Turn on pump and valve
        digitalWrite(PUMP, LOW);
        digitalWrite(VALVES, LOW);
        cycles++;
        cycle_start = millis();
        cycle_status = 1;
    }else{
        // Turn off pump and valve
        digitalWrite(PUMP, HIGH);
        digitalWrite(VALVES, HIGH);
        cycle_end = millis();
        cycle_status = 0;
        update_volume();
    }
}

void Brew::update_volume(){
    int total_time = cycle_end - cycle_start;
    int new_volume = PUMP_FLOW_RATE * (total_time / 1000);
    volume += new_volume;
    brewed = round((float)volume / (float)ONE_CUP);
    float p = ((float)volume / (float)brew_size) * 100;
    percent = (int)p;
    // If complete save cups total and play finished tone
    if(volume >= brew_size){
        total_cups += brewed;
        save();
        //TODO play completed tone
    }
    Serial.print(volume);
    Serial.print(" / ");
    Serial.println(brew_size);
    Serial.println(p);
    Serial.print("Percent: ");
    Serial.println(percent);
}

void Brew::cycle(){
    if(cycle_status){
      if(temp <= BOILER_BREW_LOW &&  millis() > (cycle_start + CYCLE_ON_TIME)){
          toggle_cycle(0);
      }
    }else{
      if(temp >= BOILER_BREW && millis() > (cycle_end + CYCLE_REST)){
          toggle_cycle(1);
      }
    }
}

void Brew::loop_(){
  // Prime on startup
  if(state && !prime_status){
        prime();
    }
  updateThermo();
  boiler();
  if(state && volume < brew_size){
    cycle();
  }
}

void Brew::updateThermo(){
    if(millis() > last_check + temp_delay){
        // max6675 library is dumb so it only updates on readC() readF() only converts
        thermocouple.readCelsius();
        temp = thermocouple.readFarenheit(); // decimal precision is not needed
        last_check = millis();
    }
    
}





 
