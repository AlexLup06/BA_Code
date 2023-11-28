import re
from graph import Graph
import os

root = os.getenv("root")

pattern = r"\[(\d+)\]"

for s in range(3):
    for networkId in range(8):
        g = Graph(300)

        start_reading = False
        read_routers = False
        read_surrogates = False
        numSurrogates = 5 * (s + 1)
        file_path = (
            root
            + "/CDNsimulator/simulations/networks_s"
            + str(numSurrogates)
            + "/network"
            + str(networkId)
            + ".ned"
        )

        print("file_path: ", file_path)
        surrogates = []
        dijkstras = [[] for x in range(5 * (s + 1))]
        content = []
        count = 0
        with open(file_path, "r") as file:
            for line in file:
                if "originServer.ethg++" in line:
                    continue
                content.append(line)
                if "//routers and clients from here" in line:
                    read_routers = True
                    continue

                if ("client[" in line) and read_routers:
                    continue

                if line.strip() == "":
                    continue

                if line == "":
                    continue

                if ("surrogateServer" in line) and ("router" in line):
                    read_surrogates = True

                if read_routers and not read_surrogates:
                    matches = re.findall(pattern, line)
                    # print("routers: ",matches)
                    g.graph[int(matches[0])][int(matches[1])] = 1
                    g.graph[int(matches[1])][int(matches[0])] = 1

                if read_routers and read_surrogates and count < 5 * (s + 1):
                    # print("line: ", line)
                    match = re.findall(pattern, line)
                    # print("surrogate: ", match[1])
                    count += 1

                    surrogates.append(int(match[1]))

        content.pop()

        for j in range(len(surrogates)):
            dijkstras[j] = g.dijkstra(surrogates[j])

        leastDiff = 100000000
        leastDiffIndex = -1
        for i in range(300):
            minDist = 100000000
            maxDist = 0
            distances = []
            for j in range(len(surrogates)):
                distances.append(dijkstras[j][i])

            for j in range(len(distances)):
                if distances[j] < minDist:
                    minDist = distances[j]
                if distances[j] > maxDist:
                    maxDist = distances[j]

            if maxDist - minDist < leastDiff:
                leastDiff = maxDist - minDist
                leastDiffIndex = i

            # print("differences: ", distances)

        print("leastDiff: ", leastDiff)
        print("leastDiffIndex: ", leastDiffIndex)
        print("\n")

        content.append(
            "       originServer.ethg++ <--> Eth400G {length=10000m;} <--> router["
            + str(leastDiffIndex)
            + "].ethg++;\n"
        )
        content.append("}")

        with open(file_path, "w") as file:
            file.writelines(content)
