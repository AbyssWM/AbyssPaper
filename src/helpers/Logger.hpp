#pragma once

#include <abyssutils/cli/Logger.hpp>
#include "Memory.hpp"

#define LOG_DEBUG Abyssutils::CLI::LOG_DEBUG
#define LOG_ERR   Abyssutils::CLI::LOG_ERR
#define LOG_WARN  Abyssutils::CLI::LOG_WARN
#define LOG_TRACE Abyssutils::CLI::LOG_TRACE
#define LOG_CRIT  Abyssutils::CLI::LOG_CRIT

inline UP<Abyssutils::CLI::CLogger> g_logger = makeUnique<Abyssutils::CLI::CLogger>();
