#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __unix__
#define gmtime_s gmtime_r
#endif

#endif
