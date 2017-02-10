#ifndef LOG_H
#define LOG_H

enum log_tag {
  LOG_DEBUG,
  LOG_INFO,
  LOG_ERROR,
};

#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_DEBUG
#endif

#ifndef LOG_FILE
#define LOG_FILE stdout
#endif

void log_error(const char *message, ...);
void log_info(const char *message, ...);
void log_debug(const char *message, ...);

#endif
