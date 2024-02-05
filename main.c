#include <msp430.h>
#include "peripherals.h"
#include <math.h>

void swDelay(char numLoops);

void BuzzerOn2(int period);

void configUserLED2(char);

void setup();

int pressed();

void configUserLED(char inbits);

struct Notes{
    int pitch;//period for buzzer2 function
    int duration;//for delay
    unsigned char LED;//mapped to which led
};

void countdown(void);

void play_note(struct Notes, int);

int play_song(struct Notes[]);

void startTimerA2(void);

void stopTimerA2(void);

int point = 0;

unsigned int leap_cnt, timer;

void main(void){
    int score;

    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
        // You can then configure it properly, if desired

    _BIS_SR(GIE);   //global interrupt enable

    // Configure User LEDs as outputs
    // Red     P1.0
    // Green   P4.7
    P1SEL &= ~BIT0;
    P1DIR |= BIT0;

    P4SEL &= ~BIT7;
    P4DIR |= BIT7;

    P1OUT &=~(BIT0);
    P4OUT &= ~(BIT7);
    // Done

    struct Notes noteA;
    noteA.pitch = 440;
    noteA.duration = 100;
    noteA.LED = 0x08;

    struct Notes noteBflat;
    noteBflat.pitch = 466;
    noteBflat.duration = 100;
    noteBflat.LED = 0x08;

    struct Notes noteB;
    noteB.pitch = 494;
    noteB.duration = 100;
    noteB.LED = 0x08;

    struct Notes noteC;
    noteC.pitch = 523;
    noteC.duration = 100;
    noteC.LED = 0x08;

    struct Notes noteCsharp;
    noteCsharp.pitch = 554;
    noteCsharp.duration =100;
    noteCsharp.LED = 0x04;

    struct Notes noteD;
    noteD.pitch = 587;
    noteD.duration = 100;
    noteD.LED = 0x04;

    struct Notes noteEb;
    noteEb.pitch = 622;
    noteEb.duration = 100;
    noteEb.LED = 0x02;

    struct Notes noteE;
    noteE.pitch = 659;
    noteE.duration = 100;
    noteE.LED = 0x02;

    struct Notes noteF;
    noteF.pitch = 698;
    noteF.duration = 100;
    noteF.LED = 0x01;

    struct Notes noteFsharp;
    noteFsharp.pitch = 740;
    noteFsharp.duration = 100;
    noteFsharp.LED = 0x01;

    struct Notes noteG;
    noteG.pitch = 784;
    noteG.duration = 100;
    noteG.LED = 0x01;

    struct Notes noteAflat;
    noteAflat.pitch = 831;
    noteAflat.duration = 100;
    noteAflat.LED = 0x01;

    struct Notes sotw[28] = {noteC, noteD, noteEb, noteC, noteD, noteF, noteEb, noteC, noteD, noteEb, noteD, noteC, noteC, noteD, noteEb, noteC, noteD, noteF, noteEb, noteC, noteD, noteEb, noteD, noteC, noteC, noteD, noteEb, noteC};
    struct Notes tbm[28] = {noteE, noteD, noteC, noteE, noteD, noteC, noteF, noteE, noteD,noteF, noteE, noteD, noteE, noteD, noteC, noteE, noteD, noteC, noteF, noteE, noteD,noteF, noteE, noteD, noteE, noteD, noteC, noteE};

    setup();    //buttons

    unsigned char currKey=0;

    initLeds(); //game LEDs

    configDisplay();
    configKeypad();

    int state = 0;

    while (1)    // Forever loop
    {
        switch (state){
        case 0:

            Graphics_drawStringCentered(&g_sContext, "Welcome", AUTO_STRING_LENGTH, 48, 25, OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "MSP430 Hero", AUTO_STRING_LENGTH, 48, 35, OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "Start press *", AUTO_STRING_LENGTH, 48, 45, OPAQUE_TEXT);
            Graphics_drawStringCentered(&g_sContext, "Reset press #", AUTO_STRING_LENGTH, 48, 55, OPAQUE_TEXT);
            Graphics_flushBuffer(&g_sContext);
            currKey = 0;
            char songnum;
            while(currKey == 0){
                currKey = getKey();
            }
            if (currKey == '*'){
                Graphics_clearDisplay(&g_sContext);
                Graphics_flushBuffer(&g_sContext);
                //song menu
                Graphics_drawStringCentered(&g_sContext, "Press Keypad", AUTO_STRING_LENGTH, 48, 15, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "1. Smoke on", AUTO_STRING_LENGTH, 48, 25, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "the Water", AUTO_STRING_LENGTH, 48, 35, OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "2. Three", AUTO_STRING_LENGTH, 48, 55, OPAQUE_TEXT);
                Graphics_drawStringCentered(&g_sContext, "Blind Mice", AUTO_STRING_LENGTH, 48, 65, OPAQUE_TEXT);
                Graphics_flushBuffer(&g_sContext);

                currKey = 0;

                while(currKey == 0){
                    currKey = getKey();
                }
                Graphics_clearDisplay(&g_sContext);
                Graphics_flushBuffer(&g_sContext);

                if (currKey == '#'){
                    state = 0;
                    break;
                }

                songnum = currKey;
                countdown();    //3, 2, 1
                state = 1;
            }
            break;

        case 1:
            //smoke
            if(songnum =='1'){
                score = play_song(sotw);
            }
            //mice
            else if(songnum =='2'){
                score = play_song(tbm);
            }

            if (score < 18){    //must score 18 points to win
                Graphics_drawStringCentered(&g_sContext, "LOSER", AUTO_STRING_LENGTH, 48, 25, OPAQUE_TEXT); //humiliation
                Graphics_flushBuffer(&g_sContext);
            }

            else {
                Graphics_drawStringCentered(&g_sContext, "WINNER", AUTO_STRING_LENGTH, 48, 25, OPAQUE_TEXT); //celebration
                Graphics_flushBuffer(&g_sContext);
            }

            state = 0;  // end game, back to welcome
        }
    }
}

void startTimerA2(void)
{
    TA2CTL = TASSEL_1| MC_1 | ID_0;
    TA2CCR0 = 0xA3;      //163
    TA2CCTL0 = CCIE;     // enable Timer A2 interrupt
}

void stopTimerA2(void)
{
    TA2CTL = MC_0;
    TA2CCTL0 &= ~CCIE;     // disable Timer A2 interrupt
    timer = 0;
}

#pragma vector=TIMER2_A0_VECTOR
__interrupt void Timer_A2_ISR(void){
    if(leap_cnt < 1024){
        timer++;
        leap_cnt++;
    }else{                  //not needed for this short application, but helps negate error
        timer+=2;
        leap_cnt = 0;
    }
}

void swDelay(char numLoops){

    volatile unsigned int i,j;      // volatile to prevent removal in optimization
    for (j=0; j<numLoops; j++)
    {
        i = 50000 ;                 // SW Delay
        while (i > 0)               // could also have used while (i)
            i--;
    }
}

void setup(){ //buttons
    P7SEL &= ~ (BIT4|BIT0);     //bit 4 and 0 are 0
    P3SEL &= ~ (BIT6);          //bit 6 is 0
    P2SEL &= ~ (BIT2);          //bit 2 is 0

    P7DIR &= ~ (BIT4|BIT0);
    P3DIR &= ~ (BIT6);
    P2DIR &= ~ (BIT2);

    P7REN |= (BIT4|BIT0);
    P3REN |= (BIT6);
    P2REN |= (BIT2);

    P7OUT |= (BIT4|BIT0);
    P3OUT |= (BIT6);
    P2OUT |= (BIT2);
}

int pressed(){ //buttons
    int on = 0x00;
    int S1 = P7IN & BIT0;
    int S2 = P3IN & BIT6;
    int S3 = P2IN & BIT2;
    int S4 = P7IN & BIT4;

    if (S1 == 0){
        on = (on|BIT3);
    }
    if (S2 == 0){
        on = (on|BIT2);
    }
    if (S3 == 0){
        on = (on|BIT1);
    }
    if (S4 == 0){
        on = (on|BIT0);
    }
    return on;
}

void configUserLED(char inbits){    //game LEDs
    char mask = 0;

    if (inbits && BIT0){
        mask |= BIT4;
    }

    if (inbits && BIT1){
        mask |= BIT3;
    }

    if (inbits && BIT2){
        mask |= BIT1;
    }

    if (inbits && BIT3){
        mask |= BIT2;
    }
    P6OUT|= mask;
}

void BuzzerOn2(int freq)
{
    // Initialize PWM output on P3.5, which corresponds to TB0.5
    P3SEL |= BIT5; // Select peripheral output mode for P3.5
    P3DIR |= BIT5;

    TB0CTL  = (TBSSEL__ACLK|ID__1|MC__UP);  // Configure Timer B0 to use ACLK, divide by 1, up mode
    TB0CTL  &= ~TBIE;                       // Explicitly Disable timer interrupts for safety

    // Now configure the timer period, which controls the PWM period
    // Doing this with a hard coded values is NOT the best method
    // We do it here only as an example. You will fix this in Lab 2.
    int rate = 32768/freq;
    TB0CCR0   = rate;                   // Set the PWM period in ACLK ticks
    TB0CCTL0 &= ~CCIE;                  // Disable timer interrupts

    // Configure CC register 5, which is connected to our PWM pin TB0.5
    TB0CCTL5  = OUTMOD_7;                   // Set/reset mode for PWM
    TB0CCTL5 &= ~CCIE;                      // Disable capture/compare interrupts
    TB0CCR5   = TB0CCR0/2;                  // Configure a 50% duty cycle
}

void countdown(void)
{
    Graphics_clearDisplay(&g_sContext); // Clear the display
    int t = 0;
    while (t <= 800){
        startTimerA2();
        t = timer;
        if (timer < 200){
            Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 25, OPAQUE_TEXT);
            setLeds(1);
        }
        if (400>timer && timer>= 200){
            Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 35, OPAQUE_TEXT);
            setLeds(2);
        }
        if (600>timer && timer>= 400){
            Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 45, OPAQUE_TEXT);
            setLeds(4);
        }
        if (timer>= 600){
            Graphics_drawStringCentered(&g_sContext, "Go", AUTO_STRING_LENGTH, 48, 55, TRANSPARENT_TEXT);
            setLeds(15);
            BuzzerOn2(440);
        }
        Graphics_flushBuffer(&g_sContext);
    }
    BuzzerOff();
    stopTimerA2();
    Graphics_clearDisplay(&g_sContext);
    setLeds(0);
}

void play_note(struct Notes note, int passed){

    while((timer - passed) < note.duration){    //time for this note only
        BuzzerOn2(note.pitch);
        setLeds(note.LED);
    }
    BuzzerOff();
}

int play_song(struct Notes song[]){
    int i = 0;
    int point = 0;
    int state;
    char currKey;
    char button;
    int current_duration = 0;
    startTimerA2();

    for(i=0; i<28; i++){
        current_duration += song[i].duration;

        unsigned char currLED = song[i].LED;

        while ((timer <= (current_duration))){
            play_note(song[i], current_duration);

            button = pressed();

            if(button == currLED){
                point++;
                configUserLED2(2);//turn on green user led

            }else if(button != currLED){
                configUserLED2(1); //turn on red user led

            }
            button = 0;
        }

        currKey = getKey();
        if (currKey == '#'){
            state = 0;
            stopTimerA2();
            break;
        }
    }
    configUserLED2(0);
    stopTimerA2();

    //after game point display
    int p1, p2;
    char total_points[2];

    if(point>=10){
        p1 = floor(point/10);
        p2 = point%10;
        total_points[0]= p1 + 48;
        total_points[1]=  p2 + 48;
    }
    else{
        total_points[0]= point + 0x30;
    }

    Graphics_drawStringCentered(&g_sContext, "you scored ", AUTO_STRING_LENGTH, 48, 45, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, total_points, AUTO_STRING_LENGTH, 48, 55, OPAQUE_TEXT);
    Graphics_drawStringCentered(&g_sContext, " points", AUTO_STRING_LENGTH, 48, 65, OPAQUE_TEXT);
    Graphics_flushBuffer(&g_sContext);
    swDelay(5);
    Graphics_clearDisplay(&g_sContext);
    state = 0;
    return point;
}

void configUserLED2(char inbits){ //red and green LED's for right and wrong
    inbits &= 0x0003;

    if (inbits == 0x0000 || inbits == 0x0002){
        P1OUT &= ~BIT0; //LED1 is off
    }
    else if (inbits== 0x0001 || inbits == 0x0003){
        P1OUT |= BIT0;  //LED1 is lit
    }
    if (inbits == 0x0000 || inbits == 0x0001){
        P4OUT &= ~BIT7; //LED2 is off
    }
    else if (inbits== 0x0002 || inbits == 0x0003){
        P4OUT |= BIT7;  //LED2 is lit
    }
}
