#ifndef __SHT_SESNSOR_H__
#define __SHT_SESNSOR_H__

#define SHT_I2C_DEFAULT_ADDR    0x44
#define SHT_I2C_FREQUENCY_HZ    600000
#define SHT_NUM_OF_RX_BYTES     10

class ShtSensor: public TwoWire
{
    enum UpdateState_t
    {
        SEND_SINGLE_SHOT,
        WAIT_FOR_DATA,
        RECEIVE_DATA,
        ERROR,
    };

    enum UpdateState_t iUpdateState;

    uint8_t iAddr;

    float iTemp;

    float iHum;

    /**
     * Flushes internal buffers to prepare I2C interface for receiving data from SHT sensor 
     * 
     * @param aMaxFllushes maximal number of internal flush() function calls       
    */
    void FlushData( const uint16_t aMaxFllushes );

    bool SendSingleShot();

    void ReceiveAndCompute();

    public:
        ShtSensor( const uint8_t aSDA, const uint8_t aSCL, const uint8_t addr = SHT_I2C_DEFAULT_ADDR ): 
            TwoWire( 0 ), 
            iAddr( addr )
        {
            begin( aSDA, aSCL, addr );
            setClock( SHT_I2C_FREQUENCY_HZ );
            flush();
        }

        void Update();

        float GetTemperature()
        {
            return iTemp;
        }

        float GetHumidity()
        {
            return iHum;
        }
};

#endif /* __SHT_SESNSOR_H__ */