typedef struct _atype
{
    float bg[1], cg[1];
    bool ant;
} atype;

int foo(atype **rng_stream)
{
    typedef float ty[1];
    ty *tt = &((*rng_stream)->bg);
    return tt[0] == 0.01f;
}

int main()
{
   atype s, *ps;
   s.bg[0] = 0.01f;
   ps = &s;
   return !foo(&ps);
} 
