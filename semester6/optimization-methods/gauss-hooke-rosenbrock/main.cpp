#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <string>
#include <chrono>

using namespace std;

class Vector {
public:
    double x, y;
    Vector(double x = 0, double y = 0) : x(x), y(y) {}
    Vector operator+(const Vector& o) const { return Vector(x + o.x, y + o.y); }
    Vector operator-(const Vector& o) const { return Vector(x - o.x, y - o.y); }
    Vector operator*(double s) const { return Vector(x * s, y * s); }
    Vector operator/(double s) const { return Vector(x / s, y / s); }
    double dot(const Vector& o) const { return x * o.x + y * o.y; }
    double norm() const { return sqrt(x * x + y * y); }
    Vector normalized() const {
        double n = norm();
        return (n > 1e-12) ? Vector(x / n, y / n) : Vector(0, 0);
    }
};

class Function {
public:
    virtual double operator()(const Vector& v) const = 0;
    virtual ~Function() {}
};

// f1(x) = 10*(x1 + x2 - 10)^2 + (x1 - x2 + 4)^2
class QuadraticFunction1 : public Function {
public:
    double operator()(const Vector& v) const override {
        double t1 = v.x + v.y - 10;
        double t2 = v.x - v.y + 4;
        return 10 * t1 * t1 + t2 * t2;
    }
};

// f2(x) = 100*(x2 - x1^2)^2 + (1 - x1)^2
class RosenbrockFunction : public Function {
public:
    double operator()(const Vector& v) const override {
        double t1 = v.y - v.x * v.x;
        double t2 = 1 - v.x;
        return 100 * t1 * t1 + t2 * t2;
    }
};

// Золотое сечение
double goldenSection(const Function & f, const Vector & x, const Vector & d,
    double eps, int maxIter1d = 50) {
    double a = -5.0, b = 5.0;
    double fa = f(x + d * a);
    double fb = f(x + d * b);

    // Расширяем интервал если нужно
    while (fa < fb && a < -100) { a *= 2; fa = f(x + d * a); }
    while (fb < fa && b > 100) { b *= 2; fb = f(x + d * b); }

    const double phi = (sqrt(5) - 1) / 2;
    double c = b - phi * (b - a);
    double d_ = a + phi * (b - a);
    double fc = f(x + d * c);
    double fd = f(x + d * d_);

    for (int iter = 0; iter < maxIter1d && (b - a) > eps; ++iter) {
        if (fc < fd) {
            b = d_; d_ = c; fd = fc;
            c = b - phi * (b - a);
            fc = f(x + d * c);
        }
        else {
            a = c; c = d_; fc = fd;
            d_ = a + phi * (b - a);
            fd = f(x + d * d_);
        }
    }
    return (a + b) / 2;
}

class Optimizer {
public:
    virtual string name() const = 0;
    virtual void optimize(const Function& f, const Vector& start, double eps, double eps1d,
        vector<Vector>& history, vector<double>& history_vals, int maxIter = 1000) = 0;
    virtual ~Optimizer() {}
};

// Метод Гаусса
class GaussOptimizer : public Optimizer {
public:
    string name() const override { return "Gauss"; }

    void optimize(const Function& f, const Vector& start, double eps, double eps1d,
        vector<Vector>& history, vector<double>& history_vals, int maxIter) override {
        Vector x = start;
        double fval = f(x);
        history.push_back(x);
        history_vals.push_back(fval);

        for (int iter = 0; iter < maxIter; ++iter) {
            Vector x_old = x;
            double f_old = fval;

            // По первой координате (S1 = [1, 0])
            Vector d1(1, 0);
            double lambda1 = goldenSection(f, x, d1, eps1d);
            x = x + d1 * lambda1;
            fval = f(x);
            history.push_back(x);
            history_vals.push_back(fval);

            // По второй координате (S2 = [0, 1])
            Vector d2(0, 1);
            double lambda2 = goldenSection(f, x, d2, eps1d);
            x = x + d2 * lambda2;
            fval = f(x);
            history.push_back(x);
            history_vals.push_back(fval);

            // Критерий остановки после полного цикла
            if (fabs(fval - f_old) < eps && (x - x_old).norm() < eps) break;
        }
    }
};

// Метод Хука-Дживса
class HookeJeevesOptimizer : public Optimizer {
public:
    string name() const override { return "HookeJeeves"; }

    void optimize(const Function& f, const Vector& start, double eps, double eps1d,
        vector<Vector>& history, vector<double>& history_vals, int maxIter) override {
        Vector xB = start; // базисная точка
        double fB = f(xB);
        Vector delta(0.5, 0.5); // начальный шаг
        double alpha = 0.5; // коэффициент уменьшения

        history.push_back(xB);
        history_vals.push_back(fB);

        for (int iter = 0; iter < maxIter && delta.norm() > eps; ++iter) {
            Vector xN = xB;
            double fN = fB;

            // По x1
            Vector xTest = xN + Vector(delta.x, 0);
            double fTest = f(xTest);
            if (fTest < fN) {
                xN = xTest; fN = fTest;
            }
            else {
                xTest = xN - Vector(delta.x, 0);
                fTest = f(xTest);
                if (fTest < fN) {
                    xN = xTest; fN = fTest;
                }
            }

            // По x2
            xTest = xN + Vector(0, delta.y);
            fTest = f(xTest);
            if (fTest < fN) {
                xN = xTest; fN = fTest;
            }
            else {
                xTest = xN - Vector(0, delta.y);
                fTest = f(xTest);
                if (fTest < fN) {
                    xN = xTest; fN = fTest;
                }
            }

            // Если нет улучшения - уменьшаем шаг
            if (fN >= fB) {
                delta = delta * alpha;
                continue;
            }

            history.push_back(xN);
            history_vals.push_back(fN);

            Vector S = xN - xB;
            if (S.norm() > 1e-12) {
                double lambda = goldenSection(f, xN, S, eps1d);
                Vector xNew = xN + S * lambda;
                double fNew = f(xNew);

                if (fNew < fN) {
                    history.push_back(xNew);
                    history_vals.push_back(fNew);
                    xB = xNew; fB = fNew;
                }
                else {
                    xB = xN; fB = fN;
                }
            }
            else {
                xB = xN; fB = fN;
            }
        }
    }
};

// Метод Розенброка 
class RosenbrockOptimizer : public Optimizer {
public:
    string name() const override { return "Rosenbrock"; }

    void optimize(const Function& f, const Vector& start, double eps, double eps1d,
        vector<Vector>& history, vector<double>& history_vals, int maxIter) override {
        Vector x = start;
        double fval = f(x);
        history.push_back(x);
        history_vals.push_back(fval);

        // Начальные направления - оси координат
        vector<Vector> S = { Vector(1,0), Vector(0,1) };
        const int n = 2;

        int stagnation_count = 0;
        const int MAX_STAGNATION = 5;

        for (int iter = 0; iter < maxIter; ++iter) {
            Vector x_old = x;
            double f_old = fval;

            // Минимизация по направлениям S_i
            vector<double> lambda(n, 0.0);
            for (int i = 0; i < n; ++i) {
                lambda[i] = goldenSection(f, x, S[i], eps1d);

                // Проверка на слишком маленький шаг
                if (fabs(lambda[i]) < 1e-12) {
                    lambda[i] = 0.0;
                }

                x = x + S[i] * lambda[i];
                fval = f(x);
                history.push_back(x);
                history_vals.push_back(fval);
            }

            double improvement = f_old - fval;
            if (improvement < eps1d) {
                stagnation_count++;
                if (stagnation_count >= MAX_STAGNATION) {
                    // Застой - останавливаемся
                    break;
                }
            }
            else {
                stagnation_count = 0;
            }

            vector<Vector> A(n);
            for (int i = 0; i < n; ++i) {
                A[i] = Vector(0, 0);
                for (int j = i; j < n; ++j) {
                    A[i] = A[i] + S[j] * lambda[j];
                }
            }

            // Проверка на вырождение
            double total_movement = 0.0;
            for (int i = 0; i < n; ++i) {
                total_movement += fabs(lambda[i]);
            }

            // Если общее перемещение слишком мало - не обновляем направления
            if (total_movement < 1e-6) {
                if (fabs(fval - f_old) < eps && (x - x_old).norm() < eps) break;
                continue;
            }

            vector<Vector> S_new(n);
            bool degenerate = false;

            double norm_A0 = A[0].norm();
            if (norm_A0 > 1e-8) {
                S_new[0] = A[0] / norm_A0;
            }
            else {
                // Вырождение - оставляем старое направление
                S_new[0] = S[0];
                degenerate = true;
            }

            // S_2^(k+1) по формуле Палмера (для n=2)
            if (n > 1) {
                double norm_A1 = A[1].norm();

                if (norm_A0 > 1e-8 && norm_A1 > 1e-8) {
                    double sqrt_norm_A0 = sqrt(norm_A0);

                    Vector term1 = A[1] * sqrt_norm_A0;
                    Vector term2 = A[0] * (A[1].dot(A[0]) / sqrt_norm_A0);
                    Vector B = term1 - term2;

                    double norm_B = B.norm();
                    if (norm_B > 1e-8) {
                        S_new[1] = B / norm_B;
                    }
                    else {
                        // Вырождение - ортогональное дополнение к S_new[0]
                        S_new[1] = Vector(-S_new[0].y, S_new[0].x);
                        degenerate = true;
                    }
                }
                else {
                    S_new[1] = Vector(-S_new[0].y, S_new[0].x);
                    degenerate = true;
                }
            }

            if (degenerate || fabs(S_new[0].dot(S_new[1])) > 0.1) {
                static int bad_update_count = 0;
                bad_update_count++;
                if (bad_update_count > 10) {
                    S = { Vector(1,0), Vector(0,1) };
                    bad_update_count = 0;
                }
            }
            else {
                S = S_new;
            }

            // Критерий остановки
            if (fabs(fval - f_old) < eps && (x - x_old).norm() < eps) break;
        }
    }
};

int main() {
    QuadraticFunction1 f1;
    RosenbrockFunction f2;

    vector<Vector> starts_f1 = { Vector(0, 0) };
    vector<Vector> starts_f2 = { Vector(-1.2, 1) };

    vector<double> eps1d_list = { 1e-2, 1e-4, 1e-6 };

    GaussOptimizer gauss;
    HookeJeevesOptimizer hooke;
    RosenbrockOptimizer rosenbrock;

    vector<Optimizer*> optimizers = { &gauss, &hooke, &rosenbrock };
    vector<string> funcNames = { "f1", "f2" };
    vector<Function*> functions = { &f1, &f2 };

    double eps = 1e-6;
    int maxIter = 500;

    for (auto opt : optimizers) {
        for (size_t ifunc = 0; ifunc < functions.size(); ++ifunc) {
            string filename = opt->name() + "_" + funcNames[ifunc] + ".csv";
            ofstream file(filename);
            file << "method,function,start_x,start_y,eps1d,iter,x,y,f,time_us\n";

            const vector<Vector>& starts = (ifunc == 0) ? starts_f1 : starts_f2;
            const Function& func = *functions[ifunc];

            cout << "Метод: " << opt->name() << ", Функция: " << funcNames[ifunc] << endl;

            for (const Vector& start : starts) {
                for (double eps1d : eps1d_list) {
                    vector<Vector> history;
                    vector<double> fvals;

                    auto start_time = chrono::high_resolution_clock::now();
                    opt->optimize(func, start, eps, eps1d, history, fvals, maxIter);
                    auto end_time = chrono::high_resolution_clock::now();
                    auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);

                    for (size_t i = 0; i < history.size(); ++i) {
                        file << opt->name() << ","
                            << funcNames[ifunc] << ","
                            << start.x << "," << start.y << ","
                            << eps1d << ","
                            << i << ","
                            << history[i].x << "," << history[i].y << ","
                            << fvals[i] << ","
                            << duration.count() << "\n";
                    }

                    cout << "  eps1d: " << setw(8) << eps1d
                        << ", Итераций: " << setw(4) << (history.size() - 1)
                        << ", f_min: " << scientific << setprecision(2) << fvals.back()
                        << ", x*: (" << fixed << setprecision(4) << history.back().x
                        << ", " << history.back().y << ")" << endl;
                }
            }
            file.close();
            cout << endl;
        }
    }

    return 0;
}
