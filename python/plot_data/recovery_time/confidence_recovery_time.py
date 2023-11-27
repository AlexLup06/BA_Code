import sys
import numpy as np
import json
import matplotlib.pyplot as plt
import scipy.stats as st

sys.path.append("../common")
from helper_functions import draw_confidence_interval

args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]
confidence = float(args[3])

fig = plt.figure(figsize=(8, 5))  # Adjust the width and height as needed


file_path = (
    "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/data/recovery_time/"
    + scenario
    + "_s"
    + str(num_surrogates)
    + "_recovery_times.json"
)

# data looks like this:
# data = {
#     "recovery_time_s5_cp0": [30, 28 , ... , 31],
#     "recovery_time_s5_cp1": [30, 28 , ... , 31],
#     "recovery_time_s5_cp2": [30, 28 , ... , 31],
#     "recovery_time_s5_cp3": [30, 28 , ... , 31],
# }

with open(file_path, "r") as file:
    data = json.load(file)

all_recovery_times = [0] * 4
for key in data:
    recovery_times = data[key]

    cache_policy = key.split("_")[3][2]
    all_recovery_times[int(cache_policy)] = recovery_times

plt.tick_params(axis="both", labelsize=12)

draw_confidence_interval(all_recovery_times)
plt.title(str(num_surrogates) + " Surrogate Servers", fontsize=16)
plt.ylabel("Recovery Time [s]", fontsize=16)
plt.show()
