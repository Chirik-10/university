#include <iostream>
#include <vector>
#include <cmath>
#include <functional>
#include <fstream>
#include <iomanip>
#include <string>
#include <limits>
#include <algorithm>

using namespace std;

class Vector {
public:
    vector<double> data;

    Vector() {}
    explicit Vector(int n) : data(n, 0.0) {}
    Vector(const vector<double>& v) : data(v) {}

    int size() const { return static_cast<int>(data.size()); }
    double& operator[](int i) { return data[i]; }
    const double& operator[](int i) const { return data[i]; }

    Vector operator+(const Vector& other) const {
        Vector result(size());
        for (int i = 0; i < size(); i++) result[i] = data[i] + other[i];
        return result;
    }

    Vector operator-(const Vector& other) const {
        Vector result(size());
        for (int i = 0; i < size(); i++) result[i] = data[i] - other[i];
        return result;
    }

    Vector operator*(double lambda) const {
        Vector result(size());
        for (int i = 0; i < size(); i++) result[i] = data[i] * lambda;
        return result;
    }

    double dot(const Vector& other) const {
        double sum = 0.0;
        for (int i = 0; i < size(); i++) sum += data[i] * other[i];
        return sum;
    }

    double norm() const { return sqrt(dot(*this)); }

    Vector& operator=(const Vector& other) {
        if (this != &other) data = other.data;
        return *this;
    }

    void print() const {
        cout << "[";
        for (int i = 0; i < size(); i++)
            cout << fixed << setprecision(6) << data[i] << (i < size() - 1 ? ", " : "");
        cout << "]";
    }
};

class Matrix {
public:
    vector<vector<double>> data;
    int rows, cols;

    Matrix() : rows(0), cols(0) {}
    Matrix(int n, int m) : rows(n), cols(m) { data.resize(n, vector<double>(m, 0.0)); }
    explicit Matrix(int n) : rows(n), cols(n) { data.resize(n, vector<double>(n, 0.0)); }

    static Matrix identity(int n) {
        Matrix I(n);
        for (int i = 0; i < n; i++) I[i][i] = 1.0;
        return I;
    }

    vector<double>& operator[](int i) { return data[i]; }
    const vector<double>& operator[](int i) const { return data[i]; }

    Vector operator*(const Vector& v) const {
        Vector result(rows);
        for (int i = 0; i < rows; i++) {
            result[i] = 0.0;
            for (int j = 0; j < cols; j++) result[i] += data[i][j] * v[j];
        }
        return result;
    }

    Matrix operator+(const Matrix& other) const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                result[i][j] = data[i][j] + other[i][j];
        return result;
    }

    Matrix operator-(const Matrix& other) const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                result[i][j] = data[i][j] - other[i][j];
        return result;
    }

    Matrix& operator=(const Matrix& other) {
        if (this != &other) { rows = other.rows; cols = other.cols; data = other.data; }
        return *this;
    }

    Matrix symmetrized() const {
        Matrix result(rows, cols);
        for (int i = 0; i < rows; i++)
            for (int j = 0; j < cols; j++)
                result[i][j] = (data[i][j] + data[j][i]) / 2.0;
        return result;
    }
};

class Function {
public:
    virtual double value(const Vector& x) const = 0;
    virtual Vector gradient(const Vector& x) const = 0;
    virtual string name() const = 0;
    virtual Vector getStartPoint() const = 0;
    virtual Vector getExactMinimum() const = 0;
    virtual ~Function() {}
};

class QuadraticFunction : public Function {
public:
    double value(const Vector& x) const override {
        double term1 = 10 * pow(x[0] + x[1] - 10, 2);
        double term2 = pow(x[0] - x[1] + 4, 2);
        return term1 + term2;
    }

    Vector gradient(const Vector& x) const override {
        Vector grad(2);
        grad[0] = 20 * (x[0] + x[1] - 10) + 2 * (x[0] - x[1] + 4);
        grad[1] = 20 * (x[0] + x[1] - 10) - 2 * (x[0] - x[1] + 4);
        return grad;
    }

    string name() const override { return "Quadratic"; }
    Vector getStartPoint() const override { return Vector({ 0.0, 0.0 }); }
    Vector getExactMinimum() const override { return Vector({ 3.0, 7.0 }); }
};

class RosenbrockFunction : public Function {
public:
    double value(const Vector& x) const override {
        double term1 = 100 * pow(x[1] - x[0] * x[0], 2);
        double term2 = pow(1 - x[0], 2);
        return term1 + term2;
    }

    Vector gradient(const Vector& x) const override {
        Vector grad(2);
        grad[0] = -400 * x[0] * (x[1] - x[0] * x[0]) - 2 * (1 - x[0]);
        grad[1] = 200 * (x[1] - x[0] * x[0]);
        return grad;
    }

    string name() const override { return "Rosenbrock"; }
    Vector getStartPoint() const override { return Vector({ -1.2, 1.0 }); }
    Vector getExactMinimum() const override { return Vector({ 1.0, 1.0 }); }
};

struct OptimizationResult {
    Vector x_min;
    double f_min;
    int iterations;
    int function_calls;
    int gradient_calls;
    vector<Vector> trajectory;
    string method_name;
    string function_name;

    OptimizationResult(const string& mname, const string& fname)
        : method_name(mname), function_name(fname), f_min(0.0),
        iterations(0), function_calls(0), gradient_calls(0) {
    }
};

struct Bracket {
    double a, b, c, fa, fb, fc;
    Bracket() : a(0), b(0), c(0), fa(0), fb(0), fc(0) {}
};

class LineSearch {
private:
    double lambda_init, lambda_max;
    int max_shrink, max_expand;

    double compute_adaptive_lambda(double grad_norm) const {
        if (grad_norm > 1000) return 1e-6;
        if (grad_norm > 100) return 1e-5;
        if (grad_norm > 10) return 1e-4;
        if (grad_norm > 1) return 1e-3;
        return lambda_init;
    }

public:
    LineSearch(double init = 1.0, double max_l = 1e6, int shrink = 50, int expand = 60)
        : lambda_init(init), lambda_max(max_l), max_shrink(shrink), max_expand(expand) {
    }

    Bracket bracketMinimum(const Function& f, const Vector& x0, const Vector& s,
        double f0, int& f_calls, double init_lambda) const {

        auto phi = [&](double l) {
            Vector p = x0 + s * l;
            f_calls++;
            return f.value(p);
            };

        Bracket br;
        br.a = 0.0; br.fa = f0;
        br.b = init_lambda; br.fb = phi(br.b);

        // Если шаг слишком большой, уменьшаем его
        int shrink = 0;
        while (br.fb >= br.fa && abs(br.b) > 1e-14 && shrink < max_shrink) {
            br.b *= 0.5;
            br.fb = phi(br.b);
            shrink++;
        }

        // Если даже очень маленький шаг не улучшает, возвращаем ноль
        if (br.fb >= br.fa || shrink >= max_shrink) {
            return br;
        }

        // Расширяем интервал, пока функция убывает
        br.c = 2.0 * br.b;
        br.fc = phi(br.c);

        int expand = 0;
        while (br.fc < br.fb && abs(br.c) < lambda_max && expand < max_expand) {
            br.a = br.b; br.fa = br.fb;
            br.b = br.c; br.fb = br.fc;
            br.c = 2.0 * br.c;
            br.fc = phi(br.c);
            expand++;
        }

        return br;
    }

    double goldenSearch(const Function& f, const Vector& x0, const Vector& s,
        const Bracket& br, double eps_line, double f0, int& f_calls) const {

        if (br.a == 0.0 && br.b == 0.0 && br.c == 0.0) return 0.0;

        double left = min(br.a, br.c);
        double right = max(br.a, br.c);
        const double gr = (sqrt(5.0) - 1.0) / 2.0;

        auto phi = [&](double l) {
            Vector p = x0 + s * l;
            f_calls++;
            return f.value(p);
            };

        double x1 = right - gr * (right - left);
        double x2 = left + gr * (right - left);
        double f1 = phi(x1);
        double f2 = phi(x2);

        int iter = 0;
        while ((right - left) > eps_line && iter < 200) {
            if (f1 < f2) {
                right = x2;
                x2 = x1; f2 = f1;
                x1 = right - gr * (right - left);
                f1 = phi(x1);
            }
            else {
                left = x1;
                x1 = x2; f1 = f2;
                x2 = left + gr * (right - left);
                f2 = phi(x2);
            }
            iter++;
        }

        double lambda = (f1 < f2) ? x1 : x2;
        double f_lambda = (f1 < f2) ? f1 : f2;

        // Гарантируем монотонное убывание
        if (f_lambda >= f0) {
            double lam = lambda * 0.1;
            for (int bt = 0; bt < 20 && abs(lam) > 1e-14; bt++) {
                f_lambda = phi(lam);
                if (f_lambda < f0) {
                    lambda = lam;
                    break;
                }
                lam *= 0.5;
            }
            if (f_lambda >= f0) return 0.0;
        }

        return lambda;
    }

    double search(const Function& f, const Vector& x, const Vector& direction,
        double f0, double eps_line, int& f_calls) {

        // Адаптивный начальный шаг в зависимости от нормы градиента
        double grad_norm = direction.norm();
        double init_lambda = compute_adaptive_lambda(grad_norm);

        Bracket br = bracketMinimum(f, x, direction, f0, f_calls, init_lambda);
        return goldenSearch(f, x, direction, br, eps_line, f0, f_calls);
    }
};

class OptimizationMethod {
protected:
    double eps_stop, eps_line;
    string method_name;
    LineSearch line_search;

public:
    OptimizationMethod(double eps_grad, double eps_ls, const string& name)
        : eps_stop(eps_grad), eps_line(eps_ls), method_name(name),
        line_search(1.0, 1e6, 50, 60) {
    }

    virtual OptimizationResult minimize(const Function& f, const Vector& x0) = 0;
    virtual ~OptimizationMethod() {}
    string getName() const { return method_name; }
};

class SteepestDescent : public OptimizationMethod {
public:
    SteepestDescent(double eps_grad, double eps_ls)
        : OptimizationMethod(eps_grad, eps_ls, "Steepest Descent") {
    }

    OptimizationResult minimize(const Function& f, const Vector& x0) override {
        OptimizationResult result(method_name, f.name());
        Vector x = x0;
        result.trajectory.push_back(x);

        double fx = f.value(x); result.function_calls++;
        Vector grad = f.gradient(x); result.gradient_calls++;

        for (int iter = 0; iter < 20000; iter++) {
            if (grad.norm() < eps_stop) break;

            Vector s = grad * (-1.0);

            double lambda = line_search.search(f, x, s, fx, eps_line, result.function_calls);
            if (abs(lambda) < 1e-14) {
                // Если шаг слишком маленький, пробуем увеличить
                lambda = 1e-8;
                Vector x_test = x + s * lambda;
                double f_test = f.value(x_test); result.function_calls++;
                if (f_test >= fx) break;
            }

            Vector x_new = x + s * lambda;
            double f_new = f.value(x_new); result.function_calls++;
            Vector grad_new = f.gradient(x_new); result.gradient_calls++;

            result.trajectory.push_back(x_new);

            x = x_new; fx = f_new; grad = grad_new;
            result.iterations = iter + 1;
        }
        result.x_min = x; result.f_min = fx;
        return result;
    }
};

class ConjugateGradient : public OptimizationMethod {
protected:
    int n;
    bool use_reset;

public:
    ConjugateGradient(double eps_grad, double eps_ls, const string& name, bool reset = true)
        : OptimizationMethod(eps_grad, eps_ls, name), use_reset(reset), n(0) {
    }

    virtual double computeBeta(const Vector& grad_new, const Vector& grad_old,
        const Vector& direction_old) = 0;

    OptimizationResult minimize(const Function& f, const Vector& x0) override {
        OptimizationResult result(method_name, f.name());
        Vector x = x0;
        n = x.size();
        result.trajectory.push_back(x);

        double fx = f.value(x); result.function_calls++;
        Vector grad = f.gradient(x); result.gradient_calls++;

        Vector s = grad * (-1.0);

        for (int iter = 0; iter < 20000; iter++) {
            if (grad.norm() < eps_stop) break;

            double lambda = line_search.search(f, x, s, fx, eps_line, result.function_calls);
            if (abs(lambda) < 1e-14) break;

            Vector x_new = x + s * lambda;
            double f_new = f.value(x_new); result.function_calls++;
            Vector grad_new = f.gradient(x_new); result.gradient_calls++;

            result.trajectory.push_back(x_new);

            double beta = computeBeta(grad_new, grad, s);
            Vector s_new = grad_new * (-1.0) + s * beta;

            if (use_reset && (iter + 1) % (n + 1) == 0) {
                s_new = grad_new * (-1.0);
            }

            // Проверка направления спуска
            if (grad_new.dot(s_new) >= 0.0) {
                s_new = grad_new * (-1.0);
            }

            x = x_new; fx = f_new; grad = grad_new; s = s_new;
            result.iterations = iter + 1;
        }
        result.x_min = x; result.f_min = fx;
        return result;
    }
};

class FletcherReeves : public ConjugateGradient {
public:
    FletcherReeves(double eps_grad, double eps_ls)
        : ConjugateGradient(eps_grad, eps_ls, "Fletcher-Reeves", true) {
    }

    double computeBeta(const Vector& grad_new, const Vector& grad_old,
        const Vector& direction_old) override {
        double grad_old_norm2 = grad_old.dot(grad_old);
        if (grad_old_norm2 < 1e-18) return 0.0;
        return grad_new.dot(grad_new) / grad_old_norm2;
    }
};

class PolakRibiere : public ConjugateGradient {
public:
    PolakRibiere(double eps_grad, double eps_ls)
        : ConjugateGradient(eps_grad, eps_ls, "Polak-Ribiere", true) {
    }

    double computeBeta(const Vector& grad_new, const Vector& grad_old,
        const Vector& direction_old) override {
        Vector grad_diff = grad_new - grad_old;
        double numerator = grad_new.dot(grad_diff);
        double denominator = grad_old.dot(grad_old);

        if (abs(denominator) < 1e-18) return 0.0;
        return max(0.0, numerator / denominator);
    }
};

class VariableMetric : public OptimizationMethod {
protected:
    Matrix eta;
    int n;

public:
    VariableMetric(double eps_grad, double eps_ls, const string& name)
        : OptimizationMethod(eps_grad, eps_ls, name), eta(), n(0) {
    }

    virtual Matrix updateEta(const Matrix& eta_current, const Vector& dx,
        const Vector& dg) = 0;

    OptimizationResult minimize(const Function& f, const Vector& x0) override {
        OptimizationResult result(method_name, f.name());
        Vector x = x0;
        n = x.size();
        eta = Matrix::identity(n);

        double fx = f.value(x); result.function_calls++;
        Vector grad = f.gradient(x); result.gradient_calls++;
        result.trajectory.push_back(x);

        for (int iter = 0; iter < 20000; iter++) {
            if (grad.norm() < eps_stop) break;

            Vector s = eta * grad * (-1.0);

            // Проверка направления спуска
            if (grad.dot(s) >= 0.0) {
                eta = Matrix::identity(n);
                s = grad * (-1.0);
            }

            double lambda = line_search.search(f, x, s, fx, eps_line, result.function_calls);
            if (abs(lambda) < 1e-14) break;

            Vector x_new = x + s * lambda;
            double f_new = f.value(x_new); result.function_calls++;
            Vector grad_new = f.gradient(x_new); result.gradient_calls++;

            result.trajectory.push_back(x_new);

            Vector dx_vec = x_new - x;
            Vector dg_vec = grad_new - grad;

            if (dx_vec.norm() > 1e-12 && dg_vec.norm() > 1e-12) {
                double dx_dg = dx_vec.dot(dg_vec);
                if (dx_dg > 1e-12) {
                    Matrix eta_new = updateEta(eta, dx_vec, dg_vec);
                    eta = eta_new.symmetrized();
                }
            }

            // Проверка положительной определённости
            Vector test_dir = eta * grad_new;
            if (grad_new.dot(test_dir) <= 0.0) {
                eta = Matrix::identity(n);
            }

            x = x_new; fx = f_new; grad = grad_new;
            result.iterations = iter + 1;
        }
        result.x_min = x; result.f_min = fx;
        return result;
    }
};

class Broyden : public VariableMetric {
public:
    Broyden(double eps_grad, double eps_ls)
        : VariableMetric(eps_grad, eps_ls, "Broyden") {
    }

    Matrix updateEta(const Matrix& eta_current, const Vector& dx,
        const Vector& dg) override {
        Vector eta_dg = eta_current * dg;
        Vector u = dx - eta_dg;
        double denominator = u.dot(dg);

        if (abs(denominator) < 1e-18) return eta_current;

        Matrix delta_eta(n, n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                delta_eta[i][j] = u[i] * u[j] / denominator;

        return eta_current + delta_eta;
    }
};

class DFP : public VariableMetric {
public:
    DFP(double eps_grad, double eps_ls)
        : VariableMetric(eps_grad, eps_ls, "DFP") {
    }

    Matrix updateEta(const Matrix& eta_current, const Vector& dx,
        const Vector& dg) override {
        double dx_dg = dx.dot(dg);
        if (abs(dx_dg) < 1e-18) return eta_current;

        Vector eta_dg = eta_current * dg;
        double dg_eta_dg = dg.dot(eta_dg);

        Matrix term1(n, n);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                term1[i][j] = dx[i] * dx[j] / dx_dg;

        Matrix term2(n, n);
        if (abs(dg_eta_dg) > 1e-18) {
            for (int i = 0; i < n; i++)
                for (int j = 0; j < n; j++)
                    term2[i][j] = eta_dg[i] * eta_dg[j] / dg_eta_dg;
        }

        return eta_current + term1 - term2;
    }
};

class ResultWriter {
private:
    string filename;

public:
    explicit ResultWriter(const string& fname) : filename(fname) {
        ofstream file(filename);
        file << "Method,Function,Iterations,FunctionCalls,GradientCalls,x1,x2,f_min,Success\n";
        file.close();
    }

    void writeResult(const OptimizationResult& result) {
        ofstream file(filename, ios::app);
        Vector exact_min = (result.function_name == "Quadratic")
            ? Vector({ 3.0, 7.0 }) : Vector({ 1.0, 1.0 });

        bool success = (abs(result.x_min[0] - exact_min[0]) < 1e-2 &&
            abs(result.x_min[1] - exact_min[1]) < 1e-2);

        file << result.method_name << "," << result.function_name << ","
            << result.iterations << "," << result.function_calls << ","
            << result.gradient_calls << ","
            << fixed << setprecision(8)
            << result.x_min[0] << "," << result.x_min[1] << ","
            << result.f_min << "," << (success ? "Yes" : "No") << "\n";
        file.close();
    }

    void writeTrajectory(const OptimizationResult& result) {
        string traj_filename = "trajectory_" + result.method_name + "_" +
            result.function_name + ".csv";
        ofstream file(traj_filename);
        file << "x1,x2,iteration\n";
        file << fixed << setprecision(16);
        for (size_t i = 0; i < result.trajectory.size(); i++) {
            file << result.trajectory[i][0] << ","
                << result.trajectory[i][1] << "," << i << "\n";
        }
        file.close();
    }

    void writeAccuracyStudy(const string& fname,
        const vector<pair<double, OptimizationResult>>& results) {
        ofstream file(fname);
        file << "Tolerance,Method,Iterations,FunctionCalls,GradientCalls\n";
        for (const auto& res : results) {
            file << res.first << "," << res.second.method_name << ","
                << res.second.iterations << "," << res.second.function_calls << ","
                << res.second.gradient_calls << "\n";
        }
        file.close();
    }
};

void investigateLineSearchAccuracy(ResultWriter& writer) {
    cout << "\nИсследование зависимости от точности одномерного поиска\n";
    QuadraticFunction f;
    Vector x0 = f.getStartPoint();
    vector<double> tolerances = { 1e-1, 1e-2, 1e-3, 1e-4, 1e-5, 1e-6 };
    double eps_grad = 1e-8;
    vector<pair<double, OptimizationResult>> accuracy_results;

    for (double tol : tolerances) {
        cout << "\nТочность одномерного поиска: " << tol << endl;
        SteepestDescent sd(eps_grad, tol);
        FletcherReeves fr(eps_grad, tol);
        PolakRibiere pr(eps_grad, tol);
        Broyden broyden(eps_grad, tol);
        DFP dfp(eps_grad, tol);

        vector<OptimizationMethod*> methods = { &sd, &fr, &pr, &broyden, &dfp };
        vector<string> names = { "Steepest Descent", "Fletcher-Reeves", "Polak-Ribiere",
                               "Broyden", "DFP" };

        for (size_t i = 0; i < methods.size(); i++) {
            OptimizationResult result = methods[i]->minimize(f, x0);
            cout << names[i] << ": итераций=" << result.iterations
                << ", вычислений f=" << result.function_calls << endl;
            accuracy_results.push_back({ tol, result });
        }
    }
    writer.writeAccuracyStudy("accuracy_study.csv", accuracy_results);
}

void investigateStartPointDependence(ResultWriter& writer) {
    cout << "\nИсследование зависимости от начального приближения\n";
    QuadraticFunction f;
    double eps_grad = 1e-8, eps_line = 1e-4;
    vector<Vector> starts = { Vector({0.0, 0.0}), Vector({-5.0, 5.0}), Vector({10.0, -5.0}) };
    vector<string> start_names = { "[0,0]", "[-5,5]", "[10,-5]" };

    for (size_t idx = 0; idx < starts.size(); idx++) {
        cout << "\nНачальная точка: " << start_names[idx] << endl;
        SteepestDescent sd(eps_grad, eps_line);
        FletcherReeves fr(eps_grad, eps_line);
        PolakRibiere pr(eps_grad, eps_line);
        Broyden broyden(eps_grad, eps_line);
        DFP dfp(eps_grad, eps_line);

        vector<OptimizationMethod*> methods = { &sd, &fr, &pr, &broyden, &dfp };
        vector<string> names = { "Steepest Descent", "Fletcher-Reeves", "Polak-Ribiere",
                               "Broyden", "DFP" };

        cout << string(70, '-') << endl;
        cout << "Метод            | Итерации | Вызовы f | Вызовы ∇f" << endl;
        cout << string(70, '-') << endl;

        for (size_t i = 0; i < methods.size(); i++) {
            OptimizationResult result = methods[i]->minimize(f, starts[idx]);
            writer.writeResult(result);
            writer.writeTrajectory(result);
            cout << left << setw(16) << names[i] << " | "
                << right << setw(8) << result.iterations << " | "
                << setw(8) << result.function_calls << " | "
                << setw(9) << result.gradient_calls << endl;
        }
    }
}

int main() {
    cout << fixed << setprecision(6);
    double eps_grad = 1e-8, eps_line = 1e-4;

    QuadraticFunction quad_func;
    RosenbrockFunction ros_func;
    Vector quad_x0 = quad_func.getStartPoint();
    Vector ros_x0 = ros_func.getStartPoint();

    SteepestDescent sd(eps_grad, eps_line);
    FletcherReeves fr(eps_grad, eps_line);
    PolakRibiere pr(eps_grad, eps_line);
    Broyden broyden(eps_grad, eps_line);
    DFP dfp(eps_grad, eps_line);

    vector<OptimizationMethod*> methods = { &sd, &fr, &pr, &broyden, &dfp };
    vector<string> method_names = { "Steepest Descent", "Fletcher-Reeves", "Polak-Ribiere",
                                  "Broyden", "DFP" };

    ResultWriter writer("optimization_results.csv");

    cout << "\nМИНИМИЗАЦИЯ КВАДРАТИЧНОЙ ФУНКЦИИ\n";
    cout << "Начальная точка: "; quad_x0.print(); cout << "\n";
    cout << "Точный минимум: [3, 7]\n\n";

    cout << "Результаты (x0 = [0, 0]):\n";
    cout << string(85, '-') << endl;
    cout << "Метод               | Итерации | Вызовы f | Вызовы grad(f) |   x1    |   x2    |   f_min" << endl;
    cout << string(85, '-') << endl;

    for (size_t i = 0; i < methods.size(); i++) {
        OptimizationResult result = methods[i]->minimize(quad_func, quad_x0);
        writer.writeResult(result);
        writer.writeTrajectory(result);

        cout << left << setw(18) << method_names[i] << " | "
            << right << setw(8) << result.iterations << " | "
            << setw(8) << result.function_calls << " | "
            << setw(9) << result.gradient_calls << " | "
            << setw(8) << result.x_min[0] << " | "
            << setw(8) << result.x_min[1] << " | "
            << setw(12) << result.f_min << endl;
    }

    cout << "\n\nМИНИМИЗАЦИЯ ФУНКЦИИ РОЗЕНБРОКА\n";
    cout << "Начальная точка: "; ros_x0.print(); cout << "\n";
    cout << "Точный минимум: [1, 1]\n\n";

    cout << "Результаты (x0 = [-1.2, 1]):\n";
    cout << string(85, '-') << endl;
    cout << "Метод               | Итерации | Вызовы f | Вызовы grad(f) |   x1    |   x2    |   f_min" << endl;
    cout << string(85, '-') << endl;

    for (size_t i = 0; i < methods.size(); i++) {
        OptimizationResult result = methods[i]->minimize(ros_func, ros_x0);
        writer.writeResult(result);
        writer.writeTrajectory(result);

        string success = (result.f_min < 1e-6) ? "да" : " ";
        cout << left << setw(18) << method_names[i] << " | "
            << right << setw(8) << result.iterations << " | "
            << setw(8) << result.function_calls << " | "
            << setw(9) << result.gradient_calls << " | "
            << setw(8) << result.x_min[0] << " | "
            << setw(8) << result.x_min[1] << " | "
            << setw(12) << result.f_min << " " << success << endl;
    }

    investigateLineSearchAccuracy(writer);
    investigateStartPointDependence(writer);

    cout << "\nРезультаты записаны в файлы\n";
    cout << "Файлы траекторий: trajectory_*.csv\n";
    cout << "Сводный файл: optimization_results.csv\n";

    return 0;
}
