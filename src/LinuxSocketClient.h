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

#pragma once

#include <Arduino.h>

const int BUFFER_SIZE = 1024;

class LinuxSocketClient : public Client
{
public:
    virtual int     connect( IPAddress ip, uint16_t port );
    virtual int     connect( const char *host, uint16_t port );
    virtual size_t  write( uint8_t data );
    virtual size_t  write( const uint8_t *buf, size_t size );
    virtual int     available( void );
    virtual int     read( void );
    virtual int     read( uint8_t *buf, size_t size );
    virtual int     peek( void );
    virtual void    flush( void );
    virtual void    stop( void );
    virtual uint8_t connected( void );
    virtual         operator bool( void );

    void recv_thread_loop( void );

private:
    char     buffer[BUFFER_SIZE];
    uint16_t buffer_head  = 0;
    uint16_t buffer_tail  = 0;
    int      m_fd         = -1;
    bool     is_connected = false;

    pthread_t recv_thread;
};

inline LinuxSocketClient::operator bool( void )
{
    return connected();
}