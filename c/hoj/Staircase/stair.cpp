#include <iostream>
#define MAX_N (501)
#define MAX_S (32)
using namespace std;
long long stairs[MAX_N][MAX_S];

int calc_stair()
{
	int i, j, step, N;
	long long Q;
	for (i = 1; i < MAX_N ; i++)
		stairs[i][1] = 1;
	for (N = 3; N < MAX_N ; N++) {
		Q = 0;
		for (step = 2; step * (step + 1) <= 2 * N; step++) {
			for (i = N - step; 2 * i >= step * (step - 1); i -= step)
			       stairs[N][step] += stairs[i][step - 1];
			Q += stairs[N][step];
		}
		stairs[N][0] = Q;
	}
	return 0;
}

int main()
{
	int n;
	for (i = 0; i < 100; i++) {
	}
	calc_stair();
	while (cin >> n && n)
		cout << stairs[n][0] << endl;
	return 0;
}
