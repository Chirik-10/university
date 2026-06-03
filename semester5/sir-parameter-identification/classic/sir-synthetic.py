!pip install torch torchvision torchaudio
!pip install pandas numpy scikit-learn matplotlib plotnine
from google.colab import drive
drive.mount('/content/drive')
import numpy as np
import pandas as pd
from scipy.integrate import solve_ivp
from scipy.optimize import minimize
import matplotlib.pyplot as plt

df = pd.read_csv('/content/drive/MyDrive/Colab/pinn_sir/sir_data.csv')
t_data = df['time'].values
S_obs = df['Susceptible'].values
I_obs = df['Infected'].values
R_obs = df['Recovered'].values

S0, I0, R0 = S_obs[0], I_obs[0], R_obs[0]

def sir_ode(t, y, beta, gamma):
    S, I, R = y
    dSdt = -beta * S * I
    dIdt = beta * S * I - gamma * I
    dRdt = gamma * I
    return [dSdt, dIdt, dRdt]

def loss_function(params):
    beta, gamma = params
    if beta <= 0 or gamma <= 0:
        return np.inf

    sol = solve_ivp(sir_ode, (t_data[0], t_data[-1]), [S0, I0, R0],
                    t_eval=t_data, args=(beta, gamma), method='RK45')
    S_pred, I_pred, R_pred = sol.y

    mse_S = np.mean((S_pred - S_obs)**2)
    mse_I = np.mean((I_pred - I_obs)**2)
    mse_R = np.mean((R_pred - R_obs)**2)
    return mse_S + mse_I + mse_R

x0 = np.array([0.1, 0.1]) # начальное приближение

# Логгирование истории итераций
history = []
def loss_with_logging(params):
    val = loss_function(params)
    history.append((params[0], params[1], val))
    return val

result = minimize(loss_with_logging, x0, method='Nelder-Mead',
                  options={'xatol': 1e-8, 'fatol': 1e-12, 'maxiter': 1000, 'disp': True})

beta_est, gamma_est = result.x
print(f"Оценка: β = {beta_est:.6f}, γ = {gamma_est:.6f}")
print(f"Число итераций: {result.nfev}")
print(f"Значение функции потерь: {result.fun:.2e}")

sol_final = solve_ivp(sir_ode, (t_data[0], t_data[-1]), [S0, I0, R0],
                     t_eval=t_data, args=(beta_est, gamma_est), method='RK45')
S_final, I_final, R_final = sol_final.y

fig, axs = plt.subplots(3, 2, figsize=(12, 10))

axs[0,0].plot(t_data, S_obs, 'b-', label=r'S(t) - истинные', linewidth=2, alpha=0.5)
axs[0,0].plot(t_data, S_final, 'b--', label=r'S*(t) - восстановленные', linewidth=2)
axs[0,0].set_ylabel(r'S(t)')
axs[0,0].legend()
axs[0,0].grid(True)

axs[0,1].plot(t_data, I_obs, 'g-', label=r'I(t) - истинные', linewidth=2, alpha=0.5)
axs[0,1].plot(t_data, I_final, 'g--', label=r'I*(t) - восстановленные', linewidth=2)
axs[0,1].set_ylabel(r'I(t)')
axs[0,1].legend()
axs[0,1].grid(True)

axs[1,0].plot(t_data, R_obs, 'r-', label=r'R(t) - истинные', linewidth=2, alpha=0.5)
axs[1,0].plot(t_data, R_final, 'r--', label=r'R*(t) - восстановленные', linewidth=2)
axs[1,0].set_ylabel(r'R(t)')
axs[1,0].legend()
axs[1,0].grid(True)

betas = [h[0] for h in history]
axs[1,1].plot(range(len(betas)), betas, 'b-', label=r'$\beta^k$')
axs[1,1].axhline(y=beta_est, color='b', linestyle='--', label=r'$\hat{\beta}$')
axs[1,1].set_ylabel(r'$\beta$')
axs[1,1].legend()
axs[1,1].grid(True)

gammas = [h[1] for h in history]
axs[2,0].plot(range(len(gammas)), gammas, 'g-', label=r'$\gamma^k$')
axs[2,0].axhline(y=gamma_est, color='g', linestyle='--', label=r'$\hat{\gamma}$')
axs[2,0].set_ylabel(r'$\gamma$')
axs[2,0].set_xlabel('Итерация')
axs[2,0].legend()
axs[2,0].grid(True)

axs[2,1].axis('off')
plt.tight_layout(rect=[0, 0, 1, 0.96])
plt.show()
