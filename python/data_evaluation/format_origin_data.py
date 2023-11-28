import json
import sys
import os

root = os.getenv("root")

#
# format origin server data
#


def remove_trailing_zeros(arr):
    while arr and arr[-1] == 0:
        arr.pop()
    return arr


args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]

for cache_policy in range(4):
    filename = (
        root
        + "/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_cp"
        + str(cache_policy)
        + "/origin.json"
    )

    new_key = (
        scenario
        + "_s"
        + str(num_surrogates)
        + "_cp"
        + str(cache_policy)
        + "_originload"
    )
    output_data = {}
    output_data[new_key] = {"time": [], "values": []}

    with open(filename, "r") as file:
        results = json.load(file)

    for key in results:
        print("getting data from: ", key)

        target_value = results[key]

        for vector in target_value["vectors"]:
            if vector["name"] == "load:vector":
                wo_trailing_zeros = remove_trailing_zeros(vector["value"])
                means = []  # workload every second
                for i in range(0, len(wo_trailing_zeros), 100):
                    # mean = sum(wo_trailing_zeros[i : i + 100]) / 100
                    means.append(max(wo_trailing_zeros[i : i + 100]))

                # output_data[new_key]["load"]["time"].append(x)
                output_data[new_key]["values"].append(means)

    outfilename = (
        root
        + "/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_cp"
        + str(cache_policy)
        + "/origin_formatted.json"
    )

    with open(outfilename, "w") as output_file:
        json.dump(output_data, output_file, indent=2)
