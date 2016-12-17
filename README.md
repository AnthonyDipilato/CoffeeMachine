Coffee Machine
    
This is project is the arduino code running my home built coffee machine.
You can see the  build log and details for the project at [url]
    
The arduino controls all functions for the coffee machine.
Inputs include:
- 3 buttons
- switch
- Rotary encoder with push button
- Max6675 Thermocontroller

Outputs include:
- 3 lights (one controlled by arduino)
- 20x4 LCD module
- 4 relays
    
Brew functions are controlled in Brew class. 
    
    
USE AT YOUR ON RISK.
I am posting this project for EDUCATIONAL USE ONLY.
This project involves electricity, high heat, and water/steam.
I will not be held liable damages and/or injuries resulting from the use of this code or from copying this project.
    
Required libraries
- LiquidCrstal_I2C, there are a few versions floating around a couple have an issue with only printing the first character of a string.
- Adafruit Max31855 library
    
 