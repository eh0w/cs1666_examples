#include <iostream>

int x = 0;

int main() {
	using namespace std;
	
	int x = 0;
	x = 1;
	::x = 2;

	cout << x << "\n";
	cout << ::x << "\n";
}
