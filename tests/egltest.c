#include <stdio.h>
#include "minunit.h"

static const char *
first_test (void)
{
    mu_assert (1 == 3);
}

static const char *
second_test (void)
{
    mu_assert (1 == 4);
}

int main (void)
{
    mu_run_test (first_test);
    mu_run_test (second_test);
    return 0;
}
