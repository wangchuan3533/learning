#include <iostream>
using namespace std;
int main()
{
	int count;
	int n, value;
	while (cin >> n) {	
		if (n == 1) {
			cout << 1 << endl;
			continue;
		}
		value = 1;
		count = 1;
		while (value) {
			value = (value * 10 + 1) % n;
			count++;
		}
		cout << count << endl;
	}
	return 0;
}
