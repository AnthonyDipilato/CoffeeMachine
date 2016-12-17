/*    Brew Class
 *   
 *    Class manages brewing while active
 *  
 */

#ifndef Brew_h
#define Brew_h

#include "Arduino.h"

class Brew
{
    public:
        Brew(int test);
        void loop_();
        void start(int brew_switch);
        void save();
        void stop_();
        void boiler();
        void cycle();
        void prime();
        void toggle_boiler(int action);
        void updateThermo();
        void toggle_cycle(int action);
        void update_volume();
        void toggle_pump(int action);
        int cycle_status;
        long cycle_start;
        long cycle_end;
        int value;
        int brew_size;
        int brew_cups; // how many cups we are brewing
        int brewed; // how many cups brewed
        long brew_time; // total pump time from brew cycles
        int cycles; // count of brew cycles
        int volume; // total volume brewed (ml)
        int percent;
        int state;
        int temp;
        long last_check;
        int temp_delay; // interval to check temp
        int valve_state;
        int boiler_state;
        int pump_state;
        int prime_status;
        long prime_start;
        int test;
        int total_cups; // total cups brewed
        long last_add;
        long pump_time;
        
    private:
        int none;
};



#endif

