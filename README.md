# SDLE Assignment

This assignment aims to develop a local-first shopping list application.
For this it was implemented, in C++, a distributed system with various clients and servers running at the same time,
while ensuring its scalability.

All shopping lists are represented as a Conflict-Free Replicated Data Type, namely a state-based composable map,
described in [European Seventh Framework Programme ICT call 10](https://pages.lip6.fr/syncfree/attachments/article/46/WP3-report.pdf#page=31), 
with an embedded stated-based counter, described in [The problem with embedded CRDT counters and a solution](https://repositorium.sdum.uminho.pt/bitstream/1822/51503/1/Problem-Solution-Counters-PAPOC2016.pdf). 
This ensures that, even if multiple clients change the same shopping list at the same time, eventually they will be able to merge and remain
consistent throughout the system, achiving strong enventual consitency.

For messaging [ZeroMQ](https://zeromq.org/) and [JSON](https://www.json.org/) were user. ZeroMQ provides various functionalities, but we only used REP/REQ, ROUTER, Load 
Balancing Message Broker, Majordomo Pattern.

The cloud element, we've implemented a system from scratch, that includes multiple nodes in a architecture based on Amazon's Dynamo, described in [Dynamo: Amazon’s Highly Available Key-value Store](https://www.allthingsdistributed.com/files/amazon-dynamo-sosp2007.pdf), using a use a partition-aware broker that routes requests directly to the appropriate nodes and Consistent hashing.

For more details, please see the full [presentation](doc/SDLEg14.pdf).

Group members:

1. João Coelho (<up202004846@up.pt>)
2. João Mota (<up202108677@up.pt>)
3. Pedro Lima (<up20218806@up.pt>)
4. António Rego (<up202108666@up.pt>)
