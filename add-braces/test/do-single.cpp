void func(int);
int main(int argc, char* argv[]) {
  do
    func(argc);
  while(--argc);
}
