import sys
import numpy as np
import json
import os

root = os.getenv("root")

# calculate the average load of the origin server
#
# Data looks like this:
#
# data = {
#    "flash_s5_cp0_origin": {"time": [[run1],[run2],...[run32]], "values": [[run1],[run2],...[run32]]}
#    "flash_s5_cp1_origin": {"time": [], "values": []}
#    "flash_s5_cp2_origin": {"time": [], "values": []}
#    "flash_s5_cp3_origin": {"time": [], "values": []}
# }
# The arrays are already filtered for duplicates caused by failed requests

args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]


for cache_policy in range(4):
    key = (
        scenario
        + "_s"
        + str(num_surrogates)
        + "_cp"
        + str(cache_policy)
        + "_originload"
    )

    filename = (
        root
        + "/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_cp"
        + str(cache_policy)
        + "/origin_formatted.json"
    )

    # get results
    with open(filename, "r") as file:
        results = json.load(file)
    results = results[key]

    # calc response times of all runs for the cache policy
    # the response times for a run are in order of when they are received
    averages = [0] * 1000
    for i in range(len(results["values"])):
        origin_load = results["values"][i]

        # if scenario == "flash":
        #     origin_load = origin_load[130:246]
        # else:
        #     origin_load = origin_load[80:]

        for j in range(len(origin_load)):
            averages[j] += origin_load[j] / len(results["values"])

    # while averages and averages[0] == 0:
    #     averages.pop(0)
    # while averages and averages[-1] == 0:
    #     averages.pop()

    # write that data to a file
    # the file has an array for each cache policy: in total four arrays (there is three of those files)
    outfile = (
        root
        + "/data/origin_load/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_origin_load.json"
    )

    with open(outfile, "r") as file:
        data = json.load(file)
    data["originload_s" + str(num_surrogates) + "_cp" + str(cache_policy)] = averages

    with open(outfile, "w") as file:
        json.dump(data, file)
