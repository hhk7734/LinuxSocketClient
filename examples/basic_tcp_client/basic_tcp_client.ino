#include "LinuxSocketClient.h"

LinuxSocketClient client;
IPAddress         ip( 123, 123, 123, 123 );    // Set Host ip
uint16_t          port = 123;                  // Set Host port

void setup( void )
{
    if( !client.connect( ip, port ) )
    {
        Serial.println( "Failed to connect." );
    }

    client.print( "Hello!" );
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
        Serial.println( "Socket is disconnected." );
    }
}