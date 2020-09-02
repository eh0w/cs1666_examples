#include <iostream>

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

class Container {
	public:
		virtual int& operator[](int) = 0;
		virtual int size() = 0;
		virtual ~Container() {}
};

class MyArr_container : public Container {
	public:
		MyArr_container(int s) : m(s) { }
		~MyArr_container() {}
		int& operator[](int i) override {
			return m[i];
		}
		int size() override { return m.size(); }
	private:
		MyArray m;
};

int main() {
	using namespace std;

	Container *c = new MyArr_container(10);

	cout << c->size() << endl;
}
