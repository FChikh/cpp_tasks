#include "old_tests.h"
#include "biginteger.h"

int main() {
    testBigInteger<BigInteger>();
    std::cerr << "BigInteger tests passed. Now starting Rational tests...";
    testRational<Rational, BigInteger>();
    std::cout << 0;
}