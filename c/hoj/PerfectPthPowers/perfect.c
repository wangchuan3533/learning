#include <stdio.h>
#include <math.h>
#define PRECISE (1e-12)
int main()
{
    double root;
    int i, n, x1, x2, negative;
    while (scanf("%d", &n) && n) {
        if (n < 0) {
            n = -n;
            negative = 1;
        } else
            negative = 0;
        for (i = 31; i > 0; i--) {
            if (negative && (i % 2 == 0))
                continue;
            root = pow(n, 1.0 / i);
            x1 = root;
            x2 = root + 1;
            if (fabs(root - x1) <= PRECISE || fabs(x2 - root) <= PRECISE)
                break;
        }
        printf("%d\n", i);
    }
    return 0;
}
