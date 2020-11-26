#include "stdarg.h"
#include "iostream"

using namespace std;

int sum(int count, ...)
{
    int sumValue = 0;

    va_list args;
    va_start(args, count);
    while (count--)
    {
        sumValue += va_arg(args, int);
    }
    va_end(args);
    return sumValue;
}

int main(int argc, const char* argv[])
{
    cout << sum(3, 3, 5, 7);
}