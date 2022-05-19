#include <iostream>

struct Complex{
	double re;
	double im;	
};

struct Complex *add(struct Complex& A, struct Complex& B){
	struct Complex *C = new struct Complex;
	C->re = A.re + B.re;
	C->im = A.im + B.im;
	return C;
}

int main(){
	struct Complex A, B, *C;
	
	std::cout << "Enter first complex number: ";
	std::cin >> A.re >> A.im;
	std::cout << "Enter second complex number: ";
	std::cin >> B.re >> B.im;

	C = add(A, B);

	std::cout << "Result: " << C->re << " " << C->im << std::endl;

	delete C;

	return 0;
}
