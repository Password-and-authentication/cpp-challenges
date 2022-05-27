#include <iostream>
#include <list>
#include <algorithm>
#include <numeric>


void fizzBuzz(std::list<int>& arr) {
    std::for_each(arr.begin(), arr.end(), [](int num) {
        if (num % 15 == 0) {
            std::cout << "FIZZBUZZ ";
        } else if (num % 3 == 0) {
            std::cout << "FIZZ ";
        } else if (num % 5 == 0) {
            std::cout << "BUZZ ";
        } else {
            std::cout << num << " ";
        };
    });
}

int main() {

    std::list<int> arr(35);
    std::iota(arr.begin(), arr.end(), 1);
    fizzBuzz(arr);
}