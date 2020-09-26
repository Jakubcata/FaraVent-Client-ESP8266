#ifndef __SHT_COMMAND_H__
#define __SHT_COMMAND_H__

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

/**
 * Size of SHT command in bytes
*/
#define SHT_CMD_SIZE        2

/**
 * Size of SHT response in bytes
*/
#define SHT_RESPONSE_SIZE   6

class ShtCmdBase
{
    /**
     * Buffer for SHT command data
    */
    uint8_t iBuff[SHT_CMD_SIZE];

    /**
     * Number of SHT command bytes
    */
    const uint8_t iSize;
    
    public:
        /**
         * Constructor for a SHT command
         * See Datasheet SHT3x-DIS
         * 
         * @param aMSB Most significant byte in a response
         * @param aLSB Least significant byte in a response
        */
        ShtCmdBase( uint8_t aMSB, uint8_t aLSB ): iBuff{ aMSB, aLSB }, iSize( sizeof iBuff )
        {
        }

        /**
         * Overloaded index operator to access command data
         * 
         * @param aIndex Byte index of command to access
         * @return reference to corresponding byte in a command
        */
        uint8_t& operator[]( uint8_t aIndex ) 
        {
            assert( aIndex < iSize );

            return iBuff[aIndex];
        }

        /**
         * Getter for command size
         * 
         * @return Number of bytes in a command
        */
        uint8_t GetSize() const
        {
            return iSize;
        }
};

class SingleShotCmd : public ShtCmdBase
{
    public:
        /**
        * Enum representing clock stretching setting of a Single shot command
        */
        enum ClkStretching : uint8_t 
        { 
            eEnabled = 0x2C, 
            eDisabled = 0x24
        };

        /**
         * Constructor for Single shot SHT command
         * See Datasheet SHT3x-DIS
         * 
         * @param aClkStretch Clock stretching support
         * @param aRepeatCode Repeatability code 
        */
        SingleShotCmd( ClkStretching aClkStretch = eDisabled, uint8_t aRepeatCode = 0 ): ShtCmdBase( aClkStretch, aRepeatCode )
        {
        }
};

class ShtResponseBase
{
    /**
     * Lookup table for CRC8 computation
    */
    static const uint8_t iCRCTable[256];

    protected:
        /**
        * Receive buffer for response from SHT sensor
        */
        uint8_t iRxBuff[SHT_RESPONSE_SIZE];
        
        /**
         * Number of SHT response bytes
        */
        const uint8_t iSize;

        /**
         * Computes 8 bit CRC value 
         * Polynomial x^8 + x^5 + x^4 + 1 (0x31)
         * 
         * @param aMsg a message CRC value is computed from
         * @param aBytesCount number of bytes in message
         * @return computed CRC value           
        */
        uint8_t GetCRC( uint8_t const aMsg[], const uint32_t aBytesCount );
    
    public:
        /**
         * Constructor for base SHT response
         * 
         * @param aSize size of response in bytes          
        */
        ShtResponseBase( uint8_t aSize ): iSize( aSize )
        {
        }

        /**
         * Getter for response size
         * 
         * @return Number of bytes in a response
        */
        uint8_t GetSize() const
        {
            return iSize;
        }

        /**
         * Overloaded index operator to access response data
         * 
         * @param aIndex Byte index of response to access
         * @return reference to corresponding byte in a response
        */
        uint8_t& operator[]( uint8_t aIndex ) 
        {
            assert( aIndex < iSize );

            return iRxBuff[aIndex];
        }
};

class ShtDataResponse : public ShtResponseBase
{
    public:
        /**
         * Constructor for SHT data response         
        */
        ShtDataResponse(): ShtResponseBase( SHT_RESPONSE_SIZE )
        {
        }

        /**
         * Returns received raw temperature
         * 
         * @return Raw temperature       
        */
        uint16_t GetRawTemp() const
        {
            return ( ( (uint16_t) iRxBuff[0] ) << 8 | iRxBuff[1] );
        }

        /**
         * Returns received raw humidity
         * 
         * @return Raw humidity       
        */
        uint16_t GetRawHum() const 
        {
            return ( ( (uint16_t)iRxBuff[3]) << 8 | iRxBuff[4] );
        }

        /**
         * Checks integrity of temperature packed in a response 
         * 
         * @return validity of temperature       
        */
        bool IsTempValid()
        {
            return ( GetCRC( iRxBuff, 2 ) == iRxBuff[2] );
        }

        /**
         * Checks integrity of humidity packed in a response 
         * 
         * @return validity of humidity       
        */
        bool IsHumValid()
        {
            return ( GetCRC( &iRxBuff[3], 2 ) == iRxBuff[5] );
        }
};

#endif /* __SHT_COMMAND_H__ */