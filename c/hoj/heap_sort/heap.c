#include <stdio.h>
#include <string.h>
#define max_len (50001)

int max_heap[max_len];
int min_heap[max_len];

int make_max_heap(int *heap, int n);
int make_min_heap(int *heap, int n);
int remake_max_heap(int *heap, int n);
int remake_min_heap(int *heap, int n);

int main()
{
	int n, i, j;
	int max, min;
	while (scanf("%d", &n) == 1 && n) {
		for (i = 1; i <= n; i++) {
			scanf("%d", max_heap + i);
			min_heap[i] = max_heap[i];
		}
		make_max_heap(max_heap, n);
		make_min_heap(min_heap, n);
		while (n > 1) {
			max = max_heap[1];
			min = min_heap[1];
			max_heap[1] = max_heap[n];
			min_heap[1] = min_heap[n--];
			remake_max_heap(max_heap, n);
			remake_min_heap(min_heap, n);
			max_heap[1] = max * max_heap[1] + 1;
			min_heap[1] = min * min_heap[1] + 1;
			remake_max_heap(max_heap, n);
			remake_min_heap(min_heap, n);
	}
		printf("%d\n", min_heap[1] - max_heap[1]);
	}
	return 0;


}

int make_max_heap(int *heap, int n)
{
	int i, j;
	for (i = 2; i <= n; i++)
		for (j = i; heap[j] > heap[j / 2] && j > 1; j /= 2) {
			heap[j] ^= heap[j / 2];
			heap[j / 2] ^= heap[j];
			heap[j] ^= heap[j / 2];
		}
}

int make_min_heap(int *heap, int n)
{
	int i, j;
	for (i = 2; i <= n; i++)
		for (j = i; heap[j] < heap[j / 2] && j > 1; j /= 2) {
			heap[j] ^= heap[j / 2];
			heap[j / 2] ^= heap[j];
			heap[j] ^= heap[j / 2];
		}
}
int remake_max_heap(int *heap, int n)
{
	int i, j;
	for (i = 1; 2 * i <= n;) {
		if ( 2 * i == n) {
			if (heap[i] < heap[2 * i]) {
				heap[i] ^= heap[2 * i];
				heap[2 * i] ^= heap[i];
				heap[i] ^= heap[2 * i];
			}
			break;
		}
		j = i;
		if (heap[2 * i] > heap[j]) j = 2 * i;
		if (heap[2 * i + 1] > heap[j]) j = 2 * i + 1;
		if (j == i) break;
		heap[i] ^= heap[j];
		heap[j] ^= heap[i];
		heap[i] ^= heap[j];
		i = j;
	}

}

int remake_min_heap(int *heap, int n)
{
	int i, j;
	for (i = 1; 2 * i <= n;) {
		if ( 2 * i == n) {
			if (heap[i] > heap[2 * i]) {
				heap[i] ^= heap[2 * i];
				heap[2 * i] ^= heap[i];
				heap[i] ^= heap[2 * i];
			}
			break;
		}
		j = i;
		if (heap[2 * i] < heap[j]) j = 2 * i;
		if (heap[2 * i + 1] < heap[j]) j = 2 * i + 1;
		if (j == i) break;
		heap[i] ^= heap[j];
		heap[j] ^= heap[i];
		heap[i] ^= heap[j];
		i = j;
	}

}

