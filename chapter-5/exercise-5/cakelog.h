#include <sys/types.h>
#include <stdbool.h>

int cakelog_initialise(const char *executable_name, bool force_flush);
int cakelog_stop(void);
ssize_t cakelog(const char* msg_str, ...);
char * get_timestamp(void);

