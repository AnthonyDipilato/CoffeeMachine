# Coffee Machine
![Coffee Machine](http://anthonydipilato.com/wp-content/uploads/2017/03/img_3953.jpg width=400)
    
This is project is the arduino code running my home built coffee machine.
You can see the  build log and details for the project at [http://anthonydipilato.com/2016/10/20/coffee-machine/](http://anthonydipilato.com/2016/10/20/coffee-machine/)
    
The arduino controls all functions for the coffee machine.

```    
USE AT YOUR ON RISK.
I am posting this project for EDUCATIONAL USE ONLY.
This project involves electricity, high heat, and water/steam.
I will not be held liable damages and/or injuries resulting from the use of this code or from copying this project.
```

### Input
- 3 Momentary Buttons
- Switch
- Rotary encoder with push button
- Max6675 Thermocontroller

### Outputs
- 3 lights (one controlled by arduino)
- 20x4 LCD module
- 4 relays
    
Brew functions are controlled in Brew class. 
    
### Required libraries
- [LiquidCrystal_I2C](https://github.com/fdebrabander/Arduino-LiquidCrystal-I2C-library) there are a few versions floating around a couple have an issue with only printing the first character of a string.
- [Adafruit Max31855 library](https://github.com/adafruit/Adafruit-MAX31855-library)

### Author
Anthony DiPilato, Anthony@bumbol.com

### License
All code is available under the MIT license. See LICENSE file for info.

    
 
