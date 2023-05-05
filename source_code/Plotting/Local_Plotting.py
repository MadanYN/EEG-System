from itertools import count
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation

# plt.style.use('Solarize_Light2')
channel_num = str(input("Channel Number ? "))

data = pd.read_csv('EEG_Data.csv') #reading locally stored data in csv file
x_vals = data['Time(s)'] #time axis of plot
y_vals = data['Channel '+ channel_num] # channel data value wrt time
plt.plot(x_vals, y_vals, color="blue")
plt.gca().set_ylim([-200, 200])
plt.ylabel("Voltage(uV)")
plt.xlabel("Time (sec)")
plt.title("Channel " + channel_num)
plt.tight_layout()
plt.grid()
plt.show()