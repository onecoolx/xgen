extern int printf(char const*,...);
int g;
int run2(int c, int d)
{
    return c ^ d;
}

double get1(int a)
{
    return (double)(a + 345);
}

float get2(int a)
{
    return (float)(a * 13);
}

int run1()
{
    int i;
    for (i = 0; i < 10000; i++) {
        if (i % 2) {
            g = get1(g);
        } else {
            g = get2(g);
        }
    }
    printf("\n%d\n",g);
}

int main()
{
    g = 20;
    run1();
    return 0;
}
