#ifndef MATHTYPES_H_INCLUDED
#define MATHTYPES_H_INCLUDED

#include <cmath>

namespace Inugami
{

template <typename T, int S>
class Vector
{
    static_assert(S>0, "S must be at least 1.");
public:
    Vector<T, S>() {}

    Vector<T, S>(const T &a)
    {
        for (int i=0; i<S; ++i)
        {
            data[i] = a;
        }
    }

    Vector<T, S>(const T &a, const T &b, const T &c)
    {
        static_assert(S>=3, "Vector must have at least 3 components.");
        data[0] = a;
        data[1] = b;
        data[2] = c;
    }

    Vector<T, S>(const T &a, const T &b, const T &c, const T &d)
    {
        static_assert(S>=4, "Vector must have at least 4 components.");
        data[0] = a;
        data[1] = b;
        data[2] = c;
        data[3] = d;
    }

    template <int S2>
    Vector<T, 3> cross(const Vector<T, S2> &in) const
    {
        static_assert(S>=3, "Vector must have at least 3 components.");
        static_assert(S2>=3, "Input vector must have at least 3 components.");
        return Vector<T, 3>(
            data[1]*in.data[2] - data[2]*in.data[1],
            data[2]*in.data[0] - data[0]*in.data[2],
            data[0]*in.data[1] - data[1]*in.data[0]
        );
    }

    T dot(const Vector<T, S> &in) const
    {
        T rval = T(0);
        for (int i=0; i<S; ++i)
        {
            rval += data[i]*in.data[i];
        }
        return rval;
    }

    Vector<T, S> &operator=(const Vector<T, S> &in)
    {
        for (int i=0; i<S; ++i)
        {
            data[i] = in.data[i];
        }
        return *this;
    }

    T &operator[](int i)
    {
        return data[i];
    }

    Vector<T, S> &normalize()
    {
        T sum = T(0);
        for (int i=0; i<S; ++i)
        {
            sum += data[i]*data[i];
        }
        if (sum == T(0)) return *this;
        T mag = std::sqrt(sum);
        for (int i=0; i<S; ++i)
        {
            data[i] /= mag;
        }
        return *this;
    }

    Vector<T, S> operator-() const
    {
        Vector<T, S> tmp;
        for (int i=0; i<S; ++i)
        {
            tmp[i] = -data[i];
        }
        return tmp;
    }

    template <int S2>
    Vector<T, (S<S2)?S:S2> operator+(const Vector<T, S2> &in) const
    {
        Vector<T, (S<S2)?S:S2> tmp;
        for (int i=0; i<((S<S2)?S:S2); ++i)
        {
            tmp[i] = data[i]+in.data[i];
        }
        return tmp;
    }

    template <int S2>
    Vector<T, (S<S2)?S:S2> operator-(const Vector<T, S2> &in) const
    {
        Vector<T, (S<S2)?S:S2> tmp;
        for (int i=0; i<((S<S2)?S:S2); ++i)
        {
            tmp[i] = data[i]-in.data[i];
        }
        return tmp;
    }

    T operator*(const Vector<T, S> &in) const
    {
        return dot(in);
    }

    Vector<T, S> operator*(const T &in) const
    {
        Vector<T, S> tmp;
        for (int i=0; i<S; ++i)
        {
            tmp[i] = data[i]*in;
        }
        return tmp;
    }

    Vector<T, S> operator/(const T &in) const
    {
        Vector<T, S> tmp;
        for (int i=0; i<S; ++i)
        {
            tmp[i] = data[i]/in;
        }
        return tmp;
    }

    template <int S2>
    Vector<T, S> &operator+=(const Vector<T, S2> &in)
    {
        for (int i=0; i<((S<S2)?S:S2); ++i)
        {
            data[i] += in.data[i];
        }
        return *this;
    }

    template <int S2>
    Vector<T, S> &operator-=(const Vector<T, S2> &in)
    {
        for (int i=0; i<((S<S2)?S:S2); ++i)
        {
            data[i] -= in.data[i];
        }
        return *this;
    }

    Vector<T, S> &operator*=(const T &in)
    {
        for (int i=0; i<S; ++i)
        {
            data[i] *= in;
        }
        return *this;
    }

    Vector<T, S> &operator/=(const T &in)
    {
        for (int i=0; i<S; ++i)
        {
            data[i] /= in;
        }
        return *this;
    }

    template <int S2>
    Vector<T, S> &set(const Vector<T, S2> &in, int offset)
    {
        int a = (S<S2)?S:S2;
        int b = (a<S-offset)?a:S-offset;
        for (int i=0; i<b; ++i)
        {
            data[i+offset] = in.data[i];
        }
        return *this;
    }

    T data[S];
};

template <typename T, int S>
Vector<T, S> operator*(const T &in, const Vector<T, S> &vec)
{
    Vector<T, S> tmp;
    for (int i=0; i<S; ++i)
    {
        tmp[i] = vec.data[i]*in;
    }
    return tmp;
}

template <typename T, int R, int C>
class Matrix
{
    static_assert(R>0, "R must be at least 1.");
    static_assert(C>0, "C must be at least 1.");
public:
    Matrix<T, R, C>() {}
    Matrix<T, R, C>(const T &in)
    {
        for (int i=0; i<R; ++i) for (int j=0; j<C; ++j) data[i].data[j] = in;
    }

    Matrix<T, R, C> &operator=(const Matrix<T, R, C> &in)
    {
        for (int i=0; i<R; ++i)
        {
            data[i] = in.data[i];
        }
        return *this;
    }

    Vector<T, C> &operator[](int i)
    {
        return data[i];
    }

    template <int A>
    Matrix<T, R, A> operator*(const Matrix<T, C, A> &in) const
    {
        Matrix<T, R, A> tmp;
        for (int i=0; i<R; ++i)
        {
            for (int j=0; j<A; ++j)
            {
                tmp[i][j] = T(0);
                for (int k=0; k<C; ++k)
                {
                    tmp[i][j] += data[i].data[k]*in.data[k].data[j];
                }
            }
        }
        return tmp;
    }

    Vector<T, C> operator*(const Vector<T, C> &in) const
    {
        Vector<T, C> tmp;
        for (int i=0; i<R; ++i)
        {
            tmp[i] = data[i]*in;
        }
        return tmp;
    }

    Matrix<T, C, R> transpose() const
    {
        Matrix<T, C, R> tmp;
        for (int i=0; i<R; ++i)
        {
            for (int j=0; j<C; ++j)
            {
                tmp.data[j].data[i] = data[i].data[j];
            }
        }
        return tmp;
    }

    Vector<T, C> data[R];
};

template <typename T>
class Quaternion
{
public:
    Quaternion<T>() : v(T(0)), w(0), x(v[0]), y(v[1]), z(v[2]) {}

    template <int S>
    Quaternion<T>(const T &s, const Vector<T, S> &vin) :
        v(vin.data[0], vin.data[1], vin.data[2]),
        w(s),
        x(v[0]),
        y(v[1]),
        z(v[2])
    {}

    Quaternion<T> &operator=(const Quaternion<T> &in)
    {
        w = in.w;
        v = in.v;
        return *this;
    }

    Quaternion<T> &normalize()
    {
        T s = std::sqrt(w*w+x*x+y*y+z*z);
        w /= s;
        v /= s;
        return *this;
    }

    Quaternion<T> conjugate() const
    {
        return Quaternion<T>(w, -v);
    }

    Quaternion<T> operator*(const Quaternion<T> &in) const
    {
        return Quaternion<T>(
            w*in.w - v*in.v,
            w*in.v + v*in.w + v.cross(in.v)
        );
    }

    Vector<T, 3> operator()(const Vector<T, 3> &in) const
    {
        Vector<T, 4> tmp(in[0], in[1], in[2]);
        tmp[3] = 1.0;
        tmp = applyMatrix(tmp);
        return Vector<T, 3>(tmp[0], tmp[1], tmp[2]);
    }

    Vector<T, 4> operator()(const Vector<T, 4> &in) const
    {
        return applyMatrix(in);
    }

    Vector<T, 4> applyMatrix(const Vector<T, 4> &in) const
    {
        return getMatrix()*in;
    }

    Matrix<T, 4, 4> getMatrix() const
    {
        Matrix<T, 4, 4> r;
        r[0][0] = 1 -2*y*y -2*z*z;
        r[0][1] =    2*x*y -2*z*w;
        r[0][2] =    2*x*z +2*y*w;
        r[0][3] = 0;
        r[1][0] =    2*x*y +2*z*w;
        r[1][1] = 1 -2*x*x -2*z*z;
        r[1][2] =    2*y*z -2*x*w;
        r[1][3] = 0;
        r[2][0] =    2*x*z -2*y*w;
        r[2][1] =    2*y*z +2*x*w;
        r[2][2] = 1 -2*x*x -2*y*y;
        r[2][3] = 0;
        r[3][0] = 0;
        r[3][1] = 0;
        r[3][2] = 0;
        r[3][3] = 1;
        return r;
    }

    Vector<T, 3> v;
    T w, &x, &y, &z;
};

template <typename T>
Quaternion<T> quatFromAxisAngle(const T &angle, const Vector<T, 3> &axis)
{
    return Quaternion<T>(std::cos(angle/T(2)), axis*std::sin(angle/T(2)));
}

template <typename T>
class Transformation
{
public:
    Transformation<T>() : loc(T(0)), rot(T(1), Vector<T, 3>(T(0))) {loc[3]=1.0;}

    Transformation<T>(const Vector<T, 4> &inVec, const Quaternion<T> &inQuat) :
        loc(inVec),
        rot(inQuat)
    {}

    Transformation<T> &translate(const T &a, const T &b, const T &c)
    {
        return translate(Vector<T, 3>(a, b, c));
    }

    Transformation<T> &translate(const Vector<T, 3> &in)
    {
        loc += in;
        return *this;
    }

    Transformation<T> &rotate(const T &angle, const Vector<T, 3> &axis)
    {
        Quaternion<T> tmp = quatFromAxisAngle(angle, axis);
        rot = tmp*rot;
        rot.normalize();
        return *this;
    }

    Transformation<T> &rotate(const T &angle, const T &x, const T &y, const T &z)
    {
        return rotate(angle, Vector<T, 3>(x, y, z));
    }

    Transformation<T> &rotated(const T &angle, const Vector<T, 3> &axis)
    {
        return rotate(toRadians(angle), axis);
    }

    Transformation<T> &rotated(const T &angle, const T &x, const T &y, const T &z)
    {
        return rotate(toRadians(angle), Vector<T, 3>(x, y, z));
    }

    Matrix<T, 4, 4> getMatrix()
    {
        Matrix<T, 4, 4> tmp(T(0));
        tmp[0][3] = loc[0];
        tmp[1][3] = loc[1];
        tmp[2][3] = loc[2];
        return rot.getMatrix()*tmp;
    }

    Vector<T, 3> operator*(const Vector<T, 3> &in)
    {
        Vector<T, 4> tmp(in.data[0], in.data[1], in.data[2], T(1));
        tmp = getMatrix()*tmp;
        return Vector<T, 3>(tmp[0], tmp[1], tmp[2]);
    }

    Vector<T, 4> operator*(const Vector<T, 4> &in)
    {
        return getMatrix()*in;
    }

    Transformation<T> operator*(const Transformation<T> &in)
    {
        auto newloc = rot(in.loc) + loc;
        newloc[3] = T(1.0);
        return Transformation<T>(
            newloc,
            rot*in.rot
        );
    }

    Vector<T, 4> loc;
    Quaternion<T> rot;
};

} // namespace Inugami

#endif // MATHTYPES_H_INCLUDED
