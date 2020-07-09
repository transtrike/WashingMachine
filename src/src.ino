#include <Arduino.h>

//Water Storage Levels
#define EMPTY_WATER        1
#define HALF_FILLED_WATER  2
#define FULLY_FILLED_WATER 3

//Heater Pins
#define LIGHTLY_HEATED_WATER  1
#define MEDIUMLY_HEATED_WATER 2
#define HEAVILY_HEATED_WATER  3

//Motor Pins
#define MOTOR_F_R       12
#define MOTOR_PWM       10

//Water Level Pins
#define WATER_LEVEL_2   9
#define WATER_LEVEL_1   8
#define WATER_LEVEL_EMP 7

//Tempreture Level Pins
#define TEMP_LEVEL_HIGH 6
#define TEMP_LEVEL_MED  5
#define TEMP_LEVEL_LOW  4

//Program Selection Pins
#define PROG_SELECT_4   3
#define PROG_SELECT_3   2
#define PROG_SELECT_2   1
#define PROG_SELECT_1   0

//Outputs
#define WATER_VALVE     A0
#define WATER_PUMP      A1
#define WATER_HEATER    A2
#define LOCK_DOOR       A3

//Relay Pin
#define RELAY           A7

//Global And Default Variables
#define DEFAULT_MOTOR_CYCLES 5
#define MAX_MOTOR_SPEED      255
#define MEDIUM_MOTOR_SPEED   128
#define LOW_MOTOR_SPEED      50

//Methods Declaration
void DefineIO();
void PowerOnRelay();
void PowerOnRelay();
void LockDoor();
void UnlockDoor();
void FulfillProgram(byte selectedProgram);
byte ReadProgram();
void FillWater();
bool ReadWaterLevel();
void HeatWater();
bool ReadHeatLevel(byte level);
void SpinMotor(unsigned int cycles, byte speed);
void Wash(unsigned int cycles, byte speed);
void Centrifuge(unsigned int cycles, byte speed);
void TurnPumpON();
void TurnPumpOFF();

//First you need to select a program from 1-F

//Initialize all startup vars
void setup()
{
    //Select which ports does what
    DefineIO();

    //Turn on the Realy to read the inputs from the sensors
    PowerOnRelay();
    delay(1000);
    //Lock the "door"
    LockDoor();

    //Read the selected program from the user
    byte selectedProgram = ReadProgram();
    FulfillProgram(selectedProgram);

    UnlockDoor();
    delay(5000);
}

void loop() {}

void DefineIO()
{
    //Motor(OUTPUT):
    //  D12 - F/R
    //  D10 - PWM
    pinMode(MOTOR_F_R, OUTPUT);
    pinMode(MOTOR_PWM, OUTPUT);

    //Water Level System(INPUT):
    //  D9 - WL2
    //  D8 - WL1
    //  D7 - WL_EMPTY
    pinMode(WATER_LEVEL_2, INPUT);
    pinMode(WATER_LEVEL_1, INPUT);
    pinMode(WATER_LEVEL_EMP, INPUT);

    //Tempreture Level System(INPUT):
    //  D6 - TL_HIGH
    //  D5 - TL_MED
    //  D4 - TL_LOW
    pinMode(TEMP_LEVEL_HIGH, INPUT);
    pinMode(TEMP_LEVEL_MED, INPUT);
    pinMode(TEMP_LEVEL_LOW, INPUT);

    //Program Selection(INPUT):
    //  D3 - PS4
    //  D2 - PS3
    //  RX - PS2
    //  TX - PS1
    pinMode(PROG_SELECT_1, INPUT);
    pinMode(PROG_SELECT_2, INPUT);
    pinMode(PROG_SELECT_3, INPUT);
    pinMode(PROG_SELECT_4, INPUT);

    //Outputs
    pinMode(WATER_VALVE, OUTPUT);
    pinMode(WATER_PUMP, OUTPUT);
    pinMode(WATER_HEATER, OUTPUT);

    pinMode(LOCK_DOOR, OUTPUT);

    pinMode(RELAY, OUTPUT);
}

void PowerOnRelay()
{
    digitalWrite(RELAY, HIGH);
}

void LockDoor()
{
    digitalWrite(LOCK_DOOR, HIGH);
}

void UnlockDoor()
{
    digitalWrite(LOCK_DOOR, LOW);
}

byte ReadProgram()
{
    //Table of truth:
    //  PS1|PS2|PS3|PS4|PG Num
    //   0 | 0 | 0 | 0 |  P0  
    //   0 | 0 | 0 | 1 |  P1
    //   0 | 0 | 1 | 0 |  P2
    //   0 | 0 | 1 | 1 |  P3
    //   0 | 1 | 0 | 0 |  P4
    //   0 | 1 | 0 | 1 |  P5
    //   0 | 1 | 1 | 0 |  P6
    //   0 | 1 | 1 | 1 |  P7
    //   1 | 0 | 0 | 0 |  P8
    //   1 | 0 | 0 | 1 |  P9
    //   1 | 0 | 1 | 0 |  P10
    //   1 | 0 | 1 | 1 |  P11
    //   1 | 1 | 0 | 0 |  P12
    //   1 | 1 | 0 | 1 |  P13
    //   1 | 1 | 1 | 0 |  P14
    //   1 | 1 | 1 | 1 |  P15

    int PS1 = digitalRead(PROG_SELECT_1);
    int PS2 = digitalRead(PROG_SELECT_2);
    int PS3 = digitalRead(PROG_SELECT_3);
    int PS4 = digitalRead(PROG_SELECT_4);

    if(PS1 == 0 && PS2 == 0 && PS3 == 0 && PS4 == 0) return 0;
    if(PS1 == 0 && PS2 == 0 && PS3 == 0 && PS4 == 1) return 1;
    if(PS1 == 0 && PS2 == 0 && PS3 == 1 && PS4 == 0) return 2;
    if(PS1 == 0 && PS2 == 0 && PS3 == 1 && PS4 == 1) return 3;
    if(PS1 == 0 && PS2 == 1 && PS3 == 0 && PS4 == 0) return 4;
    if(PS1 == 0 && PS2 == 1 && PS3 == 0 && PS4 == 1) return 5;
    if(PS1 == 0 && PS2 == 1 && PS3 == 1 && PS4 == 0) return 6;
    if(PS1 == 0 && PS2 == 1 && PS3 == 1 && PS4 == 1) return 7;
    if(PS1 == 1 && PS2 == 0 && PS3 == 0 && PS4 == 0) return 8;
    if(PS1 == 1 && PS2 == 0 && PS3 == 0 && PS4 == 1) return 9;
    if(PS1 == 1 && PS2 == 0 && PS3 == 1 && PS4 == 0) return 10;
    if(PS1 == 1 && PS2 == 0 && PS3 == 1 && PS4 == 1) return 11;
    if(PS1 == 1 && PS2 == 1 && PS3 == 0 && PS4 == 0) return 12;
    if(PS1 == 1 && PS2 == 1 && PS3 == 0 && PS4 == 1) return 13;
    if(PS1 == 1 && PS2 == 1 && PS3 == 1 && PS4 == 0) return 14;
    if(PS1 == 1 && PS2 == 1 && PS3 == 1 && PS4 == 1) return 15;
    else                                             return 0;
} 

void FulfillProgram(byte selectedProgram)
{
    switch (selectedProgram)
    {
        //Standard washing program
        case 1: 
        {
            FillWater(HALF_FILLED_WATER);
            HeatWater(TEMP_LEVEL_MED);

            Wash(DEFAULT_MOTOR_CYCLES, MAX_MOTOR_SPEED);
            Centrifuge(DEFAULT_MOTOR_CYCLES, MAX_MOTOR_SPEED);

            break;
        }
        //Just Centrifuge
        case 2:
        {
            Centrifuge(100, MAX_MOTOR_SPEED);
            break;
        }
        //Quick Program
        case 3:
        {
            FillWater(HALF_FILLED_WATER);

            Wash(10, MAX_MOTOR_SPEED);
            Centrifuge(10, MAX_MOTOR_SPEED);

            break;
        }
        //Double Washing
        case 4:
        {
            FillWater(FULLY_FILLED_WATER);
            HeatWater(MEDIUMLY_HEATED_WATER);
            Wash(100, MEDIUM_MOTOR_SPEED);
            Centrifuge(100, MEDIUM_MOTOR_SPEED);

            delay(5000);

            FillWater();
            Wash(100, MEDIUM_MOTOR_SPEED);
            Centrifuge(100, MEDIUM_MOTOR_SPEED);

            TurnPumpON();
            delay(2000);
            TurnPumpOFF();

            break;
        }
        //Gental Spin
        case 5:
        {
            FillWater(HALF_FILLED_WATER);
            Wash(150, LOW_MOTOR_SPEED);
            Centrifuge(150, LOW_MOTOR_SPEED);

            break;
        }
        //Wool
        case 6:
        {
            FillWater(FULLY_FILLED_WATER);
            Wash(250, MAX_MOTOR_SPEED);
            Centrifuge(100, MAX_MOTOR_SPEED);

            break;
        }
        //Sport Wash
        case 7:
        {
            FillWater(FULLY_FILLED_WATER);
            Wash(250, MAX_MOTOR_SPEED);

            TurnPumpON();
            delay(10000);
            TurnPumpOFF();
        }
        default:
        {
                //If P0 is selected:
                //  Wait for user to reselect a prorgam and then read it
                //  Start washing with the selected program
                delay(5000);
                selectedProgram = ReadProgram();
        }
    }
}

void FillWater(byte level)
{
    digitalWrite(WATER_VALVE, HIGH);

    while (1)
    {
        bool filled = ReadWaterLevel(level);

        if (filled == true)
        {
            digitalWrite(WATER_VALVE, LOW);
            break;
        }
    }
}

bool ReadWaterLevel(byte level)
{
    int waterLevel = 0;

    if (level == EMPTY_WATER)
        waterLevel = digitalRead(WATER_LEVEL_EMP);
    else if (level == WATER_LEVEL_1)
        waterLevel = digitalRead(WATER_LEVEL_1);
    else if (level == WATER_LEVEL_2)
        waterLevel = digitalRead(WATER_LEVEL_2);

    return waterLevel == 1 
           ? true : false;
}

void HeatWater(byte level)
{
    digitalWrite(WATER_HEATER, HIGH);
    
    while (1)
    {
        bool heated = ReadHeatLevel(level);

        if (heated == true)
        {
            digitalWrite(WATER_HEATER, LOW);
            break;
        }
    }
}

bool ReadHeatLevel(byte level)
{
    int heatLevel = 0;

    if (level == LIGHTLY_HEATED_WATER)
        heatLevel = digitalRead(TEMP_LEVEL_LOW);
    else if (level == MEDIUMLY_HEATED_WATER)
        heatLevel = digitalRead(TEMP_LEVEL_MED);
    else if (level == TEMP_LEVEL_HIGH)
        heatLevel = digitalRead(TEMP_LEVEL_HIGH);

    return heatLevel == 1 
           ? true : false;
}

void SpinMotor(unsigned int cycles, byte speed)
{   
    for (byte i = 0; i < cycles; i++)
    {
        //Spin forward
        digitalWrite(MOTOR_F_R, HIGH); //Select Forward
        analogWrite(MOTOR_PWM, speed); //Define Speed

        delay(1500);

        //Spin backwards
        digitalWrite(MOTOR_F_R, LOW ); //Select Backwards 
        analogWrite(MOTOR_PWM, speed); //Define Speed
    }
}

void Wash(unsigned int cycles, byte speed)
{   
    SpinMotor(cycles, speed);
}

void Centrifuge(unsigned int cycles, byte speed)
{
    TurnPumpON();

    SpinMotor(cycles, speed);

    TurnPumpOFF();
}

void TurnPumpON()
{
    digitalWrite(WATER_PUMP, HIGH);
}

void TurnPumpOFF()
{
    digitalWrite(WATER_PUMP, HIGH);
}
