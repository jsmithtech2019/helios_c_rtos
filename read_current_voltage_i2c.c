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
    uint8_t rxBuf[4];      // Receive buffer
    uint8_t txBuf[4];      // Transmit buffer

    I2C_Params_init(&params);
    // Open I2C
    handle = I2C_open(Board_I2C, &params);
    if (handle == NULL) {
        System_abort("Error Initializing I2C\n");
    }
    else {
        System_printf("I2C Initialized!\n");
    }


    uint16_t i;
    double changed;

    // Initialize master I2C transaction structure
    i2cTrans.writeBuf   = txBuf;
    i2cTrans.writeCount = 1;
    i2cTrans.readCount  = 2;
    i2cTrans.readBuf    = rxBuf;
    i2cTrans.slaveAddress = 64; // where the slave is, default

    // Write to the slave
    I2C_transfer(handle, &i2cTrans);

    while(1){
        txBuf[0] = 0x02; // voltage register
        if (I2C_transfer(handle, &i2cTrans)) {
            changed = rxBuf[0] << 8 | rxBuf[1];
            changed = changed * 1.25 / 1000;

            //System_printf("Voltage : %x %x\n", rxBuf[0], rxBuf[1]);
            System_printf("Changed : %f Volts\n", changed);
        }
        else {
            System_printf("I2C Bus fault\n");
        }

        txBuf[0] = 0x01; // current register
        if (I2C_transfer(handle, &i2cTrans)) {
            changed = rxBuf[0] << 8 | rxBuf[1];
            changed = changed * 1.25 / 1000;
            System_printf("Current : %f Amps\n\n", changed);
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


