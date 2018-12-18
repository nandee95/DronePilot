////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2018 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "SFML_Packet.h"

namespace sf
{
////////////////////////////////////////////////////////////
Packet::Packet() :
m_readPos(0),
m_sendPos(0),
m_size(0),
m_isValid(true)
{

}


////////////////////////////////////////////////////////////
Packet::~Packet()
{

}


////////////////////////////////////////////////////////////
void Packet::append(const void* data, size_t sizeInBytes)
{
    if (data && (sizeInBytes > 0))
    {
        if(m_size == 0)
        {
            m_size=sizeInBytes;
            m_data = (char*)malloc(sizeInBytes);
            memcpy(m_data,data,sizeInBytes);
        }
        else
        {
            m_size+=sizeInBytes;
            m_data = (char*)realloc(m_data,m_size);
            memcpy(&m_data[m_size - sizeInBytes],data,sizeInBytes);
        }
    }
}


////////////////////////////////////////////////////////////
void Packet::clear()
{
    if(m_data != NULL) free(m_data);
    m_size = 0;
    m_readPos = 0;
    m_isValid = true;
}


////////////////////////////////////////////////////////////
const void* Packet::getData() const
{
    return m_data == NULL ? &m_data[0] : NULL;
}


////////////////////////////////////////////////////////////
size_t Packet::getDataSize() const
{
    return m_size;
}


////////////////////////////////////////////////////////////
bool Packet::endOfPacket() const
{
    return m_readPos >= m_size;
}


////////////////////////////////////////////////////////////
Packet::operator BoolType() const
{
    return m_isValid ? &Packet::checkSize : NULL;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(bool& data)
{
    Uint8 value;
    if (*this >> value)
        data = (value != 0);

    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Int8& data)
{
    if (checkSize(sizeof(data)))
    {
        data = *reinterpret_cast<const Int8*>(&m_data[m_readPos]);
        m_readPos += sizeof(data);
    }

    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Uint8& data)
{
    if (checkSize(sizeof(data)))
    {
        data = *reinterpret_cast<const Uint8*>(&m_data[m_readPos]);
        m_readPos += sizeof(data);
    }

    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Int16& data)
{
    if (checkSize(sizeof(data)))
    {
        data = ntohs(*reinterpret_cast<const Int16*>(&m_data[m_readPos]));
        m_readPos += sizeof(data);
    }

    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Uint16& data)
{
    if (checkSize(sizeof(data)))
    {
        data = ntohs(*reinterpret_cast<const Uint16*>(&m_data[m_readPos]));
        m_readPos += sizeof(data);
    }

    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Int32& data)
{
    if (checkSize(sizeof(data)))
    {
        data = ntohl(*reinterpret_cast<const Int32*>(&m_data[m_readPos]));
        m_readPos += sizeof(data);
    }

    return *this;
}


////////////////////////////////////////////////////////////
sf::Uint32 Packet::nextStringLen()
{
    Uint32 data;
    if (checkSize(sizeof(data)))
    {
        data = ntohl(*reinterpret_cast<const Int32*>(&m_data[m_readPos]));
    }

    return data;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(Uint32& data)
{
    if (checkSize(sizeof(data)))
    {
        data = ntohl(*reinterpret_cast<const Uint32*>(&m_data[m_readPos]));
        m_readPos += sizeof(data);
    }

    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(float& data)
{
    if (checkSize(sizeof(data)))
    {
        data = *reinterpret_cast<const float*>(&m_data[m_readPos]);
        m_readPos += sizeof(data);
    }

    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(double& data)
{
    if (checkSize(sizeof(data)))
    {
        data = *reinterpret_cast<const double*>(&m_data[m_readPos]);
        m_readPos += sizeof(data);
    }

    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(char* data)
{
    // First extract string length
    Uint32 length = 0;
    *this >> length;
    if ((length > 0) && checkSize(length))
    {
        // Then extract characters
        memcpy(data, &m_data[m_readPos], length);
        data[length] = '\0';

        // Update reading position
        m_readPos += length;
    }

    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator >>(String& data)
{
    // First extract the string length
    Uint32 length = 0;
    *this >> length;
    
    data = "";

    if ((length > 0) && checkSize(length * sizeof(Uint32)))
    {
        // Then extract characters
        for (Uint32 i = 0; i < length; ++i)
        {
            Uint8 character = 0;
            *this >> character;
            data += character;
        }
    }

    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(bool data)
{
    *this << static_cast<Uint8>(data);
    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Int8 data)
{
    append(&data, sizeof(data));
    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Uint8 data)
{
    append(&data, sizeof(data));
    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Int16 data)
{
    Int16 toWrite = htons(data);
    append(&toWrite, sizeof(toWrite));
    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Uint16 data)
{
    Uint16 toWrite = htons(data);
    append(&toWrite, sizeof(toWrite));
    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Int32 data)
{
    Int32 toWrite = htonl(data);
    append(&toWrite, sizeof(toWrite));
    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(Uint32 data)
{
    Uint32 toWrite = htonl(data);
    append(&toWrite, sizeof(toWrite));
    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(float data)
{
    append(&data, sizeof(data));
    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(double data)
{
    append(&data, sizeof(data));
    return *this;
}


////////////////////////////////////////////////////////////
Packet& Packet::operator <<(const char* data)
{
    // First insert string length
    Uint32 length = static_cast<Uint32>(strlen(data));
    *this << length;

    // Then insert characters
    append(data, length * sizeof(char));

    return *this;
}

////////////////////////////////////////////////////////////
Packet& Packet::operator <<(const String& data)
{
    // First insert the string length
    Uint32 length = static_cast<Uint32>(m_size);
    *this << length;

    // Then insert characters
    if (length > 0)
    {
        for (size_t c=0;data.length();c++)
            *this << data[c];
    }

    return *this;
}


////////////////////////////////////////////////////////////
bool Packet::checkSize(size_t size)
{
    m_isValid = m_isValid && (m_readPos + size <= m_size);

    return m_isValid;
}


////////////////////////////////////////////////////////////
const void* Packet::onSend(size_t& size)
{
    size = getDataSize();
    return getData();
}


////////////////////////////////////////////////////////////
void Packet::onReceive(const void* data, size_t size)
{
    append(data, size);
}

} // namespace sf