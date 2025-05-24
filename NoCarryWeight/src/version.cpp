#include "version.h"
#include <DetourModKit/logger.hpp>

namespace Version
{
    void logVersionInfo()
    {
        auto &logger = DetourModKit::Logger::getInstance();
        logger.log(DetourModKit::LOG_INFO, std::string(MOD_NAME) + " " + VERSION_TAG);
        logger.log(DetourModKit::LOG_INFO, "By " + std::string(AUTHOR));
        logger.log(DetourModKit::LOG_INFO, "Source: " + std::string(REPOSITORY));
        logger.log(DetourModKit::LOG_DEBUG, "Built on " + std::string(BUILD_DATE) + " at " + std::string(BUILD_TIME));
    }
}
