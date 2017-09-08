aggregator - code to collect data in real time
collector - code to collect the data from the aggregators
distributor - code to do network transport of actual data
publisher - runs on source hosts, publishes information gathered by the
            collector, or from other sources.
subscriber - runs on destination hosts, subscribes to updates from source
             publishers
tools - userspace tools, esp. to invoke injection of filesystem metadata
        information to the publisher
bdwgc - garbage collecting memory allocation

N.B. for publisher/subscriber example, see VortexOpenSplice/examples/streams/Throughput/isocpp
