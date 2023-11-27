import sys
import numpy as np
import json
import matplotlib.pyplot as plt
import seaborn as sns
import pandas as pd
import os

root = os.getenv('root')

sys.path.append("../common")
from helper_functions import draw_violin


args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]
fig = plt.figure(figsize=(8, 5))  # Adjust the width and height as needed

file_path = (
    root+"/data/request_response_time/"
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


with open(file_path, "r") as file:
    data = json.load(file)

response_times = [0] * 4
for key in data:
    cache_policy = key.split("_")[3][2]
    response_times[int(cache_policy)] = data[key]

draw_violin(response_times, 1, 0, 0)

# plt.ylim(-1, 18)
plt.ylabel("Response Time [s]", fontsize=16)
plt.title(str(num_surrogates) + " Surrogates Servers", fontsize=16)
plt.tick_params(axis="both", labelsize=12)
plt.show()
