#pragma once

#include <string>

namespace feather::utils::datatypes
{
    struct Strings
    {
        // Dir names
        inline static const char32_t HOME_DIRECTORY = U'~';
        inline static const std::string CURR_DIRECTORY = "./";

        // Storage names
        inline static const std::string UNDO_STACK_NAME = "us";
        inline static const std::string STORAGE_EXTENSION = ".fea";
        inline static const std::string TEMP_FILE_SUFFIX = ".tmp";
        inline static const std::string COLORS_FILE_NAME = "_c.fea";
        inline static const std::string DELETIONS_FILE_NAME = "_d.fea";
        inline static const std::string INITIAL_SEARCH_TO_CURSOR = "1_IS_";
        inline static const std::string INITIAL_SEARCH_FROM_CURSOR = "2_IS_";
        inline static const std::string SECONDARY_SEARCH = "SS_";

        // BOTTOM BAR WINDOW ERRORS
        inline static const std::u32string BYTE_DOES_NOT_EXIST = U"Byte does not exist...";
        inline static const std::u32string CACHE_STASHED = U"Changes stashed";
        inline static const std::u32string CACHE_NOTHING_TO_STASH = U"Nothing to stash...";
        inline static const std::u32string CACHE_TURNED_OFF = U"Cache is turned off...";
        inline static const std::u32string DROPPPED_ALL_BREAKPOINTS = U"Dropped all breakpoints";
        inline static const std::u32string DISK_NO_SPACE_LEFT = U"Not enough disk space...";
        inline static const std::u32string FILE_CONTENT_CHANGED = U"File was modified!";
        inline static const std::u32string FILE_DELETED_DURING_SAVING = U"Fail, source file deleted during saving...";
        inline static const std::u32string FILE_IS_FOLDER = U"File is folder...";
        inline static const std::u32string FILE_DOES_NOT_EXIST = U"File does not exist...";
        inline static const std::u32string FOLDER_DOES_NOT_EXIST = U"Folder does not exist...";
        inline static const std::u32string FOLDER_NO_PERMISSIONS_TO_WRITE = U"No permissions to write to that folder...";
        inline static const std::u32string INVALID_REGEX = U"Invalid regex...";
        inline static const std::u32string LINE_DOES_NOT_EXIST = U"Line does not exist...";
        inline static const std::u32string NO_CHANGES_MADE = U"No changes";
        inline static const std::u32string NOT_ENOUGH_ARGUMENTS = U"Not enough arguments";
        inline static const std::u32string NOT_ENOUGH_OR_WRONG_ARGUMENTS = U"Not enough or wrong arguments";
        inline static const std::u32string NOT_FOUND = U"Not found";
        inline static const std::u32string REMOVE_UNTIL_SINGLE_CHARACTER_ALLOWED = U"Only single character allowed...";
        inline static const std::u32string TERMINAL_NOT_SUPPORT_COLORS = U"Colors are not supported...";
        inline static const std::u32string UNRECOGNIZED_COMMAND = U"Unrecognized command...";
        inline static const std::u32string UNSUPPORTED_COLOR = U"Unsupported color";

        // Other
        inline static const std::string LICENSE_EXPIRED = "The license has expired...";
        inline static const std::u32string ADDED_BREAKPOINT = U"Added breakpoint";
        inline static const std::u32string BYTES = U"BYTES";
        inline static const std::u32string EMPTY_STRING = U"";
        inline static const std::u32string FILE_ALREADY_EXIST = U"File already exist. Override? y/n";
        inline static const std::u32string FILE_NO_LONGER_AVAILABLE = U"File no longer available";
        inline static const std::u32string FILE_WAS_MODIFIED = U"File was concurrently modified. Override? y/n";
        inline static const std::u32string INTERRUPTED = U"Interrupted";
        inline static const std::u32string INVALID_HEX_STRING = U"Invalid hex string";
        inline static const std::u32string INVALID_REPLACE_STRING = U"Invalid replace string";
        inline static const std::u32string NO_BREAKPOINTS_SET = U"No breakpoints set...";
        inline static const std::u32string NOTHING_TO_PASTE = U"Nothing to paste";
        inline static const std::u32string NOTHING_TO_UNDO = U"Nothing to undo";
        inline static const std::u32string NOTHING_TO_REDO = U"Nothing to redo";
        inline static const std::u32string NOT_ENOUGH_PERMISSIONS = U"Not enough permissions";
        inline static const std::u32string COPIED_LINE = U"Copied line";
        inline static const std::u32string CUT_LINE = U"Cut line";
        inline static const std::u32string COPIED_TEXT = U"Copied text";
        inline static const std::u32string CUT_TEXT = U"Cut text";
        inline static const std::u32string HIGHLIGHTED_LINE = U"Highlighted line";
        inline static const std::u32string SAVE_SUCCESS = U"Saved successful";
        inline static const std::u32string WINDOW_SWITCH_THE_ONLY_WINDOW = U"This is the only window";
        inline static const std::u32string UNKNOWN_PROBLEM = U"Unknown problem occured";
        inline static const std::u32string WORKING_STRING = U"Working...";

        // Modes
        inline static const std::u32string BROWSE_SEARCH_MODE = U"-- SEARCH MODE --";
        inline static const std::u32string BROWSE_SEARCH_DOWN_MODE = U"-- SEARCH DOWN MODE --";
        inline static const std::u32string BROWSE_SEARCH_UP_MODE = U"-- SEARCH UP MODE --";
        inline static const std::u32string READ_MODE = U"-- READ_MODE --";
        inline static const std::u32string WRITE_MODE = U"-- WRITE_MODE --";

        // ACTION mode
        inline static const std::u32string SAVING = U"Saving";
        inline static const std::u32string SAVE_INTERRUPTED = U"Interrupted. Remove partially saved file? y/n";

        // SEARCH MODE
        inline static const std::u32string NOTHING_WAS_FOUND = U"Nothing was found...";
        inline static const std::u32string SEARCHING_DOWN = {U'S', U'e', U'a', U'r', U'c', U'h', U'i', U'n', U'g', U' ', 0x9386E2};
        inline static const std::u32string SEARCHING_UP = {U'S', U'e', U'a', U'r', U'c', U'h', U'i', U'n', U'g', U' ', 0x9186E2};

        // SHORTCUTS HEADERS
        inline static const std::u32string EDIT_STRING = U"      EDIT           ";
        inline static const std::u32string MOVE_STRING = U"      MOVE           ";
        inline static const std::u32string REMOVE_STRING = U"      REMOVE         ";
        inline static const std::u32string OTHERS_STRING = U"      OTHER         ";
        inline static const std::u32string TRIAL_TIMEOUT =       U"    Trial has timed out.     ";
    };
} // namespace feather::utils::datatypes
