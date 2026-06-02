# Лабораторная работа 1 (структурный подход)

## Задание
Методами структурного программирования реализовать возможность работы с тремя распределениями (основным, смесью и эмпирическим). Для каждого из распределений реализовать функции для вычисления плотности, характеристик (математического ожидания, дисперсии, коэффициентов асимметрии и эксцесса) и моделирования случайной величины. Разработать функции, тестирующие правильность работы программы.

## Файлы
- distribution.h/cpp - Основные функции
- tests.cpp - Тестирование
- main.cpp - Демонстрация
 
## Тестирование
### 1. Сравнение с табличными значениями
<img width="827" height="1173" alt="image" src="https://github.com/user-attachments/assets/1c324b9f-9098-449f-b083-c24c50646fad" />

### 2. Набор тестов для основного распределения
<img width="709" height="410" alt="image" src="https://github.com/user-attachments/assets/18ac42bd-cb56-4c41-92ee-24bf8df4e837" />

### 3. Набор тестов для смеси распределений
<img width="709" height="456" alt="image" src="https://github.com/user-attachments/assets/20898f73-b60a-4ff2-8013-65ccd2afe3a7" />

### 4. Тестирование эмпирического распределения и функций моделирования случайных величин для всех распределений
#### 4.1. Основное распределение
1) Выборка n = 100

<img width="709" height="532" alt="image" src="https://github.com/user-attachments/assets/a36068bd-cb0f-4ab6-8f33-3edb647a1bd0" />
<img width="945" height="755" alt="image" src="https://github.com/user-attachments/assets/a071732d-0707-4dbb-96cd-fea4c57900eb" />

2) Выборка n = 1000
<img width="886" height="654" alt="image" src="https://github.com/user-attachments/assets/e9ad531b-b29c-42f5-8559-67a7f31df861" />
<img width="945" height="755" alt="image" src="https://github.com/user-attachments/assets/84186cf1-641e-4965-b8fe-e0f0bf1de5be" />

3) Выборка n = 10000
<img width="886" height="662" alt="image" src="https://github.com/user-attachments/assets/f50615e8-e726-4d67-8bdd-44ccd853315a" />
<img width="886" height="708" alt="image" src="https://github.com/user-attachments/assets/16693cae-23ed-4c46-a3ba-8c4eb9b5d3b1" />

#### 4.2. Смесь распределений
1) Выборка n = 100
<img width="709" height="638" alt="image" src="https://github.com/user-attachments/assets/e001a975-5498-4a55-9a35-02e1909cabab" />
<img width="886" height="708" alt="image" src="https://github.com/user-attachments/assets/bde71c0f-788b-4562-a4da-0a5d251e45ec" />

2) Выборка n = 1000
<img width="709" height="643" alt="image" src="https://github.com/user-attachments/assets/4b0ad2af-79b6-4b16-8b2d-693ce857fc2d" />
<img width="945" height="755" alt="image" src="https://github.com/user-attachments/assets/93fd72d8-63dc-4bae-b73d-16c89763e8cb" />

3) Выборка n = 10000
<img width="709" height="639" alt="image" src="https://github.com/user-attachments/assets/29c88846-030e-4e0f-a6e8-13a9ea444698" />
<img width="886" height="708" alt="image" src="https://github.com/user-attachments/assets/1f117d95-a99f-42a1-a478-6f377ec96bc9" />

#### 4.3. Сравнение
а) Основное распределение
<img width="945" height="468" alt="image" src="https://github.com/user-attachments/assets/e026f0ab-bfbc-4db6-9e4f-c0b90a20d820" />

Средние: 0.9981 и 0.9944

Дисперсии: 1.3768 и 1.3662

б) Смесь распределений
<img width="945" height="468" alt="image" src="https://github.com/user-attachments/assets/14eefda3-e63d-4deb-8cb1-882f90863b9f" />

Средние: 1.4910 и 1.4716

Дисперсии: 5.4620 и 5.4647
