#include <iostream>

namespace A {
	int x = 7;
	int y = 10;
}

namespace B {
	int x = 8;
	int y = 20;
}

int x = 0;

int main() {
	using namespace std;
	
	int x = 0;

	cout << "local x:  " << x << "\n";
	cout << "global x:  " << ::x << "\n";
	cout << "A x:  " << A::x << "\n";
	cout << "B x:  " << B::x << "\n";
	cout << "\n";

	x = 1;
	::x = 2;
	A::x = 3;
	B::x = 4;
	x = 5;

	cout << "local x:  " << x << "\n";
	cout << "global x:  " << ::x << "\n";
	cout << "A x:  " << A::x << "\n";
	cout << "B x:  " << B::x << "\n";
	cout << "\n";

	using namespace A;

	x = 11;
	y = 11;

	cout << "local x:  " << x << "\n";
	cout << "global x:  " << ::x << "\n";
	cout << "A x:  " << A::x << "\n";
	cout << "B x:  " << B::x << "\n";
	cout << "A y:  " << A::y << "\n";
	cout << "B y:  " << B::y << "\n";
	cout << "\n";

	using namespace B;

	x = 21;
	//y = 21;  // Would result in an error, because reference to y is ambiguous

	cout << "local x:  " << x << "\n";
	cout << "global x:  " << ::x << "\n";
	cout << "A x:  " << A::x << "\n";
	cout << "B x:  " << B::x << "\n";	
	cout << "A y:  " << A::y << "\n";
	cout << "B y:  " << B::y << "\n";	
	cout << "\n";
}
