import json
import sys
import re

#
# format surrogate server data
#

def remove_trailing_zeros(arr):
    while arr and arr[-1] == 0:
        arr.pop()
    return arr


args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]


for cache_policy in range(4):
    node = "surrogate"

    filename = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_cp"
        + str(cache_policy)
        + "/surrogate.json"
    )

    new_key = (
        scenario + "_s" + str(num_surrogates) + "_cp" + str(cache_policy) + "_" + node
    )
    output_data = {
        new_key: {
            "load_from_client": {
                "time": [[] for _ in range(num_surrogates)],
                "values": [[] for _ in range(num_surrogates)],
            },
            "load_from_surrogate": {
                "time": [[] for _ in range(num_surrogates)],
                "values": [[] for _ in range(num_surrogates)],
            },
            "request_in_from_surrogate": {
                "time": [[] for _ in range(num_surrogates)],
            },
            "response_out_to_surrogate": {
                "time": [[] for _ in range(num_surrogates)],
            },
            "request_in_from_client": {
                "time": [[] for _ in range(num_surrogates)],
            },
            "response_out_to_client": {
                "time": [[] for _ in range(num_surrogates)],
            },
        }
    }

    with open(filename, "r") as file:
        results = json.load(file)

    for key in results:
        print("getting data from: ", key)

        target_value = results[key]

        for vector in target_value["vectors"]:
            match = re.search(r"surrogateServer\[(\d+)\]", vector["module"])
            surrogate_index = int(match.group(1))

            if vector["name"] == "surrogateLoadObjects:vector":
                wo_trailing_zeros = remove_trailing_zeros(vector["value"])
                output_data[new_key]["load_from_surrogate"]["time"][
                    surrogate_index
                ].append(vector["time"])
                output_data[new_key]["load_from_surrogate"]["values"][
                    surrogate_index
                ].append(wo_trailing_zeros)

            if vector["name"] == "loadSurrogate:vector":
                wo_trailing_zeros = remove_trailing_zeros(vector["value"])
                output_data[new_key]["load_from_client"]["time"][
                    surrogate_index
                ].append(vector["time"])
                output_data[new_key]["load_from_client"]["values"][
                    surrogate_index
                ].append(wo_trailing_zeros)

            if vector["name"] == "surrogateObjectRequestIn:vector":
                output_data[new_key]["request_in_from_surrogate"]["time"][
                    surrogate_index
                ].append(vector["time"])

            if vector["name"] == "surrogateObjectResponseOut:vector":
                output_data[new_key]["response_out_to_surrogate"]["time"][
                    surrogate_index
                ].append(vector["time"])

            if vector["name"] == "surrogateClientRequestIn:vector":
                output_data[new_key]["request_in_from_client"]["time"][
                    surrogate_index
                ].append(vector["time"])

            if vector["name"] == "surrogateClientResponseOut:vector":
                output_data[new_key]["response_out_to_client"]["time"][
                    surrogate_index
                ].append(vector["time"])

    outfilename = (
        "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/"
        + scenario
        + "_s"
        + str(num_surrogates)
        + "_cp"
        + str(cache_policy)
        + "/surrogate_formated.json"
    )

    with open(outfilename, "w") as output_file:
        json.dump(output_data, output_file, indent=2)
