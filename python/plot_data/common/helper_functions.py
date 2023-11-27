import numpy as np
import scipy.stats as st
import matplotlib.pyplot as plt
import seaborn as sns
import math

__all__ = ["draw_confidence_interval", "draw_violin", "draw_cdf"]


# plot the confidence interval of the data
def plot_confidence_interval(
    x, values, confidence, color="#2187bb", horizontal_line_width=0.25
):
    mean = np.mean(values)
    confidence_interval = st.norm.interval(
        confidence=confidence, loc=mean, scale=st.sem(values)
    )

    bottom = confidence_interval[0]
    top = confidence_interval[1]

    width = 2.25

    left = x - horizontal_line_width / 2
    right = x + horizontal_line_width / 2
    plt.plot([x, x], [top, bottom], color=color, linewidth=width)
    plt.plot([left, right], [top, top], color=color, linewidth=width)
    plt.plot([left, right], [bottom, bottom], color=color, linewidth=width)
    plt.plot(x, mean, "o", color="#f44336", markersize=6.5)

    print("Confidence Interval: ", confidence_interval, ". Mean: ", mean)

    return mean, confidence_interval


# draw the confidence interval
def draw_confidence_interval(data, conf=0.90):
    plt.xticks(
        [0, 1, 2, 3],
        ["Closest Surrogate", "Random Surrogate", "Load Balance", "Closest Origin"],
    )
    plt.grid()
    for i in range(4):
        plot_confidence_interval(i, data[i], conf)


# draw the violin plot
def draw_violin(data, tixk, more, lim):
    sns.set(style="whitegrid")  
    plt.figure(figsize=(8, 5))  
    colors = ["#606060", "#606060", "#606060", "#606060"]
    ax = sns.violinplot(
        data=data,
        inner="quart",
        palette=colors,
        common_norm=True,
        fill=False,
        linewidth=2,
    )
    plt.xticks(
        [0, 1, 2, 3],
        ["Closest Surrogate", "Random Surrogate", "Load Balance", "Closest Origin"],
    )

    flattened_data = [item for sublist in data for item in sublist]
    maximum = np.max(flattened_data)
    minimum = np.min(flattened_data)

    maximum = math.ceil(maximum / 10) * 10 + more
    minimum = math.floor(minimum / 10) * 10

    if lim != 0:
        plt.yticks(np.arange(minimum, maximum, tixk))

# draw cumulative distribution function
def draw_cdf(data, cp_index):
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
        cache_policy = key.split("_")[cp_index][2]
        legend.append(names[int(cache_policy)])
        recovery_times = data[key]
        sorted_recovery_times = np.sort(recovery_times)
        cdf = np.arange(1, len(sorted_recovery_times) + 1) / len(sorted_recovery_times)
        plt.plot(
            sorted_recovery_times,
            cdf,
            marker="",
            color=color[i],
            linestyle=linestyle[i],
            label=names[int(cache_policy)],
            linewidth=2,
        )
    plt.legend(fontsize=12)
    plt.grid(True)
    plt.ylabel("CDF", fontsize=16)
    plt.figure(figsize=(6, 8)) 
