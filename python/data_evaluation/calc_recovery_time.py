import sys
import numpy as np
import json


def find_index_to_reach_value(array, target_value):
    for i, value in enumerate(array):
        if value >= target_value:
            return i

# calculate the recovery time of the network
#
# Data looks like this:
# output_data = {
#     new_key: {
#         "send": {"time": [[run1],[run2],...,[run32]], "values": [[],[],...,[]]},
#         "receive": {"time": [[],[],...,[]], "values": [[],[],...,[]]},
#         "fail": {"time": [[],[],...,[]], "values": [[],[],...,[]]},
#     }
# }

args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]

for cache_policy in range(3,4):
    key = scenario + "_s" + str(num_surrogates) + "_cp" + str(cache_policy) + "_client"
    filename = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/"
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

    # calc recovery times of all runs for the cache policy
    recovery_times = []
    for i in range(len(results["send"]["time"])):
        sendTimes = np.array(results["send"]["time"][i])
        sendValues = np.array(results["send"]["values"][i])
        receiveTimes = np.array(results["receive"]["time"][i])
        receiveValues = np.array(results["receive"]["values"][i])

        sorted_sendTimes = np.full(len(sendValues), -1)

        for j in range(len(sendValues)):
            sendValue = sendValues[j]
            sorted_sendTimes[sendValue] = sendTimes[j]

        response_times = []
        for j in range(len(receiveValues)):
            receiveValue = receiveValues[j]
            response_times.append(receiveTimes[j] - sorted_sendTimes[receiveValue])

        # 1. get response times from 246s to end (we do 240 sec because measure receive time)
        if scenario == "flash" or scenario == "final_flash":
            index = find_index_to_reach_value(receiveTimes, 246)
            response_times = response_times[index:]

        # 2. calc average response time
        mean = np.mean(response_times[-2000:])

        # 3. calc after what time a threshold (10%) of mean is reached
        recovered_index = -1
        for k in range(len(response_times) - 2000):
            if np.mean(response_times[k : k + 2000]) - mean < 0.1 * mean:
                recovered_index = k
                break

        # 4. calc recovery time
        recovery_time = receiveTimes[recovered_index + index] - 246
        print(
            "recovery time: "
            + str(recovery_time)
            + ". s="
            + str(num_surrogates)
            + ", cp="
            + str(cache_policy)
            + ", run="
            + str(i)
        )
        recovery_times.append(recovery_time)

    # save recovery times
    outfile = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/data/recovery_time/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_recovery_times.json"
    )
    with open(outfile, "r") as file:
        data = json.load(file)

    data[
        "recovery_time_s" + str(num_surrogates) + "_cp" + str(cache_policy)
    ] = recovery_times

    with open(outfile, "w") as file:
        json.dump(data, file)


# # flash crowd starts at 120s and ends at 246s
# # 10 seconds pass before we start to measure: 130s - 246s
# #
# # data looks like this:
# # output_data = {
# #     new_key: {
# #         "load_from_client": {
# #             "time": [
# #                 [[run1],[run2],...,[run32]]
# #                 ... for each surroguate
# #                 [[run1],[run2],...,[run32]]
# #             ],
# #             "values": ...,
# #         },
# #         "load_from_surrogate": {
# #             "time": ...
# #             "values": ...
# #         },
# #         "request_in_from_surrogate": {
# #             "time": ...
# #         },
# #         "response_out_to_surrogate": {
# #             "time": ...
# #         },
# #         "request_in_from_client": {
# #             "time": ...
# #         },
# #         "response_out_to_client": {
# #             "time": ...
# #         },
# #     }
# # }

# args = sys.argv
# num_surrogates = int(args[1])

# for cache_policy in range(4):
#     filename = (
#         "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/flash_s"
#         + str(num_surrogates)
#         + "_cp"
#         + str(cache_policy)
#         + "/surrogate_formated.json"
#     )

#     key = "flash_s" + str(num_surrogates) + "_cp" + str(cache_policy) + "_" + node
#     results
#     with open(filename, "r") as file:
#         results = json.load(file)
#     results = results[key]

#     # calc response times of all runs for the cache policy

#     num_runs = len(results["load_from_client"]["time"][0])
#     for i in range(num_runs):
#         # 1. 0s - 130s
#         from_index = find_index_to_reach_value(load_from_client["time"], 130)
#         load_from_client = results["load_from_client"][values][i][:from_index]

#         # 2. calculate avergae load from client
#         avergae_load_from_client = np.mean(load_from_client)

#         # save recovery times
#         outfile = (
#             "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/data/recovery_time/flash_s"
#             + str(num_surrogates)
#             + "_recovery_times.json"
#         )
#         with open(file_path, "r") as file:
#             data = json.load(file)

#         data["surrogate" + i] = []
