int main(int argc, char*[]) {
  if (argc == 0)
    return 1;
  else if (argc == 1)
    return 0;
  else if (argc == 42)
    return -1;
  else
    return argc;
}
