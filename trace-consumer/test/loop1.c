int main(int argc, char* argv[]) {
  int ret = 0;
  for (int i = 0; i < argc; i++)
    ret += i;
  return ret;
}
