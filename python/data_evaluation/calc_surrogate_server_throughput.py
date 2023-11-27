import sys
import numpy as np
import json
import math


def find_index_to_reach_value(array, target_value):
    for i, value in enumerate(array):
        if value >= target_value:
            return i


# calculate the throughput of the surrogates
#
# data looks like this:
# data = {
#     "flash_s5_cp0": {
#         "load_from_client": {
#             "time": [
#                 [[run1],[run2],...,[run32]]
#                 ... for each surroguate
#                 [[run1],[run2],...,[run32]]
#             ],
#             "values": ...,
#         },
#         "load_from_surrogate": {
#             "time": ...
#             "values": ...
#         },
#         "request_in_from_surrogate": {
#             "time": ...
#         },
#         "response_out_to_surrogate": {
#             "time": ...
#         },
#         "request_in_from_client": {
#             "time": ...
#         },
#         "response_out_to_client": {
#             "time": ...
#         },
#     }
# }


args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]


for cache_policy in range(4):
    key = (
        scenario + "_s" + str(num_surrogates) + "_cp" + str(cache_policy) + "_surrogate"
    )
    filename = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_cp"
        + str(cache_policy)
        + "/surrogate_formated.json"
    )

    # get results
    with open(filename, "r") as file:
        results = json.load(file)
    results = results[key]

    # throughput_rates = [[] for _ in range(num_surrogates)]
    throughput_rates = []

    # calc throughput of each surrogate in all runs for the cache policy. Add the throughputs of the surrogates together every second
    for i in range(len(results["response_out_to_client"]["time"][0])):
        all_throughput = [0] * 1000
        for surrogate in range(num_surrogates):
            response_out_to_client = np.array(
                results["response_out_to_client"]["time"][surrogate][i]
            )

            # 1. get flash crowd data
            if scenario == "flash"or scenario == "final_flash":
                begin_flash_index = find_index_to_reach_value(
                    response_out_to_client, 130
                )
                end_flash_index = find_index_to_reach_value(response_out_to_client, 246)
                response_out_to_client = response_out_to_client[
                    begin_flash_index:end_flash_index
                ]

            # 2. calc throughput
            for j in range(len(response_out_to_client)):
                second = math.floor(response_out_to_client[j])
                all_throughput[second] += 1

            # throughput_rates[surrogate].extend(throughput)

        # remove leading and trailing zeros
        while all_throughput and all_throughput[0] == 0 and (scenario == "flash"or scenario == "final_flash"):
            all_throughput.pop(0)

        while all_throughput and all_throughput[-1] == 0:
            all_throughput.pop()
            
        if scenario == "base":
            all_throughput = all_throughput[100 : len(all_throughput) - 50]

        throughput_rates.extend(all_throughput)

    # save recovery times
    outfile = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/data/surrogate_throughput/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_throughput_surrogates.json"
    )
    with open(outfile, "r") as file:
        data = json.load(file)

    data[
        "throughput_s" + str(num_surrogates) + "_cp" + str(cache_policy)
    ] = throughput_rates

    with open(outfile, "w") as file:
        json.dump(data, file)
