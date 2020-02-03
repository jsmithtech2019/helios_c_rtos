// API USAGE DOCUMENTATION
// http://software-dl.ti.com/dsps/dsps_public_sw/sdo_sb/targetcontent/tirtos/2_21_00_06/exports/tirtos_full_2_21_00_06/products/tidrivers_cc13xx_cc26xx_2_21_00_04/docs/doxygen/html/_i2_c_8h.html

#include <ti/drivers/I2C.h>
#include <ti/drivers/i2c/I2CCC26XX.h>

// Notes:
// I2C_MODE_BLOCKING -> task execution is blocked during the I2C transaction
// I2C_MODE_CALLBACK -> task execution not blocked, other transactions can
// happen or be queued. When transfer completed I2C driver will call a user
// specified callback function (from a HWI or SWI context, depending on the device).

// Receive 10 bytes over I2C in I2C_MODE_BLOCKING. #############################
int main(void){
    // Locals
    I2C_Handle handle;
    I2C_Params params;
    I2C_Transaction i2cTrans;
    uint8_t rxBuf[32];      // Receive buffer
    uint8_t txBuf[32];      // Transmit buffer

    // Configure I2C parameters.
    I2C_Params_init(&params);

    // Initialize master I2C transaction structure
    i2cTrans.writeCount   = 0;
    i2cTrans.writeBuf     = txBuf;
    i2cTrans.readCount    = 10;
    i2cTrans.readBuf      = rxBuf;
    i2cTrans.slaveAddress = 0x3C;

    // Open I2C
    handle = I2C_open(Board_I2C, &params);

    for(int i=0; i < 1000; i++){
        // Do I2C transfer receive
        I2C_transfer(handle, &i2cTrans);
    }
}

//##############################################################################