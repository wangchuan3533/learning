#include <stdio.h>
#define MAX_N (100)
#define MAX_M (100)
#define QUEUE_SIZE (1024)
#define PUSH_QUEUE(xx, yy, tt, TYPE) do {\
	queue[end].x = (xx);\
	queue[end].y = (yy);\
	queue[end].time = (tt);\
	queue[end].type = (TYPE);\
	map[(yy)][(xx)] = 'v';\
	end = (end + 1) % QUEUE_SIZE;\
	size++;\
} while (0)
#define POP_QUEUE() do {\
	front = (front + 1) % QUEUE_SIZE;\
	size--;\
} while (0)
struct position {
	int x;
	int y;
	int time;
	char type;
};
struct position queue[QUEUE_SIZE];
int front, end, size;
int M, N;
char map[MAX_N][MAX_M];
int calc_time(int x, int y, int dx, int dy, int time);
int main()
{
	int t, next_turn;
	int x, y, time, type;

	scanf("%d", &t);
	while (t--) {
		scanf("%d %d\n", &N, &M);
		for (y = 0; y < N; y++)
			gets(map[y]);
		front = end = size = 0;
		for (y = 0; y < N && size == 0; y++)
			for (x = 0; x < M; x++)
				if (map[y][x] == 'S') {
					PUSH_QUEUE(x, y, 0, map[y][x]);
					break;
				}
		while (size) {
			x = queue[front].x;
			y = queue[front].y;
			time = queue[front].time;
			type = queue[front].type;
			switch (type) {
			case 'S':
				if (map[y][x + 1] == '-')
					PUSH_QUEUE(x + 1, y, time + 1, map[y][x + 1]);
				if (map[y][x - 1] == '-')
					PUSH_QUEUE(x - 1, y, time + 1, map[y][x - 1]);
				if (map[y + 1][x] == '|')
					PUSH_QUEUE(x, y + 1, time + 1, map[y + 1][x]);
				if (map[y - 1][x] == '|')
					PUSH_QUEUE(x, y - 1, time + 1, map[y - 1][x]);
				break;
			case '-':
				switch (map[y][x + 1]) {
				case '-':
					PUSH_QUEUE(x + 1, y, time + 1, map[y][x + 1]);
					break;
				case 'D':
					printf("%d\n", time);
					goto cleanup;
					break;
				case '+':
					map[y][x + 1] = 'v';
					if (map[y + 1][x + 1] == '|')
						PUSH_QUEUE(x + 1, y + 1, time + 1, map[y + 1][x + 1]);
					if (map[y - 1][x + 1] == '|')
						PUSH_QUEUE(x + 1, y - 1, time + 1, map[y - 1][x + 1]);
					break;
				default:
					if (map[y][x + 1] >= '1'
						&& map[y][x + 1] <= '9') {
						next_turn = calc_time(x, y, 1, 0, time);
						if (next_turn > time)
							PUSH_QUEUE(x, y, time + 1, type);
						else {
							map[y][x + 1] = 'v';
							if (map[y + 1][x + 1] == '|')
								PUSH_QUEUE(x + 1, y + 1, time + 1, map[y + 1][x + 1]);
							if (map[y - 1][x + 1] == '|')
								PUSH_QUEUE(x + 1, y - 1, time + 1, map[y - 1][x + 1]);
							if (map[y][x + 2] == '-')
								PUSH_QUEUE(x + 2, y, time + 1, map[y][x + 2]);
						}
					}
					break;
				}
				switch (map[y][x - 1]) {
				case '-':
					PUSH_QUEUE(x - 1, y, time + 1, map[y][x - 1]);
					break;
				case 'D':
					printf("%d\n", time);
					goto cleanup;
					break;
				case '+':
					map[y][x - 1] = 'v';
					if (map[y + 1][x - 1] == '|')
						PUSH_QUEUE(x - 1, y + 1, time + 1, map[y + 1][x - 1]);
					if (map[y - 1][x - 1] == '|')
						PUSH_QUEUE(x - 1, y - 1, time + 1, map[y - 1][x - 1]);
					break;
				default:
					if (map[y][x - 1] >= '1'
						&& map[y][x - 1] <= '9') {
						next_turn = calc_time(x, y, -1, 0, time);
						if (next_turn > time)
							PUSH_QUEUE(x, y, time + 1, type);
						else {
							map[y][x - 1] = 'v';
							if (map[y + 1][x - 1] == '|')
								PUSH_QUEUE(x - 1, y + 1, time + 1, map[y + 1][x - 1]);
							if (map[y - 1][x - 1] == '|')
								PUSH_QUEUE(x - 1, y - 1, time + 1, map[y - 1][x - 1]);
							if (map[y][x - 2] == '-')
								PUSH_QUEUE(x - 2, y, time + 1, map[y][x - 2]);
						}
					}
					break;
				}
				break;
			case '|':
				switch (map[y + 1][x]) {
				case '|':
					PUSH_QUEUE(x, y + 1, time + 1, map[y + 1][x]);
					break;
				case 'D':
					printf("%d\n", time);
					goto cleanup;
					break;
				case '+':
					map[y + 1][x] = 'v';
					if (map[y + 1][x + 1] == '-')
						PUSH_QUEUE(x + 1, y + 1, time + 1, map[y + 1][x + 1]);
					if (map[y + 1][x - 1] == '-')
						PUSH_QUEUE(x - 1, y + 1, time + 1, map[y + 1][x - 1]);
					break;
				default:
					if (map[y + 1][x] >= '1'
						&& map[y + 1][x] <= '9') {
						next_turn = calc_time(x, y, 0, 1, time);
						if (next_turn > time)
							PUSH_QUEUE(x, y, time + 1, type);
						else {
							map[y + 1][x] = 'v';
							if (map[y + 1][x + 1] == '-')
								PUSH_QUEUE(x + 1, y + 1, time + 1, map[y + 1][x + 1]);
							if (map[y + 1][x - 1] == '-')
								PUSH_QUEUE(x - 1, y + 1, time + 1, map[y + 1][x - 1]);
							if (map[y + 2][x] == '|')
								PUSH_QUEUE(x, y + 2, time + 1, map[y + 2][x]);
						}
					}
					break;
				}
				switch (map[y - 1][x]) {
				case '|':
					PUSH_QUEUE(x, y - 1, time + 1, map[y - 1][x]);
					break;
				case 'D':
					printf("%d\n", time);
					goto cleanup;
					break;
				case '+':
					map[y - 1][x] = 'v';
					if (map[y - 1][x + 1] == '-')
						PUSH_QUEUE(x + 1, y - 1, time + 1, map[y - 1][x + 1]);
					if (map[y - 1][x - 1] == '-')
						PUSH_QUEUE(x - 1, y - 1, time + 1, map[y - 1][x - 1]);
					break;
				default:
					if (map[y - 1][x] >= '1'
						&& map[y - 1][x] <= '9') {
						next_turn = calc_time(x, y, 0, -1, time);
						if (next_turn > time)
							PUSH_QUEUE(x, y, time + 1, type);
						else {
							map[y - 1][x] = 'v';
							if (map[y - 1][x - 1] == '-')
								PUSH_QUEUE(x - 1, y - 1, time + 1, map[y - 1][x - 1]);
							if (map[y - 1][x + 1] == '-')
								PUSH_QUEUE(x + 1, y - 1, time + 1, map[y - 1][x + 1]);
							if (map[y - 2][x] == '|')
								PUSH_QUEUE(x, y - 2, time + 1, map[y - 2][x]);
						}
					}
					break;
				}
				break;
			}
			POP_QUEUE();
		}
cleanup:
		if (!size)
			printf("impossible\n");
	}
	return 0;
}

int calc_time(int x, int y, int dx, int dy, int time)
{
	int t = map[y + dy][x + dx] - '0';
	time /= t;
	if (map[y + dy][x + dx + 1] == '*'
			|| map[y + dy][x + dx + 1] == ' ') {
		if (dy == -1)
			time = (time + 2) / 3 * 3;
		if (dy == 1)
			time = (time + 1) / 3 * 3 + 1;
		if (dx == 1)
			time = time / 3 * 3 + 2;
		time *= t;
		return time;
	}
	if (map[y + dy][x + dx - 1] == '*'
			|| map[y + dy][x + dx - 1] == ' ') {
		if (dy == -1)
			time = (time + 2) / 3 * 3;
		if (dx == -1)
			time = (time + 1) / 3 * 3 + 1;
		if (dy == 1)
			time = time / 3 * 3 + 2;
		time *= t;
		return time;

	}
	if (map[y + dy + 1][x + dx] == '*'
			|| map[y + dy + 1][x + dx] == ' ') {
		if (dy == 1)
			time = (time + 2) / 3 * 3;
		if (dx == 1)
			time = (time + 1) / 3 * 3 + 1;
		if (dx == -1)
			time = time / 3 * 3 + 2;
		time *= t;
		return time;
	}
	if (map[y + dy - 1][x + dx] == '*'
			|| map[y + dy - 1][x + dx] == ' ') {
		if (dy == -1)
			time = (time + 2) / 3 * 3;
		if (dx == -1)
			time = (time + 1) / 3 * 3 + 1;
		if (dx == 1)
			time = time / 3 * 3 + 2;
		time *= t;
		return time;
	}
	if (dy == -1)
		time = (time + 3) / 4 * 4;
	if (dx == -1)
		time = (time + 2) / 4 * 4 + 1;
	if (dy == 1)
		time = (time + 1) / 4 * 4 + 2;
	if (dx == 1)
		time = time / 4 * 4 + 3;
	time *= t;
	return time;
}
