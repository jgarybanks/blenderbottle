This is *EXTREMELY* a work in progress. More accurately, I started it and kinda dropped it for lack of time. It's definitely something I plan on picking back up though, and the design below should be taken with a grain of sald (add Kafka, mq, etc).

aggregator - code to collect data in real time
collector - code to collect the data from the aggregators, or from other
            sources. Has a persistent data store, and provides metadata
            to the publisher.
distributor - code to do network transport of actual data
publisher - runs on source hosts, publishes information gathered by the
            collector
subscriber - runs on destination hosts, subscribes to updates from source
             publishers
tools - userspace tools, esp. to invoke injection of filesystem metadata
        information to the publisher
bdwgc - garbage collecting memory allocation

N.B. for publisher/subscriber example, see VortexOpenSplice/examples/streams/Throughput/isocpp


Another other idea is to use a mongodb replication set and a pub/sub,
where the collector is also the publisher, and is responsible for inserts
into mongo. It receives information about filesystem changes that have
occurred, and puts them in mongo. It gets an _id back, and publishes that.
http://download.prismtech.com/docs/Vortex/apis/lite/isocpp/a00110.html
http://download.prismtech.com/docs/Vortex/pdfs/OpenSplice_DDSTutorial.pdf

Any subscribers that are interested in the details
http://download.prismtech.com/docs/Vortex/pdfs/OpenSplice_DDSTutorial.pdf
(see esp. the case of string (filename) as a templated primitive. Since 
you're not doing the actual data in the pub/sub layer, you don't have to
worry about anything like blocks or compression or anything else, just the
filenames. Perhaps encryption is in order, though.

spawn a new thread and request the _id from their local distributor, which
contacts the local mongodb. If the local doesn't have it, and is behind > 1
second, it can reach out to the primary mongo host and request it from there.
The filesystem object is then applied (how to do serialization in the case
of > 1 thread?, Do The subscribers have to be openslice keyless subscribers
per repset?). This does, however, allow for in-order processing of some
repsets and best-possible throughput in other repsets.

The collector doesn't have to be particularly fast, only as fast as mongo can
insert data. Since the lookups never (?) happen by anything other than the
_id, we don't have to index by anything in particular. 
