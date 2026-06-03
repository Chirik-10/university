from google.colab import drive
drive.mount('/content/drive')
import pandas as pd
import numpy as np
from scipy.integrate import solve_ivp
from scipy.optimize import minimize
import matplotlib.pyplot as plt

url = "https://raw.githubusercontent.com/CSSEGISandData/COVID-19/master/csse_covid_19_data/csse_covid_19_time_series/time_series_covid19_confirmed_global.csv"
df = pd.read_csv(url)
italy_row = df[df["Country/Region"] == "Italy"].iloc[0, 4:]
dates = pd.to_datetime(df.columns[4:])

# Обрезка до восходящей фазы (24.02–25.03.2020)
start_date = pd.Timestamp("2020-02-24")
end_date = pd.Timestamp("2020-03-25")
mask = (dates >= start_date) & (dates <= end_date)
confirmed = italy_row[mask].values.astype(float)
t = np.arange(len(confirmed))

# Нормировка на население Италии
P = 60_360_000
I_data = confirmed / P

# Начальные условия
I0 = I_data[0]
R0 = 0.0
S0 = 1.0 - I0
y0 = [S0, I0, R0]

def sir_model(t, y, beta, gamma):
    S, I, R = y
    dSdt = -beta * S * I
    dIdt = beta * S * I - gamma * I
    dRdt = gamma * I
    return [dSdt, dIdt, dRdt]

def loss_function(params):
    beta, gamma = params
    if beta <= 0 or gamma <= 0:
        return np.inf
    sol = solve_ivp(sir_model, (t[0], t[-1]), y0, t_eval=t, args=(beta, gamma), method='RK45')
    I_model = sol.y[1]
    return np.mean((I_model - I_data) ** 2)

# Начальное приближение
x0 = [0.6, 0.1]  # из Calafiore [8]

result = minimize(loss_function, x0, method='Nelder-Mead',
                  options={'xatol': 1e-8, 'fatol': 1e-12, 'maxiter': 1000})

beta_est, gamma_est = result.x
print(f"Оценка параметров:")
print(f"β = {beta_est:.4f}")
print(f"γ = {gamma_est:.4f}")
print(f"R0 = β/γ = {beta_est / gamma_est:.2f}")
print(f"Значение функции потерь: {result.fun:.2e}")

# Условия сходимости
history = []
def loss_with_log(params):
    val = loss_function(params)
    history.append((params[0], params[1], val))
    return val

result_log = minimize(loss_with_log, x0, method='Nelder-Mead',
                      options={'xatol': 1e-8, 'fatol': 1e-12, 'maxiter': 1000})

# Построение графиков
sol_final = solve_ivp(sir_model, (t[0], t[-1]), y0, t_eval=t, args=(beta_est, gam-ma_est), method='RK45')
S_final, I_final, R_final = sol_final.y

fig, axs = plt.subplots(2, 2, figsize=(10, 8))

axs[0,0].plot(t, I_final, 'g-', label=r'I*(t) - восстановленные')
axs[0,0].plot(t, I_data, 'g--', label=r'I(t) - данные')
axs[0,0].set_ylabel(r'I(t)')
axs[0,0].legend()
axs[0,0].grid(True)

axs[0,1].plot(t, S_final, 'b-', label=r'S*(t)')
axs[0,1].set_ylabel(r'S(t)')
axs[0,1].legend()
axs[0,1].grid(True)

axs[1,0].plot(t, R_final, 'r-', label=r'R*(t)')
axs[1,0].set_ylabel(r'R(t)')
axs[1,0].legend()
axs[1,0].grid(True)

betas = [h[0] for h in history]
gammas = [h[1] for h in history]
axs[1,1].plot(betas, 'b-', label=r'$\beta^k$')
axs[1,1].plot(gammas, 'g-', label=r'$\gamma^k$')
axs[1,1].set_ylabel(r'Параметры')
axs[1,1].legend()
axs[1,1].grid(True)

plt.tight_layout(rect=[0, 0, 1, 0.95])
plt.savefig('sir_italy_real.png', dpi=200)
plt.show()

df_out = pd.DataFrame({
    'time': t,
    'Susceptible': S_final,
    'Infected': I_final,
    'Recovered': R_final
})
df_out.to_csv('/content/drive/MyDrive/Colab/pinn_sir/sir_italy.csv', index=False)
