/*    Configuration
 *   
 *    Default configurations for firmware
 *    User changed settings are saved to EEPROM but can be reset to these.
 *  
 */

#ifndef Configuration_h
#define Configuration_h

// Pins for relays
#define PUMP    2
#define VALVES  3
#define HEAT    5
#define LIGHT   4 // machine uses 110v lamps so we use a relay to toggle the indicator light

// Pins for thermo CLK, CS, SO
#define THERMO_CLK  13
#define THERMO_CS   11
#define THERMO_SO   12

//  Temperature 
#define TEMP_THRESHOLD    3     //  Instead of using a pid library will use a min max threshold to trigger heater
#define BREW_TEMPERATURE  200   //  We will manually calibrate to acheive the temp until we install a secondary thermocouple
#define BOILER_MAX        215   //  Max boiler temperature before shutoff always off
#define BOILER_IDLE       175   //  Temperature target when not brewing

//  Brew Cycle 
//  The coil cannot heat on the fly to brew temperature. So we will use cycles to allow water in boiler to heat to temperature
#define BOILER_BREW       194   //  Temperature target for each brew cycle
#define BOILER_BREW_LOW   170   //  Low threshold to stop brew cycle
#define CYCLE_ON_TIME     10000     //  Minimum run time in seconds for each brew cycle
#define CYCLE_REST        2000     //  Minumum wait time between brew cycles

//  Brew Volume
//  A "coffee cup" is not a cup (8oz) it is 5.07oz or 150ml
//  We are using milliters to maintain sanity.
#define ONE_CUP   150     //  ml

//  The brew switch toggles between large and small batches.
#define LARGE_POT  12     //  cups
#define SMALL_POT  8      //  cups

//  Pump Settings
//  We don't have a way to measure brew volume, so we will estimate it using a pump flow rate
#define PUMP_FLOW_RATE  6     // ml/s
#define PUMP_MIN_RUN    1000     // Minimum run time for the pump in ms (also includes pump button)

//  Priming
//  All times in ms
#define PRIME_TIME        5000   // On startup the pump will prime into reservoir
#define PRIME_BREW        3000   // When brew mode is started it will prime again
#define PRIME_BREW_CYCLE  1000   // Steam tends to break our siphon so we will do a short prime in the begging of each brew cycle. Will also reduce sputtering.

#endif

