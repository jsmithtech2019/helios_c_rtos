/* Texas A&M University
** Electronics Systems Engineering Technology
** ESET-420 Capstone Design
** Author: Jack Smith & Christian Ledgard
** File: monolith.c
** --------
**
**/

/* XDCtools Header files */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Clock.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>

/* TI-RTOS Header files */
#include <ti/drivers/PIN.h>
#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>

/* Board Header files */
#include "Board.h"

/* Standard Header files */
#include <ti/drivers/GPIO.h>

/* include header */
#include "monolith.h"

I2C_Handle handle;
I2C_Params params;
I2C_Transaction i2cTrans;

uint8_t rxBuf[4];      // Receive buffer
uint8_t txBuf[4];      // Transmit buffer

extern Semaphore_Handle truckSema, trailerSema;
extern double truckGlobal, trailerGlobal;

/* gpio pins to toggle */
uint16_t gpioToggle[8] = {23,25,26,27,30,29,28,24};

void MonolithTask(UArg arg0, UArg arg1)
{
    params.bitRate = I2C_400kHz;

    I2C_Params_init(&params);
    // Open I2C
    handle = I2C_open(Board_I2C, &params);
    if (handle == NULL) {
        System_abort("Error Initializing I2C\n");
    }
    else {
        System_printf("I2C Initialized!\n");
    }

    i2cTrans.writeBuf   = txBuf;
    i2cTrans.readBuf    = rxBuf;
    i2cTrans.writeCount = 1;
    i2cTrans.readCount  = 2;
    txBuf[0] = 0x01; // current register

    uint16_t i;
    double reading;

    while(1)
    {
        if(Semaphore_pend(truckSema, BIOS_NO_WAIT))
        {
            // Truck test
            i2cTrans.slaveAddress = 64;

            for(i = 0; i < 4; i++)
            {
                // Toggle GPIO correctly and get reading for Truck
                reading = processReading(i);

                // Send value to bluetooth to transmit
                truckGlobal = reading;
                Semaphore_post(truckSema);
            }
        }
        else if(Semaphore_pend(trailerSema, BIOS_NO_WAIT))
        {
            // Truck test
            i2cTrans.slaveAddress = 65;

            for(i = 0; i < 4; i++)
            {
                // Toggle GPIO correctly and get reading for Trailer
                reading = processReading(i+4);

                // Send value to bluetooth to transmit
                trailerGlobal = reading;
                Semaphore_post(trailerSema);
            }
        }
        Task_sleep(10);
    }
    /* uninitialize I2C */
    I2C_close(handle);
    System_printf("I2C closed!\n");

    System_flush();
}

double processReading(uint16_t ndx)
{
    uint16_t i;
    double total, changed;
    bool done = false;

    /* toggle the specified gpio */
    GPIO_write(gpioToggle[ndx], Board_GPIO_ON);

    while(!done)
    {
        // Poll i2c line
        I2C_transfer(handle, &i2cTrans);
        changed = rxBuf[0] << 8 | rxBuf[1];
        changed = changed * 1.25 / 1000;
        // reset overflow amperage to 0
        if(changed > 75){
            changed = 0.0000;
        }

        if(changed > 0.2)
        {
            total += changed;
            i++;
        }
        else if(i > 1000)
        {
            done = true;
        }
        else
        {
            i++;
            Task_sleep(10);
        }
    }

    /* toggle the specified gpio */
    GPIO_write(gpioToggle[ndx], Board_GPIO_OFF);

    return total/i;
}
