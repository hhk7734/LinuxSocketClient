/*
 * Copyright (C) 2019  Hyeonki Hong <hhk7734@gmail.com>
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

#include "LinuxSocketClient.h"

#include <stdio.h>     // sprintf()
#include <string.h>    // memset()
#include <sys/ioctl.h>
#include <pthread.h>

namespace c_wrap
{
#include <unistd.h>    // close()
#include <sys/types.h>
#include <sys/socket.h>    // socket(), connect()
#include <netdb.h>         // htons(), getaddrinfo()
#include <arpa/inet.h>     // inet_addr()
}    // namespace c_wrap

void *thread_loop_wrapper( void *arg )
{
    LinuxSocketClient *p_client = static_cast<LinuxSocketClient *>( arg );
    p_client->recv_thread_loop();
}

void LinuxSocketClient::recv_thread_loop( void )
{
    while( is_connected )
    {
        char temp;
        if( c_wrap::recv( m_fd, &temp, 1, 0 ) == 0 )
        {
            is_connected = false;
        }
        else
        {
            buffer[buffer_head] = temp;
            buffer_head         = ( ( buffer_head ) + 1 ) % BUFFER_SIZE;

            if( buffer_head == buffer_tail )
            {
                buffer_tail = ( ( buffer_tail ) + 1 ) % BUFFER_SIZE;
            }
        }
    }
}

int LinuxSocketClient::connect( IPAddress ip, uint16_t port )
{
    stop();

    int                        s_fd;
    struct c_wrap::sockaddr_in sin;
    char                       ip_str[16];
    sprintf( ip_str, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3] );

    sin.sin_addr.s_addr = c_wrap::inet_addr( ip_str );
    sin.sin_family      = AF_INET;
    sin.sin_port        = c_wrap::htons( port );

    s_fd = c_wrap::socket( AF_INET, c_wrap::SOCK_STREAM, 0 );
    if( s_fd < 0 )
    {
        return false;
    }

    if( c_wrap::connect(
            s_fd, ( struct c_wrap::sockaddr * )&sin, sizeof( sin ) )
        != 0 )
    {
        return false;
    }

    m_fd = s_fd;

    is_connected = true;
    pthread_create( &recv_thread, NULL, thread_loop_wrapper, this );

    return true;
}

int LinuxSocketClient::connect( const char *host, uint16_t port )
{
    stop();

    struct c_wrap::addrinfo  hints;
    struct c_wrap::addrinfo *result, *rp;
    int                      s_fd;

    memset( &hints, 0, sizeof( struct c_wrap::addrinfo ) );
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = c_wrap::SOCK_STREAM;
    hints.ai_flags    = 0;
    hints.ai_protocol = 0; /* Any protocol */

    char port_s[5];
    sprintf( port_s, "%d", port );
    if( c_wrap::getaddrinfo( host, port_s, &hints, &result ) != 0 )
    {
        return false;
    }

    for( rp = result; rp != NULL; rp = rp->ai_next )
    {
        if( rp->ai_family == AF_INET )
        {
            s_fd = c_wrap::socket(
                rp->ai_family, c_wrap::SOCK_STREAM, rp->ai_protocol );
            if( s_fd < 0 )
            {
                continue;
            }

            if( c_wrap::connect( s_fd, rp->ai_addr, rp->ai_addrlen ) == 0 )
            {
                // struct c_wrap::sockaddr_in *sin
                //     = ( struct c_wrap::sockaddr_in * )rp->ai_addr;
                // char ip_str[INET_ADDRSTRLEN];
                // inet_ntop( rp->ai_family,
                //            &( sin->sin_addr ),
                //            ip_str,
                //            sizeof( ip_str ) );
                // printf( "%s\n", ip_str );
                break; /* Success */
            }
            c_wrap::close( s_fd );
        }
    }

    if( rp == NULL )
    {
        return false;
    }

    c_wrap::freeaddrinfo( result );

    m_fd = s_fd;

    is_connected = true;
    pthread_create( &recv_thread, NULL, thread_loop_wrapper, this );

    return true;
}

size_t LinuxSocketClient::write( uint8_t data )
{
    return c_wrap::send( m_fd, &data, 1, 0 );
}

size_t LinuxSocketClient::write( const uint8_t *buf, size_t size )
{
    return c_wrap::send( m_fd, buf, size, 0 );
}

int LinuxSocketClient::available( void )
{
    return ( BUFFER_SIZE + buffer_head - buffer_tail ) % BUFFER_SIZE;
}

int LinuxSocketClient::read( void )
{
    if( available() != 0 )
    {
        char temp   = buffer[buffer_tail];
        buffer_tail = ( buffer_tail + 1 ) % BUFFER_SIZE;
        return temp;
    }
    else
    {
        return -1;
    }
}

int LinuxSocketClient::read( uint8_t *buf, size_t size )
{
    if( available() < size )
    {
        size = available();
    }
    for( int i = 0; i < size; ++i )
    {
        buf[i] = read();
    }
    return size;
}

int LinuxSocketClient::peek( void )
{
}

void LinuxSocketClient::flush( void )
{
}

void LinuxSocketClient::stop( void )
{
    if( m_fd >= 0 )
    {
        c_wrap::close( m_fd );
    }
}

uint8_t LinuxSocketClient::connected( void )
{
    return is_connected;
}
