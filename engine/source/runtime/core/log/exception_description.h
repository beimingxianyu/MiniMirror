//
// Created by 北冥咸鱼 on 2023/8/23.
//
#include "runtime/platform/base/cross_platform_header.h"
#include "runtime/core/log/log_system.h"


#define MM_TRACE_DESCRIPTION(description) [function_name = MM_FUNCTION_NAME](MM::ErrorCode error_code){ \
        MM_LOG_SYSTEM->CheckResult(error_code, std::string("[") + MM_FUNCTION_NAME + "]    " + description, MM::LogSystem::LogSystem::LogLevel::TRACE);                                                                                                 \
}

#define MM_INFO_DESCRIPTION(description) [function_name = MM_FUNCTION_NAME](MM::ErrorCode error_code){ \
        MM_LOG_SYSTEM->CheckResult(error_code, std::string("[") + MM_FUNCTION_NAME + "]    " + description, MM::LogSystem::LogSystem::LogLevel::INFO);                                                                                                 \
}

#define MM_DEBUG_DESCRIPTION(description) [function_name = MM_FUNCTION_NAME](MM::ErrorCode error_code){ \
        MM_LOG_SYSTEM->CheckResult(error_code, std::string("[") + MM_FUNCTION_NAME + "]    " + description, MM::LogSystem::LogSystem::LogLevel::DEBUG);                                                                                                 \
}

#define MM_WRAN_DESCRIPTION(description) [function_name = MM_FUNCTION_NAME](MM::ErrorCode error_code){ \
        MM_LOG_SYSTEM->CheckResult(error_code, std::string("[") + MM_FUNCTION_NAME + "]    " + description, MM::LogSystem::LogSystem::LogLevel::WARN);                                                                                                 \
}

#define MM_ERROR_DESCRIPTION(description) [function_name = MM_FUNCTION_NAME](MM::ErrorCode error_code){ \
        MM_LOG_SYSTEM->CheckResult(error_code, std::string("[") + MM_FUNCTION_NAME + "]    " + description, MM::LogSystem::LogSystem::LogLevel::ERROR);                                                                                                 \
}

#define MM_FATAL_DESCRIPTION(description) function_name = MM_FUNCTION_NAME](MM::ErrorCode error_code){ \
        MM_LOG_SYSTEM->CheckResult(error_code, std::string("[") + MM_FUNCTION_NAME + "]    " + description, MM::LogSystem::LogSystem::LogLevel::FATAL);                                                                                                 \
}