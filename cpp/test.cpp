#include <iostream>
class a {
public:
    void print();
};

class b:a {
public:
    void print();
};

void a::print()
{
    std::cout << "a\n";
}

void b::print()
{
    std::cout << "b\n";
}

int main() {
    b c;
    c.print();
    return 0;
}
