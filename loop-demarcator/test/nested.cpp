int main(int argc, char* argv[]) {
  int a[3][3];
  int b[3][3];
  int c[3][3];

#pragma demarcate
  for (int i = 0; i < 3; i++)
#pragma demarcate
    for (int j = 0; j < 3; j++)
#pragma demarcate
      for (int k = 0; k < 3; k++)
        c[i][j] += a[i][k] + b[k][j];
}
