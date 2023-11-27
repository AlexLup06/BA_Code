import sys
import numpy as np
import json
import matplotlib.pyplot as plt
import os

root = os.getenv("root")

args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]

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
#     "response_times_s5_cp0": [],
#     "response_times_s5_cp1": [],
#     "response_times_s5_cp2": [],
#     "response_times_s5_cp3": [],
# }


plt.figure(figsize=(8, 6))
ax = plt.gca()
all_response_times = [[] for _ in range(4)]
with open(file_path, "r") as file:
    data = json.load(file)
for key in data:
    running_means = np.zeros_like(data[key], dtype=float)

    # Calculate the running mean
    for i in range(len(data[key])):
        running_means[i] = np.mean(data[key][: i + 1])

    all_response_times.append(running_means)

    x = np.arange(
        0,
        7000,
        1,
    )

    cache_policy = key.split("_")[3][2]
    all_response_times[int(cache_policy)] = running_means

colors = ["b", "g", "r", "c", "m", "y", "k", "#FFA07A", "#00FFFF", "#8A2BE2"]
for i in range(len(all_response_times)):
    plt.plot(x, all_response_times[i], label="Array 1", color=colors[i])


plt.legend()
plt.xlabel("X-axis")
plt.ylabel("Y-axis")
plt.title("Multiple Arrays in One Graph")
plt.show()
