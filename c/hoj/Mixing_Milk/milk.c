#include <stdio.h>
#include <stdlib.h>
#define MAX_PRICE (1000)
struct list {
	int price;
	int amount;
	struct list *next;
};
void main()
{
	struct list head, end, *tmp1, *tmp2, *lists;
	int amount, cur_amount, price, n, i;
	scanf("%d %d", &amount, &n);
	lists = (struct list *)malloc(n * sizeof(struct list));
	head.price = 0;
	head.amount = 0;
	head.next = &end;
	end.price = MAX_PRICE + 1;
	end.amount = 0;
	end.next = NULL;
	for (i = 0; i < n; i++) {
		scanf("%d %d", &(lists[i].price), &(lists[i].amount));
		tmp1 = &head;
		while (tmp1->price <= lists[i].price) {
			tmp2 = tmp1->next;
			if (lists[i].price < tmp2->price)
				break;
			else
				tmp1 = tmp2;
		}
		lists[i].next = tmp2;
		tmp1->next = &lists[i];
	}
	cur_amount = 0;
	price = 0;
	tmp1 = &head;
	while (cur_amount + tmp1->amount < amount) {
		cur_amount += tmp1->amount;
		price += tmp1->price * tmp1->amount;
		tmp1 = tmp1->next;
	}
	price += tmp1->price * (amount - cur_amount);
	printf("%d\n", price);
	free(lists);
}