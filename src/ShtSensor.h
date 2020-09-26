#ifndef __SHT_SESNSOR_H__
#define __SHT_SESNSOR_H__

#include <stdint.h>
#include <stdbool.h>
#include "ShtCommand.h"

/**
 *  Default address of a SHT sensor
 */
#define SHT_I2C_DEFAULT_ADDR    0x44

/**
 * Default frequency of I2C bus on CLK signal in Hz 
 */ 
#define SHT_I2C_FREQUENCY_HZ    600000

/**
 *  Time needed for measuring data by SHT sensor in milliseconds
 */
#define SHT_RESPONSE_TIME_MS    15

/**
 *  Timeout for SHT sensor to respond on a command in milliseconds
 */
#define SHT_RESPONSE_TIMEOUT_MS 20

/**
 *  Invalid value of temperature
 */ 
#define INVALID_TEMPERATURE     -1234

/**
 *  Invalid value of humidity
 */
#define INVALID_HUMIDITY        -1

/**
 *  Enum representing error codes of SHT sensor
 */
enum ShtSensorErr
{
    eNoError,
    eNotResponding
};

class ShtSensor: public TwoWire
{
    /** 
     * An address of SHT sensor connected on the I2C bus
    */
    uint8_t iAddr;

    /**
     * Computed temperature received from a SHT sensor
    */
    float iTemp;

    /**
     * Computed humidity received from a SHT sensor
    */
    float iHum;

    /**
     * Timestamp of the last command sent to SHT sensor
    */
    uint64_t iLastCmdTime;

    /**
     * Error code for SHT sensor
    */
    ShtSensorErr iErrorCode;

    /**
     * Flushes internal buffers to prepare I2C interface for receiving data from SHT sensor 
     * 
     * @param aMaxFllushes maximal number of internal flush() function calls       
    */
    void FlushData( const uint16_t aMaxFllushes );

    /** 
     * Sends a command to SHT sensor
     * 
     * @param aCmd command to be sent
     * @return True if command was successfully sent
    */
    bool SendCommand( ShtCmdBase &aCmd );

    /** 
     * Receives a response on a previous command
     * 
     * @param aResponse received response
     * @param aTimeout timeout in millisencods to receive a response
     * @return Receive success within passed-in timeout
    */
    bool ReceiveResponse( ShtDataResponse &aResponse, const uint8_t aTimeout );

    /** 
     * Detects if SHT sensor is ready to send measured data
     * 
     * @return True if it is time to receive measured data
    */
    bool IsTimeToReceive() const
    {
        return ( millis() - iLastCmdTime ) > SHT_RESPONSE_TIME_MS;
    }

    /** 
     * Processes received data response from SHT sensor
     * 
     * Checks if response fields are valid and computes temperature and humidity
     * from received raw data
     * 
     * @param aResponse received response
    */
    void ProcessResponse( ShtDataResponse &aResponse );

    public:
        /**
         * Constructor to initialize I2C interface and flush internal buffers
         * 
         * @param aSDA SDA I2C pin number SHT sensor is connected to
         * @param aSCL SCL I2C pin number SHT sensor is connected to
         * @param aAddr I2C address of SHT sensor on I2C bus 
        */    
        ShtSensor( const uint8_t aSDA, const uint8_t aSCL, const uint8_t aAddr = SHT_I2C_DEFAULT_ADDR ): 
            TwoWire( 0 ), 
            iAddr( aAddr )
        {
            begin( aSDA, aSCL, aAddr );
            setClock( SHT_I2C_FREQUENCY_HZ );
            flush();
        }

        /**
         * Updates temperature and humudity values from SHT sensor  
        */
        void Update();

        /**
         * Returns the last computed temperature from SHT sensor
         * 
         * @return value of temperature in Celsius  
        */
        float GetTemperature() const
        {
            return ( iErrorCode == ShtSensorErr::eNoError ) ? iTemp : INVALID_TEMPERATURE;
        }

        /**
         * Returns the last computed humidity from SHT sensor
         * 
         * @return value of humidity in %  
        */
        float GetHumidity() const
        {
            return ( iErrorCode == ShtSensorErr::eNoError ) ? iHum : INVALID_HUMIDITY;
        }
};

#endif /* __SHT_SESNSOR_H__ */