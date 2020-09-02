#include <iostream>

int main() {
	using namespace std;

	int i;
	double d;
	char* s = new char[24];

	cin >> i >> d >> s;

	cout << i << "\n" << d << "\n" << s << "\n";

	/* Try the following inputs:
	"""
	123
	4.5
	asdf
	"""

	"""123 4.5 asdf"""

	"""123 4.5asdf"""
	*/
}
