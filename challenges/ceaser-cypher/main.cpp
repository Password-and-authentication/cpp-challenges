#include <algorithm>
#include <iostream>



using std::string;

enum class Type {
    encrypt,
    decrypt,
};

string ceaserCypher(string str, int shift, Type type) {
  std::for_each(str.begin(), str.end(), [shift, type](char &ch) {
      char base{};
      switch (ch) {
          case 'a' ... 'z': base = (type == Type::encrypt ? 'a' : 'z'); break;
          case 'A' ... 'Z': base = (type == Type::encrypt ? 'A' : 'Z'); break;
          default: return;
      }
      switch (type) {
          case Type::encrypt:
            ch = (ch - base + shift) % 26 + base;
            break;
          case Type::decrypt:
            ch = (ch - base - shift) % 26 + base;
            break;
          default: break;
      }
  });
  return str;
}


int main() {

    std::cout << ceaserCypher("jomms", 6, Type::encrypt); // pussy

    std::cout << ceaserCypher("gyriivcc nzcczrdj", 69, Type::decrypt); // pharrell williams

    std::cout << ceaserCypher("aqdou muij", 10000000, Type::encrypt); // kanye west

    std::cout << ceaserCypher("GSL", 690000000, Type::decrypt); // SEX
};