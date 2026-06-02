from google.colab import drive
drive.mount('/content/drive')
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats

sample1 = np.loadtxt('/content/drive/MyDrive/Colab/sample.txt')
sample2 = np.loadtxt('/content/drive/MyDrive/Colab/new_sample.txt')
#sample1 = np.loadtxt('/content/drive/MyDrive/Colab/mix_sample.txt')
#sample2 = np.loadtxt('/content/drive/MyDrive/Colab/new_mix_sample.txt')
x = np.linspace(min(sample1.min(), sample2.min()), 
                max(sample1.max(), sample2.max()), 1000)
plt.figure(figsize=(10, 5))
plt.subplot(1, 2, 1)
plt.plot(x, kde1(x), 'blue', linewidth=2, label='Исходная')
plt.plot(x, kde2(x), 'red', linewidth=2, label='Новая')
plt.xlabel('x')
plt.ylabel('Плотность')
plt.title('Сравнение (линии)')
plt.legend()
plt.grid(True, alpha=0.3)
plt.subplot(1, 2, 2)
plt.hist(sample1, bins=50, alpha=0.7, color='blue', density=True, label='Исходная')
plt.hist(sample2, bins=50, alpha=0.7, color='red', density=True, label='Новая')
plt.xlabel('x')
plt.ylabel('Плотность')
plt.title('Сравнение (гистограммы)')
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.show()
print(f"Средние: {np.mean(sample1):.4f} и {np.mean(sample2):.4f}")
print(f"Дисперсии: {np.var(sample1):.4f} и {np.var(sample2):.4f}")
