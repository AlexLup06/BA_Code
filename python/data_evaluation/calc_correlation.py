import sys
import numpy as np
import json
import os

root = os.getenv("root")

#
# calculate correlation between the number of failed requests and the recovery time
#

args = sys.argv
num_surrogates = int(args[1])
scenario = args[2]


file_path_fail = (
    root
    + "/data/count_failed_requests/"
    + scenario
    + "_s"
    + str(num_surrogates)
    + "_count_failed_requests.json"
)

file_path_rec = (
    root
    + "/data/recovery_time/"
    + scenario
    + "_s"
    + str(num_surrogates)
    + "_recovery_times.json"
)

with open(file_path_fail, "r") as file:
    data = json.load(file)
all_count_failed_requests = []
for key in data:
    count_failed_requests = data[key]
    cache_policy = key.split("_")[3][2]

    all_count_failed_requests.extend(count_failed_requests)

with open(file_path_rec, "r") as file:
    data = json.load(file)
all_recovery_time = []
for key in data:
    recovery_time = data[key]
    cache_policy = key.split("_")[3][2]

    all_recovery_time.extend(recovery_time)

correlation = np.corrcoef(all_count_failed_requests, all_recovery_time)[0, 1]
print("correlation: ", correlation)
