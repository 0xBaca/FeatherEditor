#include "action/SaveChangesAction.hpp"
#include "utils/BufferFiller.hpp"
#include "utils/Filesystem.hpp"
#include "utils/algorithm/FastSearch.hpp"
#include "utils/helpers/Signal.hpp"
#include "utils/datatypes/Strings.hpp"
#include "reader/KeyStroke.hpp"

extern feather::utils::FEATHER_MODE currentFeatherMode;
extern std::unique_ptr<const feather::config::Configuration> configuration;

extern sig_atomic_t sigIntReceived;
extern sig_atomic_t sigBusReceived;

namespace feather::action
{
    SaveChangesAction::SaveChangesAction(SaveChangesActionInput &&input)
        : Action<SaveChangesActionInput, SaveChangesActionOutput>(input, SaveChangesActionOutput(utils::datatypes::ERROR::NONE, std::nullopt))
    {
    }

    void SaveChangesAction::cleanUp(utils::datatypes::Uuid const &windowUUID)
    {
        getActionInput().printingOrchestrator->removeChangesRelatedToWindow(getActionInput().windowUUID);
    }

    SaveChangesActionOutput SaveChangesAction::execute(std::shared_ptr<utils::FilesystemInterface> filesystem)
    {
        utils::helpers::unblockSignals();
        auto actionStartTime = std::chrono::steady_clock::now();
        bool alreadyRefreshed = false;
        // Saving when source file was deleted is impossible
        if (!filesystem->isFileExist(getActionInput().storage->getName()))
        {
            return SaveChangesActionOutput{utils::datatypes::ERROR::FILE_NO_LONGER_AVAILABLE, std::nullopt};
        }
        auto storage = getActionInput().storageFactory->getStorage(getActionInput().storage->getName(), getActionInput().windowUUID);
        bool replaceOpenedFile = (getActionInput().savedFileName == storage->getName());
        pair firstInvalidPos = getActionInput().printingOrchestrator->getFirstInvalidPosition(getActionInput().windowUUID, storage);
        size_t framePosition = getActionInput().printingOrchestrator->convertVirtualPositionToByteOffset(getActionInput().bufferFiller->getFramePositions().startFramePosition, getActionInput().windowUUID);
        pair currPosition = getActionInput().printingOrchestrator->getFirstValidPosition(getActionInput().windowUUID);

        if (getActionInput().printingOrchestrator->isAnyChangesMade(getActionInput().windowUUID) || !replaceOpenedFile)
        {
            std::shared_ptr<utils::storage::AbstractStorage> tmpFile = replaceOpenedFile ? actionInput.storageFactory->getStorage(utils::helpers::Conversion::toHiddenFile(actionInput.storage->getName()), getActionInput().windowUUID) : actionInput.storageFactory->getStorage(std::string(getActionInput().savedFileName), getActionInput().windowUUID);
            do
            {
                if (sigIntReceived)
                {
                    utils::helpers::blockSignals();
                    return SaveChangesActionOutput{utils::datatypes::ERROR::INTERRUPTED, tmpFile->getName()};
                }
                else if (sigBusReceived)
                {
                    utils::helpers::blockSignals();
                    return SaveChangesActionOutput{utils::datatypes::ERROR::FILE_NO_LONGER_AVAILABLE, tmpFile->getName()};
                }
                auto nextChunk = getActionInput().printingOrchestrator->getContinousCharacters(currPosition, configuration->getMemoryBytesRelaxed(), storage, getActionInput().windowUUID);
                if (nextChunk.first.empty())
                {
                    break;
                }
                tmpFile->writeChunk(nextChunk.first, nextChunk.first.size());
                currPosition = nextChunk.second;
                getActionInput().windowsManager->refreshProgresWindow(getActionInput().currFrameVisitor, getActionInput().windowUUID, std::make_pair(utils::windows::FramePositions(currPosition, pair{0, 0}), currPosition));
                utils::NCursesWrapper::applyRefresh();
                if (nextChunk.second >= firstInvalidPos)
                {
                    break;
                }
#ifndef _FEATHER_TEST_
                // Check if window dimensions has not changed
                timeout(0);
                if (KEY_RESIZE == getActionInput().windowsManager->getMainWindow(getActionInput().windowUUID)->getLibraryWindowHandler()->getCharacter())
                {
                    getActionInput().windowsManager->recreateWindows();
                    getActionInput().windowsManager->refreshAllWindows(getActionInput().currFrameVisitor);
                    utils::helpers::Lambda::subWindowPrint(getActionInput().savedFileNameWideString, std::nullopt, getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getBottomBarWindow(getActionInput().windowUUID).value()), getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getProgressBarWindow(getActionInput().windowUUID).value()), getActionInput().currFrameVisitor);
                    utils::NCursesWrapper::applyRefresh();
                }
                timeout(-1);
#endif
                if (!alreadyRefreshed && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - actionStartTime) > SAVE_GRACE_PERIOD)
                {
                    currentFeatherMode = utils::FEATHER_MODE::ACTION_MODE_SAVING_IN_PROGRESS;
                    getActionInput().windowsManager->refreshAllWindows(getActionInput().currFrameVisitor);
                    utils::helpers::Lambda::subWindowPrint(getActionInput().savedFileNameWideString, std::nullopt, getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getBottomBarWindow(getActionInput().windowUUID).value()), getActionInput().windowsManager->getSubWindow(getActionInput().windowsManager->getProgressBarWindow(getActionInput().windowUUID).value()), getActionInput().currFrameVisitor);
                    utils::NCursesWrapper::applyRefresh();
                    alreadyRefreshed = true;
                }
            } while (true);
        }

        if (replaceOpenedFile)
        {
            std::shared_ptr<utils::storage::AbstractStorage> tmpFile = actionInput.storageFactory->getStorage(utils::helpers::Conversion::toHiddenFile(actionInput.storage->getName()), getActionInput().windowUUID);
            getActionInput().bufferFiller->replaceMappedFile(tmpFile, framePosition);
            utils::algorithm::SearchResultsController::normalizeResultsAfterSave(getActionInput().windowUUID, getActionInput().printingOrchestrator, getActionInput().storage);
            reader::KeyStroke::normalizeAfterSave(getActionInput().printingOrchestrator, getActionInput().windowUUID);
            getActionInput().printingOrchestrator->normalizeAfterSave(getActionInput().windowUUID);
            cleanUp(getActionInput().windowUUID);
        }
        else
        {
            std::shared_ptr<utils::storage::AbstractStorage> savedFile = actionInput.storageFactory->getStorage(std::string(getActionInput().savedFileName), getActionInput().windowUUID);
            savedFile->setPermissions(storage->getPermissions());
        }
        utils::helpers::blockSignals();
        return SaveChangesActionOutput{utils::datatypes::ERROR::NONE, std::nullopt};
    }
} // namespace feather::action