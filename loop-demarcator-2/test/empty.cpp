#include <cstdlib>

void dummy(void);

int main(int argc, char* argv[]) {
#pragma demarcate
  for(int i = 0; i < argc; i++)
    ;

#pragma demarcate
  for(int i = 0; i < argc; i++) {

  }

#pragma demarcate
  while(rand());

#pragma demarcate
  while(rand())
    ;

#pragma demarcate
  while (rand()) {
    ;
  }

#pragma demarcate
  do {

  } while(rand());
}
