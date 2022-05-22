#include <cstdlib>

int func() {
  int i;
#pragma extract
  while(rand() < 3)
    i = rand();

#pragma extract
  return i;
}

int main(int argc, char* argv[]) {
#pragma extract
  return atoi(argv[argc - 1]);
}
