#include <iostream>
#include <utility>

class MyArray {
	public:
		MyArray(int s) :arr{new int[s]}, sz{s} {}
		MyArray() :arr{nullptr}, sz{0} {}
		~MyArray() {
			delete[] arr;
		}

		MyArray(const MyArray& a)
		:arr{new int[a.sz]}, sz{a.sz} {
			for (int i=0; i<sz; i++)
				arr[i] = a.arr[i];
		}
		MyArray& operator=(const MyArray& a) {
			int* p = new int[a.sz];
			for (int i=0; i<a.sz; i++)
				p[i] = a.arr[i];
			delete[] arr;
			arr = p;
			sz = a.sz;
			return *this;	
		}

		MyArray(MyArray&& a)
		:arr{a.arr}, sz{a.sz} {
			a.arr = nullptr;
			a.sz = 0;
		}
		MyArray& operator=(MyArray&& a) {
			if (this != &a) {
				arr = a.arr;
				sz = a.sz;
				a.arr = nullptr;
				a.sz = 0;
			}
	    	return *this;
		}

		int& operator[](int i){
			return arr[i];
		}
		int size() {
			return sz;
		}
		int* arr;
		int sz;
};

int main() {
	using std::cout;
	using std::endl;
	using std::move;

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

	// move assignment
	MyArray p;
	p = move(m);

	cout << "m:\n";
	cout << "\tsize: " << m.sz << "\n";
	cout << "\tcontents: \n";
	// avoid segfault
	if (m.arr) {
		for (auto i=0; i<10; i++)
			cout << "\t\t" << m[i] << "\n";
	}
	cout << endl;

	cout << "p:\n";
	cout << "\tsize: " << p.sz << "\n";
	cout << "\tcontents: \n";
	for (auto i=0; i<10; i++)
		cout << "\t\t" << p[i] << "\n";
	cout << endl;
}
