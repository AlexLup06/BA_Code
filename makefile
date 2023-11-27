all: omnet inet cdnsimulator


omnet:
	@echo "Extracting OMNeT++"
	@tar -xzf omnetpp-6.0.2-core.tar
	@echo "Done"
	@echo "Building OMNeT++"
	@cd omnetpp-6.0.2 && source setenv && ./configure WITH_QTENV=no WITH_OSG=no && make MODE=release -j8
	@echo "Done"

inet:
	@echo "Extracting INET"
	@tar -xzf inet-4.5.2-src.tar
	@echo "Done"
	@echo "Building INET"
	@cd omnetpp-6.0.2 && source setenv && cd ../inet4.5 && source setenv && make makefiles && make MODE=release -j8
	@echo "Done"

cdnsimulator:
	@echo "Building CDN Simulator"
	@cd CDNsimulator && make MODE=release -j8 all