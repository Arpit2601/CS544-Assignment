# CS544-Assignment

### For question 1:

1) `$ cd Question_1`

2) `$ g++ routing.cpp -o routing`

3) `$ ./-routing -top topologyfile -conn connectionsfile -rt routingtablefile -ft forwardingtablefile -path pathsfile - flag hop|dist - p 0|1`

E.g. to run ARPANET connections with 24 node topology using dist as cost parameter and optimistic approach:

`./routing -top top24.txt -conn ARPANET_100.txt -rt routingTable.txt -ft forwardingTable.txt -path pathsFile.txt -flag dist -p 0`


### For question 2: