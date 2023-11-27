import sys
import numpy as np
import json
import matplotlib.pyplot as plt

args = sys.argv
scenario = args[1]

fig, axes = plt.subplots(nrows=1, ncols=3, figsize=(15, 5))  # Adjust figsize as needed

for j, num_surrogates in enumerate([5, 10, 15]):
    file_path = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/data/origin_load/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_origin_load.json"
    )

    # data looks like this:
    # data = {
    #     "originload_s5_cp0": [],
    #     "originload_s5_cp1": [],
    #     "originload_s5_cp2": [],
    #     "originload_s5_cp3": [],
    # }

    all_origin_loads = [[] for _ in range(4)]
    with open(file_path, "r") as file:
        data = json.load(file)

    max_length = 0
    for key in data:
        if len(data[key]) > max_length:
            max_length = len(data[key])

        cache_policy = key.split("_")[2][2]
        all_origin_loads[int(cache_policy)] = data[key]



    names = ["Closest Surrogate", "Random Surrogate", "Load Balance", "Closest Origin"]
    colors = [
        "#E69F00",
        "#56B4E9",
        "#009E73",
        "#0072B2",
        "#D55E00",
        "#CC79A7",
        "#F0E442",
    ]
    linestyle = ["-", "--", "-.", ":", "-", "--", "-."]
    for i in range(len(all_origin_loads)):
        while all_origin_loads[i] and all_origin_loads[i][-1] == 0:
            all_origin_loads[i].pop()
        x = np.arange(
            0,
            len(all_origin_loads[i]),
            1,
        )
        axes[j].plot(
            x,
            all_origin_loads[i],
            color=colors[i],
            label=names[i],
            linestyle=linestyle[i],
            linewidth=2.5,
        )

    axes[j].tick_params(axis="both", labelsize=14)
    axes[j].set_xlabel("Time in [s]", fontsize=16)
    if j == 0:
        axes[j].set_ylabel("Origin Server Load", fontsize=16)
    axes[j].set_title(str(num_surrogates) + "Surrogate Servers", fontsize=16)
    # if j == 2:
        # axes[j].legend(fontsize=11, loc="upper right")
    # else:
    axes[j].legend(fontsize=11)
    axes[j].grid(True)


# Adjust spacing between subplots
plt.subplots_adjust(wspace=2)
plt.tight_layout()

# Display the plots
plt.show()
