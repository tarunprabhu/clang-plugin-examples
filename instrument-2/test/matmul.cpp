#include <iostream>

template <typename T>
void init(T a[3][3], T b[3][3], T c[3][3]) {
#pragma instrument loop name("i")
  for (unsigned i = 0; i < 3; i++) {
#pragma instrument loop name("j")
    for (unsigned j = 0; j < 3; j++) {
      a[i][j] = i * 3 + j + 1;
      b[i][j] = 9 - (i * 3 + j);
      c[i][j] = 0;
    }
  }
}

#pragma instrument function mode(full) args()
template <typename T>
void matmul(const T a[3][3], const T b[3][3], T c[3][3]) {
  for (unsigned i = 0; i < 3; i++)
    for (unsigned j = 0; j < 3; j++)
      for (unsigned k = 0; k < 3; k++)
        c[i][j] += a[i][k] * b[k][j];
}

int main(int argc, char* argv[]) {
  int a[3][3];
  int b[3][3];
  int c[3][3];

#pragma instrument line
  init(a, b, c);

  matmul(a, b, c);

#pragma instrument region name("result")
  {
    for (int i = 0; i < 3; i++) {
      for (int j = 0; j < 3; j++)
        std::cout << c[i][j] << " ";
      std::cout << "\n";
    }
  }

  return 0;
}
