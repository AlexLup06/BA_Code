import sys
import numpy as np
import json
import matplotlib.pyplot as plt
import os

root = os.getenv('root')

args = sys.argv
scenario = args[1]


# data looks like this:
# data = {
#     "failed_requests_s5_cp0": [],
#     "failed_requests_s5_cp1": [],
#     "failed_requests_s5_cp2": [],
#     "failed_requests_s5_cp3": [],
# }

fig, axes = plt.subplots(nrows=1, ncols=3, figsize=(15, 5))  # Adjust figsize as needed

for j, num_surrogates in enumerate([5, 10, 15]):
    outfile = (
        root+"/Code/data/failed_requests/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_failed_requests.json"
    )

    with open(outfile, "r") as file:
        data = json.load(file)

    names = ["Closest Surrogate", "Random Surrogate", "Load Balance", "Closest Origin"]
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
        cache_policy = int(key.split("_")[3][2])
        failed_means = data[key]

        minimum = 0
        while failed_means and failed_means[0] == 0:
            failed_means.pop(0)
            minimum += 1

        x = np.arange(
            minimum,
            minimum + len(failed_means),
            1,
        )

        axes[j].plot(
            x,
            failed_means,
            label=names[cache_policy],
            marker="",
            color=color[i],
            linestyle=linestyle[i],
            linewidth=2.5,
        )

    axes[j].tick_params(axis="both", labelsize=14)
    axes[j].set_xlabel("Time in [s]", fontsize=16)
    if j == 0:
        axes[j].set_ylabel("Number of Failed Requests", fontsize=16)
    axes[j].set_title(str(num_surrogates) + "Surrogate Servers", fontsize=16)
    if j == 2:
        axes[j].legend(fontsize=11, loc="upper right")
    else:
        axes[j].legend(fontsize=11)
    axes[j].grid(True)



# Adjust spacing between subplots
plt.subplots_adjust(wspace=3)
plt.tight_layout()

# Display the plots
plt.show()