#include <stdio.h>
#include <stdlib.h>
struct farmer {
	int price;
	int amount;
};
void create_heap(struct farmer *heap, int n);
void main()
{
	int amount, cur_amount, price, n, i, j;
	struct farmer *heap, tmp;
	scanf("%d %d", &amount, &n);
	heap = (struct farmer *)malloc((n + 1) * sizeof(struct farmer));
	for (i = 1; i <= n; i++)
		scanf("%d %d", &heap[i].price, &heap[i].amount);
	create_heap(heap, n);
	cur_amount = 0;
	price = 0;
	while (cur_amount + heap[1].amount < amount) {
		cur_amount += heap[1].amount;
		price += heap[1].price * heap[1].amount;
		heap[1].price = heap[n].price;
		heap[1].amount = heap[n--].amount;
		i = 1;
		while ( 2 * i <= n) {
			if (2 * i == n) {
				if (heap[i].price > heap[2 * i].price) {
					tmp.price = heap[i].price;
					tmp.amount = heap[i].amount;
					heap[i].price = heap[2 * i].price;
					heap[i].amount = heap[2 * i].amount;
					heap[2 * i].price = tmp.price;
					heap[2 * i].amount = tmp.amount;
				}
				break;
			}
			j = i;
			if (heap[2 * i].price < heap[j].price) j = 2 * i;
			if (heap[2 * i + 1].price < heap[j].price) j = 2 * i + 1;
			if (j == i) break;
			tmp.price = heap[i].price;
			tmp.amount = heap[i].amount;
			heap[i].price = heap[j].price;
			heap[i].amount = heap[j].amount;
			heap[j].price = tmp.price;
			heap[j].amount = tmp.amount;
			i = j;
		}
	}
	price += heap[1].price * (amount - cur_amount);
	printf("%d\n", price);
	free(heap);
}

void create_heap(struct farmer *heap, int n)
{
	int i, j;
	struct farmer tmp;
	for (i = 2; i <= n; i++) {
		j = i;
		while (j > 1 && heap[j].price < heap[j / 2].price) {
			tmp.price = heap[j].price;
			tmp.amount = heap[j].amount;
			heap[j].price = heap[j / 2].price;
			heap[j].amount = heap[j / 2].amount;
			heap[j / 2].price = tmp.price;
			heap[j / 2].amount = tmp.amount;
			j /= 2;
		}
	}
}
