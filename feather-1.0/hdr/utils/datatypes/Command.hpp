#pragma once

namespace feather::utils::datatypes
{
    enum class Command
    {
        BREAKPOINT_ADD,
        BREAKPOINT_NEXT,
        BREAKPOINT_PREVIOUS,
        COPY_LINE,
        GOTO_FILE_BEGIN,
        GOTO_FILE_END,
        GOTO_LINE_BEGIN,
        GOTO_LINE_END,
        GOTO_NEXT_WORD,
        GOTO_PREVIOUS_WORD,
        HIGHLIGHT_LINE,
        HIGHLIGHT_BETWEEN_BRACKETS,
        NONE,
        PASTE,
        REMOVE_BETWEEN_BRACKETS,
        REMOVE_BETWEEN_WHITE_CHARACTERS,
        REMOVE_LINE,
        REMOVE_MULTIPLE_LINES,
        REMOVE_SINGLE_CHAR,
        REMOVE_UNTIL_FILE_BEGIN,
        REMOVE_UNTIL_FILE_END,
        REMOVE_UNTIL_LINE_END,
        REMOVE_WORD_BACKWARD,
        REMOVE_WORD_FORWARD,
        SHIFT_RIGHT,
        SHIFT_LEFT
    };
}