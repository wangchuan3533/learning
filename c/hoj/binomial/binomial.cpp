#include <iostream>
using namespace std;
int main()
{
	int i, n, k;
	long long prod;
	while ((cin >> n >> k) && n) {
		if (n - k < k)
			k = n - k;
		prod = 1;
		for (i = 0; i < k; i++) {
			prod *= n - i;
			prod /= i + 1;
		}
		cout << prod << endl;
	}
	return 0;
}
