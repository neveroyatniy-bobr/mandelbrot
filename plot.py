import numpy as np
import matplotlib.pyplot as plt

data_file_name = "calc_times.csv"

try:
    data = np.loadtxt(data_file_name)
except OSError:
    print("Файл data.csv не найден!")
    data = np.array([])

mean_val = np.mean(data)
std_dev = np.std(data)\

data = data[np.abs(data - mean_val) <= 3 * std_dev]

print(f"Среднее: {mean_val:.2f}")
print(f"Погрешность (std): {std_dev:.2f}")

plt.hist(data, bins=5, color='skyblue', edgecolor='black', alpha=0.7)

plt.axvline(mean_val, color='red', linestyle='dashed', linewidth=2, label=f'Среднее: {mean_val:.2f}')

plt.axvspan(mean_val - std_dev, mean_val + std_dev, color='red', alpha=0.1, label=f'Погрешность (±{std_dev:.2f})')

plt.title("Гистограмма с анализом")
plt.xlabel("Значение")
plt.ylabel("Частота")
plt.legend()

plt.show()