#include <iostream>
struct bool_struct {
    bool a;
    bool b;
};
int main()
{
    std::cout << "single bool var size is " << sizeof(bool) << std::endl;
    std::cout << "bool struct size is " << sizeof(bool_struct) << std::endl;
    return 0;
}
