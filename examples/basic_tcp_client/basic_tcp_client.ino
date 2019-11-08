#include "LinuxSocketClient.h"

LinuxSocketClient client;
IPAddress         ip( 123, 123, 123, 123 );    // Set Host ip
uint16_t          port = 123;                  // Set Host port

void setup( void )
{
}

void loop( void )
{
    if( client.available() )
    {
        char temp = client.read();
        Serial.print( temp );
    }

    if( !client.connected() )
    {
        Serial.println();

        while( !client.connect( ip, port ) )
        {
            Serial.println( "Connection failed." );
            Serial.println( "Will try again after 5 seconds." );
            delay( 5000 );
        }

        Serial.println( "Connection success." );
        client.println( "Hello!" );
    }
}