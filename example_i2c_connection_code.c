/*
 *  ======== example_i2c_connection_code.c ========
 */
/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>

/* TI-RTOS Header files */
#include <ti/drivers/PIN.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>

/* Board Header files */
#include "Board.h"

#define TASKSTACKSIZE       1024
#define TMP007_OBJ_TEMP     64  /* Object Temp Result Register */

Task_Struct task0Struct;
Char task0Stack[TASKSTACKSIZE];

void taskFxn(UArg arg0, UArg arg1)
{
    // Locals
    I2C_Handle handle;
    I2C_Params params;
    I2C_Transaction i2cTrans;
    uint8_t rxBuf[16];      // Receive buffer
    uint8_t txBuf[16];      // Transmit buffer
    uint8_t read;

    I2C_Params_init(&params);
    // Open I2C
    handle = I2C_open(Board_I2C, &params);
    if (handle == NULL) {
        System_abort("Error Initializing I2C\n");
    }
    else {
        System_printf("I2C Initialized!\n");
    }


    int i;
    int j;
    uint16_t        reading;
    uint32_t x;
    uint32_t y;

    // Select the Current register to read from
    for(x = 0; x < 16; x++){
        txBuf[x] = 0;
    }
    txBuf[15] = 1;

    // Initialize master I2C transaction structure
    i2cTrans.writeCount   = 16;
    i2cTrans.writeBuf     = txBuf;
    i2cTrans.readCount    = 2;
    i2cTrans.readBuf      = rxBuf;
    i2cTrans.slaveAddress = 64;

    // Write to the slave
    I2C_transfer(handle, &i2cTrans);

    while(1){
        if (I2C_transfer(handle, &i2cTrans)) {
            System_printf("Value : %i %i %i\n", rxBuf[2], rxBuf[1], rxBuf[0]);
        }
        else {
            System_printf("I2C Bus fault\n");
        }

        System_flush();
        Task_sleep(1000000 / Clock_tickPeriod);
    }

    /* Deinitialized I2C */
    I2C_close(handle);
    System_printf("I2C closed!\n");

    System_flush();
}

//     for(y = 0; y < 16; y++){
//         txBuf[y] = 0;
//     }
//     i2cTrans.readCount    = 3;
//     i2cTrans.writeCount   = 0;

//     while(1){
//         if (I2C_transfer(handle, &i2cTrans)) {
//             //read = rxBuf[0];
//             //System_printf("Value : %i\n", rxBuf[0]);
//             System_printf("Value : %i %i %i\n", rxBuf[2], rxBuf[1], rxBuf[0]);
// //            for(j = 2; j >= 0; j--){
// //                //System_printf("%i", asciiToBinary(rxBuf[j]));
// //                System_printf("%i", rxBuf[j]);
// //                System_printf(" ");
// //            }
//             //asciiToBinary(read);
//             //j = int(read);
//             //i = read;
//             //System_printf("Final val : %d\n", rxBuf[0]);
//             //System_printf("Val : %d\n", rxBuf);

//             //System_printf("Value : %d %d %d %d\n", rxBuf[3], rxBuf[2], rxBuf[1], rxBuf[0]);
//             //System_printf("%d %d %d %d\n", i, (int)rxBuf[2], (int)rxBuf[1], (int)rxBuf[0]);
//         }
//         else {
//             System_printf("I2C Bus fault\n");
//         }

//         System_flush();
//         Task_sleep(1000000 / Clock_tickPeriod);
//     }

//     /* Deinitialized I2C */
//     I2C_close(handle);
//     System_printf("I2C closed!\n");

//     System_flush();
// }

int main(void)
{
    Task_Params taskParams;

    /* Call board init functions */
    Board_initGeneral();

    /* Construct tmp007 Task thread */
    Task_Params_init(&taskParams);
    taskParams.stackSize = TASKSTACKSIZE;
    taskParams.stack = &task0Stack;
    Task_construct(&task0Struct, (Task_FuncPtr)taskFxn, &taskParams, NULL);

    System_printf("Starting the I2C example\nSystem provider is set to SysMin."
                  " Halt the target to view any SysMin contents in ROV.\n");
    /* SysMin will only print to the console when you call flush or exit */
    System_flush();

    /* Start BIOS */
    BIOS_start();

    return (0);
}


//int main(void){
//    uint16_t        reading;
//    unsigned int    i;
//    unsigned int    j;
//    // Locals
//    I2C_Handle handle;
//    I2C_Params params;
//    I2C_Transaction i2cTrans;
//    uint8_t rxBuf[32];      // Receive buffer
//    uint8_t txBuf[32];      // Transmit buffer
//
//    I2C_Params_init(&params);
//    // Open I2C
//    handle = I2C_open(Board_I2C, &params);
//    if (handle == NULL) {
//        System_abort("Error Initializing I2C\n");
//    }
//    else {
//        System_printf("I2C Initialized!\n");
//    }
//
//
//
//    while(1){
//        j += 1;
//        // Do I2C transfer receive
//        if (I2C_transfer(handle, &i2cTrans)) {
//            /* Extract degrees C from the received data; see TMP102 datasheet */
//            reading = (rxBuf[0] << 6);
//
//            System_printf("%d %d\n", j, reading);
//        }
//        else {
//            System_printf("I2C Bus fault\n");
//        }
//
//        System_flush();
//        for(i=0; i < 2000; i++){
//            // do literally nothing
//            i+=1;
//        }
//    }
//}



//#define TASKSTACKSIZE   512
//
//Task_Struct task0Struct;
//Char task0Stack[TASKSTACKSIZE];
//
///* Pin driver handle */
//static PIN_Handle ledPinHandle;
//static PIN_State ledPinState;

/*
 * Application LED pin configuration table:
 *   - All LEDs board LEDs are off.
 */
//PIN_Config ledPinTable[] = {
//    Board_LED0 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
//    Board_LED1 | PIN_GPIO_OUTPUT_EN | PIN_GPIO_LOW | PIN_PUSHPULL | PIN_DRVSTR_MAX,
//    PIN_TERMINATE
//};

/*
 *  ======== heartBeatFxn ========
 *  Toggle the Board_LED0. The Task_sleep is determined by arg0 which
 *  is configured for the heartBeat Task instance.
 */
//Void heartBeatFxn(UArg arg0, UArg arg1)
//{
//    while (1) {
//        Task_sleep((UInt)arg0);
//        PIN_setOutputValue(ledPinHandle, Board_LED0,
//                           !PIN_getOutputValue(Board_LED0));
//    }
//}

/*
 *  ======== main ========
 */
//int main(void)
//{
//    Task_Params taskParams;
//
//    /* Call board init functions */
//    Board_initGeneral();
//    // Board_initI2C();
//    // Board_initSPI();
//    // Board_initUART();
//    // Board_initWatchdog();
//
//    /* Construct heartBeat Task  thread */
//    Task_Params_init(&taskParams);
//    taskParams.arg0 = 1000000 / Clock_tickPeriod;
//    taskParams.stackSize = TASKSTACKSIZE;
//    taskParams.stack = &task0Stack;
//    Task_construct(&task0Struct, (Task_FuncPtr)heartBeatFxn, &taskParams, NULL);
//
//    /* Open LED pins */
//    ledPinHandle = PIN_open(&ledPinState, ledPinTable);
//    if(!ledPinHandle) {
//        System_abort("Error initializing board LED pins\n");
//    }
//
//    PIN_setOutputValue(ledPinHandle, Board_LED1, 1);
//
//    System_printf("Starting the example\nSystem provider is set to SysMin. "
//                  "Halt the target to view any SysMin contents in ROV.\n");
//    /* SysMin will only print to the console when you call flush or exit */
//    System_flush();
//
//    /* Start BIOS */
//    BIOS_start();
//
//    return (0);
//}


