from google.colab import drive
drive.mount('/content/drive')
import numpy as np
import matplotlib.pyplot as plt

empirical_mix = np.loadtxt('/content/drive/MyDrive/Colab/oop3/empirical_mix_density.txt')
theoretical_mix = np.loadtxt('/content/drive/MyDrive/Colab/oop3/theoretical_mix_density.txt')

plt.figure(figsize=(10, 8))

bin_width = empirical_mix[1, 0] - empirical_mix[0, 0]

plt.bar(empirical_mix[:, 0], empirical_mix[:, 1],
        width=bin_width * 0.8,
        alpha=0.7, color='lightblue', edgecolor='black', linewidth=0.8,
        label='Эмпирическая плотность смеси')

plt.plot(theoretical_mix[:, 0], theoretical_mix[:, 1], 'red', linewidth=2,
         label='Теоретическая плотность смеси')

plt.xlim(min(theoretical_mix[0, 0], empirical_mix[0, 0]),
         max(theoretical_mix[-1, 0], empirical_mix[-1, 0]))

plt.xlabel('x')
plt.ylabel('Плотность')
plt.title('Теоретическая и эмпирическая плотности смеси распределений')
plt.legend()
plt.grid(True, alpha=0.3)
plt.tight_layout()
plt.show()
