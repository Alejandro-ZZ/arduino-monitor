# Ardu monitor

Voltage, current, temperature and date/time monitor using Arduino pro mini (ATmega328P). Data is recorded every specific range of time (this case every 30s), saved to a microSD card and shown in a 16x2 LCD display.

## MicroSD files

This code writes the data read in a microSD memory through SPI communication and MicroSD Adapter that includes a level shifter. The information is recorded in a CSV  file format (Comma Separated Values) and then processed on the computer. Headers have to be created, in all memory files used, before running the system. The program uses two files: 

**1. `DATA.txt`:** in this file all main values are saved in the following format: 

  <p align="center">
    Date, Time, Battery[V], Shunt_Bat[mV], Current[A], Temperature[°C]
  </p>
  
Where `Shunt_Bat[mV]` is the shunt resistor voltage and `Current[A]` the current taken from the battery.

**2. `LOG.txt`:** this file is used to record every time the system is reset. This data is format as follows: 

  <p align="center">
    Date, Time, Log
  </p>

Where `Log` is the log message, in this case it'll be "Reset System".

## LCD display info

The follow image show how the data is displayed in a 16x2 LCD.

![alt text](https://github.com/Alejandro-ZZ/arduino-monitor/blob/master/LCD_Data.jpg)

## Results and graphics

The system was used to monitor an 12V uninterrupted power supply (UPS) charge and discharge cycles. A total of 2426 data where taken. The system was executed for around 20 hours and data were recorded every 30s. Then, the `DATA.txt` file is proccessed with Excel and the following graphics were created. 

**DISCHARGING CYCLE:** The discharge average current was around 1.25A.

![alt text](https://github.com/Alejandro-ZZ/arduino-monitor/blob/master/Descarga_Bateria_12V.png)


**CHARGING CYCLE:** Battery is charge at 500mA. The orange curve shows the current behavior and the blue one the voltage behavior.

![alt text](https://github.com/Alejandro-ZZ/arduino-monitor/blob/master/Carga_Bateria_12V.png)
