#ifndef _SONOFF_H_
#define _SONOFF_H_

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <HTTPClient.h>

// TBD: implement GetResponse()
// TBD: Create class for rest objects
class Sonoff
{
    /** 
     * A Sonoff device ID
    */
    const String iDevID;

    /** 
     * An IP address of Sonoff
    */
    const String iIP;

    /** 
     * A port Sonoff is listening to
    */
    const uint16_t iPort;
    
    /** 
     * HTTP request URL base used to send POST request to Sonoff
    */
    String iUrlBase;

    /**
     * Returns HTTP request to set switch ON or OFF 
     * 
     * @param aSwitchState requested switch state       
    */
    String GetSwitchRequest( bool aSwitchState );

    /**
     * Sends HTTP POST request to Sonoff
     * 
     * @param aSubDir A subdirectory of URL suach as switch, pulse, wifi
     * @param aPayload A Paylod to be sent within POST request
     * @param aTimeout Request timeout    
    */
    int SendPost( String aSubDir, String aPayload, uint16_t aTimeout = 1000 );

    public:
        /**
        * Sonoff constructor
        * 
        * @param aIP An IP address of Sonoff
        * @param aPort A port Sonoff is listening to
        * @param aDevID Device ID of Sonoff
        * @param aChannelNum Number of channels    
        */
        Sonoff( String aIP, uint16_t aPort=8081, String aDevID="", uint8_t aChannelNum=1 ): 
            iIP( aIP ), 
            iPort( aPort ),
            iDevID ( aDevID )
        {
            iUrlBase = "http://" + aIP + ':' + aPort + '/' + "zeroconf/";
        }

        /**
        * Switch Sonoff ON using HTTP request
        */
        bool SwitchOn();

        /**
        * Switch Sonoff OFF using HTTP request
        */
        bool SwitchOff();
};

#endif /* _SONOFF_H_ */