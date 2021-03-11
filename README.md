# CS544-Assignment

### For question 1:

1) `$ cd Question_1`

2) `$ g++ routing.cpp -o routing`

3) `$ ./-routing -top topologyfile -conn connectionsfile -rt routingtablefile -ft forwardingtablefile -path pathsfile - flag hop|dist - p 0|1`

E.g. to run ARPANET connections with 24 node topology using dist as cost parameter and optimistic approach:

`./routing -top top24.txt -conn ARPANET_100.txt -rt routingTable.txt -ft forwardingTable.txt -path pathsFile.txt -flag dist -p 0`


### For question 2:

1) `$ cd Question_2`

2) `$ g++ case_a.cpp`

3) `$./a.out`

The above code will run **Case A** simulation. It will ask you to enter lambda and mu value. After that, the simulation will run and, values will be printed on the console and in a file `case_a.txt`. Similarly to run the code for **Case B** and **Case C** replace `$ g++ case_a.cpp`with `$ g++ case_b.cpp` and `$ g++ case_c.cpp` respectively, and execute the generated executable using `$./a.out`. 