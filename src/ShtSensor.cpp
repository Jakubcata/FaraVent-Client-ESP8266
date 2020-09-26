#include <stdint.h>
#include <stdbool.h>
#include <Arduino.h>
#include <Wire.h>
#include "ShtCommand.h"
#include "ShtSensor.h"

void ShtSensor::FlushData( const uint16_t aMaxFllushes )
{
    uint16_t flushes = aMaxFllushes; 
    
    // Flush internal buffers of I2C interface
    flush();
    
    // Call read function to make sure that there are no bytes in the receive buffer
    while( available() && flushes --> 0 )
    {
        read();
    }
}

bool ShtSensor::SendCommand( ShtCmdBase &aCmd )
{
    bool success = false;
    const uint8_t cmd_size = aCmd.GetSize();

    // Flush I2C interface before transmission starts
    FlushData( 1000 );

    beginTransmission( iAddr );
    // Command is successfully sent if number of sent bytes equals to size of a command
    success = write( &(aCmd[0]), cmd_size ) == cmd_size;  
    endTransmission();

    return success;
}

bool ShtSensor::ReceiveResponse( ShtDataResponse &aResponse, const uint8_t aTimeout )
{
    const uint8_t response_size = aResponse.GetSize();
    
    uint8_t rx_count = 0;
    bool tm_elapsed = false;

    const uint64_t timeout = millis() + aTimeout;

    // Try to receive response from SHT sensor
    while ( ( rx_count < response_size ) && !tm_elapsed )
    {
        rx_count = requestFrom( iAddr, response_size );
        tm_elapsed = millis() > timeout;
    }

    // Get received data if timeout not elapsed
    if ( !tm_elapsed )
    {
        for ( uint8_t byte = 0; ( byte < response_size ) && available(); ++byte )
        {
            aResponse[byte] = read();
        }
    }

    return ( rx_count == response_size );
}

void ShtSensor::ProcessResponse( ShtDataResponse &aResponse )
{
    // Process temperature 
    if ( aResponse.IsTempValid() )
    {
        iTemp = -45 + 175.0 * ( aResponse.GetRawTemp() / 65535.0 );
    }
    else
    {
        Serial.print( "Invalid Temperature packet received!\n" );
        // TODO: Tmeperature packet is invalid
    }
    
    // Process humidity
    if ( aResponse.IsHumValid() )
    {
        iHum = 100.0 * ( aResponse.GetRawHum() / 65535.0 );
    }
    else
    {
        Serial.print( "Invalid Humidity packet received!\n" );
        // TODO: Humidity packet is invalid
    }
}

void ShtSensor::Update()
{
    enum TransmitMode
    {
        eTransmit,
        eReceive
    };

    static TransmitMode mode = eTransmit;

    if ( mode == eTransmit )
    {
        SingleShotCmd shot_cmd;

        // If command was successfully sent
        if ( SendCommand( shot_cmd ) )
        {
            // Store timestamp of sent command
            iLastCmdTime = millis();

            // Command successfully sent, go to receive mode
            mode = eReceive;
        }
    }
    else
    {
        // If time to measure data by SHT sensor elapsed
        if ( IsTimeToReceive() )
        {
            ShtDataResponse response;

            if ( ReceiveResponse( response, SHT_RESPONSE_TIMEOUT_MS ) )
            {
                // Response from SHT sensor received so set error code to eNoError
                iErrorCode = ShtSensorErr::eNoError;

                // Process received response
                ProcessResponse( response );
            }
            else
            {
                iErrorCode = ShtSensorErr::eNotResponding;

                // SHT sensor did not respond within 20 ms
                Serial.print( "SHT sensor did not respond within 20ms!\n" );
            }
            
            mode = eTransmit;    
        }
    }
}