import sys
import numpy as np
import json
import matplotlib.pyplot as plt
import scipy.stats as st
import os

root = os.getenv("root")

sys.path.append("../common")
from helper_functions import draw_confidence_interval

args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]
confidence = float(args[3])


file_path = (
    root
    + "/data/request_response_time/"
    + scenario
    + "_s"
    + str(num_surrogates)
    + "_response_times.json"
)

# data looks like this:
# data = {
#     "response_times_s5_cp0": [0.2 , 0.4 , ... , 0.6],
#     "response_times_s5_cp1": [0.2 , 0.4 , ... , 0.6],
#     "response_times_s5_cp2": [0.2 , 0.4 , ... , 0.6],
#     "response_times_s5_cp3": [0.2 , 0.4 , ... , 0.6],
# }
fig = plt.figure(figsize=(8, 5))  # Adjust the width and height as needed

with open(file_path, "r") as file:
    data = json.load(file)
all_response_times = [0] * 4
for key in data:
    response_times = data[key]
    print(key)
    cache_policy = key.split("_")[3][2]

    all_response_times[int(cache_policy)] = response_times

plt.xticks(
    [0, 1, 2, 3],
    ["Closest Surrogate", "Random Surrogate", "Load Balance", "Closest Origin"],
    fontsize=12,
)
plt.yticks(fontsize=12)


draw_confidence_interval(all_response_times, confidence)
plt.grid(True)
plt.title(str(num_surrogates) + " Surrogate Servers", fontsize=16)
plt.ylabel("Response Time [s]", fontsize=16)
plt.show()
