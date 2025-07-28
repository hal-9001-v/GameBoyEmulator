#include "machine.h"
#include <unistd.h>
#include "tests.h"

int main()
{
    return start_tests();

    int exit = 0;

    init();

    while (!exit)
    {
        tick();

        sleep(10);
    }

    return 0;
}
