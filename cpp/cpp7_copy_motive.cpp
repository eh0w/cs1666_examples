#include <iostream>

class MyArray {
	public:
		MyArray(int s) :arr{new int[s]}, sz{s} {}
		MyArray() :arr{nullptr}, sz{0} {}
		~MyArray() {
			delete[] arr;
		}
		int& operator[](int i){
			return arr[i];
		}
		int size() {
			return sz;
		}
		// moved into public to ease example
		int* arr;
		int sz;
};

int main() {
	using namespace std;

	MyArray m(10);
	for (auto i=0; i<10; i++)
		m[i] = i;

	// copy initialization
	MyArray n = m;

	// copy assignment
	MyArray o;
	o = m;

	n.sz = 20;
	n[2] = 20;

	o.sz = 30;
	o[3] = 30;

	cout << "m:\n";
	cout << "\tsize: " << m.sz << "\n";
	cout << "\tcontents: \n";
	// note: unable to use range for loop on dynamically allocated array
	for (auto i=0; i<10; i++)
		cout << "\t\t" << m[i] << "\n";
	cout << endl;

	cout << "n:\n";
	cout << "\tsize: " << n.sz << "\n";
	cout << "\tcontents: \n";
	for (auto i=0; i<10; i++)
		cout << "\t\t" << n[i] << "\n";
	cout << endl;

	cout << "o:\n";
	cout << "\tsize: " << o.sz << "\n";
	cout << "\tcontents: \n";
	for (auto i=0; i<10; i++)
		cout << "\t\t" << o[i] << "\n";
	cout << endl;
}
