// Lab4Fmain.c
// Runs on MSPM0
// Uses ST7735.c to display the clock
// patterns to the LCD.
//    16-bit color, 128 wide by 160 high LCD
// Mark McDermott, Daniel Valvano and Jonathan Valvano
// June 27, 2026

// Specify your hardware connections, feel free to change
// PB4 is squarewave output to speaker
// PA18 is mode select (on Launchpad)
// PA27 is left
// PA15 is right
// PA28 is up
// PA16 is down
// if alarm is sounding, any button will quiet the alarm

#include <ti/devices/msp/msp.h>
#include <stdio.h>
#include <stdlib.h>
#include "../inc/LaunchPad.h"
#include "../inc/Clock.h"
#include "../inc/SPI.h"
#include "../inc/ST7735.h"
#include "../inc/Timer.h"
#include "Lab3.h"
#include <math.h>
#include "../inc/ADC.h"
#include "../lib/ADC_aleena.h"
#include "../lib/Sound.h"
#include "../lib/ClockFace.h"   // provides: const unsigned short clock[6400]  (80x80 bitmap)
#include "../inc/UART.h"
#include "../lib/UART.h"
#include "../lib/FIFO.h"


// ----------------------------------------------------------------------------
// ----------------------       MAIN       ------------------------------------
// ----------------------------------------------------------------------------

//Function Declarations
void Speaker_Init(void);
void Switch_Init(void);
void LED_Init(void);
void LED_Out(uint32_t data);
void LED_Toggle(void);
void Clock_Display(void);
uint32_t Get_Button1_Press(void);
uint32_t Get_Button2_Press(void);
void Button_Screen_Logic(void);
void Clock_DisplayAnalog(void);
void Screen_Display(void);
void Draw_Set_Screen(char* title, uint32_t display_hours, uint32_t display_mins);


//Global Variables
volatile uint32_t ADC_Volume_raw = 0; //Pulls the live slidepot data
volatile uint32_t ADC_Value_Human = 0; //Sends to websocket
volatile uint32_t DisplayMode = 0; //digital = 0, analog = 1
volatile uint32_t Screen_sel = 0; //Which Screen display we are seeing on the LCD
volatile uint32_t DisplayUpdate = 1; // Start at 1 to draw the initial screen
volatile uint32_t CursorPos = 0; // Start at option 0 of main menu
volatile uint32_t AlarmBannerDrawn = 0; //Latch so the "ALARM!" banner paints exactly once per
// ring instead of repainting every main-loop pass while it's sounding

volatile uint32_t Btn1_Press_Count = 0; //debug
volatile uint32_t Btn2_Press_Count = 0; //debug

// Global Clock State
volatile uint8_t Mode = 0;   // 0 = 12-hour, 1 = 24-hour
volatile uint8_t Hour = 12;
volatile uint8_t Minute = 0;
volatile uint8_t Second = 0;

// Time-of-day globals, maintained by TIMG0_IRQHandler, read by main()
// NOTE: these are shared between the ISR (writer) and main() (reader).
// See critical-section handling in Clock_Display() below.
volatile uint32_t ms = 0;       // milliseconds, 0 to 999
volatile uint32_t Seconds = 0;  // 0 to 59
volatile uint32_t Minutes = 0;  // 0 to 59
volatile uint32_t Hours   = 12; // 0 to 23, starting time -- change as needed
volatile uint32_t NewTime = 1;  // flag: 1 means display needs updating

// Flag to trigger Wi-Fi transmission
volatile uint8_t Send_Flag = 0;

#define ESP8266_RST (1<<25)  // PA25
#define ESP8266_RDY    (1<<19)   // PB19 (RSLK2 boards may use PB9)

#define RDY_TIMEOUT_MS 30000
volatile uint8_t WifiReady = 0;

void Debug_OutString(const char *s){
  while(*s){
    UART_OutChar(*s);
    s++;
  }
}

void RESET_8266(void){
  // Initialize reset port on PA25 as GPIO Output
  IOMUX->SECCFG.PINCM[PA25INDEX] = 0x81;     // GPIO mode
  GPIOA->DOE31_0 |= ESP8266_RST;             // Enable output on PA25
  
  // Generate a reset signal to ESP8266 on Port PA25
  GPIOA->DOUTCLR31_0 = ESP8266_RST;          // Drive PA25 LOW (Active reset)
  Clock_Delay1ms(10);                        // Hold reset low for 10ms
  GPIOA->DOUTSET31_0 = ESP8266_RST;          // Drive PA25 HIGH (Release reset)
  
  // Give the ESP8266 time to initialize before asking if it is ready
  Clock_Delay1ms(500); 

  Debug_OutString("\n\rResetting the ESP8266\n\r");
}



uint8_t SETUP_WIFI(void) {
  uint32_t waited = 0;
  IOMUX->SECCFG.PINCM[PB19INDEX] = 0x00040081;
  Debug_OutString("\n\rWaiting for WiFi RDY\n\r");
  while((GPIOB->DIN31_0 & ESP8266_RDY) == 0){
    if(waited >= RDY_TIMEOUT_MS){
      Debug_OutString("\n\rRDY timeout\n\r");
      return 0;
    }
    Clock_Delay1ms(1);
    waited++;
  }
  Debug_OutString("\n\rRDY asserted\n\r");
  return 1;
}

void Input(void){
  // Student writes this
  //char ws_cmd;
  while(RxFifo1_Size() > 0){
    char c = RxFifo1_Get();
    UART_OutChar(c);              // debug UART only
    __disable_irq();
    switch(c){
      case '1': Mode ^= 1; break;
      case '2': Hours   = (Hours+1)%24; break;
      case '3': Hours   = (Hours==0)?23:Hours-1; break;
      case '4': Minutes = (Minutes+1)%60; break;
      case '5': Minutes = (Minutes==0)?59:Minutes-1; break;
      case '6': Seconds = (Seconds+1)%60; break;
      case '7': Seconds = (Seconds==0)?59:Seconds-1; break;
      default: break;             // ignores '\n' and '\r'
    }
    __enable_irq();
    NewTime = 1;
    DisplayUpdate = 1;
    Send_Flag = 1;   // echo new time to the web page right away
  }

    // if(RxFifo1_Size() > 0){ // if unread data in  FIFO
  //   char ws_cmd = RxFifo1_Get(); //get next character (oldest)
  //   UART1_OutChar(ws_cmd); //debug uart
  //   __disable_irq();
  //   uint8_t cmd_num = ws_cmd - '0';   // ASCII to int conversion
  
  //   // Command #1: Toggle MODE select 
  //   if(cmd_num == 0x1)  {  
  //       if (Mode == 0x1) Mode =0x0;
  //       else if (Mode == 0x0) Mode = 0x1;
  //   }

  //   // Command #2: Hour ++
  //   else if (cmd_num == 0x2) {
  //     Hour = (Hour + 1) % 24;
  //   }  

  //   // Command #3: Hour --
  //   else if (cmd_num == 0x3) {
  //     if (Hour == 0) Hour = 23;
  //     else Hour--;
  //   }

  //   // Command #4: Min ++
  //   else if (cmd_num == 0x4) {
  //     Minute = (Minute + 1) % 60;
  //   }

  //   // Command #5: Min --
  //   else if (cmd_num == 0x5) {
  //     if (Minute == 0) Minute = 59;
  //     else Minute--;
  //   }

  //   // Command #6: Sec ++
  //   else if (cmd_num == 0x6) {
  //     Second = (Second + 1) % 60;
  //   }

  //   // Command #7: Sec --
  //   else if (cmd_num == 0x7) {
  //     if (Second == 0) Second = 59;
  //     else Second--;
  //   }

  //   __enable_irq();
  //   NewTime = 1;
  // }
}


  int main0(void){ // main0 test of LCD
  __disable_irq(); 
  LaunchPad_Init();
  Clock_Init_HFXT_40_80MHz(0);  // 0.005% accurate running off external crystal oscillator

  ST7735_InitR(INITR_REDTAB); //INITR_REDTAB for AdaFruit, INITR_BLACKTAB for SPI HiLetgo ST7735R
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_SetCursor(0, 0);
  ST7735_OutString("Lab 3 \nLCD test main0\n");


  while(1){
    
  } 
} 



// Alarm globals
volatile uint32_t AlarmSeconds = 0;  // 0 to 59
volatile uint32_t AlarmMins = 0;  // 0 to 59
volatile uint32_t AlarmHours   = 0; // 0 to 23

volatile uint32_t AlarmArmed    = 0; //set to 1 when waiting to fire at AlarmHours:AlarmMins:AlarmSeconds and cleared by ISR when it fires
volatile uint32_t AlarmSounding = 0; // =1 when actively ringing and waiting for the user to press any button to dismiss it
  
//Digital Clock Timer Configuration
// TIMG0 is configured to interrupt every 1ms.
// BusFreq = 80MHz -> TimerG0 clock source is ULPCLK = 40MHz (see Timer.c comment)
// frequency = TimerClock/prescale/period = 40,000,000/40/1000 = 1000 Hz (1ms period)
#define TIMERG0_PRESCALE 40
#define TIMERG0_PERIOD   1000
#define TIMERG0_PRIORITY 2

// ----------------------------------------------------------------------------
// -----------------  Analog Clock Face Layout Constants  ---------------------
// ----------------------------------------------------------------------------
// Face bitmap is 80x80 (clock[] from ClockFace.h). Screen is 128 wide by 160 tall.
// Centered horizontally: x = (128-80)/2 = 24
// Placed near the top: top edge at pixel row 5
// ST7735_DrawBitmap takes the BOTTOM-LEFT corner, so y = top + height - 1
#define CLOCK_FACE_W       80
#define CLOCK_FACE_H       80
#define CLOCK_FACE_TOP     10
#define CLOCK_FACE_X       ((128 - CLOCK_FACE_W)/2)              // 24
#define CLOCK_FACE_Y       (CLOCK_FACE_TOP + CLOCK_FACE_H - 1)   // 84 (bottom-left y for DrawBitmap)
#define CLOCK_CENTER_X     (CLOCK_FACE_X + CLOCK_FACE_W/2)        // 64
#define CLOCK_CENTER_Y     (CLOCK_FACE_TOP + CLOCK_FACE_H/2)      // 45

#define HOUR_HAND_LEN      18
#define MIN_HAND_LEN       28
#define SEC_HAND_LEN       35

// Digital "HH:MM:SS" readout placed under the analog face.
// Face bottom pixel = 84, so start text at row 9 (pixel rows 90-99) for a small gap.
// 8 characters * 6px = 48px wide; column 7 (42px) centers it closely on the 128px screen.
#define DIGITAL_TEXT_ROW   9
#define DIGITAL_TEXT_COL   7


void TIMG0_IRQHandler(void){// runs every 1ms
  if((TIMG0->CPU_INT.IIDX) == 1){ // this will acknowledge
    //GPIOB->DOUTTGL31_0 = BLUE;  // profile PB22
    //GPIOB->DOUTTGL31_0 = BLUE;  // profile PB22

    ms++;                       // count milliseconds
    if(ms >= 1000){
      ms = 0;
      Send_Flag = 1; //broadcast new time to main (update)
      Seconds++;
      if(Seconds >= 60){
        Seconds = 0;
        Minutes++;
        if(Minutes >= 60){
          Minutes = 0;
          Hours++;
          if(Hours >= 24){
            Hours = 0;
          }
        }
      }
      NewTime = 1;              // flag main() to redraw the LCD

      if (AlarmArmed) {
          GPIOB->DOUTTGL31_0 = GREEN; 
      }

      //fires only if AlarmArmed == 1
      if(AlarmArmed && Hours==AlarmHours && Minutes==AlarmMins && Seconds==AlarmSeconds){
        AlarmSounding = 1; 
        AlarmArmed = 0;
        GPIOB->DOUTCLR31_0 = GREEN; // force off so not stuck on
    }

    

    //GPIOB->DOUTTGL31_0 = BLUE;  // profile PB22
    }
  }
}




int main(void){
  __disable_irq();

  // Disable and clear TIMG0 IRQ16 (clean slate before arming below)
  NVIC->ICER[0] = (1U << 16);
  NVIC->ICPR[0] = (1U << 16);

    LaunchPad_Init();
  Clock_Init_HFXT_40_80MHz(0);
  UART_Init();          // debug UART
  UART1_Init();         // ESP UART, now ready before any use
  ST7735_InitR(INITR_REDTAB);
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_SetCursor(0, 0);
  ST7735_OutString("Starting ESP...\n");
  RESET_8266();
  WifiReady = SETUP_WIFI();
  if(!WifiReady){
    ST7735_OutString("WiFi not ready\n");
    Clock_Delay1ms(2000);
  }
  ST7735_FillScreen(ST7735_BLACK);
  ST7735_FillScreen(ST7735_BLACK);   // clear status text
  Switch_Init();
  Speaker_Init();
  LED_Init();
  ADC_Init5();
  Sound_Disable();      // removed the extra Sound_Enable()

  TimerG0_IntArm(TIMERG0_PERIOD, TIMERG0_PRESCALE, TIMERG0_PRIORITY);
  __enable_irq();

  // ESP boot chatter may contain digits that look like commands, so discard it
  while(RxFifo1_Size() > 0){ (void)RxFifo1_Get(); }

  while(1){               // interrupts every 1ms

    Input();
    // Sample the slidepot: This value (0 to 4095) will beread by Sound ISR
    ADC_Volume_raw = ADC_In5();

    ADC_Value_Human = (ADC_Volume_raw * 100) / 4095; //calculates the human representation of volume from 0 to 100
    

    // Build CSV string to send to Web Application// Student writes this
    if (Send_Flag == 1) {
      char csv_string[40];
      uint32_t h, m, s;
      __disable_irq();
      h = Hours; m = Minutes; s = Seconds;
      __enable_irq();
      
      // Build CSV string: "Mode,Hour,Minute,Second\n"
      sprintf(csv_string, "%d,%lu,%lu,%lu,%lu\n",Mode, (unsigned long)h, (unsigned long)m,(unsigned long)s, (unsigned long)ADC_Value_Human);
      
      // Send to ESP8266 via UART1
      UART1_OutString(csv_string);
      
      Send_Flag = 0; 
    }

    

    //Button and Screen Select Logic
    Button_Screen_Logic();
    Screen_Display();

    if(NewTime){ //checks for update screen flag
      NewTime = 0;
      Clock_DisplayAnalog();  // draws analog face+hands, then "HH:MM:SS" underneath, every update
    }

    //self explanatory
    if(AlarmSounding){
      Sound_Enable();
    } else {
      Sound_Disable();
    }

    __WFI();
  }
}

// ----------------------------------------------------------------------------
// Clock_Display
// Formats and draws the current time as "HH:MM:SS" on the LCD.
// Hours/Minutes/Seconds are shared with TIMG0_IRQHandler (a writer), so this
// function disables interrupts just long enough to make an atomic local copy,
// removing the critical section that would otherwise exist if the ISR fired
// between reading Hours, Minutes, and Seconds separately.
// ----------------------------------------------------------------------------
void Clock_Display(void){
  uint32_t myH, myM, myS;
  char buffer[9]; // "HH:MM:SS" + null terminator

  __disable_irq();
  myH = Hours;
  myM = Minutes;
  myS = Seconds;
  __enable_irq();

  buffer[0] = '0' + (myH/10);
  buffer[1] = '0' + (myH%10);
  buffer[2] = ':';
  buffer[3] = '0' + (myM/10);
  buffer[4] = '0' + (myM%10);
  buffer[5] = ':';
  buffer[6] = '0' + (myS/10);
  buffer[7] = '0' + (myS%10);
  buffer[8] = 0;

  ST7735_SetCursor(0, 13);   // pick a row below the startup text
  ST7735_OutString(buffer);
}


// ----------------------------------------------------------------------------
// Clock_DisplayAnalog
// Redraws the 80x80 clock face bitmap (this also erases the previous hand
// positions, since the fresh bitmap overwrites them), then draws the hour,
// minute, and second hands as lines from the face center, then draws the
// digital "HH:MM:SS" readout underneath via Clock_Display().
// Hours/Minutes/Seconds are copied under a brief critical section for the
// same reason described in Clock_Display() above.
// ----------------------------------------------------------------------------
void Clock_DisplayAnalog(void){
  uint32_t myH, myM, myS;
  float angleHour, angleMin, angleSec;
  int32_t hx, hy, mx, my, sx, sy;

  __disable_irq();
  myH = Hours;
  myM = Minutes;
  myS = Seconds;
  __enable_irq();

  // Angle 0 points straight up (12 o'clock); angles increase clockwise.
  angleSec  = myS * (2.0f * 3.14159265f / 60.0f);
  angleMin  = (myM + myS/60.0f) * (2.0f * 3.14159265f / 60.0f);
  angleHour = ((myH % 12) + myM/60.0f) * (2.0f * 3.14159265f / 12.0f);

  hx = CLOCK_CENTER_X + (int32_t)(HOUR_HAND_LEN * sinf(angleHour));
  hy = CLOCK_CENTER_Y - (int32_t)(HOUR_HAND_LEN * cosf(angleHour));
  mx = CLOCK_CENTER_X + (int32_t)(MIN_HAND_LEN  * sinf(angleMin));
  my = CLOCK_CENTER_Y - (int32_t)(MIN_HAND_LEN  * cosf(angleMin));
  sx = CLOCK_CENTER_X + (int32_t)(SEC_HAND_LEN  * sinf(angleSec));
  sy = CLOCK_CENTER_Y - (int32_t)(SEC_HAND_LEN  * cosf(angleSec));

  if (Screen_sel == 1) {
    // Redraw the face fresh each update; this also erases the prior hands.
    ST7735_DrawBitmap(CLOCK_FACE_X, CLOCK_FACE_Y, clock, CLOCK_FACE_W, CLOCK_FACE_H);
    ST7735_DrawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, hx, hy, ST7735_RED);
    ST7735_DrawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, mx, my, ST7735_WHITE);
    ST7735_DrawLine(CLOCK_CENTER_X, CLOCK_CENTER_Y, sx, sy, ST7735_MAGENTA);
    Clock_Display();   // draw "HH:MM:SS" underneath the analog face
  }
}


/*

*/
void Screen_Display(void) {

  //if alarm is sounding it takes over the entire display, overriding what the screen was showing
  if (AlarmSounding) {
    if (!AlarmBannerDrawn) {
      AlarmBannerDrawn = 1;
      ST7735_FillScreen(ST7735_RED);
      ST7735_SetCursor(0, 30);
      ST7735_OutString("*** ALARM! ***\nPress any button\nto silence");
    }
    return; // don't draw the normal menu/clock/set screens while ringing
  } else {
    AlarmBannerDrawn = 0; // reset the latch so the banner reappears next ring
  }

  // if (Screen_sel == 1 && NewTime) {
  //   NewTime = 0;
  //   Clock_DisplayAnalog();
  // }


  if (DisplayUpdate) {
    DisplayUpdate = 0; // acknowledge
    //Screen_sel = 1; //for test 
    
    switch(Screen_sel) {

      case 0: // Main Menu
        ST7735_SetCursor(0, 0);
        ST7735_OutString("--- MAIN MENU ---\n\n");
        
        // Option 0 (Curser select Clock Display)
        ST7735_SetCursor(0, 2);
        if (CursorPos == 0) {
            ST7735_OutString("-> View Clock \n");
        } else {
            ST7735_OutString("   View Clock \n"); 
        }
        
        // Option 1 (Curser select Time Set)
        ST7735_SetCursor(0, 3);
        if (CursorPos == 1) {
            ST7735_OutString("-> Set Time   \n");
        } else {
            ST7735_OutString("   Set Time   \n");
        }
        
        // Option 1 (Curser select Alarm Set)
        ST7735_SetCursor(0, 4);
        if (CursorPos == 2) {
            ST7735_OutString("-> Set Alarm  \n");
        } else {
            ST7735_OutString("   Set Alarm  \n");
        }
        break;
        
      
      case 1: // Time Display
        ST7735_SetCursor(0, 0);
        ST7735_OutString("--- CLOCK ---    \n");
        if(NewTime){ //checks for update screen flag
          NewTime = 0;
          Clock_DisplayAnalog();  // draws analog face+hands, then "HH:MM:SS" underneath, every update
        }
        break;
      
      case 2: // Time Set
        Draw_Set_Screen("--- SET TIME --- \n\n", Hours, Minutes);
        break;

      case 3: // Alarm Set
        Draw_Set_Screen("--- SET ALARM ---\n\n", AlarmHours, AlarmMins);
        break;
    }
  }
}

// Returns 1 if Button 1 (PA18) is fully pressed and released, 0 else (software debounce)
uint32_t Get_Button1_Press(void) {
    if ((GPIOA->DIN31_0 & (1<<27)) != 0) {
        Clock_Delay1ms(10); // Wait 10*T to debounce the touch
        if ((GPIOA->DIN31_0 & (1<<27)) != 0) {
            while ((GPIOA->DIN31_0 & (1<<27)) != 0) {} // Wait for release
            Clock_Delay1ms(10); // Wait 10*T to debounce the release
            return 1;
        }
    }
    return 0;
}

// Returns 1 if Button 2 (PB21) is pressed and released, 0 else (software debounce)
uint32_t Get_Button2_Press(void) {
    if ((GPIOA->DIN31_0 & (1<<15)) != 0) {
        Clock_Delay1ms(10); // Wait 10*T to debounce the touch
        if ((GPIOA->DIN31_0 & (1<<15)) != 0) {
            while ((GPIOA->DIN31_0 & (1<<15)) != 0) {} // Wait for release
            Clock_Delay1ms(10); // Wait 10*T to debounce the release
            return 1;
        }
    }
    return 0;
}

void Draw_Set_Screen(char* title, uint32_t display_hours, uint32_t display_mins) {
    ST7735_SetCursor(0, 0);
    ST7735_OutString(title);
    
    // Set Hours
    ST7735_SetCursor(0, 2);
    if (CursorPos == 0) ST7735_OutString("-> Hrs: [");
    else ST7735_OutString("   Hrs:  ");
    
    ST7735_OutUDec(display_hours); 
    
    if (CursorPos == 0) ST7735_OutString("]   \n"); 
    else ST7735_OutString("    \n");

    // Set Minutes
    ST7735_SetCursor(0, 3);
    if (CursorPos == 1) ST7735_OutString("-> Min: [");
    else ST7735_OutString("   Min:  ");
    
    ST7735_OutUDec(display_mins); 
    
    if (CursorPos == 1) ST7735_OutString("]   \n"); 
    else ST7735_OutString("    \n");
    
    // Helpful button hints at the bottom
    ST7735_SetCursor(0, 10);
    ST7735_OutString("B1: Add  B2: Next");
}


volatile uint32_t btn1 = 0;
volatile uint32_t btn2 = 1;
void Button_Screen_Logic(void) {
  btn1 = Get_Button1_Press();
  btn2 = Get_Button2_Press();

  //pressing any button silences the alarm
  if (AlarmSounding) {
    if (btn1 == 1 || btn2 == 1) {
      AlarmSounding = 0;
      DisplayUpdate = 1; // force a redraw of whatever screen we return to
    }
    return;
  }

  if (btn1 == 1) { //pressed menu option toggle
    Btn1_Press_Count++; //debug

    
    // Navigate Main Menu
    if (Screen_sel == 0) {
      CursorPos++; 
      if (CursorPos > 2) { 
          CursorPos = 0; // Wrap back to option 0
      }
    }
    
    // Navigate Clock Time Set
    else if (Screen_sel == 2) {
      __disable_irq(); // Enter critical section
      // Set Time: Increment the active value
      if (CursorPos == 0) { Hours++; if (Hours > 12) Hours = 1; }
      if (CursorPos == 1) { Minutes++; if (Minutes > 59) Minutes = 0; }
      Seconds = 0; //default
      __enable_irq();  // Exit critical section
    }
    
    // Navigate Alarm Time Set
    else if (Screen_sel == 3) {
      // Set Alarm: Increment the active value
      if (CursorPos == 0) { AlarmHours++; if (AlarmHours > 12) AlarmHours = 1; }
      if (CursorPos == 1) { AlarmMins++; if (AlarmMins > 59) AlarmMins = 0; }
      AlarmSeconds = 0; //default
    }
    
    DisplayUpdate = 1; // Redraw the cursor arrow based on variable CursorPos
  }

  if (btn2 == 1) { //pressed menu option select
    Btn2_Press_Count++; //debug
  
    // Select on Main Menu
    if (Screen_sel == 0) {
      if (CursorPos == 0) Screen_sel = 1; // Go to Clock Style Select
      else if (CursorPos == 1) Screen_sel = 2; // Go to Set Time
      else if (CursorPos == 2) Screen_sel = 3; // Go to Set Alarm
      
      CursorPos = 0; // Reset cursor for next screen
      ST7735_FillScreen(ST7735_BLACK); // Clear screen for transition
    }

    //Select on Clock Display
    else if (Screen_sel == 1) {
      Screen_sel = 0; 
      CursorPos = 0;
      ST7735_FillScreen(ST7735_BLACK); // Clear screen for transition
    }

    //Select on Clock Time Set and Alarm Time Set
    else if (Screen_sel == 2 || Screen_sel == 3) { 
      // Setting Time/Alarm Sequence
      if (CursorPos == 0) { // Confirmed Hours, move to Minutes
        CursorPos = 1; 
      } 
      else if (CursorPos == 1) { // Confirmed Minutes, done. Return to Menu
        if (Screen_sel == 3) { //once the user finishes confirming alarm, alarm get armed
          AlarmArmed = 1;
        }
        Screen_sel = 0; 
        CursorPos = 0; // Reset for the Main Menu
        ST7735_FillScreen(ST7735_BLACK); // Clear screen for transition
      }
    }
      
    DisplayUpdate = 1; // Draw new screen
  }
}


void Speaker_Init(void){
  Sound_Init(0);
}

void Switch_Init(void){
  // PA18 (S1) and PB21 (S2) inputs with internal pull-up resistors
  // IOMUX->SECCFG.PINCM[PA27INDEX] = 0x00010081; 
  // IOMUX->SECCFG.PINCM[PA15INDEX] = 0x00010081;
  // GPIOA->DOE31_0 &= ~(1 << 27);  
  // GPIOA->DOE31_0 &= ~(1 << 15); 
  // 1. Configure IOMUX for pure inputs (Input Enable bit set, no internal resistors)
  IOMUX->SECCFG.PINCM[PA27INDEX] = 0x00040081; 
  IOMUX->SECCFG.PINCM[PA15INDEX] = 0x00040081;

  // 2. Clear DOE bits to 0 to ensure they are configured as inputs
  //While the code below is not atomic, it is called before the interrupts are enabled so there is no issue
  GPIOA->DOE31_0 &= ~(1 << 27);  
  GPIOA->DOE31_0 &= ~(1 << 15);
}

void LED_Init(void){
   // ****ECE445L write this ****

}
void LED_Out(uint32_t data){
   // ****ECE445L write this ****

}
void LED_Toggle(void){
   // ****ECE445L write this ****

}