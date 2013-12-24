#include <iostream>
using namespace std;
int seq[500000];
int main()
{
		long long num;
		int i, j, n;
		while ((cin >> n) && n) {
				num  = 0;
				for (i = 0; i < n; i++) {
						cin >> seq[i];
						for (j = 0; j < i; j++)
								if (seq[j] > seq[i])
										num++;
				}
				cout << num << endl;
		}
		return 0;
}
