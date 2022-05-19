#include <iostream>

int main(){
	int n;
	std::cout << "Enter Number: ";
	std::cin >> n;
	std::cout << "Factors:";
	for(int i = 1; i <= n; i++){
		if(n%i == 0){
			std::cout << i << " ";
		}
	}
	std::cout << std::endl;
	return 0;
}
