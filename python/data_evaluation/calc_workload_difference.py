import sys
import numpy as np
import json
import math


def find_index_to_reach_value(array, target_value):
    for i, value in enumerate(array):
        if value >= target_value:
            return i


# calculate the workload difference of the surrogates
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

# calculate workload difference for each cache miss policy. An array with all differences of the workload of the surrogate
# with most and with least requests from other surrogates per second
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

    with open(filename, "r") as file:
        results = json.load(file)
    results = results[key]

    all_workload_differences = []
    for k in range(len(results["request_in_from_surrogate"]["time"][0])):
        all_request_in_per_sec = [0] * num_surrogates
        for j in range(num_surrogates):
            request_in_from_surrogate = results["request_in_from_surrogate"]["time"][j][
                k
            ]

            # get between 130s and 246s
            if scenario == "flash"or scenario == "final_flash":
                start_index = find_index_to_reach_value(request_in_from_surrogate, 130)
                end_index = find_index_to_reach_value(request_in_from_surrogate, 246)
                request_in_from_surrogate = request_in_from_surrogate[
                    start_index:end_index
                ]

            # calc input rate for each surrogate
            request_in_per_sec = [0] * 10000
            for i in range(len(request_in_from_surrogate)):
                request_in_per_sec[math.floor(request_in_from_surrogate[i])] += 1
            all_request_in_per_sec[j] = request_in_per_sec[130:246]

        # calc difference between most and least for each index
        workload_difference = []
        for values_at_index in zip(*all_request_in_per_sec):
            greatest_value = max(values_at_index)
            lowest_value = min(values_at_index)
            difference = greatest_value - lowest_value
            workload_difference.append(difference)

        all_workload_differences.extend(workload_difference)

    # save workload difference
    outfile = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/data/workload_difference/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_workload_difference.json"
    )
    with open(outfile, "r") as file:
        data = json.load(file)

    data[
        "workload_difference_s" + str(num_surrogates) + "_cp" + str(cache_policy)
    ] = all_workload_differences

    with open(outfile, "w") as file:
        json.dump(data, file)
