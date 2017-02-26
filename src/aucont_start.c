#include <start_options.h>

struct start_opts opts;

int main(int argc, char *argv[])
{
    get_start_options(&opts, argc, argv);
}
