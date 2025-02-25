#include <stdint.h>
#include <stdbool.h>

#include "inc/msp432p401r.h"

#define SW1     0x02 // on the left side of the LaunchPad board
#define SW2     0x10 // on the right side of the LaunchPad board
#define RED     0x01
#define GREEN   0x02
#define BLUE    0x04

#define LED1 P1->OUT
#define LED2 P2->OUT
#define SW1_PIN P1->IN
#define SW2_PIN P1->IN

// Version 1
// initialize P1.1 and P1.4 and make them inputs (P1.1 and P1.4 built-in buttons)
// initialize P1.0 as output to red LED
void Port1Init(void) {
    P1->SEL0 = 0x00;
    P1->SEL1 = 0x00;    // configure P1.4 and P1.1 as GPIO
    P1->DIR = 0x01;     // make P1.4 and P1.1 in, P1.0 output
    P1->REN = 0x12;     // enable pull resistors on P1.4 and P1.1
    P1->OUT = 0x12;     // P1.4 and P1.1 are pull-up
}

// read P1.4, P1.1 inputs
uint8_t Port1Input(void) {
    return P1->IN & (SW1 | SW2);
}

// write output to P1.0
void Port1Output(uint8_t data) {  
    P1->OUT = (P1->OUT & ~0x01) | data;
}

// initialize P2.2-P2.0 and make them outputs (P2.2-P2.0 built-in LEDs)
void Port2Init(void) {
    P2->SEL0 = 0x00;
    P2->SEL1 = 0x00;    // configure P2.2-P2.0 as GPIO
    P2->DS = 0x07;      // make P2.2-P2.0 high drive strength
    P2->DIR = 0x07;     // make P2.2-P2.0 out
    P2->OUT = 0x00;     // all LEDs off
}
  
int main1(void) {
    Port1Init(); 
    Port2Init();
    while (true) {
        // switches are negative logic on P1.1 and P1.4
        uint8_t switches = Port1Input() ^ (SW1 | SW2);
        switch (switches) {
            case SW1:
                P2->OUT = BLUE;
                Port1Output(1);
                break;
            case SW2:
                P2->OUT = RED;
                Port1Output(1);
                break;
            case SW1 | SW2:
                P2->OUT = BLUE | RED;
                Port1Output(1);
                break;
            default: // neither switch pressed
                P2->OUT = 0;
                Port1Output(0);
        }
    }
}

// Version 2
void Port1InitFriendly(void) {
    P1->SEL0 &= ~0x13;
    P1->SEL1 &= ~0x13;  // 1) configure P1.4 P1.1 P1.0 as GPIO
    P1->DIR &= ~0x12;   // 2) make P1.4 and P1.1 in
    P1->DIR |= 0x01;    // 2) make P1.0 out
    P1->REN |= 0x12;    // 3) enable pull resistors on P1.4 and P1.1
}

void Port2InitFriendly(void) {
    P2->SEL0 &= ~0x07;
    P2->SEL1 &= ~0x07;  // 1) configure P2.2-P2.0 as GPIO
    P2->DIR |= 0x07;    // 2) make P2.2-P2.0 out
    P2->DS |= 0x07;     // 3) activate increased drive strength
    P2->OUT &= ~0x07;   //    all LEDs off
}

// write three outputs bits of P2
void Port2Output(uint8_t data) {  
    P2->OUT = (P2->OUT & ~0x07) | data;
}

int main2(void) {
    Port1InitFriendly();
    Port2InitFriendly();
    while (true) {
        // switches are negative logic on P1.1 and P1.4
        uint8_t switches = Port1Input() ^ (SW1 | SW2);
        switch (switches) {
            case SW1:
                Port2Output(BLUE);
                Port1Output(1);
                break;
            case SW2:
                Port2Output(RED);
                Port1Output(1);
                break;
            case SW1 | SW2:
                Port2Output(BLUE | RED);
                Port1Output(1);
                break;
            default: // neither switch is pressed
                Port2Output(0);
                Port1Output(0);
        }
    }
}

// Version 3
void SwitchInit(void) {
    P1->SEL0 &= ~0x12;
    P1->SEL1 &= ~0x12;    // 1) configure P1.4 and P1.1 as GPIO
    P1->DIR &= ~0x12;     // 2) make P1.4 and P1.1 in
    P1->REN |= 0x12;      // 3) enable pull resistors on P1.4 and P1.1
    P1->OUT |= 0x12;      //    P1.4 and P1.1 are pull-up
}

// bit-banded addresses, positive logic
#define SW2IN ((*((volatile uint8_t *)(0x42098010)))^1)
#define SW1IN ((*((volatile uint8_t *)(0x42098004)))^1)

void RedLEDInit(void) {
    P1->SEL0 &= ~0x01;
    P1->SEL1 &= ~0x01;   // 1) configure P1.0 as GPIO
    P1->DIR |= 0x01;     // 2) make P1.0 out
}

// bit-banded address
#define REDLED (*((volatile uint8_t *)(0x42098040)))

void ColorLEDInit(void) {
    P2->SEL0 &= ~0x07;
    P2->SEL1 &= ~0x07;    // 1) configure P2.2-P2.0 as GPIO
    P2->DIR |= 0x07;      // 2) make P2.2-P2.0 out
    P2->DS |= 0x07;       // 3) activate increased drive strength
    P2->OUT &= ~0x07;     //    all LEDs off
}

// bit-banded addresses
#define BLUEOUT  (*((volatile uint8_t *)(0x42098068)))
#define GREENOUT (*((volatile uint8_t *)(0x42098064)))
#define REDOUT   (*((volatile uint8_t *)(0x42098060)))

int main3(void) {
    SwitchInit();
    ColorLEDInit();
    RedLEDInit();
    GREENOUT = 0;
    while (true) {
        if (SW1IN || SW2IN) {
            REDLED = 1;
        } else {
            REDLED = 0;
        }
        if (SW1IN) {
            BLUEOUT = 1;
        } else {
            BLUEOUT = 0;
        }
        if (SW2IN) {
            REDOUT = 1;
        } else {
            REDOUT = 0;
        }
    }
}

// Your solution here
int main(void) {
    
    while (true) {
            // Stop watchdog timer
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;

    // Set LED1 (P1.0) as output
    P1->DIR |= RED;
    // Set LED2 (P2.0, P2.1, P2.2) as output
    P2->DIR |= RED | GREEN | BLUE;
    // Set SW1 (P1.1) and SW2 (P1.4) as input
    P1->DIR &= ~(SW1 | SW2);
    // Enable pull-up resistors for switches
    P1->REN |= SW1 | SW2;
    P1->OUT |= SW1 | SW2;

    while (1) {
        // Read switch states
        bool sw1_pressed = !(SW1_PIN & SW1);
        bool sw2_pressed = !(SW2_PIN & SW2);

        // Control LED1
        if (sw1_pressed ^ sw2_pressed) {
            LED1 |= RED; // LED1 ON
        } else {
            LED1 &= ~RED; // LED1 OFF
        }

        // Control LED2
        if (sw1_pressed && sw2_pressed) {
            LED2 |= GREEN; // LED2 GREEN ON
            LED2 |= BLUE;  // LED2 BLUE ON
            LED2 &= ~RED;  // LED2 RED OFF
        } else if (sw1_pressed) {
            LED2 |= GREEN; // LED2 GREEN ON
            LED2 &= ~BLUE; // LED2 BLUE OFF
            LED2 &= ~RED;  // LED2 RED OFF
        } else if (sw2_pressed) {
            LED2 &= ~GREEN; // LED2 GREEN OFF
            LED2 |= BLUE;   // LED2 BLUE ON
            LED2 &= ~RED;   // LED2 RED OFF
        } else {
            LED2 &= ~GREEN; // LED2 GREEN OFF
            LED2 &= ~BLUE;  // LED2 BLUE OFF
            LED2 |= RED;    // LED2 RED ON
        }
    }
 }
}