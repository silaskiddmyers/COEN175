/*
 * This file will not be run through your compiler.
 */

# include <stdio.h>

extern int towers();

int call_towers(int n, int from, int to, int spare)
{
    printf("call_towers:n = %d\n", n);
    if (n > 1)
	towers(n - 1, from, to, spare);
}


int print_move(int from, int to)
{
    printf("move a peg from %d to %d\n", from, to);
}


int print(int n)
{
    printf("value = %d\n", n);
}
