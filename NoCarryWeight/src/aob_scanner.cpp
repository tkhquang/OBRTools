/**
 * @file aob_scanner.cpp
 * @brief Implementation of Array-of-Bytes (AOB) parsing and scanning.
 */

#include "aob_scanner.h"
#include "logger.h"
#include "utils.h"

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cctype>
#include <algorithm>

/**
 * @struct PatternByte
 * @brief Internal helper representing a parsed AOB element.
 */
struct PatternByte
{
    BYTE value;       /**< Byte value (if not wildcard). */
    bool is_wildcard; /**< True if represents '??' or '?'. */
};

/**
 * @brief Internal parser: AOB string -> structured PatternByte vector.
 */
std::vector<PatternByte> parseAOBInternal(const std::string &aob_str)
{
    std::vector<PatternByte> pattern_elements;
    std::string trimmed_aob = trim(aob_str);
    std::istringstream iss(trimmed_aob);
    std::string token;
    auto &logger = Logger::getInstance();
    int token_idx = 0;

    if (trimmed_aob.empty())
    {
        logger.log(LOG_WARNING, "AOB Parser: Input string is empty.");
        return pattern_elements;
    }

    logger.log(LOG_DEBUG, "AOB Parser: Parsing string: '" + trimmed_aob + "'");

    // Reserve some space to avoid reallocation
    pattern_elements.reserve(std::count(trimmed_aob.begin(), trimmed_aob.end(), ' ') + 1);

    while (iss >> token)
    {
        token_idx++;

        // Check for wildcard tokens
        if (token == "??" || token == "?")
        {
            pattern_elements.emplace_back(PatternByte{0x00, true});
        }
        // Check for valid 2-digit hex byte token
        else if (token.length() == 2 &&
                 std::isxdigit(static_cast<unsigned char>(token[0])) &&
                 std::isxdigit(static_cast<unsigned char>(token[1])))
        {
            // Convert hex to byte without exceptions
            char *endptr = nullptr;
            unsigned long byte_val = std::strtoul(token.c_str(), &endptr, 16);

            // Check conversion success
            if (*endptr != '\0' || byte_val > 0xFF)
            {
                logger.log(LOG_ERROR, "AOB Parser: Hex conversion error for '" +
                                          token + "' (Token #" + std::to_string(token_idx) + ")");
                return {};
            }

            pattern_elements.emplace_back(PatternByte{static_cast<BYTE>(byte_val), false});
        }
        // Invalid token format
        else
        {
            logger.log(LOG_ERROR, "AOB Parser: Invalid token '" + token +
                                      "' at position " + std::to_string(token_idx) +
                                      ". Expected '?"
                                      "?', '?', or 2 hex digits (e.g., FF).");
            return {}; // Return empty on invalid token error
        }
    }

    if (pattern_elements.empty() && token_idx > 0)
    {
        logger.log(LOG_ERROR, "AOB Parser: Parsing yielded no valid elements.");
    }
    else if (!pattern_elements.empty())
    {
        logger.log(LOG_DEBUG, "AOB Parser: Successfully parsed " +
                                  std::to_string(pattern_elements.size()) + " elements.");
    }

    return pattern_elements;
}

/**
 * @brief Public AOB parser: string -> vector<BYTE> with 0xCC wildcards.
 */
std::vector<BYTE> parseAOB(const std::string &aob_str)
{
    std::vector<PatternByte> internal_pattern = parseAOBInternal(aob_str);
    std::vector<BYTE> byte_vector;

    if (internal_pattern.empty())
    {
        if (!trim(aob_str).empty())
        {
            Logger::getInstance().log(LOG_ERROR, "AOB: Final parsed pattern is "
                                                 "empty due to errors in input string.");
        }
        return byte_vector;
    }

    // Convert PatternByte vector to BYTE vector using 0xCC placeholder
    byte_vector.reserve(internal_pattern.size());
    for (const auto &element : internal_pattern)
    {
        byte_vector.push_back(element.is_wildcard ? 0xCC : element.value);
    }

    Logger::getInstance().log(LOG_DEBUG, "AOB: Converted pattern for scanning "
                                         "(0xCC=wildcard).");
    return byte_vector;
}

/**
 * @brief Scans memory region for a byte pattern including 0xCC wildcards.
 */
BYTE *FindPattern(BYTE *start_address, size_t region_size,
                  const std::vector<BYTE> &pattern_with_placeholders)
{
    auto &logger = Logger::getInstance();
    const size_t pattern_size = pattern_with_placeholders.size();

    // --- Input Validation ---
    if (pattern_size == 0 || start_address == nullptr)
    {
        logger.log(LOG_ERROR, "FindPattern: Invalid input (empty pattern or "
                              "null start address).");
        return nullptr;
    }

    if (region_size < pattern_size)
    {
        logger.log(LOG_WARNING, "FindPattern: Region size (" +
                                    std::to_string(region_size) + ") < pattern size (" +
                                    std::to_string(pattern_size) + "). No match possible.");
        return nullptr;
    }

    logger.log(LOG_DEBUG, "FindPattern: Scanning " +
                              std::to_string(region_size) + " bytes from " +
                              format_address(reinterpret_cast<uintptr_t>(start_address)) +
                              " for " + std::to_string(pattern_size) + " byte pattern.");

    // --- Wildcard Mask ---
    std::vector<bool> is_wildcard(pattern_size);
    int wildcard_count = 0;

    for (size_t i = 0; i < pattern_size; ++i)
    {
        is_wildcard[i] = (pattern_with_placeholders[i] == 0xCC);
        if (is_wildcard[i])
            wildcard_count++;
    }

    if (wildcard_count > 0)
    {
        logger.log(LOG_DEBUG, "FindPattern: Pattern has " +
                                  std::to_string(wildcard_count) + " wildcards.");
    }

    // --- Memory Scanning Loop ---
    const BYTE *const scan_end_addr = start_address + region_size - pattern_size;

    for (const BYTE *current_pos = start_address; current_pos <= scan_end_addr; ++current_pos)
    {
        size_t j;
        // Check each byte of the pattern against the current position
        for (j = 0; j < pattern_size; ++j)
        {
            // Skip checking wildcards, only check for mismatches in non-wildcard bytes
            if (!is_wildcard[j] && current_pos[j] != pattern_with_placeholders[j])
            {
                break; // Mismatch found, stop checking at this position
            }
        }

        // If we checked all bytes without finding a mismatch, pattern found
        if (j == pattern_size)
        {
            logger.log(LOG_DEBUG, "FindPattern: Match found at address: " +
                                      format_address(reinterpret_cast<uintptr_t>(current_pos)));
            return const_cast<BYTE *>(current_pos);
        }
    }

    logger.log(LOG_WARNING, "FindPattern: Pattern not found in region.");
    return nullptr; // Pattern not found
}
