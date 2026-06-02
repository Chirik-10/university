# Практика "Прогнозирование временных рядов: сравнение SARIMA и XGBoost"

## О проекте

В рамках практики проведено исследование двух подходов к прогнозированию временных рядов:

| Подход | Модель | Тип |
|--------|--------|-----|
| Классическая статистика | ARIMA и SARIMA | Линейная, интерпретируемая |
| Машинное обучение | XGBoost (eXtreme Gradient Boosting) | Нелинейная, ансамблевая |

Цель: Исследовать и сравнить два подхода к прогнозированию временных рядов: классические статистические модели (сезонного) авторегрессионного интегрированного скользящего среднего (Autoregressive Integrated Moving Average, ARIMA; Seasonal Autoregressive Integrated Moving Average, SARIMA) и метод машинного обучения экстремальный градиентный бустинг (eXtreme Gradient Boosting, XGBoost), с оценкой их точности, устойчивости и применимости к данным о курсе валюты (китайский юань) при разных объёмах обучающей выборки.

## Сравнение моделей на разных объёмах выборки

Точность модели ARIMA на разных объёмах выборки:
<img width="809" height="250" alt="image" src="https://github.com/user-attachments/assets/32a6dc7b-7005-46dc-8c21-cf49284958f3" />

Точность модели SARIMA на разных объёмах выборки:
<img width="809" height="252" alt="image" src="https://github.com/user-attachments/assets/5ac5ff59-f8fa-472c-b9d9-b16270e495c2" />

Точность модели XGBoost на разных объёмах выборки:
<img width="811" height="253" alt="image" src="https://github.com/user-attachments/assets/027c7f49-abb0-4144-8e14-153a447349d1" />

Зависимость точности моделей и прогноза от объёма выборки:
<img width="1008" height="502" alt="image" src="https://github.com/user-attachments/assets/f1974e8a-1b45-448d-9e07-64aae6a88ed4" />

## Прогноз курса китайского юаня на неделю
<img width="1008" height="430" alt="image" src="https://github.com/user-attachments/assets/13dae60c-e033-40c0-b8b9-b47af0437c14" />
