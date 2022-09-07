/************************************************
 *     ============= FEATHER ==============      *
 *    |	  Copyright (c) 2022                |    *
 *    |        (0xbaca@gmail.com)           |    *
 *    |        Created by: Mateusz Wojtczak |    *
 *     ====================================      *
 ************************************************/
#include "reader/KeyStroke.hpp"
#include "utils/NtpClient.hpp"
#include "utils/logger/policy/FileLogPolicy.hpp"
#include "utils/datatypes/Strings.hpp"
#include "utils/exception/FeatherInvalidArgumnetException.hpp"
#include "utils/exception/FeatherFileNotExistException.hpp"
#include "utils/ProgramOptionsParser.hpp"
#include "utils/storage/FileStorageFactory.hpp"
#include "windows/NCursesWindowsFactory.hpp"
#include "windows/WindowsManager.hpp"

#include <csetjmp>
#include <csignal>
#include <execinfo.h>
#include <filesystem>
#include <unistd.h>

#ifdef _FEATHER_TEST_
#include "googletest/include/gtest/gtest.h"
#include "googlemock/include/gmock/gmock.h"
#endif

using namespace feather::config;
using namespace feather::printer;
using namespace feather::reader;
using namespace feather::utils;
using namespace feather::utils::storage;
using namespace feather::windows;

extern std::chrono::time_point<std::chrono::steady_clock> lastKeyPressed;
extern bool ncursesInitialized;
feather::utils::FEATHER_MODE currentFeatherMode = feather::utils::FEATHER_MODE::READ_MODE;
volatile bool interrupted = false;
bool isHexMode = false;

std::unique_ptr<const Configuration> configuration;
std::shared_ptr<logger::policy::FileLogPolicy> globalLog;
std::shared_ptr<KeyStroke> globalKeyStroke;
static constexpr auto CONFIG_FILE_NAME = "featherConfig.json";
std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> toCodePointConverter;
std::wstring_convert<std::codecvt_utf8<wchar_t>> toUtf8BytesConverter;

#define NCURSES_SP_NAME(name) name##_sp

void sigBusHandler(int sig)
{
	(*globalLog)(logger::ERROR, "\t", "Fatal, file not available anymore...");
}
struct sigaction oldact, newact;
extern "C" void sigContHandler(int sig)
{
	// Flush buffer
	feather::utils::NCursesWrapper::flushInputBuffer();
}

extern "C" void sigIntHandler(int sig)
{
	// DO NOTHING
}

static void terminateHandler(int sig)
{
#ifdef _DEBUG_MODE_
	void *trace_elems[128];
	int trace_elem_count(backtrace(trace_elems, 128));
	char **stack_syms(backtrace_symbols(trace_elems, trace_elem_count));
	for (int i = 0; i < trace_elem_count; ++i)
	{
		std::cerr << stack_syms[i] << std::endl;
	}
	std::cerr << "Feather crashed, check the logs..." << std::endl;
	feather::utils::NCursesWrapper::setCursor();
	endwin();
	free(stack_syms);
	exit(1);
#endif
}

static void terminateHandler()
{
#ifdef _DEBUG_MODE_
	void *trace_elems[128];
	int trace_elem_count(backtrace(trace_elems, 128));
	char **stack_syms(backtrace_symbols(trace_elems, trace_elem_count));
	for (int i = 0; i < trace_elem_count; ++i)
	{
		std::cerr << stack_syms[i] << std::endl;
	}
	std::cerr << "Feather crashed, check the logs..." << std::endl;
	feather::utils::NCursesWrapper::setCursor();
	endwin();
	free(stack_syms);
	exit(1);
#endif
}

static void sigTermHandler(int sig)
{
	feather::utils::NCursesWrapper::endWindow();
	exit(0);
}

static void unblockSignals()
{
	sigset_t mask;
	sigemptyset(&mask);
	sigaddset(&mask, SIGBUS);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGSEGV);
	pthread_sigmask(SIG_UNBLOCK, &mask, NULL);
}

int main(int argc, const char *argv[])
{
	std::stringstream errorMessage;
	// Install signals handlers
	std::set_terminate(terminateHandler);
	std::signal(SIGBUS, sigBusHandler);
	std::signal(SIGSEGV, terminateHandler);
	std::signal(SIGINT, sigIntHandler);
	std::signal(SIGTERM, sigTermHandler);
	// Clear jump buffers

	// Unblock signal on main thread
	unblockSignals();

#ifdef _FEATHER_TEST_
	std::shared_ptr<logger::policy::FileLogPolicy> testLog = std::make_shared<logger::policy::FileLogPolicy>("./tst/ut/helpers/feather.log");
	globalLog = testLog;
	configuration.reset(new Configuration(CONFIG_FILE_NAME, testLog));
	::testing::InitGoogleTest(&argc, const_cast<char **>(argv));
	return RUN_ALL_TESTS();
#endif
#ifdef _DEBUG_MODE_
	std::shared_ptr<logger::policy::FileLogPolicy> log = std::make_shared<logger::policy::FileLogPolicy>(Configuration::getLogFilePath());
	globalLog = log;
#else
	std::shared_ptr<logger::policy::FileLogPolicy> log;
#endif
	try
	{
		configuration.reset(new Configuration(CONFIG_FILE_NAME, log));
		if (-1 == ProgramOptionsParser::parseArguments(argc, argv))
		{
			return 1;
		}
		configuration->overrideConfiguration();
		isHexMode = configuration->isHexMode();
		// Start windows manager
		std::shared_ptr<AbstractStorageFactory> storageFactory = std::make_shared<FileStorageFactory>();
		std::shared_ptr<WindowsManager> windowsManager = WindowsManager::getInstance(NCursesWindowsFactory::getInstance(log), log);

		// Create all windows
		auto storage = storageFactory->getStorageWithUtf8Check(argv[1]);
		{
			auto mainWindow = windowsManager->addNCursesMainWindow(std::make_unique<BufferFiller>(storage, storageFactory), ProgramOptionsParser::getExistingWindowUUID());
			if (configuration->isHexMode())
			{
				windowsManager->addSecondaryNCursesMainWindowForHexMode(std::make_unique<BufferFiller>(storage, storageFactory), mainWindow->getUUID());
			}
			windowsManager->addNCursesProgressWindow(mainWindow);
			windowsManager->addNCursesInfoWindow(mainWindow);
			windowsManager->addNCursesTopWindow(mainWindow);
		}
		std::shared_ptr<KeyStroke> keyStroke = std::make_shared<KeyStroke>(windowsManager, log, storageFactory);
		globalKeyStroke = keyStroke;

		// Initially set sigcont handler
		newact.sa_handler = sigContHandler;
		newact.sa_flags = 0;
		sigemptyset(&newact.sa_mask);
		sigaction(SIGCONT, &newact, &oldact);

		// Start main loop
		(*keyStroke)();
	}
	catch (exception::FeatherInvalidArgumentException const &e)
	{
		errorMessage << "Feather: " << e.what() << std::endl;
		(*log)(logger::ERROR, ">>>>>>>>>> :", e.what(), "<<<<<<<<<<");
	}
	catch (std::filesystem::filesystem_error const &e)
	{
		std::string error(e.what());
		if (error.find(": unspecified iostream_category error") != std::string::npos)
		{
			error.erase(error.find(": unspecified iostream_category error"));
		}
		errorMessage << "Feather: " << error << std::endl;
		(*log)(logger::ERROR, ">>>>>>>>>> :", e.what(), "<<<<<<<<<<");
	}
	catch (std::exception const &e)
	{
		errorMessage << "Feather: " << e.what() << std::endl;
		(*log)(logger::ERROR, ">>>>>>>>>> :", e.what(), "<<<<<<<<<<");
		(*log)(logger::ERROR, "Stacktrace");
	}

	if (errorMessage.tellp() != std::streampos(0))
	{
		if (ncursesInitialized)
		{
			feather::utils::NCursesWrapper::endWindow();
		}
		std::cerr << errorMessage.str();
		exit(-1);
	}
	exit(0);
}
