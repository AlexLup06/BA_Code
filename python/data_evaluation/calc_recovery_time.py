import sys
import numpy as np
import json
import os

root = os.getenv("root")


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

for cache_policy in range(3, 4):
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
        root
        + "/data/recovery_time/"
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
