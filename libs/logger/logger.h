#ifndef LOGGER_H
#define LOGGER_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

typedef struct {
    LogLevel min_level;
    FILE* log_file;
    char time_format[64];
    void (*debug)(const char* fmt, ...);
    void (*info)(const char* fmt, ...);
    void (*warn)(const char* fmt, ...);
    void (*error)(const char* fmt, ...);
    void (*fatal)(const char* fmt, ...);
    void (*free)(void);
} Logger;

extern Logger *global_logger;

Logger* create_logger(LogLevel level, const char* filename, const char* time_fmt);

#define LOGGER_TIME_FORMAT_ISO8601 "%Y-%m-%dT%H:%M:%S%z"
#define LOGGER_TIME_FORMAT_SIMPLE  "%Y-%m-%d %H:%M:%S"
#define LOGGER_TIME_FORMAT_FULL    "%a, %d %b %Y %H:%M:%S %Z"
#define LOGGER_TIME_FORMAT_COMPACT "%Y%m%d%H%M%S"

#ifdef LOGGER_IMPLEMENTATION

Logger *global_logger;

static void log_message(LogLevel level, const char* fmt, va_list args) {
    if (level < global_logger->min_level || !fmt) {
        return;
    }

    char time_str[128];
    time_t now = time(NULL);
    struct tm *tm = localtime(&now);
    strftime(time_str, sizeof(time_str), global_logger->time_format, tm);

    const char* level_strings[] = { "DEBUG", "INFO", "WARN", "ERROR", "FATAL" };
    fprintf(level >= LOG_WARN ? stderr : stdout, "[%s] %s ", level_strings[level], time_str);
    if (global_logger->log_file) {
        vfprintf(global_logger->log_file, fmt, args);
        fprintf(global_logger->log_file, "\n");
    } else {
        vfprintf(level >= LOG_WARN ? stderr : stdout, fmt, args);
        fprintf(level >= LOG_WARN ? stderr : stdout, "\n");
    }
}

#define DEFINE_LOG_FUNCTION(name, level)                        \
    static void name(const char* fmt, ...) {                    \
        va_list args;                                           \
        va_start(args, fmt);                                    \
        log_message(level, fmt, args);                          \
        va_end(args);                                           \
    }

DEFINE_LOG_FUNCTION(log_debug, LOG_DEBUG)
DEFINE_LOG_FUNCTION(log_info, LOG_INFO)
DEFINE_LOG_FUNCTION(log_warn, LOG_WARN)
DEFINE_LOG_FUNCTION(log_error, LOG_ERROR)
DEFINE_LOG_FUNCTION(log_fatal, LOG_FATAL)

void _logger_free(void)
{
    if (global_logger == NULL) {
        return;
    }

    if (global_logger->log_file != NULL) {
        fclose(global_logger->log_file);
    }

    free(global_logger);
}

Logger* create_logger(LogLevel level, const char* filename, const char* time_fmt) {
    if (global_logger == NULL) {
        Logger* logger = malloc(sizeof(Logger));
        if (!logger) {
            fprintf(stderr, "Failed to allocate memory for logger.\n");
            return NULL;
        }

        logger->min_level = level;
        logger->log_file = filename ? fopen(filename, "a") : NULL;
        strncpy(logger->time_format, time_fmt, sizeof(logger->time_format) - 1);
        logger->time_format[sizeof(logger->time_format) - 1] = '\0';
        logger->debug = log_debug;
        logger->info = log_info;
        logger->warn = log_warn;
        logger->error = log_error;
        logger->fatal = log_fatal;
        logger->free = _logger_free;

        global_logger = logger;
    }

    return global_logger;
}

#endif // LOGGER_IMPLEMENTATION

#ifdef __cplusplus
}
#endif

#endif // LOGGER_H

