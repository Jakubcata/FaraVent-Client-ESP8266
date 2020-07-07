#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Wire.h>
#include "Interrupt.h"
#include "ShtSensor.h"

void ShtSensor::FlushData()
{
    // Flush internal buffers of I2C interface
    flush();

    // Call read function to make sure that there are no bytes in the receive buffer
    uint16_t maxflushes = 1000;
    while( available() && maxflushes --> 0 )
    {
        read();
    }
}

bool ShtSensor::SendSingleShot()
{
    bool cmdsent = false;

    FlushData();

    const uint8_t singleshot_cmd[] = { 0x24, 0x00 }; 
    const uint8_t cmdsize = sizeof singleshot_cmd;

    beginTransmission( iAddr );
    cmdsent = write( singleshot_cmd, cmdsize ) == cmdsize;  
    endTransmission();

    return cmdsent;
}

void ShtSensor::ReceiveAndCompute()
{
    uint8_t receive_buff[6] = { 0 };
    uint8_t bytes_count = 0;

    requestFrom( iAddr, (uint8_t)6 );

    while (available() && bytes_count < 5 )
    {
        receive_buff[bytes_count++] = read();
    }

    uint16_t tempRaw = (((uint16_t) receive_buff[0]) << 8 | receive_buff[1]);
    uint16_t humRaw = (((uint16_t)receive_buff[3]) << 8 | receive_buff[4]); 

    iTemp = -45 + 175.0 * (tempRaw / 65535.0);
    iHum = 100.0 * (humRaw / 65535.0);
}

void ShtSensor::Update()
{
    static uint64_t timestamp = 0;
    uint64_t now = millis();
    
    switch ( iUpdateState )
    {
    case SEND_SINGLE_SHOT:
        if ( SendSingleShot() )
        {
            iUpdateState = WAIT_FOR_DATA;
            // Need to store timestamp when command was sent to the sensor
            timestamp = now;
        }
        break;
    case WAIT_FOR_DATA:
        // 15 ms for SHT sensor to prepare data from measurement
        if ( now - timestamp >= 15 )
        {
            iUpdateState = RECEIVE_DATA;
        }
        break;
    case RECEIVE_DATA:
        ReceiveAndCompute();

        iUpdateState = SEND_SINGLE_SHOT;
        break;

    default:
        break;
    }
}