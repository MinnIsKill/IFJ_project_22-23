/**
 * gcc -std=c99 -Wall -Wextra -Werror -g vector.c vector_tests.c -lm -o vector_tests
 * valgrind --leak-check=full ./vector_tests
**/

#include "vector.h"

int main(void)
{
    _vector vect;
    vector_init(&vect);

    vector_append(&vect, "I'm");
    vector_append(&vect, "A");
    vector_append(&vect, "Barbie");
    vector_append(&vect, "Girl");

    vector_print(&vect);
    fprintf(stderr,"\n");

    vector_delete(&vect, 3);
    vector_delete(&vect, 2);
    vector_delete(&vect, 1);

    char arr[32] = "Mamma Mia Pizzeria";

    vector_set(&vect, 0, arr);

    vector_print(&vect);

    vector_free(&vect);
}