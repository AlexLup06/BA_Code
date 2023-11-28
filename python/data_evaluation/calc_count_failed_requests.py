import sys
import numpy as np
import json
import math
import os

root = os.getenv("root")

# calculate the average number of failed requests per run
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

    count_failed_requests = []
    failed_requests = results["fail"]["values"]
    for i in range(len(failed_requests)):
        count_failed_requests.append(len(failed_requests[i]))

    # write that data to a file
    # the file has an array for each cache policy: in total four arrays (there is three of those files)
    outfile = (
        root
        + "/data/count_failed_requests/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_count_failed_requests.json"
    )

    with open(outfile, "r") as file:
        data = json.load(file)
    data[
        "failed_requests_s" + str(num_surrogates) + "_cp" + str(cache_policy)
    ] = count_failed_requests

    with open(outfile, "w") as file:
        json.dump(data, file)
