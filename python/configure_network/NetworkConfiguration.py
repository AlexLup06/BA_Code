import networkx as nx
import matplotlib.pyplot as plt
from graph import Graph
from matplotlib.lines import Line2D
from functions import (
    add_edge_to_graph,
    add_clients_and_routers,
    build_graph,
    calcSurrogateDistances,
    add_surrogates,
    closest_surrogate_to_client,
    create_client_request_files,
    create_client_requests,
)
import sys

prefix = "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/CDNsimulator/simulations"

multiplier = 200
squares_per_side = 5  # for deviding clients determining surrogate server positions
num_clients_per_node = 20
num_clients = 300
flash_crowd=True

args = sys.argv

for networkId in range(8):
    for i in range(3):
        g = Graph(300)
        G = nx.Graph()
        points = []  # (x,y) points
        edges = []  # (v1,v2, weight)
        numSurrogateServers = 5 * (i + 1)

        print("networkId: ", networkId, "; numSurrogateServers: ", numSurrogateServers)

        #
        # build graph and add edges
        #

        x_max, y_max, coordinates_to_id = build_graph(
            g,
            edges,
            points,
            multiplier,
            (
                "/Users/alexanderlupatsiy/Documents/Uni/Semester_6/Bachelor_Arbeit/Code/Networks/network"
                + str(networkId)
                + ".txt"
            ),
            networkId,
        )
        for i in range(len(edges)):
            add_edge_to_graph(G, points[edges[i][0]], points[edges[i][1]], edges[i][2])

        #
        # assign clients and routers
        #

        # clients with assigned routers
        client_to_router = add_clients_and_routers(
            prefix
            + "/networks_s"
            + str(numSurrogateServers)
            + "/network"
            + str(networkId)
            + ".ned",
            edges,
            num_clients_per_node,
            num_clients,
            networkId,
            points,
        )

        #
        # add Surrogate Servers
        #

        # router index to which surrogate server connects
        surrogateServers = [0] * numSurrogateServers

        router_to_surrogate, client_to_surr = add_surrogates(
            surrogateServers,
            numSurrogateServers,
            points,
            x_max,
            y_max,
            g,
            squares_per_side,
            coordinates_to_id,
            prefix
            + "/networks_s"
            + str(numSurrogateServers)
            + "/network"
            + str(networkId)
            + ".ned",
            client_to_router,
        )

        #
        # calculate array of all surrogate servers sorted by distance FOR EACH surrogate server
        #
        calcSurrogateDistances(
            numSurrogateServers,
            surrogateServers,
            g,
            (
                prefix
                + "/config/network_specific"
                + "_s"
                + str(numSurrogateServers)
                + "/network"
                + str(networkId)
                + "/surrogate_distances"
                + str(networkId)
                + ".txt"
            ),
        )

        #
        # calculate closest surrogate server for each client
        #

        closest_surrogate_to_client(
            g,
            surrogateServers,
            router_to_surrogate,
            client_to_router,
            (
                prefix
                + "/config/network_specific"
                + "_s"
                + str(numSurrogateServers)
                + "/network"
                + str(networkId)
                + "/clients_to_surrogate"
                + str(networkId)
            ),
            num_clients_per_node,
            client_to_surr
        )



# creating client request files
# for each network create 4 client request files
for i in range(3):
    create_client_request_files(
        prefix + "/config/base_client_requests_s" + str(5 * (i + 1)),
        num_clients_per_node,
        5 * (i + 1),
        num_clients,
        flash_crowd
    )