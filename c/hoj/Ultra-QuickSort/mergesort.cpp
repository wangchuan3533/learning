#include <iostream>
using namespace std;
#define MAX_LEN (500000)
int seq[MAX_LEN];
int tmp[MAX_LEN];
long long num_swaps;
void merge_sort(int *arr, int left, int right);
void merge(int *arr, int left_start, int right_start, int right_end);
int main()
{
	int i, j, n;
	while ((cin >> n) && n) {
		for (i = 0; i < n; i++)
			cin >> seq[i];
		num_swaps = 0;
		merge_sort(seq, 0, n - 1);
		cout << num_swaps << endl;
	}
	return 0;
}

void merge_sort(int *arr, int left, int right)
{
	int mid;
	if (left < right) {
		mid = (left + right) / 2;
		merge_sort(arr, left, mid);
		merge_sort(arr, mid + 1, right);
		merge(arr, left, mid + 1, right);
	}
	return;
}

void merge(int *arr, int left_start, int right_start, int right_end)
{
	int left_end = right_start - 1, dst_idx = left_start;
	int left_back = left_start;

	while (left_start <= left_end && right_start <= right_end)
		if (arr[left_start] <= arr[right_start])
			tmp[dst_idx++] = arr[left_start++];
		else {
			tmp[dst_idx++] = arr[right_start++];
			num_swaps += (left_end - left_start + 1);
		}
	while (left_start <= left_end)
		tmp[dst_idx++] = arr[left_start++];
	while (right_start <= right_end)
		tmp[dst_idx++] = arr[right_start++];
	for (; left_back <= right_end; left_back++)
		arr[left_back] = tmp[left_back];
}

