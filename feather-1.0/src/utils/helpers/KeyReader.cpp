#include "utils/FeatherMode.hpp"
#include "utils/Utf8Util.hpp"
#include "utils/helpers/KeyReader.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;

namespace feather::utils::helpers
{
    char32_t KeyReader::getNextCharacterFromInputBufferActionReadMode(feather::windows::WindowImplInterface const *windowImpl)
    {
        internalInputBuffer.clear();
        lastKeyPressed = std::chrono::steady_clock::now();
        int readCharacter = windowImpl->getCharacter();
        if (readCharacter == KEY_RESIZE || readCharacter == KEY_LEFT || readCharacter == KEY_RIGHT || readCharacter == 10 || readCharacter == KEY_ENTER || readCharacter == KEY_DOWN || readCharacter == KEY_UP || utils::helpers::Lambda::isBackspace(readCharacter) || U'\t' == readCharacter || 27 == readCharacter)
        {
            timeout(-1);
            return readCharacter;
        }
        size_t errorTolerance = ERROR_TOLERANCE;
        while (ERR == readCharacter && errorTolerance--)
        {
            readCharacter = windowImpl->getCharacter();
            if (readCharacter == KEY_RESIZE || readCharacter == KEY_LEFT || readCharacter == KEY_RIGHT || readCharacter == 10 || readCharacter == KEY_ENTER || readCharacter == KEY_DOWN || readCharacter == KEY_UP || utils::helpers::Lambda::isBackspace(readCharacter) || U'\t' == readCharacter || 27 == readCharacter)
            {
                timeout(-1);
                return readCharacter;
            }
        }

        size_t noBytesTakenByCharacter = 0;
        if (ERR == readCharacter && (0 == internalInputBuffer.getBufferSize()))
        {
            timeout(-1);
            return static_cast<char32_t>(ERR);
        }
        else if (ERR != readCharacter)
        {
            noBytesTakenByCharacter = utils::Utf8Util::getCharacterBytesTakenEncoded(readCharacter);
            internalInputBuffer.add((char)readCharacter);
        }

        errorTolerance = ERROR_TOLERANCE;
        while (internalInputBuffer.getBufferSize() < noBytesTakenByCharacter && errorTolerance--)
        {
            readCharacter = getch();
            if (ERR == readCharacter)
            {
                continue;
            }
            internalInputBuffer.add(static_cast<char>(readCharacter));
            errorTolerance = ERROR_TOLERANCE;
        }

        if (!internalInputBuffer.getBufferSize())
        {
            timeout(-1);
            return static_cast<char32_t>(ERR);
        }
        timeout(-1);
        return utils::helpers::Conversion::vectorOfCharToChar32_t(internalInputBuffer.getBuffer());
    }

    /* In write mode caller is responsible for block/unblock reads */
    char32_t KeyReader::getNextCharacterFromInputBufferWriteMode(feather::windows::WindowImplInterface const *windowImpl)
    {
        lastKeyPressed = std::chrono::steady_clock::now();
        int readCharacter = windowImpl->getCharacter();
        if (readCharacter == KEY_PPAGE || readCharacter == KEY_NPAGE || readCharacter == ctrl('u') || readCharacter == ctrl('d') || readCharacter == KEY_RESIZE || readCharacter == KEY_LEFT || readCharacter == KEY_RIGHT || readCharacter == 10 || readCharacter == KEY_ENTER || readCharacter == KEY_DOWN || readCharacter == KEY_UP || utils::helpers::Lambda::isBackspace(readCharacter) || U'\t' == readCharacter || 27 == readCharacter || ctrl('k') == readCharacter || ctrl('x') == readCharacter || '\n' == readCharacter)
        {
            return readCharacter;
        }
        size_t errorTolerance = ERROR_TOLERANCE;
        timeout(0);
        while (ERR == readCharacter && errorTolerance--)
        {
            readCharacter = getch();
            if (readCharacter == KEY_PPAGE || readCharacter == KEY_NPAGE || readCharacter == ctrl('u') || readCharacter == ctrl('d') || readCharacter == KEY_RESIZE || readCharacter == KEY_LEFT || readCharacter == KEY_RIGHT || readCharacter == 10 || readCharacter == KEY_ENTER || readCharacter == KEY_DOWN || readCharacter == KEY_UP || utils::helpers::Lambda::isBackspace(readCharacter) || U'\t' == readCharacter || 27 == readCharacter || ctrl('k') == readCharacter || ctrl('x') == readCharacter || '\n' == readCharacter)
            {
                return readCharacter;
            }
        }

        if (ERR == readCharacter)
        {
            return static_cast<char32_t>(ERR);
        }
        return static_cast<char32_t>(readCharacter);
    }

    char32_t KeyReader::getNextCharacterFromInputBuffer(feather::windows::WindowImplInterface const *windowImpl)
    {
        if (currentFeatherMode != FEATHER_MODE::WRITE_MODE)
        {
            return getNextCharacterFromInputBufferActionReadMode(windowImpl);
        }
        return getNextCharacterFromInputBufferWriteMode(windowImpl);
    }
} // namespace feather::utils::helpers