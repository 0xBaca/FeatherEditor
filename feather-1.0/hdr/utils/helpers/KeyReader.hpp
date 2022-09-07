#pragma once

#include "utils/helpers/RawCharactersBuffer.hpp"
#include "windows/WindowImplInterface.hpp"

namespace feather::utils::helpers
{
    class KeyReader
    {
    private:
        utils::helpers::RawCharactersBuffer internalInputBuffer;
        char32_t getNextCharacterFromInputBufferActionReadMode(feather::windows::WindowImplInterface const *);
        char32_t getNextCharacterFromInputBufferWriteMode(feather::windows::WindowImplInterface const *);

    public:
        static const int ERROR_TOLERANCE = 64;
        char32_t getNextCharacterFromInputBuffer(feather::windows::WindowImplInterface const *);
    };
} // namespace feather::utils::helpers