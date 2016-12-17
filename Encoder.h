/*    Encoder Class
 *   
 *    Manager to handle the encoder postion. And button status
 *  
 */

#ifndef Encoder_h
#define Encoder_h

#include "Arduino.h"

class Encoder
{
    public:
        Encoder(int clk,int dt, int sw);
        void check_button();
        void check();
        int button_value;
        int pos;
    private:
        int _clk;
        int _dt;
        int _sw;
        long debounce;
        int last_button;
        int debounce_delay;
        int clk_last;
        boolean bCW;
};



#endif

