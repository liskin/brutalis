#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>

/**
 * \brief Inicializace.
 */
static void common_init() __attribute__((constructor));
static void common_init()
{
    puts("common_init");
    srand(time(0) ^ getpid());
}
