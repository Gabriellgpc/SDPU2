# Codigo em python3
import numpy as np
import matplotlib.pyplot as plt
import os

for p,_,files in os.walk('../etc/'):
    pass
files.remove('ploter.py')

for i in range(0,len(files)):
    print(i, '- ', files[i])
index_file = int(input("index do arquivo para plotter?\t"))


dados = open(p+files[index_file],'r').read()
curve = dados[0]
print("curva:\t", curve)

dados = dados.split(',')

dados.pop(0)

for i in range(0,len(dados)):
    dados[i] = float(dados[i])


if(curve == '1'):
    print("Curva tipo 1")
    x = np.linspace(0,30, len(dados))
else:
    print("Curva tipo 2")
    x = np.linspace(0,40, len(dados))

y = dados

plt.plot(x,y)
plt.xlabel('t(s)')
plt.ylabel('PWM(%)')
plt.title('Q(t)')

plt.show()
