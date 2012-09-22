#ifndef CONTAINERS_H_INCLUDED
#define CONTAINERS_H_INCLUDED

namespace Inugami
{

template <typename T, int S>
class Array
{
public:
    T &operator[](int i)
    {
        return data[i];
    }
private:
    T data[S];
};

template <typename T, int R, int C>
class Matrix
{
public:
    Matrix() : rows(R), cols(C) {}

    Array<T, C> &operator[](int i)
    {
        return data[i];
    }

    template <int A>
    Matrix<T, R, A> operator*(const Matrix<T, C, A> &in)
    {
        Matrix<T, R, A> tmp;
        for (int i=0; i<R; ++i)
        {
            for (int j=0; j<A; ++j)
            {
                tmp[i][j] = T(0);
                for (int k=0; k<C; ++k)
                {
                    tmp[i][j] += data[i][k]*in[k][j];
                }
            }
        }
        return tmp;
    }

    const int rows, cols;
private:
    Array<T, C> data[R];
};

} // namespace Inugami

#endif // CONTAINERS_H_INCLUDED
