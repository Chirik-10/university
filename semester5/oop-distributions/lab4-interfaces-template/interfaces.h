#ifndef INTERFACES_H
#define INTERFACES_H

#include <stdio.h>

// Интерфейс распределения
class IDistribution {
public:
    virtual double density(double x) const = 0;
    virtual void characteristics(double* M, double* D, double* gamma1, double* gamma2) const = 0;
    virtual double random() const = 0;
    virtual ~IDistribution() {}
};

// Интерфейс персистентного объекта
class IPersistent {
public:
    virtual void save(FILE* out) const = 0;
    virtual void load(FILE* in) = 0;
    virtual ~IPersistent() {}
};
#endif
