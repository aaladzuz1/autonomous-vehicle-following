#include <xc.h>
#pragma config FOSC = HS, WDTE = OFF, PWRTE = OFF, MCLRE = ON, CP = OFF, CPD = OFF, BOREN = OFF, CLKOUTEN = OFF
#pragma config IESO = OFF, FCMEN = OFF, WRT = OFF, VCAPEN = OFF, PLLEN = OFF, STVREN = OFF, LVP = OFF
#define _XTAL_FREQ 8000000
// Konstante
#define PWM_MAX 255              // Max PWM duty cycle
#define DESIRED_DISTANCE 40     // Zeljena udaljenost od vozila (0.4 m)
#define DISTANCE_THRESHOLD 10   // Prag
#define MAX_DISTANCE 130        // Max udaljenost od senzora (1.3 m)
#define MIN_DISTANCE 20
// Prototipovi funkcija
void PWM_setup();
void Speed(int speed);
void Direct_Movement(int distance);
void Turn_Right(int distance);
void Turn_Left(int distance);
// Inicijalizacija AD konverzije
void setupAD() {
    TRISA = 0xEF;      
    ANSELA = 0xFF;                                        
    ADCON1bits.ADFM = 0;
    ADCON1bits.ADCS2 = 1;
    ADCON1bits.ADCS1 = 1;
    ADCON1bits.ADCS0 = 1;
    ADCON1bits.ADNREF = 0;
    ADCON1bits.ADPREF1 = 0;
    ADCON1bits.ADPREF0 = 0;
}
void setup() {
    TRISD = 0x00; // PORTD as output    
    ANSELD = 0x00;
    PORTD = 0x00;
    TRISC = 0x00; // PORTC as output    
    PORTC = 0x00;
}
// Citanje vrijednosti sa senzora
int Read_Value_From_Sensor(int channel)
{
    if (channel == 0) {
        ADCON0bits.ADON = 1;
        ADCON0bits.CHS4 = 0;
        ADCON0bits.CHS3 = 0;
        ADCON0bits.CHS2 = 0;
        ADCON0bits.CHS1 = 0;
        ADCON0bits.CHS0 = 0;
    }
    else {
        ADCON0bits.ADON = 1;
        ADCON0bits.CHS4 = 0;
        ADCON0bits.CHS3 = 0;
        ADCON0bits.CHS2 = 0;
        ADCON0bits.CHS1 = 0;
        ADCON0bits.CHS0 = 1;
    }
    ADCON0bits.ADGO = 1;
    while (ADCON0bits.ADGO);
    unsigned int volt = ADRESH;
    int ones = (volt * 5.0 / 255);
    int decimals = (int)(100 * (5.0 * volt/ 255 - ones));
    int Voltage = ones * 1000 + decimals * 10; //u mV
    return Voltage;
}
// Proracun udaljenosti
int Calculate_Distance(int Voltage, int sensor_channel)
{
    int distance = Voltage;
    if(sensor_channel == 1) {
    // lijevi senzor
        distance =  -0.2539 * 100 * Voltage*Voltage*Voltage /1000000000 + 1.48 * 100 * Voltage*Voltage /1000000 -2.915 * 100* Voltage /1000 + 2.258 * 100;
    }
    else {
    // desni senzor
        distance = -0.2502 * 100 * Voltage*Voltage*Voltage /1000000000 + 1.414 * 100 * Voltage*Voltage /1000000 -2.738 * 100 * Voltage/1000 + 2.127 * 100;
    }
    return distance;
}

void main() {
    setupAD();
    setup();
    PWM_setup();
    while (1) {
        int sensor1Channel = 0; // Lijevi senzor
        int sensor2Channel = 1; // desni senzor
        // Uzimanje vrijednosti napona sa senzora
        int Voltage_Sensor1 = Read_Value_From_Sensor(sensor1Channel);
        __delay_ms(10);
        int Voltage_Sensor2 = Read_Value_From_Sensor(sensor2Channel);
		// Racunanje udaljenosti 
        int distance_Sensor1 = Calculate_Distance(Voltage_Sensor1, 1); 
        int distance_Sensor2 = Calculate_Distance(Voltage_Sensor2, 2);
        // Podesavanje brzine i smjera rotacije motora
        if (distance_Sensor2 > distance_Sensor1 + DISTANCE_THRESHOLD) {         // skreni lijevo 
            Turn_Left(distance_Sensor1);
        } 
        else if (distance_Sensor2 < distance_Sensor1 - DISTANCE_THRESHOLD) {    // skreni desno
            Turn_Right(distance_Sensor2);
        } 
        else if ( (distance_Sensor2 > DESIRED_DISTANCE + DISTANCE_THRESHOLD && distance_Sensor1 > DESIRED_DISTANCE + DISTANCE_THRESHOLD) || 
                  (distance_Sensor2 < DESIRED_DISTANCE - DISTANCE_THRESHOLD && distance_Sensor1 < DESIRED_DISTANCE - DISTANCE_THRESHOLD) ) {  
            Direct_Movement((distance_Sensor1 + distance_Sensor2)/2); //prilagodi brzinu ako je van zeljene udaljenosti
        }
		else if (distance_Sensor2 < MIN_DISTANCE && distance_Sensor1 < MIN_DISTANCE) {
            Speed(0);
        }
        else {
            PORTD = 0x30;
         }
    }
    return;
}

void PWM_setup()
{
    PR2 = 0xFF;
    CCP2M3 = 1;
    CCP2M2 = 1;
    // PWM duty cycle
    CCPR2L = 0;
    // Timer2
    T2CKPS0 = 1; // prescaler 16
    T2CKPS1 = 1;
    TMR2ON = 1;
}

void Direct_Movement(int distance)
{
    int speed = (PWM_MAX * distance) / MAX_DISTANCE;
    PORTDbits.RD4 = 1; 
    PORTDbits.RD5 = 1;

    // Postavi brzinu
    Speed(speed);
}

void Turn_Right(int distance)
{
    int speed = (PWM_MAX * distance) / MAX_DISTANCE;
    PORTDbits.RD4 = 0; 
    PORTDbits.RD5 = 1;

    Speed(speed);
}

void Turn_Left(int distance)
{
    int speed = (PWM_MAX * distance) / MAX_DISTANCE;
    PORTDbits.RD4 = 1; 
    PORTDbits.RD5 = 0;

    Speed(speed);
}

void Speed(int speed)
{
    // ogranicenje brzine
    if(speed > PWM_MAX)
        speed = PWM_MAX;
    else if(speed < 0)
        speed = 0;

    // PWM duty cycle
    CCPR2L = speed;
}
