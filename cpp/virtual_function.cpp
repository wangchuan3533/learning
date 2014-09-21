#include <stdio.h>
class a {
public:
    virtual void init();
};
class b : public a {
public:
    virtual void init();
};

void a::init()
{
    printf("a");
}

void b::init()
{
    printf("b");
}

int main(int argc, char **argv)
{
    b bb;
    bb.init();
}

