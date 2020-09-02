#include <iostream>

int main() {
	using namespace std;

	int arr[5] = {0, 1, 2, 3, 4};
	int* p = &arr[3];

	cout << "Check 1:\n" << *p << "\n" << endl;

	int x = 5;
	int y = 6;
	int& r1 = x;
	int& r2 = y;

	cout << "Check 2:\n" << x << " " << y << " " << r1 << " " << r2 << "\n" << endl;

	r1 = r2;

	cout << "Check 3:\n" << x << " " << y << " " << r1 << " " << r2 << "\n" << endl;

	r1 = 7;

	cout << "Check 4:\n" << x << " " << y << " " << r1 << " " << r2 << "\n" << endl;

	int q = *p;
	int* s = &q;
	
	*p = 8;

	cout << "Check 5:\n" << *p << " " << q << " " << *s << "\n" << endl;

	q = 9;

	cout << "Check 6:\n" << *p << " " << q << " " << *s << "\n" << endl;

	*s = 10;

	cout << "Check 7:\n" << *p << " " << q << " " << *s << "\n" << endl;

	for (auto i : arr) {
		i++;
	}

	for (auto i : arr) {
		cout << i << "\n";
	}

	cout << endl;

	for (auto& i : arr) {
		i++;
	}

	for (auto i : arr) {
		cout << i << "\n";
	}

	cout << endl;
}
