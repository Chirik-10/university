from google.colab import drive
drive.mount('/content/drive')
import numpy as np
import matplotlib.pyplot as plt
empirical = np.loadtxt('/content/drive/MyDrive/Colab/oop2/empirical_density_class.txt')
theoretical = np.loadtxt('/content/drive/MyDrive/Colab/oop2/theoretical_density_class.txt')
plt.figure(figsize=(10, 8))
bin_width = empirical[1, 0] - empirical[0, 0]
plt.bar(empirical[:, 0], empirical[:, 1],
        width=bin_width * 0.8,
        alpha=0.7, color='lightblue', edgecolor='black', linewidth=0.8,
        label='Эмпирическая плотность')
plt.plot(theoretical[:, 0], theoretical[:, 1], 'red', linewidth=2,
         label='Теоретическая плотность')
plt.xlim(min(theoretical[0, 0], empirical[0, 0]),
         max(theoretical[-1, 0], empirical[-1, 0]))
plt.xlabel('x')
plt.ylabel('Плотность')
plt.title('Теоретическая и эмпирическая плотности распределения')
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.show()
