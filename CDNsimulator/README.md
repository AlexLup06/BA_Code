# CDNsimulator
CDNsimulator for my bachelor thesis

Install:

1. download from ...
2. unzip
3. go into the root directory of the project
4. type "make all"
5. go into /CDNsimulator/simulations and run a simulation: ../out/clang-release/src/CDNsimulator -f omnetpp.ini -c NumSugg5 -r '$numNet=0 && $cp=1 && $reqId<2&& $reqId>=0' -m -u Cmdenv -n .:../src:../../inet4.5/examples:../../inet4.5/showcases:../../inet4.5/src:../../inet4.5/tests/validation:../../inet4.5/tests/networks:../../inet4.5/tutorials --image-path=../../inet4.5/images -l ../../inet4.5/src/INET
