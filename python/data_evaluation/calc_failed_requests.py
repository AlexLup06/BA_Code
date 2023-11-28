import sys
import numpy as np
import json
import math
import os

root = os.getenv("root")

# calculate the mean number of failed requests per second across all runs
#
# Data looks like this:
# output_data = {
#     new_key: {
#         "send": {"time": [[run1],[run2],...,[run32]], "values": [[],[],...,[]]},
#         "receive": {"time": [[],[],...,[]], "values": [[],[],...,[]]},
#         "fail": {"time": [[],[],...,[]], "values": [[],[],...,[]]},
#     }
# }
#
# The arrays are already filtered for duplicates caused by failed requests

args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]


for cache_policy in range(4):
    key = scenario + "_s" + str(num_surrogates) + "_cp" + str(cache_policy) + "_client"
    filename = (
        root
        + "/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_cp"
        + str(cache_policy)
        + "/client_formated.json"
    )

    # get results
    with open(filename, "r") as file:
        results = json.load(file)
    results = results[key]

    # calc response times of all runs for the cache policy
    # the response times for a run are in order of when they are received
    failed_means = [0] * 1000
    for i in range(len(results["fail"]["time"])):
        failedTimes = np.array(results["fail"]["time"][i])

        for j in range(len(failedTimes)):
            failed_means[math.floor(failedTimes[j])] += 1 / len(results["fail"]["time"])

    while failed_means and failed_means[-1] == 0:
        failed_means.pop()

    # write that data to a file
    # the file has an array for each cache policy: in total four arrays (there is three of those files)
    outfile = (
        root
        + "/data/failed_requests/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_failed_requests.json"
    )

    with open(outfile, "r") as file:
        data = json.load(file)
    data[
        "failed_requests_s" + str(num_surrogates) + "_cp" + str(cache_policy)
    ] = failed_means

    with open(outfile, "w") as file:
        json.dump(data, file)
