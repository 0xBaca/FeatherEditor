.DEFAULT_GOAL := release
PLATFORM := $(shell uname -s)

#BEFORE
_createFolders := $(shell mkdir -p     \
        lib/googlemock/bld             \
        lib/googletest/bld             \
    	obj/src/action                 \
        obj/src/action/input           \
        obj/src/action/output          \
        obj/src/cache                  \
        obj/src/config                 \
        obj/src/printer                \
        obj/src/reader                 \
        obj/src/utils/algorithm        \
        obj/src/utils/datatypes        \
        obj/src/utils/helpers          \
        obj/src/utils/logger           \
        obj/src/utils/storage          \
        obj/src/utils/windows          \
        obj/src/windows                \
        obj/src/windows/action         \
        obj/tst/integ                  \
        obj/tst/ut/action              \
        obj/tst/ut/cache               \
        obj/tst/ut/config              \
        obj/tst/ut/helpers             \
        obj/tst/ut/printer             \
        obj/tst/ut/utils               \
        obj/tst/ut/utils/algorithm     \
        obj/tst/ut/utils/helpers       \
        obj/tst/ut/utils/storage       \
        obj/tst/ut/utils/windows       \
        obj/tst/ut/utils/logger/policy \
        obj/tst/ut/windows)

#TARGET COMPILER
ifeq ($(PLATFORM), Darwin)
    CXX := clang++
    COMPILER := darwin
    COMPILER_VERSION := darwin
else
    CXX := clang++
    COMPILER := darwin
    COMPILER_VERSION := $(shell gcc --version | grep ^gcc | sed 's/^.* //g')
endif

#COMPILER FLAGS
ifeq ($(PLATFORM), Darwin)
    #OSX
    ifeq ($(TEST), true)
    COMPILER_FLAGS := -O3 -Wall -Werror -v -fmessage-length=0 -std=c++17 -g -O0 -fPIC -D _FEATHER_TEST_ -D_XOPEN_SOURCE_EXTENDED -D_GNU_SOURCE -D_OS_OSX_ -D _DEBUG_MODE_ $(CFLAGS)
    #RELEASE_LIB_PATH := -lboost_system
    else ifeq ($(DEBUG), true)
    COMPILER_FLAGS := -O3 -Wall -v -fmessage-length=0 -std=c++17 -fPIC -pg -g -fno-inline -D_XOPEN_SOURCE_EXTENDED -D _DEBUG_MODE_ -D_GNU_SOURCE -D_OS_OSX_ $(CFLAGS)
    else
    COMPILER_FLAGS := -O3 -Wall -Werror -v -fmessage-length=0 -std=c++17 -stdlib=libc++ -fPIC -fmacro-backtrace-limit=0 -D_XOPEN_SOURCE_EXTENDED -D_GNU_SOURCE -D_OS_OSX_ $(CFLAGS)
    RELEASE_LIB_PATH := -std=c++17 -fmacro-backtrace-limit=0
    endif
else
#ASSUMING WE ARE ON LINUX
    ifeq ($(TEST), true)
    COMPILER_FLAGS := -O3 -Wall -Werror -v -fmessage-length=0 -std=c++17 -g3 -fPIC -D _FEATHER_TEST_ -D_OS_LINUX_ -D _DEBUG_MODE_ $(CFLAGS)
    RELEASE_LIB_PATH := -lc++ -lc++abi -lboost_system -lstdc++fs
    else ifeq ($(DEBUG), true)
    COMPILER_FLAGS := -O3 -Wall -v -fmessage-length=0 -std=c++17 -fPIC -pg -g -fno-inline -D_XOPEN_SOURCE_EXTENDED -D _DEBUG_MODE_ -D_GNU_SOURCE -D_OS_OSX_ $(CFLAGS)
    else
    COMPILER_FLAGS := -O2 -Wall -v -fmessage-length=0 -std=c++17 -stdlib=libc++ -fPIC -fmacro-backtrace-limit=0 -D_XOPEN_SOURCE_EXTENDED -D_GNU_SOURCE
    endif
endif

#PATHS
INCLUDES := -I./hdr -I./tst/ut -I./tst/integ -I./lib/googletest/include -I./lib/googlemock/include -I./lib -I./lib/encryption -I/Users/mateuszwojtczak/Downloads/boost_1_75_0 -I/usr/include/c++/9 -I/usr/include/aarch64-linux-gnu/c++/9 -I/usr/include/c++/11 -I/usr/include/c++/11/aarch64-redhat-linux -I/usr/include/c++/12 -I/usr/include/c++/12/x86_64-redhat-linux -I/usr/include/c++/9 -I/usr/include/x86_64-linux-gnu/c++/9
OBJ_DIRECTORY := ./obj/src
UT_DIR := ./tst/ut
UT_OBJ_DIRECTORY := ./obj/tst/ut
INTEGT_OBJ_DIRECTORY := ./obj/tst/integ
UT_INTEG_OBJ_DIRECTORY := ./obj/tst/integ

#LIBS
ifeq ($(PLATFORM), Darwin)
    ifeq ($(TEST), true)
    LIBS := -lpthread -ldl -lgmock -lgtest -lncurses
    else ifeq ($(DEBUG), true)
    LIBS := -lpthread -ldl -lncurses -lpanel
    else
    LIBS := -lpthread -ldl -lncurses -lpanel
    endif
else
    ifeq ($(TEST), true)
    LIBS := -lpthread -ldl -lgmock -lgtest -lncursesw
    else ifeq ($(DEBUG), true)
    LIBS := -lpthread -ldl -lncursesw -lpanel
    else
    LIBS := -lpthread -ldl -lncursesw -lpanel
    endif
endif

#RELEASE_OBJECTS
ACTION_SRC_OBJECTS := $(OBJ_DIRECTORY)/action/Action.o $(OBJ_DIRECTORY)/action/ActionHandler.o $(OBJ_DIRECTORY)/action/AlignToMiddleOfScreenAction.o $(OBJ_DIRECTORY)/action/input/AlignToMiddleOfScreenActionInput.o $(OBJ_DIRECTORY)/action/output/AlignToMiddleOfScreenActionOutput.o $(OBJ_DIRECTORY)/action/CopyLineAction.o $(OBJ_DIRECTORY)/action/input/CopyLineActionInput.o $(OBJ_DIRECTORY)/action/output/CopyLineActionOutput.o $(OBJ_DIRECTORY)/action/EditLineAboveAction.o $(OBJ_DIRECTORY)/action/input/EditLineAboveActionInput.o $(OBJ_DIRECTORY)/action/output/EditLineAboveActionOutput.o $(OBJ_DIRECTORY)/action/EditLineBelowAction.o $(OBJ_DIRECTORY)/action/input/EditLineBelowActionInput.o $(OBJ_DIRECTORY)/action/output/EditLineBelowActionOutput.o $(OBJ_DIRECTORY)/action/EditNextCharacterAction.o $(OBJ_DIRECTORY)/action/input/EditNextCharacterActionInput.o $(OBJ_DIRECTORY)/action/output/EditNextCharacterActionOutput.o $(OBJ_DIRECTORY)/action/GotoLineAction.o $(OBJ_DIRECTORY)/action/input/GotoLineActionInput.o $(OBJ_DIRECTORY)/action/output/GotoLineActionOutput.o $(OBJ_DIRECTORY)/action/GetStringsAction.o $(OBJ_DIRECTORY)/action/input/GetStringsActionInput.o $(OBJ_DIRECTORY)/action/output/GetStringsActionOutput.o $(OBJ_DIRECTORY)/action/HighlightBetweenBracketsAction.o $(OBJ_DIRECTORY)/action/input/HighlightBetweenBracketsActionInput.o $(OBJ_DIRECTORY)/action/output/HighlightBetweenBracketsActionOutput.o $(OBJ_DIRECTORY)/action/JumpToBracketAction.o $(OBJ_DIRECTORY)/action/input/JumpToBracketActionInput.o $(OBJ_DIRECTORY)/action/output/JumpToBracketActionOutput.o $(OBJ_DIRECTORY)/action/MoveToByteAction.o $(OBJ_DIRECTORY)/action/input/MoveToByteActionInput.o $(OBJ_DIRECTORY)/action/output/MoveToByteActionOutput.o $(OBJ_DIRECTORY)/action/MoveToFileBeginAction.o $(OBJ_DIRECTORY)/action/input/MoveToFileBeginActionInput.o $(OBJ_DIRECTORY)/action/output/MoveToFileBeginActionOutput.o $(OBJ_DIRECTORY)/action/MoveToFileEndAction.o $(OBJ_DIRECTORY)/action/input/MoveToFileEndActionInput.o $(OBJ_DIRECTORY)/action/output/MoveToFileEndActionOutput.o $(OBJ_DIRECTORY)/action/MoveToLineBeginAction.o $(OBJ_DIRECTORY)/action/input/MoveToLineBeginActionInput.o $(OBJ_DIRECTORY)/action/output/MoveToLineBeginActionOutput.o $(OBJ_DIRECTORY)/action/MoveToLineEndAction.o $(OBJ_DIRECTORY)/action/input/MoveToLineEndActionInput.o $(OBJ_DIRECTORY)/action/output/MoveToLineEndActionOutput.o $(OBJ_DIRECTORY)/action/MoveToLineEndWithEditAction.o $(OBJ_DIRECTORY)/action/input/MoveToLineEndWithEditActionInput.o $(OBJ_DIRECTORY)/action/output/MoveToLineEndWithEditActionOutput.o $(OBJ_DIRECTORY)/action/MoveToNextWordAction.o $(OBJ_DIRECTORY)/action/input/MoveToNextWordActionInput.o $(OBJ_DIRECTORY)/action/output/MoveToNextWordActionOutput.o $(OBJ_DIRECTORY)/action/MoveToPreviousWordAction.o $(OBJ_DIRECTORY)/action/input/MoveToPreviousWordActionInput.o $(OBJ_DIRECTORY)/action/output/MoveToPreviousWordActionOutput.o $(OBJ_DIRECTORY)/action/PasteAction.o $(OBJ_DIRECTORY)/action/input/PasteActionInput.o $(OBJ_DIRECTORY)/action/output/PasteActionOutput.o $(OBJ_DIRECTORY)/action/RemoveAllAction.o $(OBJ_DIRECTORY)/action/input/RemoveAllActionInput.o $(OBJ_DIRECTORY)/action/output/RemoveAllActionOutput.o $(OBJ_DIRECTORY)/action/RemoveBetweenBracketsAction.o $(OBJ_DIRECTORY)/action/input/RemoveBetweenBracketsActionInput.o $(OBJ_DIRECTORY)/action/output/RemoveBetweenBracketsActionOutput.o $(OBJ_DIRECTORY)/action/RemoveBetweenWhiteCharactersAction.o $(OBJ_DIRECTORY)/action/input/RemoveBetweenWhiteCharactersActionInput.o $(OBJ_DIRECTORY)/action/output/RemoveBetweenWhiteCharactersActionOutput.o $(OBJ_DIRECTORY)/action/RemoveLineAction.o $(OBJ_DIRECTORY)/action/input/RemoveLineActionInput.o $(OBJ_DIRECTORY)/action/output/RemoveLineActionOutput.o $(OBJ_DIRECTORY)/action/RemoveUntilFileEndAction.o $(OBJ_DIRECTORY)/action/input/RemoveUntilFileEndActionInput.o $(OBJ_DIRECTORY)/action/output/RemoveUntilFileEndActionOutput.o $(OBJ_DIRECTORY)/action/RemoveUntilFileBeginAction.o $(OBJ_DIRECTORY)/action/input/RemoveUntilFileBeginActionInput.o $(OBJ_DIRECTORY)/action/output/RemoveUntilFileBeginActionOutput.o $(OBJ_DIRECTORY)/action/RemoveUntilLineEndAction.o $(OBJ_DIRECTORY)/action/input/RemoveUntilLineEndActionInput.o $(OBJ_DIRECTORY)/action/output/RemoveUntilLineEndActionOutput.o $(OBJ_DIRECTORY)/action/RemoveWordBackwardAction.o $(OBJ_DIRECTORY)/action/input/RemoveWordBackwardActionInput.o $(OBJ_DIRECTORY)/action/output/RemoveWordBackwardActionOutput.o $(OBJ_DIRECTORY)/action/RemoveWordForwardAction.o $(OBJ_DIRECTORY)/action/input/RemoveWordForwardActionInput.o $(OBJ_DIRECTORY)/action/output/RemoveWordForwardActionOutput.o $(OBJ_DIRECTORY)/action/RemoveCharacterUnderCursorAction.o $(OBJ_DIRECTORY)/action/input/RemoveCharacterUnderCursorActionInput.o $(OBJ_DIRECTORY)/action/output/RemoveCharacterUnderCursorActionOutput.o $(OBJ_DIRECTORY)/action/SaveChangesAction.o $(OBJ_DIRECTORY)/action/input/SaveChangesActionInput.o $(OBJ_DIRECTORY)/action/output/SaveChangesActionOutput.o $(OBJ_DIRECTORY)/action/SearchSubstringAction.o $(OBJ_DIRECTORY)/action/input/SearchSubstringActionInput.o $(OBJ_DIRECTORY)/action/output/SearchSubstringActionOutput.o $(OBJ_DIRECTORY)/action/ShiftRightAction.o $(OBJ_DIRECTORY)/action/input/ShiftRightActionInput.o $(OBJ_DIRECTORY)/action/output/ShiftRightActionOutput.o $(OBJ_DIRECTORY)/action/SkipEmptyCharactersAction.o $(OBJ_DIRECTORY)/action/input/SkipEmptyCharactersActionInput.o $(OBJ_DIRECTORY)/action/output/SkipEmptyCharactersActionOutput.o $(OBJ_DIRECTORY)/action/SwitchModeAction.o $(OBJ_DIRECTORY)/action/input/SwitchModeActionInput.o $(OBJ_DIRECTORY)/action/output/SwitchModeActionOutput.o

CACHE_SRC_OBJECTS := $(OBJ_DIRECTORY)/cache/Cache.o $(OBJ_DIRECTORY)/cache/List.o

CONFIG_SRC_OBJECTS := $(OBJ_DIRECTORY)/config/Config.o

PRINTER_SRC_OBJECTS := $(OBJ_DIRECTORY)/printer/ChangesStack.o $(OBJ_DIRECTORY)/printer/FilePrintingStorage.o $(OBJ_DIRECTORY)/printer/MemoryPrintingStorage.o $(OBJ_DIRECTORY)/printer/CurrFrameVisitor.o $(OBJ_DIRECTORY)/printer/HalfFrameDownVisitor.o $(OBJ_DIRECTORY)/printer/HalfFrameUpVisitor.o $(OBJ_DIRECTORY)/printer/LineDownVisitor.o $(OBJ_DIRECTORY)/printer/LineUpVisitor.o $(OBJ_DIRECTORY)/printer/FrameDownVisitor.o $(OBJ_DIRECTORY)/printer/FrameUpVisitor.o $(OBJ_DIRECTORY)/printer/PrintingOrchestrator.o $(OBJ_DIRECTORY)/printer/PrintingOrchestratorInterface.o $(OBJ_DIRECTORY)/printer/WindowStatus.o

READER_SRC_OBJECTS := $(OBJ_DIRECTORY)/reader/KeyStroke.o $(OBJ_DIRECTORY)/reader/ActionReader.o $(OBJ_DIRECTORY)/reader/Reader.o $(OBJ_DIRECTORY)/reader/SearchReader.o

UTILS_SRC_OBJECTS := $(OBJ_DIRECTORY)/utils/Filesystem.o $(OBJ_DIRECTORY)/utils/FilesystemInterface.o $(OBJ_DIRECTORY)/utils/Serializer.o $(OBJ_DIRECTORY)/utils/algorithm/FastSearch.o $(OBJ_DIRECTORY)/utils/algorithm/RegexSearch.o $(OBJ_DIRECTORY)/utils/algorithm/SearchBuffer.o $(OBJ_DIRECTORY)/utils/algorithm/SearchEngine.o $(OBJ_DIRECTORY)/utils/algorithm/SearchBufferEntry.o $(OBJ_DIRECTORY)/utils/algorithm/SearchEngineBase.o $(OBJ_DIRECTORY)/utils/algorithm/SearchResultsController.o $(OBJ_DIRECTORY)/utils/algorithm/SearchZooKeeper.o $(OBJ_DIRECTORY)/utils/datatypes/ChangePOD.o $(OBJ_DIRECTORY)/utils/datatypes/DeletionsSnapshot.o $(OBJ_DIRECTORY)/utils/datatypes/Metadata.o $(OBJ_DIRECTORY)/utils/datatypes/Uuid.o $(OBJ_DIRECTORY)/utils/helpers/Conversion.o $(OBJ_DIRECTORY)/utils/helpers/Signal.o $(OBJ_DIRECTORY)/utils/helpers/Lambda.o $(OBJ_DIRECTORY)/utils/helpers/RawCharactersBuffer.o $(OBJ_DIRECTORY)/utils/BufferFiller.o $(OBJ_DIRECTORY)/utils/BufferFillerInterface.o $(OBJ_DIRECTORY)/utils/storage/AbstractStorage.o $(OBJ_DIRECTORY)/utils/storage/AbstractStorageFactory.o $(OBJ_DIRECTORY)/utils/storage/FileStorage.o $(OBJ_DIRECTORY)/utils/storage/FileStorageFactory.o $(OBJ_DIRECTORY)/utils/storage/InMemoryStorageFactory.o $(OBJ_DIRECTORY)/utils/storage/InMemoryStorage.o $(OBJ_DIRECTORY)/utils/ProgramOptionsParser.o $(OBJ_DIRECTORY)/utils/logger/Logger.o $(OBJ_DIRECTORY)/utils/helpers/KeyReader.o $(OBJ_DIRECTORY)/utils/MoveCursorOnScreenStateMachine.o $(OBJ_DIRECTORY)/utils/NCursesWrapper.o $(OBJ_DIRECTORY)/utils/ScreenBuffer.o $(OBJ_DIRECTORY)/utils/windows/FramePositions.o $(OBJ_DIRECTORY)/utils/windows/LinesAroundPositionInformation.o $(OBJ_DIRECTORY)/utils/windows/WindowLinesLengthInformationVisitor.o

WINDOWS_SRC_OBJECTS := $(OBJ_DIRECTORY)/windows/MainWindowInterface.o $(OBJ_DIRECTORY)/windows/NCursesAgreementWindowDecorator.o $(OBJ_DIRECTORY)/windows/NCursesBottomBarWindowDecorator.o $(OBJ_DIRECTORY)/windows/NCursesExitWindowDecorator.o $(OBJ_DIRECTORY)/windows/NCursesInfoWindowDecorator.o $(OBJ_DIRECTORY)/windows/NCursesMainWindowDecorator.o $(OBJ_DIRECTORY)/windows/NCursesProgressWindowDecorator.o $(OBJ_DIRECTORY)/windows/NCursesShortcutsWindowDecorator.o $(OBJ_DIRECTORY)/windows/NCursesTopWindowDecorator.o $(OBJ_DIRECTORY)/windows/NCursesWindow.o $(OBJ_DIRECTORY)/windows/NCursesWindowsFactory.o $(OBJ_DIRECTORY)/windows/WindowImplInterface.o $(OBJ_DIRECTORY)/windows/SubWindowInterface.o $(OBJ_DIRECTORY)/windows/WindowsManager.o $(OBJ_DIRECTORY)/windows/WindowsAbstractFactory.o $(OBJ_DIRECTORY)/feather.o

#ALL_RELEASE_OBJECTS
ALL_SRC_OBJECTS := $(ACTION_SRC_OBJECTS) $(CACHE_SRC_OBJECTS) $(CONFIG_SRC_OBJECTS) $(PRINTER_SRC_OBJECTS) $(READER_SRC_OBJECTS) $(UTILS_SRC_OBJECTS) $(THREAD_POOL_SRC_OBJECTS) $(WINDOWS_SRC_OBJECTS)

#TEST_OBJECTS
ACTION_TST_OBJECTS := $(UT_OBJ_DIRECTORY)/action/ActionHandlerTest.o $(UT_OBJ_DIRECTORY)/action/MoveToByteActionTest.o $(UT_OBJ_DIRECTORY)/action/MoveToFileEndActionTest.o $(UT_OBJ_DIRECTORY)/action/MoveToNextWordActionTest.o $(UT_OBJ_DIRECTORY)/action/MoveToPreviousWordActionTest.o $(UT_OBJ_DIRECTORY)/action/RemoveCharacterUnderCursorActionTest.o $(UT_OBJ_DIRECTORY)/action/RemoveLineActionTest.o $(UT_OBJ_DIRECTORY)/action/SaveChangesActionTest.o $(UT_INTEG_OBJ_DIRECTORY)/JumpToBracketActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/RemoveLineActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/MoveToByteActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/SaveChangesActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/SwitchModeActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/EditNextCharacterActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/RemoveCharacterUnderCursorActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/CopyLineActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/MoveToNextWordActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/MoveToLineEndActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/MoveToLineEndWithEditActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/RemoveUntilLineEndActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/EditLineBelowActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/PasteActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/RemoveBetweenWhiteCharactersActionIntegTest.o 
CACHE_TST_OBJECTS := $(UT_OBJ_DIRECTORY)/cache/CacheTest.o
CONFIG_TST_OBJECTS := $(UT_OBJ_DIRECTORY)/config/ConfigTest.o
HELPERS_TST_OBECTS := $(UT_OBJ_DIRECTORY)/helpers/FileStorageHelper.o $(UT_OBJ_DIRECTORY)/helpers/TestBase.o $(UT_OBJ_DIRECTORY)/helpers/Timeout.o
PRINTER_TST_OBJECTS := $(UT_OBJ_DIRECTORY)/printer/ChangesStackTest.o $(UT_OBJ_DIRECTORY)/printer/FilePrintingStorageTest.o $(UT_OBJ_DIRECTORY)/printer/PrintingOrchestratorTest.o $(UT_OBJ_DIRECTORY)/printer/WindowStatusTest.o $(INTEGT_OBJ_DIRECTORY)/PrintingOrchestartorIntegTest.o
UTILS_TST_OBJECTS := $(UT_OBJ_DIRECTORY)/utils/algorithm/RegexSearchTest.o $(UT_OBJ_DIRECTORY)/utils/helpers/ConversionTest.o $(UT_OBJ_DIRECTORY)/utils/NCursesWrapperTest.o $(UT_OBJ_DIRECTORY)/utils/Utf8UtilTest.o $(UT_OBJ_DIRECTORY)/utils/logger/policy/FileLogPolicyTest.o $(UT_OBJ_DIRECTORY)/utils/ProgramOptionsParserTest.o $(UT_OBJ_DIRECTORY)/utils/MoveCursorOnScreenStateMachineTest.o $(UT_OBJ_DIRECTORY)/utils/ScreenBufferTest.o $(UT_OBJ_DIRECTORY)/utils/algorithm/SearchResultsControllerTest.o $(UT_OBJ_DIRECTORY)/utils/algorithm/SearchZooKeeperTest.o $(UT_INTEG_OBJ_DIRECTORY)/FastSearchIntegTest.o $(UT_OBJ_DIRECTORY)/utils/storage/FileStorageTest.o $(UT_OBJ_DIRECTORY)/utils/windows/FramePositionsTest.o $(UT_OBJ_DIRECTORY)/utils/windows/WindowLinesAroundPositionInformationVisitorTest.o $(UT_INTEG_OBJ_DIRECTORY)/BufferFillerIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/LambdaIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/RemoveWordBackwardActionIntegTest.o $(UT_INTEG_OBJ_DIRECTORY)/SerializerIntegTest.o
WINDOWS_TST_OBJECTS := $(UT_OBJ_DIRECTORY)/windows/WindowsManagerTest.o $(UT_OBJ_DIRECTORY)/windows/NCursesWindowTest.o $(UT_OBJ_DIRECTORY)/windows/NCursesBottomBarWindowDecoratorTest.o $(UT_OBJ_DIRECTORY)/windows/NCursesMainWindowDecoratorTest.o $(UT_OBJ_DIRECTORY)/windows/NCursesWindowsFactoryTest.o $(UT_INTEG_OBJ_DIRECTORY)/NCursesMainWindowDecoratorIntegTest.o

#ALL_TEST_OBJECTS
ALL_TST_OBJECTS := $(ACTION_TST_OBJECTS) $(CACHE_TST_OBJECTS) $(CONFIG_TST_OBJECTS) $(HELPERS_TST_OBECTS) $(PRINTER_TST_OBJECTS) $(UTILS_TST_OBJECTS) $(WINDOWS_TST_OBJECTS)

#OBJECT_FILES
$(OBJ_DIRECTORY)/%.o: src/%.cpp hdr/%.hpp
	$(CXX) $(INCLUDES) -c $< -o $@ $(COMPILER_FLAGS)

$(UT_OBJ_DIRECTORY)/%.o: tst/ut/%.cpp
	$(CXX) -c $< -o $@ $(INCLUDES) $(COMPILER_FLAGS)

$(UT_INTEG_OBJ_DIRECTORY)/%.o: tst/integ/%.cpp
	$(CXX) -c $< -o $@ $(INCLUDES) $(COMPILER_FLAGS)

$(OBJ_DIRECTORY)/feather.o: src/feather.cpp
	$(CXX) -c -o $@ $(INCLUDES) src/feather.cpp $(COMPILER_FLAGS)

#LINKING
release: $(ALL_SRC_OBJECTS)
	$(CXX) $(ALL_SRC_OBJECTS) $(LIBS) -v -rdynamic -o feather

debug: $(ALL_SRC_OBJECTS)
	$(CXX) $(ALL_SRC_OBJECTS) $(REALEASE_LIB_PATH) $(LIBS) -v -rdynamic -o featherDebug

#TESTING
compile_gmock_test_libs:
ifeq (,$(wildcard ./lib/googlemock/bld/libgmock.a))
	@cd ./lib/googlemock/bld && cmake .. && make && cd ../../..
endif

compile_gtest_test_libs:
ifeq (,$(wildcard ./lib/googletest/bld/libgtest.a))
	@cd ./lib/googletest/bld && cmake .. && make && cd ../../..
endif

all_tests: compile_gmock_test_libs compile_gtest_test_libs | $(ALL_SRC_OBJECTS) $(ALL_TST_OBJECTS)
	$(CXX) $(ALL_SRC_OBJECTS) $(ALL_TST_OBJECTS) $(DEBUG_COMPILER_FLAGS) $(RELEASE_LIB_PATH) $(LIBS) -o featherTest && ./featherTest

singleUnitTest: compile_gmock_test_libs compile_gtest_test_libs | $(ALL_SRC_OBJECTS) $(ALL_TST_OBJECTS)
	$(CXX) $(ALL_SRC_OBJECTS) $(ALL_TST_OBJECTS) $(DEBUG_COMPILER_FLAGS) $(RELEASE_LIB_PATH) $(LIBS) -o featherTest && ./featherTest --gtest_filter=${testName}

#CLEANUP
clean:
	@find ./obj/ -type f -exec rm -f {} \; &&            \
	rm -f ./feather &&                                   \
	rm -f ./builds/* &&                                   \
	rm -f ./data/* &&                                    \
	rm -f ./featherTest &&                               \
	rm -f ./featherDebug &&                              \
	rm -f ./test.log &&                                  \
	rm -f ./testLog.txt &&                               \
	rm -rf ./lib/googlemock/CMakeCache.txt &&            \
	rm -rf ./lib/googlemock/bld/*       &&               \
	rm -rf ./lib/googletest/CMakeCache.txt  &&           \
	rm -rf ./lib/googletest/bld/*     &&                 \
	rm -rf ./lib/googletest/CMakeFiles                   \
	rm -rf ../rpmbuild/BUILD/*                           \
	rm -rf ../rpmbuild/BUILDROOT/*                       \
	rm -rf ../rpmbuild/RPMS/*                            \
	rm -rf ../debian/feather-1.0-1/src ../debian/feather-1.0-1/hdr ../debian/feather-1.0-1/lib ../debian/feather-1.0-1/scripts ../debian/feather-1.0-1/tst ../debian/feather-1.0-1/feather ../debian/feather-1.0-1/makefile ../debian/feather-1.0-1/feather.1 ../debian/feather-1.0-1/README.md ../debian/feather-1.0-1/featherConfig.json ../debian/feather.debhelper.log ../debian/feather.substvars
	rm -f ../debian/feather-1.0_1-1_amd64.buildinfo  ../debian/feather-1.0_1-1_amd64.changes  ../debian/feather-1.0_1-1_amd64.deb  ../debian/feather-1.0-1.x86_64.tar.gz &&                                    \
	rm -rf ../rpmbuild/SOUCES/* \
	rm ../rpmbuild/SRPMS/feather-1.0-1.src.rpm \
	rm -rf ../rpmbuild/RPMS/aarch64/* \
	rm ../rpmbuild/SOURCES/* \
	rm ../rpmbuild/SRPMS/* \
	rm ../rpmbuild/RPMS/aarch64/* \
	rm -rf ../debian/*.deb \
	rm -rf ../debian/*.ddeb \
	rm -rf ../debian/*.dsc \
	#rm -rf ../debian/feather-dbgsym_1-1_arm64.ddeb \
	rm -rf ../debian/feather-1.0_1-1_arm64.buildinfo \
	rm -rf ../debian/feather-1.0_1-1_arm64.changes


#INSTALL
install_rpm_x86_64:
	mkdir -p $(BUILDROOT)/feather-1.0-1.x86_64/usr/local/bin
	mkdir -p $(BUILDROOT)/feather-1.0-1.x86_64/usr/share/man/man1
	cp feather.1 $(BUILDROOT)/feather-1.0-1.x86_64/usr/share/man/man1
	cp featherConfig.json $(BUILDROOT)/feather-1.0-1.x86_64
	cp feather $(BUILDROOT)/feather-1.0-1.x86_64/usr/local/bin

install_rpm_arm_64:
	mkdir -p $(BUILDROOT)/feather-1.0-1.aarch64/usr/local/bin
	mkdir -p $(BUILDROOT)/feather-1.0-1.aarch64/usr/share/man/man1
	cp feather.1 $(BUILDROOT)/feather-1.0-1.aarch64/usr/share/man/man1
	cp featherConfig.json $(BUILDROOT)/feather-1.0-1.aarch64
	cp feather $(BUILDROOT)/feather-1.0-1.aarch64/usr/local/bin
