int alias3(int * p, int a[10], int i, int j)
{
    //Test if pointer alias with array.
    int b, c;
    int x[200];
    *p = 20;
    if (i > j) {
        p = &a;
    } else {
        p = &b;
    }
    b = x[10];
    b = a[i];
    a[j] = c;
    return *p;
}
