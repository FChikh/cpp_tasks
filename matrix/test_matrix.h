#include <vector>
#include <exception>
#include <fstream>
#include <climits>

#undef throw

inline bool equals(double x, double y)
{
    return x - y < 1e-6 && x - y > -1e-6;
}

const int bigPrime = 433494437;

void testMatrix()
{
    // first let's test finite fields
    // Now bi is 1279th Mersenne number, it is prime number

    Finite<bigPrime> x = 1279;
    x *= x += x *= ++x;
    for (int i = 0; i < 1000000; ++i)
        ++x;
    auto y = Finite<bigPrime>(1) / x;
    if (y * x != Finite<bigPrime>(1))
        throw std::runtime_error("Finite Finite arithmetic failed.");

    // Now let's test matrices.
    using std::vector;
    vector<vector<int>> a = {{8, -4, -5, 5, 9},
                             {1, -3, -5, 0, -7},
                             {7, -5, 1, 4, 1},
                             {3, -1, 3, 2, 5}};
    vector<vector<int>> b = {{4, 0, 3, 2},
                             {1, -7, 4, 5},
                             {7, 1, 5, 3},
                             {-5, -3, -3, -1},
                             {1, -5, 2, 3}};
    const Matrix<4, 5, Finite<17>> am = a;
    const Matrix<5, 4, Finite<17>> bm = b;

    if (am.rank() != 4 || bm.rank() != 3)
        throw std::runtime_error("rank() method failed.");
    vector<vector<int>> ab = {{-23, -37, -14, 3},
                              {-41, 51, -48, -49},
                              {11, 19, -4, -9},
                              {27, -21, 24, 23}};

    const auto abm = am * bm;
    if (abm != SquareMatrix<4, Finite<17>>(ab))
        throw std::runtime_error("matrix multiplication or constructors failed.");

    const auto aminusb = am - bm.transposed();
    vector<vector<int>> diff = {{4, -5, -12, 10, 8},
                                {1, 4, -6, 3, -2},
                                {4, -9, -4, 7, -1},
                                {1, -6, 0, 3, 2}};
    if (aminusb != Matrix<4, 5, Finite<17>>(diff))
        throw std::runtime_error("Addition or subtraction failed.");

    auto newMatrix = Finite<17>(2) * aminusb * bm;
    newMatrix[2][2] = 1;

    if (newMatrix.det() != Finite<17>(14420160))
        throw std::runtime_error("Determinant is wrong.");

    if (newMatrix.trace() != Finite<17>(-345))
        throw std::runtime_error("Trace is wrong.");

    if (newMatrix.inverted() * newMatrix != Matrix<4, 4, Finite<17>>())
        throw std::runtime_error("A*A^(-1) must be equal to unity matrix.");

    std::cerr << "Tests over the finite field passed!\n";

    std::ifstream in("matr.txt");
    SquareMatrix<20> bigMatrix;
    for (int i = 0; i < 20; ++i)
        for (int j = 0; j < 20; ++j)
            in >> bigMatrix[i][j];

    vector<vector<double>> bigInvertedMatrix(20, vector<double>(20));
    for (int i = 0; i < 20; ++i)
        for (int j = 0; j < 20; ++j)
            in >> bigInvertedMatrix[i][j];

    auto anotherMatrix = bigMatrix;
    // bigMatrix.inverted();
    bigMatrix.invert();
    std::cerr << "Big matrix inverted!\n";
    for (int i = 0; i < 20; ++i)
    {
        auto row = bigMatrix.getRow(i);
        auto anotherRow = bigInvertedMatrix[i];
        for (int j = 0; j < 20; ++j)
        {
            if (!equals(static_cast<double>(row[j]), anotherRow[j]))
            {
                throw std::runtime_error("Inverted matrix is incorrect!");
            }
        }
    }
    std::cerr << "Inverted matrix is correct!\n";
    bigMatrix *= anotherMatrix;
    std::cerr << "Matrix multiplied by its inverted matrix!\n";
    SquareMatrix<20> unity = SquareMatrix<20>();
    for (int j = 0; j < 20; ++j)
    {
        auto column = bigMatrix.getColumn(j);
        auto unityColumn = unity.getColumn(j);
        for (int i = 0; i < 20; ++i)
        {
            if (column[i] != unityColumn[i])
            {
                throw std::runtime_error("Product of A and A^(-1) must be equal to unity matrix.");
            }
        }
    }

    std::cerr << "Result of A*A^(-1) is correct!\n";

    auto det = anotherMatrix.det();
    std::cerr << "Determinant is computed!\n";
    // det /= 20;
    for (int i = 0; i < 20; ++i)
        anotherMatrix[0][i] /= det;
    // now anotherMatrix is orthogonal matrix
    if (anotherMatrix.det() != 1)
        throw std::runtime_error("Dividing a row by det must make the determinant equal to 1.");

    auto transposedMatrix = anotherMatrix.transposed();
    if ((anotherMatrix *= transposedMatrix).det() != 1)
        throw std::runtime_error("Product of two matrices with det=1 must give the matrix with same determinant.");
}