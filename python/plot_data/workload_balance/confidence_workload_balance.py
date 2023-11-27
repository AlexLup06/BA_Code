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
    + "/data/workload_difference/"
    + scenario
    + "_s"
    + str(num_surrogates)
    + "_workload_difference.json"
)

# data looks like this:
# data = {
#     "workload_difference_s5_cp0": [],
#     "workload_difference_s5_cp1": [],
#     "workload_difference_s5_cp2": [],
#     "workload_difference_s5_cp3": [],
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
# plt.yticks(fontsize=12)
plt.tick_params(axis="both", labelsize=12)


draw_confidence_interval(all_response_times, confidence)
plt.grid(True)
plt.title(str(num_surrogates) + " Surrogate Servers", fontsize=16)
plt.ylabel("Workload Difference", fontsize=16)
plt.show()


# with open(file_path, "r") as file:
#     data = json.load(file)

# all_workload_differences = [0] * 4
# for key in data:
#     workload_difference = data[key]

#     cache_policy = key.split("_")[3][2]
#     all_workload_differences[int(cache_policy)] = workload_difference


# draw_confidence_interval(all_workload_differences)
# plt.title("Confidence Interval Workload Difference")
# plt.ylabel("Absolute Workload Difference")
# plt.show()
