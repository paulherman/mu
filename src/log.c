#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include "platform.h"
#include "log.h"

static void log_print(const char *tag, const char *message, va_list args) {
  time_t current_time;
  time(&current_time);

  struct tm current_time_buf;
  gmtime_s(&current_time, &current_time_buf);

  char current_time_str[256];
  strftime(current_time_str, 256, "%F - %T", &current_time_buf);

  size_t message_meta_len = strlen(message) + 256 + strlen(tag) + 16;
  char *message_meta = calloc(message_meta_len, sizeof(char));
  snprintf(message_meta, message_meta_len, "[%s] [%s] %s\n", current_time_str, tag, message);
  vfprintf(LOG_FILE, message_meta, args);
  free(message_meta);
}

void log_error(const char *message, ...) {
  if (LOG_ERROR >= LOG_LEVEL) {
    va_list args;
    va_start(args, message);
    log_print("Error", message, args);
    va_end(args);
  }
}

void log_info(const char *message, ...) {
  if (LOG_INFO >= LOG_LEVEL) {
    va_list args;
    va_start(args, message);
    log_print("Info", message, args);
    va_end(args);
  }
}

void log_debug(const char *message, ...) {
  if (LOG_DEBUG >= LOG_LEVEL) {
    va_list args;
    va_start(args, message);
    log_print("Debug", message, args);
    va_end(args);
  }
}
