[[mydeclattr]]
int a[3][3];
int b[3][3];
int c[3][3];

void init() {
  for (unsigned i = 0; i < 3; i++) {
    for (unsigned j = 0; j < 3; j++) {
      [[mystmtattr]]
      a[i][j] = i * 3 + j + 1;
      b[i][j] = 10 - a[i][j];
      c[i][j] = 0;
    }
  }
}

int main(int argc, char* argv[]) {
  init();
}
