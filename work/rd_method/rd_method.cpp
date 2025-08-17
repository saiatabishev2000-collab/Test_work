#include <iostream>
#include <cmath>
#include <fstream>
#include <limits>

using namespace std;

// Константы
const double c = 300000000.0;       // Скорость света (м/с)
const double dt12 = 1.47e-6;        // Задержка между постом 1 и 2 (с)
const double dt13 = -13.4e-6;       // Задержка между постом 1 и 3 (с)
const double d12 = c * dt12;        // Разность расстояний 1-2 (м)
const double d13 = c * dt13;        // Разность расстояний 1-3 (м)

// Координаты постов 
const double x1 = 5000.0, y_1 = 6000.0;
const double x2 = 1000.0, y2 = 1000.0;
const double x3 = 9000.0, y3 = 1000.0;

// Функция для вычисления расстояний и уравнений
void f_func(double x, double y, double &f1, double &f2, double &r1, double &r2, double &r3) {
    r1 = sqrt((x - x1)*(x - x1) + (y - y_1)*(y - y_1));
    r2 = sqrt((x - x2)*(x - x2) + (y - y2)*(y - y2));
    r3 = sqrt((x - x3)*(x - x3) + (y - y3)*(y - y3));
    f1 = r1 - r2 - d12;   // Уравнение для постов 1 и 2
    f2 = r1 - r3 - d13;   // Уравнение для постов 1 и 3
}

int main() {
    // Шаг 1: Грубый перебор для начального приближения
    double min_error = numeric_limits<double>::max();
    double x0 = 0, y0 = 0;

    // Область поиска: x [0, 10000], y [0, 12000] с шагом 100 м
    for (double x = 0; x <= 10000; x += 100) {
        for (double y = 0; y <= 12000; y += 100) {
            double r1, r2, r3, f1, f2;
            f_func(x, y, f1, f2, r1, r2, r3);
            double error = f1*f1 + f2*f2;  // Квадратичная ошибка
            if (error < min_error) {
                min_error = error;
                x0 = x;
                y0 = y;
            }
        }
    }

    // Шаг 2: Метод Ньютона для уточнения координат
    double x = x0, y = y0;
    double eps = 1e-5;       // Точность
    int max_iter = 100;      // Максимум итераций
    int iter = 0;

    while (iter < max_iter) {
        double r1, r2, r3, f1, f2;
        f_func(x, y, f1, f2, r1, r2, r3);

        // Проверка условия сходимости
        if (fabs(f1) < eps && fabs(f2) < eps) break;

        // Вычисление матрицы Якоби
        double df1dx = (x - x1) / r1 - (x - x2) / r2;
        double df1dy = (y - y_1) / r1 - (y - y2) / r2;
        double df2dx = (x - x1) / r1 - (x - x3) / r3;
        double df2dy = (y - y_1) / r1 - (y - y3) / r3;

        // Определитель матрицы Якоби
        double det = df1dx * df2dy - df1dy * df2dx;

        // Защита от вырожденной матрицы
        if (fabs(det) < 1e-10) {
            cerr << "Warning: Singular Jacobian matrix." << endl;
            break;
        }

        // Обратная матрица Якоби
        double invJ00 =  df2dy / det;
        double invJ01 = -df1dy / det;
        double invJ10 = -df2dx / det;
        double invJ11 =  df1dx / det;

        // Коррекция координат
        double dx = invJ00 * f1 + invJ01 * f2;
        double dy = invJ10 * f1 + invJ11 * f2;
        x -= dx;
        y -= dy;

        iter++;
    }

    // Вывод результатов
    cout << "IRI coordinates: (" << x << ", " << y << ")" << endl;

    // Сохранение данных для построения графика
    ofstream outfile("coordinates.txt");
    outfile << x1 << " " << y_1 << endl;
    outfile << x2 << " " << y2 << endl;
    outfile << x3 << " " << y3 << endl;
    outfile << x << " " << y << endl;
    outfile.close();

    return 0;
}
