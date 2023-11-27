1. Clone the GitHub repository from:
https://github.com/AlexLup06/BA_Code

Download INET-4.5.2 and OMNeT++.6.0.2 from the official websites to the root
directory and follow their installation guides. Then run $ source setenv in the
root directory. After that move to ./CDNsimulator and run:
$ make MODE=release all
In ./CDNsimulator/simulations/omnetpp.ini change "resultdir" to the path of
the root.

2. Start simulations by moving to ./CDNsimulator/simulations/ and run the com-
mand:
$ ../out/clang-release/src/CDNsimulator -f omnetpp.ini -c NumSugg5 -r ’$numNet=<NUM_NET> && $cp=<CP> && $reqId=<REQID>’ -m -u Cmdenv - n.:../src:../../inet4.5 examples:../../inet4.5/showcases:../../inet4.5/src:../../inet4.5/tests/validation:../../inet4.5/tests/networks:../../inet4.5/tutorials--image-path=../../inet4.5/images -l ../../inet4.5/src/INET

• <NUM_NET>:= network ID (0,...,7)
• <CP>:= cache-miss policy ((0,Closest Surrogate),(1,Random Surrogate),(2,Load
Balance),(3,Closest Origin))
• <REQID>:= request trace file ID (0,...7)


3. Evaluating Data:
First, we need to extract the data. For that run:
$ ./exportData.sh <NAME_DIR_TO_SAVE> <NUMBER_SURROGATES> <CACHE_POLICY>

However, ./data/ already has sorted and calculated data. All tools to plot data can be
found in the directory ./python/plot_data/. Please use that data. The functions used to
format the raw data and calculate the needed values are specifically written for the results
that were used in this thesis. If you have the expertise you can change the source code of
the Python scripts to accommodate your needs.
