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

class LinuxSocketClient : public Client
{
public:
    virtual int     connect( IPAddress ip, uint16_t port );
    virtual int     connect( const char *host, uint16_t port );
    virtual size_t  write( uint8_t );
    virtual size_t  write( const uint8_t *buf, size_t size );
    virtual int     available( void );
    virtual int     read( void );
    virtual int     read( uint8_t *buf, size_t size );
    virtual int     peek( void );
    virtual void    flush( void );
    virtual void    stop( void );
    virtual uint8_t connected( void );
    virtual         operator bool();
};
