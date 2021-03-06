/******************************************************************************/
/* Files to Include                                                           */
/******************************************************************************/

#if defined(__XC)
#include <xc.h>         /* XC8 General Include File */
#elif defined(HI_TECH_C)
#include <htc.h>        /* HiTech General Include File */
#endif

#ifndef __stdint_are_defined
#define __stdint_are_defined
#include <stdint.h>        /* For uint8_t definition */
#endif


#include "interrupts.h"
#include "system.h"        /* System funct/params, like osc/peripheral config */
#include "user.h"          /* User funct/params, such as InitApp */



/******************************************************************************/
/* User Global Variable Declaration                                           */
/******************************************************************************/
#ifdef useIBUTTON 
//__EEPROM_DATA(0x01, 0xB1, 0x8A, 0xF3, 0x0E, 0x00, 0x00, 0x76); //# 09,10
//__EEPROM_DATA(0x01, 0xC1, 0x15, 0xF4, 0x0E, 0x00, 0x00, 0xB0); //# 03,04
#define MAX_IButton_PASSWORDS 2    
#endif
/* i.e. uint8_t <variable_name>; */

/******************************************************************************/
/* Main Program                                                               */

/******************************************************************************/


void main(void) {
    /* Configure the oscillator for the device */
    ConfigureOscillator();

    /* Initialize I/O and Peripherals for application */
    InitApp();

#ifdef usePC2Keyboard
    PC2Keyboard_Init();
#endif     

#ifdef useKeyboard
    Initialize_Keypad();
#endif    

    //SerialWrite();

    DOOR_Last_State = DOOR_CLOSED;
    DOOR_BUTTON_Last_State = DOOR_BUTTON_RELEASED;
    LOCK1_Last_State = LOCK1_CLOSED;
    LOCK2_Last_State = LOCK2_CLOSED;

    DOOR_State = DOOR_CLOSED;
    DOOR_BUTTON_State = DOOR_BUTTON_RELEASED;
    LOCK1_State = LOCK1_CLOSED;
    LOCK2_State = LOCK2_CLOSED;
    LOCKS_State = LOCKS_LOCKED;

    Lock_Security_State = UNDEFINED;


    delaySysLEDblink = millis();
    delayone_pwm = millis();
    buzzer_delay = 0;
    buzzer_duration = 0;
    buzzer_pause_delay = 0;
    buzzer_pause_duration = 0;



#ifdef useDebugRS232
    UART_Init(4800);
#endif    


    //Initialize_IO ();
#ifdef usePWM
    PWM1_Init(PWM_frequency);
    PWM1_Set_Duty(PWM_duty_cycle);
    PWM1_Start();
#endif    

#ifdef useDebugRS232    
    UART_Write_Text("Init. ARMED\n");
#endif    





    BUZZER = BUZZER_OFF;
    BUZZER_FLUSH_PORT;


    ei();

    while (1) {

        CLRWDT();

        buzzer_process();

        sysLED_process();
        Melody_Processes();
        pwm_process();
        scan_sensors();

        switch (getDoorButtonState()) {
            case KEYPAD_RELEASED:
                UART_Write_Text("Door Button RELEASED\n");
                //passwordCode[passwordCodeCurrnetPointer]
#ifdef useIBUTTON  
                if (isMasterIButton) {
                    if (DOOR_BUTTON_Duration >= 5 && DOOR_BUTTON_Duration < 20) {
                        needClearSerialNumberToEPPROM = 1;
                        break;
                    }
                }
#endif                

                if (Security_State == DOOR_OPENED_DELAY) {
                    Melody_Stop();
                }

                //                if (Security_State == DOOR_OPENED_DELAY \
//                        || Security_State == ALARM \
//                        || Security_State == ALARM_UNLOCKING_TIMEOUT) {
                //                    if ((DOOR_BUTTON_Duration >= passwordCode[passwordCodeCurrnetPointer]) \
//                            && (DOOR_BUTTON_Duration <= (passwordCode[passwordCodeCurrnetPointer] + 10))) {
                //                        //step of password  OK
                //                        passwordCodeCurrnetPointer++;
                //                        buzzer_duration = 20;
                //                    } else {
                //                        //wrong step of password
                //                        passwordCodeCurrnetPointer = 0;
                //                        //SelectMelody(ERROR_e);
                //                        buzzer_duration = 500;
                //                    }
                //                }

                if (DOOR_BUTTON_Duration >= 5 && DOOR_BUTTON_Duration < 20) {
                    if (Security_State == DISARMED) {
                        //delayone_pwm2_max = delayone_pwm2_short;
                        Melody_Select(SECONDS_e);
                        delay_for_CloseDoor = millis_32();
                        Security_State = DELAY_PREPARE_DOOR;
#ifdef useDebugRS232            
                        UART_Write_Text("DELAY_PREPARE_DOOR\n");
#endif 
                    } else if (Security_State == ARMED) {
                        Security_State = DISARMED;
                        Lock_Security_State = UNDEFINED;                          
                        delay_for_DISARMED = millis_32();
                        Melody_Select(ALARMOFF_e);
                        pwm_flash_mode(pwm_FlashMode_short);
#ifdef useDebugRS232                   
                        UART_Write_Text("DISARMED BY DOOR BUTTON\n");
#endif                                 
                        //StopMelody();
                        //delayone_pwm2_max = delayone_pwm2_middle;
                    }
                }

                //                //Check password OK is ?
                //                if (passwordCodeCurrnetPointer >= sizeof (passwordCode)) {
                //                    //password ok
                //#ifdef useDebugRS232            
                //                    UART_Write_Text("DISARMED\n");
                //#endif 
                //                    Security_State = DISARMED;
                //                    delay_for_DISARMED = millis_32();
                //                    Melody_Select(ALARMOFF_e);
                //                    pwm_flash_mode(pwm_FlashMode_short);
                //                    passwordCodeCurrnetPointer = 0;
                //                }
                break; //case KEYPAD_RELEASED:
                //            case KEYPAD_PRESSED:
                //                //                if (Security_State == ARMED) {
                //                //                    if ((passwordCodeCurrnetPointer > 0)) {
                //                //                        if ((DOOR_BUTTON_Duration >= passwordCode[passwordCodeCurrnetPointer]) && \
////                        (DOOR_BUTTON_Duration <= passwordCode[passwordCodeCurrnetPointer] + 10)) {
                //                //                            passwordCodeCurrnetPointer++;
                //                //                            if (passwordCodeCurrnetPointer >= sizeof (passwordCode)) {
                //                //                                passwordCodeCurrnetPointer = 0;
                //                //                            }
                //                //                            buzzer_duration = 5;
                //                //                        } else {
                //                //                            passwordCodeCurrnetPointer = 0;
                //                //                            Melody_Select(ERROR_e);
                //                //                        }
                //                //                    } else {
                //                //                        //default beep
                //                //                        buzzer_duration = 5;
                //                //                    }
                //                //                } else {
                //                //                    buzzer_duration = 5;
                //                //                }
                //                //                //SelectMelody(1);
                //                UART_Write_Text("Door Button PRESSED\n");
                //                break; //case KEYPAD_PRESSED
        }//switch 

        /**
         * Ckeck locks states
         */
        if (LOCK1_State == LOCK1_CLOSED
                && LOCK2_State == LOCK2_CLOSED) {
            LOCKS_State = LOCKS_LOCKED;
            if (Lock_Security_State != UNDEFINED) {
                Lock_Security_State = UNDEFINED;
                Melody_Stop();
            }
        }
        if (LOCK1_State == LOCK1_OPENED
                && LOCK2_State == LOCK2_OPENED) {
            LOCKS_State = LOCKS_UNLOCKED;
        }


        /**
         * Check in door closed state
         * Lock state
         */
        if (DOOR_State == DOOR_CLOSED
                && Security_State != DOOR_OPENED_DELAY
                ) {

#ifdef useDebugRS232       
            if (LOCK1_State == LOCK1_OPENED) {
                if (!LOCK1_State_Displayed) {
                    UART_Write_Text("LOCK1 opened\n");
                    LOCK1_State_Displayed = true;
                }
            } else {
                if (LOCK1_State_Displayed) {
                    UART_Write_Text("LOCK1 closed\n");
                    LOCK1_State_Displayed = false;
                }
            }
            if (LOCK2_State == LOCK2_OPENED) {
                if (!LOCK2_State_Displayed) {
                    UART_Write_Text("LOCK2 opened\n");
                    LOCK2_State_Displayed = true;
                }
            } else {
                if (LOCK2_State_Displayed) {
                    UART_Write_Text("LOCK2 closed\n");
                    LOCK2_State_Displayed = false;
                }
            }
#endif               

            /**
             * Check if locks state opened all or only part of it
             * if part locks is opened the start countdown timer until
             * all locks opened
             */


            if (LOCK1_State == LOCK1_OPENED
                    || LOCK2_State == LOCK2_OPENED
                    ) {

                if (Security_State == ARMED
                        && Lock_Security_State != WAIT_OPENING_ALL_LOCKS
                        ) {
                    Lock_Security_State = WAIT_OPENING_ALL_LOCKS;
                    delay_for_Unlocking = millis_32();
#ifdef useDebugRS232                   
                    UART_Write_Text("WAIT_OPENING_ALL_LOCKS\n");
#endif                 
                }

                if (Security_State == DISARMED
                        && Lock_Security_State != WAIT_CLOSING_ALL_LOCKS
                        ) {
                    Lock_Security_State = WAIT_CLOSING_ALL_LOCKS;
                    delay_for_Locking = millis_32();
#ifdef useDebugRS232                   
                    UART_Write_Text("WAIT_CLOSING_ALL_LOCKS\n");
#endif                 
                }
            }
        }//if DOOR_State == DOOR_CLOSED

        /**
         * if state Door is opened
         */
        if (DOOR_State == DOOR_OPENED) {
            //buzzer_duration = 10;
#ifdef useDebugRS232            
            if (!DOOR_State_Displayed) {
                UART_Write_Text("DOOR opened\n");
                DOOR_State_Displayed = true;
            }
#endif       
            /**
             * Door opened, start countdown timer for enter password
             */
            if (Security_State != DOOR_OPENED_DELAY
                    && Security_State != ALARM
                    && Security_State != DISARMED
                    && Security_State != DELAY_PREPARE_DOOR
                    ) {
                Security_State = DOOR_OPENED_DELAY;
                delay_for_OpenDoor = millis_32();
                Melody_Select(SECONDS_e);
#ifdef useDebugRS232                   
                UART_Write_Text("DOOR_OPENED_DELAY\n");
#endif                 
            }

        }
#ifdef useDebugRS232            
        else { //DOOR_State == DOOR_OPENED
            if (DOOR_State_Displayed) {
                UART_Write_Text("DOOR closed\n");
                DOOR_State_Displayed = false;
            }
        }
#endif            

#ifdef useDebugRS232
        //        if (!UART_TX_Empty()){
        //                __delay_ms(200);
        //        }
#endif        

#ifdef useKeyboard   
        /*
         * Operation of enter password from keypad
         */
        uint8_t key = getKey();
        if (key != NO_KEY) {

            if (Security_State == DOOR_OPENED_DELAY) {
                Melody_Stop();
            }


            buzzer_duration = 10;

            uint8_t pinOk = checkPinCode(key);
            if (pinOk) {
                Security_State = DISARMED;
                Lock_Security_State = UNDEFINED;                
                delay_for_DISARMED = millis_32();
                Melody_Select(ALARMOFF_e);
                pwm_flash_mode(pwm_FlashMode_short);
            }

#ifdef useDebugRS232      
            UART_Write_Text("\nKEYPAD PRESED:");
            UART_Write(key);
            if (pinOk) {
                UART_Write_Text(" PWD OK, ALARMOFF ");
            };
#endif                      
            if (key == '*') {
                pinCodeCurrnetPointer = 0;
                //                Security_State = ALARM;
                //                delay_for_Alarm = millis_32();
                //                pwm_flash_mode(pwm_FlashMode_middle);
                //                Melody_Select(SOS_e);
                //                if (Security_State != DOOR_OPENED_DELAY) {
                //                    Security_State = DOOR_OPENED_DELAY;
                //                    delay_for_OpenDoor = millis_32();
                //                    Melody_Select(SECONDS_e);
                //#ifdef useDebugRS232                   
                //                    UART_Write_Text("DOOR_OPENED_DELAY\n");
                //#endif                 
                //                }
                DISARMED_LONG = true;
                delay_for_DISARMED = millis_32();
#ifdef useDebugRS232            
                UART_Write_Text("SETUP Time_for_DISARMED IS LONG NOW.\n");
#endif    
            } else if (key == '#') {
                DISARMED_LONG = false;
                delay_for_DISARMED = millis_32();
#ifdef useDebugRS232            
                UART_Write_Text("SETUP Time_for_DISARMED IS STANDARD NOW.\n");
#endif  
            };
        }
#endif

//        /*
//         * Safety WAIT_CLOSING_ALL_LOCKS after door closed
//         */
//        if ((Security_State == DISARMED || Security_State == ARMED)
//                && (DOOR_State == DOOR_CLOSED)
//                && (LOCKS_State == LOCKS_UNLOCKED)
//                && (Lock_Security_State != WAIT_CLOSING_ALL_LOCKS)
//                ) {
//            Lock_Security_State = WAIT_CLOSING_ALL_LOCKS;
//            delay_for_Unlocking = millis_32();
//#ifdef useDebugRS232                   
//            UART_Write_Text("WAIT_CLOSING_ALL_LOCKS\n");
//#endif   
//        }




        //check timers --------------------------------------

        /*  
         * After Disarming and close door, 
         * set countdown timer for reassing armed state
         */
        if ((Security_State == DISARMED) &&
                DOOR_State == DOOR_CLOSED
                ) {
            if ((millis_32() - delay_for_DISARMED) >=
                    (DISARMED_LONG ? Time_for_DISARMED_LONG : Time_for_DISARMED)) {
                Security_State = ARMED;
                pwm_flash_mode(pwm_FlashMode_long);
                Melody_Select(ALARMON_e);
#ifdef useDebugRS232           
                if (DISARMED_LONG) {
                    UART_Write_Text("LONG ");
                }
                UART_Write_Text("Time_for_DISARMED. ARMED\n");
#endif           
                DISARMED_LONG = false;
            }
        }

        /*
         *  Countdown timer for set ARMED state
         *  if all locks locked and door closed then timer forced to armed state
         */
        if ((Security_State == DELAY_PREPARE_DOOR)
                && (
                ((millis_32() - delay_for_CloseDoor) >= Time_for_CloseDoor)
                || (LOCKS_State == LOCKS_LOCKED && DOOR_State == DOOR_CLOSED)
                )
                ) {
            Security_State = ARMED;
            DISARMED_LONG = false;
#ifdef useDebugRS232            
            UART_Write_Text("\nTimeout_for_CloseDoor. ARMED\n");
#endif                
            pwm_flash_mode(pwm_FlashMode_long);
            Melody_Select(ALARMON_e);
        }

        /**
         * Countdown timer for wait unlocking all locks in time
         * 
         */
        if ((Lock_Security_State == WAIT_OPENING_ALL_LOCKS)
                && ((millis_32() - delay_for_Unlocking) >= Time_for_Unlocking)
                ) {
            Security_State = ALARM_UNLOCKING_TIMEOUT;
            delay_for_Alarm = millis_32();
            pwm_flash_mode(pwm_FlashMode_middle);
            Melody_Select(SIREN_e);
#ifdef useDebugRS232            
            UART_Write_Text("WAIT_OPENING_ALL_LOCKS. ALARM_UNLOCKING_TIMEOUT\n");
#endif   
        }

        /**
         * Notification. Wait closing locks in unarmed state
         */
        if ((Lock_Security_State == WAIT_CLOSING_ALL_LOCKS)
                && ((millis_32() - delay_for_Locking) >= Time_for_Locking)
                ) {
            //Security_State = ALARM_LOCKING_TIMEOUT;
            delay_for_Alarm = millis_32();
            pwm_flash_mode(pwm_FlashMode_middle);
            Melody_Select(SIREN_e);
#ifdef useDebugRS232            
            UART_Write_Text("WAIT_CLOSING_ALL_LOCKS. ALARM_LOCKING_TIMEOUT\n");
#endif   
        }

        /**
         * Coutdown timer after door opening
         */
        if ((Security_State == DOOR_OPENED_DELAY)
                && ((millis_32() - delay_for_OpenDoor) >= Time_for_OpenDoor)
                ) {
            Security_State = ALARM;
            delay_for_Alarm = millis_32();
            pwm_flash_mode(pwm_FlashMode_middle);
            Melody_Select(SOS_e);
#ifdef useDebugRS232            
            UART_Write_Text("DOOR_OPENED_DELAY. ALARM SOS\n");
#endif   
        }

        /**
         * Safety countdown timer for ALAMRED SIREN mode
         */
        if ((Security_State == ALARM
                || Security_State == ALARM_UNLOCKING_TIMEOUT
                || Lock_Security_State == WAIT_CLOSING_ALL_LOCKS
                )
                && ((millis_32() - delay_for_Alarm) >= Time_for_Alarm)
                ) {
            Security_State = ARMED;
            DISARMED_LONG = false;
            pwm_flash_mode(pwm_FlashMode_long);
            Melody_Stop();
#ifdef useDebugRS232            
            UART_Write_Text("ALARM_TIMEOUT. ARMED\n");
#endif   
        }

#ifdef useIBUTTON        

        if ((AuthPasswordOK == 0 || isMasterIButton) && (Detect_Slave_Device() == OW_HIGH)) {
#ifdef useDebugRS232            
            //UART_Write_Text("Detect_Slave\n");
#endif 
            OW_write_byte(0x33); // Send a command to read a serial number

            for (uint8_t i = 0; i < 8; i++) {
                serial_number[i] = OW_read_byte(); // Read 64-bit registration (48-bit serial number) number from 1-wire Slave Device
            }

            //check crc for recieved data
            uint8_t crc = calc_crc(serial_number);

            if ((crc > 0) && (crc == serial_number[7])) {
#ifdef useDebugRS232    
                for (uint8_t temp = 0; temp < 8; temp++) {
                    UART_Write(serial_number[temp]);
                }
#endif

                uint8_t SerialNumberFound = CheckSerialNumberinEPPROM(serial_number);

#ifdef useDebugRS232  
                UART_Write(SerialNumberFound);
#endif
                if (SerialNumberFound && (!AuthPasswordOK)) {
                    AuthPasswordOK = 1;
                    delay_for_Auth = millis();
                    //isMasterIButton = 0;
                    if (SerialNumberFound == EiButton_MASTER_KEY_ID) {
                        //masterKey;
                        //next key will add
                        isMasterIButton = 1;
                        needAddSerialNumberToEPPROM = 1;
                        //Melody_Select(ALARMOFF_e);
#ifdef useDebugRS232            
                        UART_Write_Text("MASTER\n");
#endif  
                        //buzzer_duration = 20;
                    } else {
#ifdef useDebugRS232                           
                        UART_Write_Text("AUTH\n");
#endif                        
                        //buzzer_duration = 10;
                    }
                }

                if ((SerialNumberFound == 0) && needAddSerialNumberToEPPROM) {
                    needAddSerialNumberToEPPROM = 0;
#ifdef useDebugRS232            
                    UART_Write_Text("ADD EPPROM\n");
#endif  
                    if (AddSerialNumberToEPPROM(serial_number)) {
                        //Melody_Select(ALARMON_e);
#ifdef useDebugRS232            
                        UART_Write_Text("ADDED EPPROM\n");
#endif  
                    }
                }
            }//crc
        }



        if (needClearSerialNumberToEPPROM) {
            needClearSerialNumberToEPPROM = 0;
            ClearSerialNumbersOfEPPROM();
#ifdef useDebugRS232            
            UART_Write_Text("CLEAR EPPROM\n");
#endif  
        }
#endif

#ifdef usePC2Keyboard
        PC2Keboard_Process();
#endif  

        if (AuthPasswordOK) {

        }

#ifdef useIBUTTON  
        //reset AuthPassword state after some time
        if (AuthPasswordOK && ((millis() - delay_for_Auth) > Time_for_Auth)) {
            AuthPasswordOK = 0;
            needAddSerialNumberToEPPROM = 0;
            needClearSerialNumberToEPPROM = 0;
            isMasterIButton = 0;
#ifdef useDebugRS232            
            UART_Write_Text("TIMEOUT Auth\n");
#endif  
        }
#endif        


        /* TODO <INSERT USER APPLICATION CODE HERE> */


    }
}

