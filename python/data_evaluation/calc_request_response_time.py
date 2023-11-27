import sys
import numpy as np
import json

# calculate the request response time
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

    # calc response times of all runs for the cache policy
    # the response times for a run are in order of when they are received
    response_times = np.empty(shape=(0,))
    for i in range(len(results["send"]["time"])):
        sendTimes = np.array(results["send"]["time"][i])
        receiveTimes = np.array(results["receive"]["time"][i])
        sendValues = np.array(results["send"]["values"][i])
        receiveValues = np.array(results["receive"]["values"][i])

        sorted_sendTimes = my_array = np.full(len(sendValues), -1)
        for j in range(len(sendValues)):
            sendValue = sendValues[j]
            sorted_sendTimes[sendValue] = sendTimes[j]

        all_receiveTimes = []
        for j in range(len(receiveValues)):
            receiveValue = receiveValues[j]
            all_receiveTimes.append(receiveTimes[j] - sorted_sendTimes[receiveValue])

        if scenario == "flash" or scenario == "final_flash":
            response_times = np.append(
                response_times,
                all_receiveTimes[6000 : int((num_surrogates / 5) * 14000)],
            )
        else:
            response_times = np.append(
                response_times, all_receiveTimes[int(2000 * (num_surrogates / 5)) :]
            )

    # write that data to a file
    # the file has an array for each cache policy: in total four arrays (there is three of those files)
    outfile = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/data/request_response_time/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_response_times.json"
    )

    with open(outfile, "r") as file:
        data = json.load(file)
    data[
        "response_times_s" + str(num_surrogates) + "_cp" + str(cache_policy)
    ] = response_times.tolist()

    with open(outfile, "w") as file:
        json.dump(data, file)
