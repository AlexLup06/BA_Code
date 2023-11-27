import sys
import numpy as np
import json
import matplotlib.pyplot as plt

sys.path.append("../common")
from helper_functions import draw_violin


args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]

file_path = (
    "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/data/surrogate_throughput/"
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

with open(file_path, "r") as file:
    data = json.load(file)

server_throughputs = [0] * 4
for key in data:
    cache_policy = key.split("_")[2][2]
    server_throughputs[int(cache_policy)] = data[key]

fig = plt.figure(figsize=(8, 5))  # Adjust the width and height as needed

draw_violin(server_throughputs, 1, 0, 0)

plt.title(str(num_surrogates) + " Surrogate Servers", fontsize=16)
plt.ylabel("Throughput Rate [res/s]", fontsize=16)

plt.tick_params(axis="both", labelsize=13)


plt.tight_layout()
plt.show()
