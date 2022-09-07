#include "utils/Utf8Util.hpp"
#include "utils/helpers/RawCharactersBuffer.hpp"

namespace feather::utils::helpers
{
RawCharactersBuffer::RawCharactersBuffer(size_t bufferCapacity)
    : buffer(bufferCapacity, 0UL), bufferSize(0UL), noUtf8Characters(0)
{
}

bool RawCharactersBuffer::add(char32_t utf8Char)
{
    char *c = (char *)&utf8Char;
    unsigned short noBytes = utils::Utf8Util::getCharacterBytesTakenLight(utf8Char);
    //Check if buffer full
    if (bufferSize + noBytes > buffer.size())
    {
        return false;
    }
    while (noBytes--)
    {
        buffer[bufferSize++] = *c++;
    }
    ++noUtf8Characters;
    return true;
}

bool RawCharactersBuffer::add(char c)
{
    //Check if buffer full
    if (bufferSize + 1UL > buffer.size())
    {
        return false;
    }
    buffer[bufferSize++] = c;
    ++noUtf8Characters;
    return true;
}


void RawCharactersBuffer::clear()
{
    bufferSize = noUtf8Characters = 0;
    std::fill(buffer.begin(), buffer.end(), 0);
}

std::vector<char> const &RawCharactersBuffer::getBuffer() const
{
    return buffer;
}

size_t RawCharactersBuffer::getBufferSize() const
{
    return bufferSize;
}

size_t RawCharactersBuffer::getNoUtf8Characters() const
{
    return noUtf8Characters;
}
} // namespace feather::utils::helpers
