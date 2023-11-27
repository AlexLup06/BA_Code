import random
import os
import sys

args = sys.argv
num_surrogates = int(args[1])

seed_value = 50
random.seed(seed_value)

path = "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/CDNsimulator/simulations/config/web_object_s15"

if os.path.exists(path):
    os.remove(path)
    print(f"{path} has been deleted.")
else:
    print(f"{path} does not exist, so it cannot be deleted.")

with open(
    path,
    "a",
) as file:
    for i in range(1000 * num_surrogates / 5):
        file.write(str(i) + " " + str(random.randint(20000, 200000)) + "\n")
