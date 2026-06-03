!pip install xgboost -q

import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from sklearn.metrics import mean_squared_error
from statsmodels.tsa.arima.model import ARIMA
from statsmodels.tsa.statespace.sarimax import SARIMAX
import xgboost as xgb
from google.colab import drive
import warnings
warnings.filterwarnings('ignore')

drive.mount('/content/drive')

file_path = '/content/drive/MyDrive/Colab/praktika/kurs.xlsx'

df = pd.read_excel(file_path, sheet_name='RC')
df['data'] = pd.to_datetime(df['data'])
df.set_index('data', inplace=True)
df.sort_index(inplace=True)

y = df['curs']

print(f"Валюта: {df['cdx'].iloc[0]}")
print(f"Всего точек: {len(y)}")
print(f"Период: с {y.index[0].date()} по {y.index[-1].date()}")

def evaluate_models(y, sample_size, forecast_days=7):
    train = y.iloc[:sample_size]
    test = y.iloc[sample_size:sample_size + forecast_days]

    if len(test) < forecast_days:
        return None, None, None, None, None, None

    # ARIMA
    try:
        model_arima = ARIMA(train, order=(5,1,0))
        fitted_arima = model_arima.fit()
        train_pred_arima = fitted_arima.fittedvalues
        rmse_train_arima = np.sqrt(mean_squared_error(train.iloc[len(train)-len(train_pred_arima):], train_pred_arima))
        pred_arima = fitted_arima.forecast(steps=forecast_days)
        rmse_test_arima = np.sqrt(mean_squared_error(test, pred_arima))
    except:
        rmse_train_arima, rmse_test_arima = np.nan, np.nan
        pred_arima = None

    # SARIMA
    try:
        model_sarima = SARIMAX(train, order=(5,1,0), season-al_order=(1,1,0,7))
        fitted_sarima = model_sarima.fit(disp=False)
        train_pred_sarima = fitted_sarima.fittedvalues
        rmse_train_sarima = np.sqrt(mean_squared_error(train.iloc[len(train)-len(train_pred_sarima):], train_pred_sarima))
        pred_sarima = fitted_sarima.forecast(steps=forecast_days)
        rmse_test_sarima = np.sqrt(mean_squared_error(test, pred_sarima))
    except:
        rmse_train_sarima, rmse_test_sarima = np.nan, np.nan
        pred_sarima = None

    # XGBoost
    try:
        df_xgb = pd.DataFrame({'y': train})
        for lag in [1,2,3,7]:
            df_xgb[f'lag_{lag}'] = df_xgb['y'].shift(lag)
        df_xgb.dropna(inplace=True)

        X = df_xgb.drop('y', axis=1)
        y_train_xgb = df_xgb['y']

        model_xgb = xgb.XGBRegressor(n_estimators=100, learning_rate=0.05, random_state=42)
        model_xgb.fit(X, y_train_xgb)

        train_pred_xgb = model_xgb.predict(X)
        rmse_train_xgb = np.sqrt(mean_squared_error(y_train_xgb, train_pred_xgb))

        last_train = train.iloc[-7:].values
        pred_xgb = []
        for step in range(forecast_days):
            features = []
            for lag in [1,2,3,7]:
                if len(pred_xgb) >= lag:
                    features.append(pred_xgb[-lag])
                else:
                    features.append(last_train[-lag] if lag <= len(last_train) else last_train[-1])
            pred = model_xgb.predict(np.array(features).reshape(1,-1))[0]
            pred_xgb.append(pred)
        rmse_test_xgb = np.sqrt(mean_squared_error(test, pred_xgb))
    except:
        rmse_train_xgb, rmse_test_xgb = np.nan, np.nan
        pred_xgb = None

    return (rmse_train_arima, rmse_test_arima,
            rmse_train_sarima, rmse_test_sarima,
            rmse_train_xgb, rmse_test_xgb)

sample_sizes = [20, 30, 50, 100, 200]
forecast_days = 7

results = []

for N in sample_sizes:
    (rmse_train_a, rmse_test_a,
     rmse_train_s, rmse_test_s,
     rmse_train_x, rmse_test_x) = evaluate_models(y, N, forecast_days)

    results.append({
        'объём выборки (N)': N,
        'ARIMA_точность_модели_RMSE': rmse_train_a,
        'ARIMA_точность_прогноза_RMSE': rmse_test_a,
        'SARIMA_точность_модели_RMSE': rmse_train_s,
        'SARIMA_точность_прогноза_RMSE': rmse_test_s,
        'XGBoost_точность_модели_RMSE': rmse_train_x,
        'XGBoost_точность_прогноза_RMSE': rmse_test_x
    })

final_table = pd.DataFrame(results)
print("ИТОГОВАЯ ТАБЛИЦА")
print(final_table.to_string(index=False))

plt.figure(figsize=(12, 6))

plt.subplot(1, 2, 1)
plt.plot(final_table['объём выборки (N)'], fi-nal_table['ARIMA_точность_модели_RMSE'], 'o-', label='ARIMA (train)', col-or='red')
plt.plot(final_table['объём выборки (N)'], fi-nal_table['SARIMA_точность_модели_RMSE'], 'o-', label='SARIMA (train)', col-or='green')
plt.plot(final_table['объём выборки (N)'], fi-nal_table['XGBoost_точность_модели_RMSE'], 'o-', label='XGBoost (train)', col-or='blue')
plt.xlabel('Объём выборки (N)')
plt.ylabel('RMSE')
plt.title('Точность моделей на обучении')
plt.legend()
plt.grid(True)

plt.subplot(1, 2, 2)
plt.plot(final_table['объём выборки (N)'], fi-nal_table['ARIMA_точность_прогноза'], 'o-', label='ARIMA (forecast)', col-or='red')
plt.plot(final_table['объём выборки (N)'], fi-nal_table['SARIMA_точность_прогноза'], 'o-', label='SARIMA (forecast)', col-or='green')
plt.plot(final_table['объём выборки (N)'], fi-nal_table['XGBoost_точность_прогноза'], 'o-', label='XGBoost (forecast)', col-or='blue')
plt.xlabel('Объём выборки (N)')
plt.ylabel('RMSE')
plt.title('Точность прогноза на 7 дней')
plt.legend()
plt.grid(True)

plt.tight_layout()
plt.show()

train_full = y.iloc[:-7]
test_full = y.iloc[-7:]

# ARIMA
model_arima_full = ARIMA(train_full, order=(5,1,0))
fitted_arima_full = model_arima_full.fit()
pred_arima_full = fitted_arima_full.forecast(steps=7)

# SARIMA
model_sarima_full = SARIMAX(train_full, order=(5,1,0), season-al_order=(1,1,0,7))
fitted_sarima_full = model_sarima_full.fit(disp=False)
pred_sarima_full = fitted_sarima_full.forecast(steps=7)

# XGBoost
df_xgb_full = pd.DataFrame({'y': train_full})
for lag in [1,2,3,7]:
    df_xgb_full[f'lag_{lag}'] = df_xgb_full['y'].shift(lag)
df_xgb_full.dropna(inplace=True)
X_full = df_xgb_full.drop('y', axis=1)
y_full = df_xgb_full['y']
model_xgb_full = xgb.XGBRegressor(n_estimators=100, learning_rate=0.05, random_state=42)
model_xgb_full.fit(X_full, y_full)

last_vals = train_full.iloc[-7:].values
pred_xgb_full = []
for step in range(7):
    features = []
    for lag in [1,2,3,7]:
        if len(pred_xgb_full) >= lag:
            features.append(pred_xgb_full[-lag])
        else:
            features.append(last_vals[-lag])
    pred = model_xgb_full.predict(np.array(features).reshape(1,-1))[0]
    pred_xgb_full.append(pred)

plt.figure(figsize=(14,6))

history_part = train_full.iloc[-30:]
full_continuous = pd.concat([history_part, test_full])

plt.plot(full_continuous.index, full_continuous.values, 'k-', 
         label=f'Реальные значения', 
         markersize=5, linewidth=1.5)

plt.plot(test_full.index, test_full.values, 'r-', 
         label=f'Реальные значения (последние 7 дней)', 
         markersize=8, linewidth=2)

plt.plot(test_full.index, pred_arima_full, 'r--', label='ARIMA прогноз', linewidth=2)
plt.plot(test_full.index, pred_sarima_full, 'g--', label='SARIMA прогноз', linewidth=2)
plt.plot(test_full.index, pred_xgb_full, 'b--', label='XGBoost прогноз', linewidth=2)

split_date = test_full.index[0]
plt.axvline(x=split_date, color='gray', linestyle=':', alpha=0.7, 
            label='Граница обучение/тест')

plt.title(f'Прогноз курса валюты на неделю')
plt.xlabel('Дата')
plt.ylabel('Курс')
plt.legend()
plt.grid(True)
plt.xticks(rotation=45)
plt.tight_layout()
plt.show()

print("ИТОГОВАЯ ТОЧНОСТЬ ПРОГНОЗА НА НЕДЕЛЮ")
print(f"ARIMA   RMSE = {np.sqrt(mean_squared_error(test_full, pred_arima_full)):.4f}")
print(f"SARIMA  RMSE = {np.sqrt(mean_squared_error(test_full, pred_sarima_full)):.4f}")
print(f"XGBoost RMSE = {np.sqrt(mean_squared_error(test_full, pred_xgb_full)):.4f}")

print("\n")
comparison = pd.DataFrame({
    'Дата': test_full.index.date,
    'Реальные': test_full.values,
    'ARIMA': pred_arima_full,
    'SARIMA': pred_sarima_full,
    'XGBoost': pred_xgb_full
})
print(comparison.to_string(index=False))
