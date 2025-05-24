#include "version.hpp"
#include <DetourModKit.hpp>

namespace Version
{
    void logVersionInfo()
    {
        auto &logger = DMKLogger::getInstance();
        logger.log(DMKLogLevel::LOG_INFO, std::string(MOD_NAME) + " " + VERSION_TAG);
        logger.log(DMKLogLevel::LOG_INFO, "By " + std::string(AUTHOR));
        logger.log(DMKLogLevel::LOG_INFO, "Source: " + std::string(REPOSITORY));
        logger.log(DMKLogLevel::LOG_DEBUG, "Built on " + std::string(BUILD_DATE) + " at " + std::string(BUILD_TIME));
    }
}
