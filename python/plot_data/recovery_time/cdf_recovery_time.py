import sys
import numpy as np
import json
import matplotlib.pyplot as plt
import scipy.stats as st

sys.path.append("../common")
from helper_functions import draw_cdf

args = sys.argv
scenario = args[1]


args = sys.argv
# num_surrogates = int(args[1])
scenario = args[1]
fig, axes = plt.subplots(nrows=1, ncols=3, figsize=(15, 5))  # Adjust figsize as needed

for j, num_surrogates in enumerate([5, 10, 15]):
    file_path = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/data/recovery_time/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_recovery_times.json"
    )

    with open(file_path, "r") as file:
        data = json.load(file)

    names = ["Closest Surrogate", "Random Surrogate", "Load Balance", "Closest Origin"]
    legend = []
    color = [
        "#E69F00",
        "#56B4E9",
        "#009E73",
        "#0072B2",
        "#D55E00",
        "#CC79A7",
        "#F0E442",
    ]
    linestyle = ["-", "--", "-.", ":", "-", "--", "-."]
    for i, key in enumerate(data):
        cache_policy = key.split("_")[3][2]
        legend.append(names[int(cache_policy)])
        recovery_times = data[key]
        sorted_recovery_times = np.sort(recovery_times)
        cdf = np.arange(1, len(sorted_recovery_times) + 1) / len(sorted_recovery_times)
        axes[j].plot(
            sorted_recovery_times,
            cdf,
            marker="",
            color=color[i],
            linestyle=linestyle[i],
            label=names[int(cache_policy)],
            linewidth=2.5,
        )
    ########################################

    # Create the first subplot (left)
    axes[j].set_title(str(num_surrogates) + " surrogate servers", fontsize=16)
    axes[j].set_xlabel("Response Time [s]", fontsize=16)
    if j == 0:
        axes[j].set_ylabel("CDF", fontsize=16)
    axes[j].grid(True)
    axes[j].tick_params(axis="both", labelsize=14)
    axes[j].legend(fontsize=11)

    # if j==1:
    #     axes[j].set_xlim(0, 18)
    # if j==2:
    #     axes[j].set_xlim(0, 28)

    # Add labels, titles, or any other customizations as needed

# Adjust spacing between subplots
plt.subplots_adjust(wspace=2)
plt.tight_layout()

# Display the plots
plt.show()


# file_path = (
#     "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/data/recovery_time/flash_s"
#     + str(num_surrogates)
#     + "_recovery_times.json"
# )

# # data looks like this:
# # data = {
# #     "recovery_time_s5_cp0": [30, 28 , ... , 31],
# #     "recovery_time_s5_cp1": [30, 28 , ... , 31],
# #     "recovery_time_s5_cp2": [30, 28 , ... , 31],
# #     "recovery_time_s5_cp3": [30, 28 , ... , 31],
# # }

# with open(file_path, "r") as file:
#     data = json.load(file)

# draw_cdf(data,3)

# plt.xlabel("Value")
# plt.title("CDF of Server Throughput")
# plt.show()
