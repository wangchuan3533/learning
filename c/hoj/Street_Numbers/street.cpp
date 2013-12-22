#include <iostream>
#include <iomanip>
#include <math.h>
using namespace std;

int main()
{
	long long k, count, root, square;
	double root_f;

	count = 0;
	for (k = 6; count < 10; k++) {
		square = 1 + 8 * k * k;
		root_f = sqrt((double)square);
		root = (long long)root_f;
		if (root * root == square) {
			root = (root - 1) / 2;
			cout << setw(10) << k << setw(10) << root << endl;
			count++;
			k *= 5;
		}
	}
	return 0;
}

