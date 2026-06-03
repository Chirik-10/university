from google.colab import drive
drive.mount('/content/drive')
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import os

data_dir = '/content/drive/MyDrive/Colab/mo2/'

def get_contour_data_f1():
    x = np.linspace(-10, 15, 100)
    y = np.linspace(-10, 15, 100)
    X, Y = np.meshgrid(x, y)
    Z = 10*(X + Y - 10)**2 + (X - Y + 4)**2
    return X, Y, Z

def get_contour_data_f2():
    x = np.linspace(-2, 3, 100)
    y = np.linspace(-1, 5, 100)
    X, Y = np.meshgrid(x, y)
    Z = 100*(Y - X**2)**2 + (1 - X)**2
    return X, Y, Z

methods = ['Gauss', 'HookeJeeves', 'Rosenbrock']
functions = ['f1', 'f2']
func_titles = {
    'f1': r'$f_1(x)=10(x_1+x_2-10)^2+(x_1-x_2+4)^2$',
    'f2': r'Функция Розенброка $f_2(x)=100(x_2-x_1^2)^2+(1-x_1)^2$'
}
method_titles = {
    'Gauss': 'Метод Гаусса',
    'HookeJeeves': 'Метод Хука-Дживса',
    'Rosenbrock': 'Метод Розенброка'
}

eps_colors = {0.01: 'red', 0.0001: 'blue', 1e-06: 'green'}
eps_styles = {0.01: '-', 0.0001: '--', 1e-06: ':'}
eps_labels = {0.01: r'$\epsilon=10^{-2}$', 0.0001: r'$\epsilon=10^{-4}$', 1e-06: r'$\epsilon=10^{-6}$'}

all_data = []
for method in methods:
    for func in functions:
        filename = os.path.join(data_dir, f"{method}_{func}.csv")
        if os.path.exists(filename):
            df = pd.read_csv(filename)
            all_data.append(df)

data = pd.concat(all_data, ignore_index=True)

for method in methods:
    for func in functions:
        mask = (data['method'] == method) & (data['function'] == func)
        df_sub = data[mask]

        if df_sub.empty:
            continue

        print(f"  {method_titles[method]} + {func}")

        fig, ax = plt.subplots(figsize=(10, 8))

        if func == 'f1':
            Xc, Yc, Zc = get_contour_data_f1()
        else:
            Xc, Yc, Zc = get_contour_data_f2()

        contour = ax.contour(Xc, Yc, Zc, levels=30, colors='gray',
                            alpha=0.4, linewidths=0.6)

        eps_values = sorted(df_sub['eps1d'].unique())

        for eps_val in eps_values:
            mask_traj = (df_sub['eps1d'] == eps_val)
            traj = df_sub[mask_traj].sort_values('iter')

            if traj.empty:
                continue

            color = eps_colors.get(eps_val, 'black')
            style = eps_styles.get(eps_val, '-')
            label = eps_labels.get(eps_val, f'eps={eps_val}')

            ax.plot(traj['x'], traj['y'], style, color=color,
                   linewidth=1.5, alpha=0.8, label=label)

            ax.plot(traj['x'].iloc[0], traj['y'].iloc[0], 'o', color=color,
                   markersize=8, markeredgecolor='black', markeredgewidth=1)
            ax.plot(traj['x'].iloc[-1], traj['y'].iloc[-1], 'o', color=color,
                   markersize=8, markeredgecolor='black', markeredgewidth=1,
                   fillstyle='none')

        ax.set_xlabel(r'$x_1$', fontsize=12)
        ax.set_ylabel(r'$x_2$', fontsize=12)
        ax.set_title(f'{method_titles[method]}', fontsize=14)
        ax.grid(True, linestyle='--', alpha=0.4)
        ax.legend(loc='best', fontsize=10, framealpha=0.9)

        if func == 'f1':
            ax.set_xlim(-10, 15)
            ax.set_ylim(-10, 15)
        else:
            ax.set_xlim(-2, 3)
            ax.set_ylim(-1, 5)

        plt.tight_layout()
        plt.show()

print("СВОДНАЯ ТАБЛИЦА")

summary = []
for method in methods:
    for func in functions:
        mask = (data['method'] == method) & (data['function'] == func)
        df_sub = data[mask]
        eps_values = sorted(df_sub['eps1d'].unique())

        for eps_val in eps_values:
            mask_traj = (df_sub['eps1d'] == eps_val)
            traj = df_sub[mask_traj].sort_values('iter')

            if not traj.empty:
                n_iter = len(traj) - 1
                final_f = traj['f'].iloc[-1]
                final_x = traj['x'].iloc[-1]
                final_y = traj['y'].iloc[-1]
                summary.append([method_titles[method], func, eps_val, n_iter,
                              f"{final_f:.2e}", f"{final_x:.4f}", f"{final_y:.4f}"])

summary_df = pd.DataFrame(summary, columns=['Метод', 'Функция', 'eps', 'Итераций', 'f_min', 'x*', 'y*'])
print(summary_df.to_string(index=False))
summary_df.to_csv('convergence_summary.csv', index=False, encoding='utf-8-sig')
