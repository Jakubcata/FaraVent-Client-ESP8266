#include "Sonoff.h"
#include <ArduinoJson.h>

String Sonoff::GetSwitchRequest( bool aSwitchState )
{
    String request;
    //TBD: Set max number of bytes for JSON data
    StaticJsonDocument<200> json_data;

    json_data["deviceid"] = iDevID;
    JsonObject data = json_data.createNestedObject("data");
    data["switch"] = aSwitchState ? "on" : "off";

    serializeJson( json_data, request );

    // Serial.println( sizeof(request) );

    return request;
}

int Sonoff::SendPost( String aSubDir, String aPayload, uint16_t aTimeout )
{
    int result = -1;

    if ( !aSubDir.isEmpty() )
    {
        HTTPClient http;

        if ( http.begin( iUrlBase + aSubDir ) )
        {
            http.addHeader( "Content-Type", "application/json" );
            http.setConnectTimeout( aTimeout );

            result = http.POST( aPayload );
        }

        http.end();
    }

    return result;
}

bool Sonoff::SwitchOn()
{   
    String payload = GetSwitchRequest( true );

    if ( SendPost( "switch", payload ) > 0 )
    {
        Serial.println("Sending POST success!");
    }
    else
    {
        Serial.println("Error on sending POST!");
    }
}

bool Sonoff::SwitchOff()
{
    String msg = GetSwitchRequest( false );

    if ( SendPost( "switch", msg ) > 0 )
    {
        Serial.println("Sending POST success!");
    }
    else
    {
        Serial.println("Error on sending POST!");
    }
}