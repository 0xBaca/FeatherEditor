#pragma once

#include <vector>

namespace feather::utils::helpers
{
    class RawCharactersBuffer
    {
    private:
        std::vector<char> buffer;
        size_t bufferSize;
        size_t noUtf8Characters;

    public:
        RawCharactersBuffer(size_t = 4UL);
        bool add(char32_t);
        bool add(char);
        void clear();
        std::vector<char> const &getBuffer() const;
        size_t getBufferSize() const;
        size_t getNoUtf8Characters() const;
    };
} // namespace feather::utils::helpers
