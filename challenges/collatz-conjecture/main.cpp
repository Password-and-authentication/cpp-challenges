#include <iostream>

int solve(int x) {
  while (x != 1) {
      std::cout << x << '\n';
    if (x % 2 == 0)
      x = x / 2;
    else
      x = (x * 3) + 1;
  }
  return x;
}

int main() {
    std::cout <<  solve(10002020); 
}