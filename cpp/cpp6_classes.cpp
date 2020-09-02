#include <iostream>

class Rectangle {
	public:
		Rectangle(int l, int w) :length{l}, width{w} {}
		void printArea() {
			std::cout << area() << std::endl;
		}
	private:
		int length, width;
		int area() {
			return length * width;
		}
};


class Square {
	public:
		Square(int s);
		void printArea();
	private:
		int side;
		int area();
};

Square::Square(int s) {
	side = s;
}

void Square::printArea() {
	std::cout << area() << std::endl;
}

int Square::area() {
	return side * side;
}


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
	private:
		int* arr;
		int sz;
};

int main() {
	Rectangle r(5, 2);
	Square* s = new Square(4);

	r.printArea();
	s->printArea();

	MyArray m(12);
	std::cout << m.size() << std::endl;

	MyArray n;
	std::cout << n.size() << std::endl;
}
