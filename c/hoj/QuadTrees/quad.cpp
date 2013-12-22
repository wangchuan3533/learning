#include <cstdio>
#include <queue>
using namespace std;

#define TYPE_00 (0)
#define TYPE_01 (1)
#define TYPE_1  (2)
#define MAX_N (512)
struct elem {
	int x;
	int y;
	int len;
};
char image[MAX_N][MAX_N];
char buf_string[MAX_N * MAX_N * 2];
queue<elem *> quad_queue;
int check(int x, int y, int len);
int main()
{
	int i, j, k, N;
	int count;
	int type_quad;
	struct elem *tmp, *tmp2;

	scanf("%d", &k);
	while (k--) {
		scanf("%d", &N);
		for (i = 0; i < N; i++)
			for (j = 0; j < N; j++)
				scanf("%d", &(image[i][j]));
		count = 0;
		tmp = new struct elem;
		tmp->x = 0;
		tmp->y = 0;
		tmp->len = N;
		quad_queue.push(tmp);
		while (!quad_queue.empty()) {
			tmp = quad_queue.front();
			type_quad = check(tmp->x, tmp->y, tmp->len);
			if (type_quad == TYPE_00) {
				buf_string[count++] = 0;
				buf_string[count++] = 0;
			}
			if (type_quad == TYPE_01) {
				buf_string[count++] = 0;
				buf_string[count++] = 1;
			}
			if (type_quad == TYPE_1) {
				tmp2 = new struct elem;
				tmp2->x = tmp->x;
				tmp2->y = tmp->y;
				tmp2->len = tmp->len >> 1;
				quad_queue.push(tmp2);
				tmp2 = new struct elem;
				tmp2->x = tmp->x + (tmp->len >> 1);
				tmp2->y = tmp->y;
				tmp2->len = tmp->len >> 1;
				quad_queue.push(tmp2);
				tmp2 = new struct elem;
				tmp2->x = tmp->x;
				tmp2->y = tmp->y + (tmp->len >> 1);
				tmp2->len = tmp->len >> 1;
				quad_queue.push(tmp2);
				tmp2 = new struct elem;
				tmp2->x = tmp->x + (tmp->len >> 1);
				tmp2->y = tmp->y + (tmp->len >> 1);
				tmp2->len = tmp->len >> 1;
				quad_queue.push(tmp2);

				buf_string[count++] = 1;
			}
			quad_queue.pop();
			delete tmp;
		}
		i = count % 4;
		switch (i) {
			case 3:
				j = 4 * buf_string[0] + 2 * buf_string[1]
					+ buf_string[2];
				printf("%c", ('0' + j));
				break;
			case 2:
				j = 2 * buf_string[0] + buf_string[1];
				printf("%c", ('0' + j));
				break;
			case 1:
				j = buf_string[0];
				printf("%c", ('0' + j));
				break;
			default:
				break;
		}
		for (; i < count; i += 4) {
			j = 8 * buf_string[i] + 4 * buf_string[i + 1]
				+ 2 * buf_string[i + 2] + buf_string[i + 3];
			if (j > 9)
				printf("%c", ('A' + j - 10));
			else
				printf("%c", ('0' + j));
		}
		printf("\n");
	}
	return 0;


}
int check(int x, int y, int len)
{
	int i, j;
	
	for (i = 0; i < len; i++)
		for (j = 0; j < len; j++)
			if (image[y + i][x + j] != image[y][x])
				goto type_1;
	return image[y][x];
type_1:
	return TYPE_1;
}
			
