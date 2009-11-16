
#include <QVector>
#include <vector>
#include "qrawvector.h"

const int N = 1000000;
double s = 0;

QVector<double> qvector_fill_and_return_helper()
{
    QVector<double> v(N);
    for (int i = 0; i != N; ++i)
        v[i] = i;
    return v;
}

QVector<double> qrawvector_fill_and_return_helper()
{
    QRawVector<double> v(N);
    for (int i = 0; i != N; ++i)
        v[i] = i;
    return v.mutateToVector();
}

QVector<double> mixedvector_fill_and_return_helper()
{
    std::vector<double> v(N);
    for (int i = 0; i != N; ++i)
        v[i] = i;
    return QVector<double>::fromStdVector(v);
}


std::vector<double> stdvector_fill_and_return_helper()
{
    std::vector<double> v(N);
    for (int i = 0; i != N; ++i)
        v[i] = i;
    return v;
}

