import json
import sys

#
# format client data
#

def remove_multiple_occurrences(arr1, arr2):
    seen = {}
    result1 = []
    result2 = []

    for i, (item1, item2) in enumerate(zip(reversed(arr1), reversed(arr2))):
        if item1 not in seen:
            seen[item1] = True
            result1.append(item1)
            result2.append(item2)

    # Reverse the results to maintain the original order
    result1.reverse()
    result2.reverse()

    return result1, result2


args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]


for cache_policy in range(4):
    filename = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_cp"
        + str(cache_policy)
        + "/client.json"
    )

    new_key = scenario + "_s" + str(num_surrogates) + "_cp" + str(cache_policy) + "_client"


    output_data = {
        new_key: {
            "send": {"time": [], "values": []},
            "receive": {"time": [], "values": []},
            "fail": {"time": [], "values": []},
        }
    }

    with open(filename, "r") as file:
        results = json.load(file)

    for key in results:
        print("getting data from: ", key)

        target_value = results[key]

        for vector in target_value["vectors"]:
            if vector["name"] == "sendTimes:vector":
                no_repeats_value, no_repeats_time = remove_multiple_occurrences(
                    vector["value"], vector["time"]
                )
                output_data[new_key]["send"]["time"].append(no_repeats_time)
                output_data[new_key]["send"]["values"].append(no_repeats_value)
            if vector["name"] == "receiveTimes:vector":
                output_data[new_key]["receive"]["time"].append(vector["time"])
                output_data[new_key]["receive"]["values"].append(vector["value"])
            if vector["name"] == "failedSessions:vector":
                output_data[new_key]["fail"]["time"].append(vector["time"])
                output_data[new_key]["fail"]["values"].append(vector["value"])


    outfilename = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_cp"
        + str(cache_policy)
        + "/client_formated.json"
    )

    with open(outfilename, "w") as output_file:
        json.dump(output_data, output_file, indent=2)
