#include <stdio.h>

int main(int argc, char **argv) {
  char *p = "Hello\n";
  char **p2 = &p;
  printf("%s", *p2); //statt printf(**p2) erstens definieren das es ein string ist und nur ein pointer
  return 0;
}
