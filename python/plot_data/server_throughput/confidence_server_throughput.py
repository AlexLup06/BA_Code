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
    + "/data/surrogate_throughput/"
    + scenario
    + "_s"
    + str(num_surrogates)
    + "_throughput_surrogates.json"
)

# data looks like this:
# data = {
#     "throughput_s5_cp1": [5,8,...10],
#     "throughput_s5_cp2": [5,8,...10],
#     "throughput_s5_cp3": [5,8,...10],
#     "throughput_s5_cp4": [5,8,...10],
# }

fig = plt.figure(figsize=(8, 5))  # Adjust the width and height as needed


with open(file_path, "r") as file:
    data = json.load(file)

all_server_throughputs = [0] * 4
for key in data:
    server_throughput = data[key]

    cache_policy = key.split("_")[2][2]
    all_server_throughputs[int(cache_policy)] = server_throughput

plt.xticks(
    [0, 1, 2, 3],
    ["Closest Surrogate", "Random Surrogate", "Load Balance", "Closest Origin"],
    fontsize=12,
)

draw_confidence_interval(all_server_throughputs, confidence)
plt.grid(True)
plt.title(str(num_surrogates) + " Surrogate Servers", fontsize=16)
plt.ylabel("Throughput Rate [res/s]", fontsize=16)
plt.yticks(fontsize=12)

plt.show()
