#include <vector>
#include <iostream>

int main() {
	std::vector<int> v {5, 10, 15, 20, 25};

	for (std::vector<int>::iterator it = v.begin(); it != v.end(); it++) {
		std::cout << *it << std::endl;
	}

	// I prefer the modern variant

	for (auto it = v.begin(); it != v.end(); it++) {
		std::cout << *it << std::endl;
	}

	// I said, the MODERN variant

	for (auto it : v) {
		std::cout << it << std::endl;
	}

	// Perfection
}
