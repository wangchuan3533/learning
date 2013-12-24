#include <stdio.h>
int main()
{
	int m[] = {100, 125, 50};
	int g[] = {0, 50, 100};
	int M, G, Z, S, E, z, s, e, f, max, mz, ms, gz, gs;
	int tmp1, tmp2;

	while (scanf("%d %d %d %d %d", &M, &G, &Z, &S, &E) == 5
			&& (M || G || Z || S || E)) {
		max = 0;
		for (z = 0, mz = 0, gz = 0; mz <= M && gz <= G;
				z++, mz += m[0], gz += g[0]) {
			for (s = 0, ms = 0, gs = 0; mz + ms <= M
				&& gz + gs <= G; s++, ms += m[1], gs += g[1]) {
				tmp1 = M - mz - ms;
				tmp1 /= m[2];
				tmp2 = G - gz - gs;
				tmp2 /= g[2];
				if (tmp1 > tmp2)
					e = tmp2;
				else
					e = tmp1;
				f = z * Z + s * S + e * E;
				if (f > max)
					max = f;
			}
		}
		printf("%d\n", max);
	}
	return 0;
}
