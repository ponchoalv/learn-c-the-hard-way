#include <stdio.h>

int main(int argc, char *argv[])
{
  char *cities[] = {"Buenos aires", "Pilar", "Rosario"};
  int i = 0;
  while (i < 3)
  {
    printf("Ciudad %d -> %s.\n", i, cities[i]);
    i++;
  }

  return 0;
}