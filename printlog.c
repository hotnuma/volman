#include "config.h"
#include <syslog.h>

void printinfo(const char * fmt, ...)
{
    openlog(APP_NAME, LOG_PID, LOG_USER);

    va_list ap;
    va_start(ap, fmt);
    vsyslog(LOG_INFO, fmt, ap);
    va_end(ap);

    closelog();
}


