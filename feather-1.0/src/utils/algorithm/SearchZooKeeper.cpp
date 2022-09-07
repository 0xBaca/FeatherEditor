#include <config/Config.hpp>
#include <utils/algorithm/SearchZooKeeper.hpp>

extern std::unique_ptr<const feather::config::Configuration> configuration;

namespace feather::utils::algorithm
{
    SearchZooKeeper::SearchZooKeeper()
    {
        firstPrevIntervalFetch = true;
        prevIntervalHasRewinded = nextIntervalAleadyHasRewinded = false;
    }

    SearchZooKeeper::SearchZooKeeper(size_t middlePosArg, size_t maxPosArg)
        : maxPos(maxPosArg), minIntervalSize(1UL), nextPos(middlePosArg), prevPos(middlePosArg), middlePos(middlePosArg), nextIntervalHasRewinded(false), prevIntervalHasRewinded(false)
    {
        firstPrevIntervalFetch = true;
        prevIntervalHasRewinded = prevIntervalAleadyHasRewinded = nextIntervalHasRewinded = nextIntervalAleadyHasRewinded = false;
    }

    SearchZooKeeper &SearchZooKeeper::operator=(SearchZooKeeper const &other)
    {
        maxPos = other.maxPos;
        minIntervalSize = other.minIntervalSize;
        nextPos = other.nextPos;
        prevPos = other.prevPos;
        nextIntervalHasRewinded = other.nextIntervalHasRewinded;
        firstPrevIntervalFetch = other.firstPrevIntervalFetch;
        nextIntervalAleadyHasRewinded = other.nextIntervalAleadyHasRewinded;
        prevIntervalAleadyHasRewinded = other.prevIntervalAleadyHasRewinded;
        prevIntervalHasRewinded = other.prevIntervalHasRewinded;
        return *this;
    }

    std::optional<pair> SearchZooKeeper::getNextInterval()
    {
        if ((prevIntervalHasRewinded || nextIntervalHasRewinded) && prevPos - nextPos < minIntervalSize)
        {
            return std::nullopt;
        }

        if (!nextIntervalAleadyHasRewinded && maxPos - nextPos < minIntervalSize)
        {
            nextIntervalHasRewinded = true;
            nextIntervalAleadyHasRewinded = true;
            nextPos = 0;
            if (prevPos - nextPos < minIntervalSize)
            {
                return std::nullopt;
            }
        }

        size_t interval = 0, intervalDelta = 0;
        if (!nextIntervalHasRewinded && !prevIntervalHasRewinded)
        {
            interval = std::min(configuration->getMemoryBytesRelaxed(), maxPos - nextPos);
            intervalDelta = std::min(DELTA, maxPos - nextPos >= interval ? maxPos - interval - nextPos : maxPos - nextPos);
        }
        else if (nextIntervalHasRewinded || prevIntervalHasRewinded)
        {
            interval = std::min(configuration->getMemoryBytesRelaxed(), prevPos - nextPos);
            intervalDelta = std::min(DELTA, std::min(maxPos - nextPos, prevPos - nextPos));
        }
        size_t lastNextPos = nextPos;
        nextPos += interval;
        return pair(interval + intervalDelta, lastNextPos);
    }

    std::optional<pair> SearchZooKeeper::getPrevInterval()
    {
        size_t localDelta = DELTA;
        if (firstPrevIntervalFetch)
        {
            localDelta = configuration->getMemoryBytesRelaxed();
        }

        if ((nextIntervalHasRewinded || prevIntervalHasRewinded) && prevPos - nextPos < minIntervalSize)
        {
            return std::nullopt;
        }

        if (!prevIntervalHasRewinded && prevPos < minIntervalSize)
        {
            prevIntervalAleadyHasRewinded = true;
            prevIntervalHasRewinded = true;
            prevPos = maxPos;
            if (prevPos - nextPos < minIntervalSize)
            {
                return std::nullopt;
            }
        }

        size_t interval = 0, intervalDelta = 0;
        if (!nextIntervalHasRewinded && !prevIntervalHasRewinded)
        {
            interval = std::min(configuration->getMemoryBytesRelaxed(), prevPos);
            intervalDelta = std::min(localDelta, maxPos - prevPos);
        }
        else if (prevIntervalHasRewinded || nextIntervalHasRewinded)
        {
            interval = std::min(configuration->getMemoryBytesRelaxed(), prevPos - nextPos);
            intervalDelta = std::min(localDelta, std::min(maxPos - (prevPos - nextPos), maxPos - prevPos));
        }
        size_t lastPrevPos = prevPos - interval;
        prevPos -= interval;
        firstPrevIntervalFetch = false;
        return pair(interval + intervalDelta, lastPrevPos);
    }

    size_t SearchZooKeeper::bytesTaken() const
    {
        size_t diff = maxPos;
        if (!hasPrevIntervalRewinded() && !hasNextIntervalRewinded())
        {
            diff = nextPos - prevPos;
        }
        else if (hasPrevIntervalRewinded())
        {
            diff = middlePos + (maxPos - prevPos) + (nextPos - middlePos);
        }
        else
        {
            diff = (middlePos - prevPos) + (maxPos - middlePos) + nextPos;
        }
        return (maxPos - diff) < minIntervalSize ? maxPos : diff;
    }

    bool SearchZooKeeper::hasNextIntervalRewinded() const
    {
        return nextIntervalHasRewinded;
    }

    bool SearchZooKeeper::hasPrevIntervalRewinded() const
    {
        return prevIntervalHasRewinded;
    }

    bool SearchZooKeeper::nextIntervalAleadyHasRewinded = false;
    bool SearchZooKeeper::prevIntervalAleadyHasRewinded = false;
}
