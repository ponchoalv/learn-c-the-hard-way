#include <stdio.h>

int main(int argc, char *argv[])
{
  if (argc == 1)
  {
    printf("You only have one argument. You suck.\n");
  }
  else if (argc > 1 && argc < 4)
  {
    printf("Here's your arguments:\n");

    // changed the initial value of i for last extra credit.
    for (int i = 1; i < argc; i++)
    {
      printf("%s ", argv[i]);
    }
    printf("\n");
  }
  else
  {
    printf("You have too many arguments. You suck.\n");
  }

  return 0;
}