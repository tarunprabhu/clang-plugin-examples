#pragma mydeclattr
void init(int a[3][3], int b[3][3], int c[3][3]) {
#pragma mystmtattr
  for (unsigned i = 0; i < 3; i++) {
#pragma mystmtattr
    for (unsigned j = 0; j < 3; j++) {
      a[i][j] = i * 3 + j + 1;
      b[i][j] = 10 - a[i][j];
      c[i][j] = 0;
    }
  }
}

int main(int argc, char* argv[]) {
  int a[3][3];
  int b[3][3];
  int c[3][3];

  init(a, b, c);
}
