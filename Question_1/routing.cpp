#include<bits/stdc++.h>

using namespace std;

map<pair<int, int>, pair<int, int>> linkInfo;   // Stores information about links i.e. {source, destination} = {cost, capacity}
map<pair<int, int>, int> linkIds;            // Stores link Ids i.e. {a,b} -> id and {b,a} -> id
map<pair<int, int>, vector<int>> firstShortestPath; // stores first shortest path from a->b i.e. map[{a, b}] = path
map<pair<int, int>, vector<int>> secondShortestPath; // stores second shortest path from a->b i.e. map[{a, b}] = path
map<int, double> bCummulative;                         // Stores summation bequ or bmax so far for corresponding link id i.e. link_id->sum(b)
map<int, pair<int, vector<int>>> pathsFileTable;    // Corresponds to Table 3 in assignment i.e. connectionId -> {pathCost, labelList}
map<int, vector<vector<int>>> forwardingTable;     // node i -> vector of {interface in, label in, interface out, label out}
int connectionsDenied;                              // Total number of connections denied
int label = 0;                                      // Global label to be assigned


// Util function to print elements of vector
void printVector(vector<int> path)
{
    for(int i=0;i<path.size();i++){cout<<path[i]<<" ";}
}

// Util function to print elements of vector in a file
void printVectorFile(vector<int> path, ofstream& file)
{
    for(int i=0;i<path.size();i++){file<<path[i]<<" ";}
}

// Used for debugging to print all the shortest paths of a node
void printFirstShortestPaths(int node)
{

    for(auto it=firstShortestPath.begin();it!=firstShortestPath.end();it++)
    {
        if(it->first.first==node)
        {
            cout<<it->first.second<<": ";
            printVector(it->second);
            cout<<endl;
        }
    }
}

// Used for debugging to print all the second shortest paths of a node
void printSecondShortestPaths(int node)
{
    for(auto it=secondShortestPath.begin();it!=secondShortestPath.end();it++)
    {
        if(it->first.first==node)
        {
            cout<<it->first.second<<": ";
            printVector(it->second);
            cout<<endl;
        }
    }
}

// Prints the cost of path in a file
void printPathCost(vector<int> path, ofstream& file, string param)
{
    if(param == "dist")
    {
        int cost = 0;
        int prev = path[0];
        for(int i=1;i<path.size();i++)
        {
            cost+=linkInfo[{prev, path[i]}].first;
            prev = path[i];
        }
        file<<cost;
    }
    else file<<path.size()-1;
    
}

// Format of file
// -------- Node i  -----------
// (Shortest path)
// Destination node path path cost 
// (Second Shortest path)
// Destination node path path cost
// If no path then just print no path with destination
// If param is dist then print path cost given in linkInfo otherwise use hop as path cost
void printRoutingTable(string routingFile, int nodeCount, string param)
{

    ofstream routingFileStream(routingFile);
    for(int i=0;i<nodeCount;i++)
    {
        routingFileStream<<"-------------- Node "<<i<<" --------------"<<endl;
        routingFileStream<<"(Shortest Paths)"<<endl;
        routingFileStream<<"Destination\t\t"<<"Path\t\t"<<"Path cost"<<endl;
        for(int j=0;j<nodeCount;j++)
        {
            if(firstShortestPath[{i, j}].size()==0 || (firstShortestPath[{i, j}].size() == 1 && firstShortestPath[{i, j}][0]!=i))
            {
                routingFileStream<<j<<"\t\t No path exists or no point in transffering packets to same node from where it was generated."<<endl;
            }
            else {routingFileStream<<j<<"\t\t< ";printVectorFile(firstShortestPath[{i, j}], routingFileStream);routingFileStream<<">\t\t";printPathCost(firstShortestPath[{i, j}], routingFileStream, param);routingFileStream<<endl;}
        }

        routingFileStream<<"(Second Shortest Paths)"<<endl;
        routingFileStream<<"Destination\t\t"<<"Path\t\t"<<"Path cost"<<endl;
        for(int j=0;j<nodeCount;j++)
        {
            if(secondShortestPath[{i, j}].size()==0 ||  (secondShortestPath[{i, j}].size() == 1 && secondShortestPath[{i, j}][0] != i))
            {
                routingFileStream<<j<<"\t\t No path exists or no point in transffering packets to same node from where it was generated."<<endl;
            }
            else {routingFileStream<<j<<"\t\t< ";printVectorFile(secondShortestPath[{i, j}], routingFileStream);routingFileStream<<">\t\t";printPathCost(secondShortestPath[{i, j}], routingFileStream, param);routingFileStream<<endl;}
        }
        routingFileStream<<endl;

    }

}

// Util function to print forwarding table
void printForwardingTable(string forwardingTableFile, int nodeCount)
{
    ofstream forwardingTableFileStream(forwardingTableFile);
    for(int i=0;i<nodeCount;i++)
    {
        forwardingTableFileStream<<"-------------- Node "<<i<<" --------------"<<endl;
        forwardingTableFileStream<<"Interface in\t"<<"Label in\t"<<"Interface out\t"<<"Label out"<<endl;
        vector<vector<int>> table = forwardingTable[i];
        for(int t=0;t<table.size();t++)
        {
            forwardingTableFileStream<<table[t][0]<<"\t\t\t\t"<<table[t][1]<<"\t\t\t\t"<<table[t][2]<<"\t\t\t\t"<<table[t][3]<<endl;
        }
        forwardingTableFileStream<<endl;
    }
}

// Function to get all the shortest paths from sourceto all possible nodes
void getAllFirstShortestPaths(int source, int parent[], int nodeCount) 
{ 
    for (int i = 0; i < nodeCount; i++) 
    { 
        vector<int> path;
        path.push_back(i);
        int dest = i;
        while(parent[dest]!=-1){path.push_back(parent[dest]);dest = parent[dest];}  // Recursively add parents
        reverse(path.begin(), path.end());
        firstShortestPath[{source, i}] = path;
    } 
} 

// Computes the shortest paths from src
void dijkstra(int src, int nodeCount, string param) 
{ 

    set<pair<int, int>> nodes;
    int parent[nodeCount];
    for(int i=0;i<nodeCount;i++){parent[i]=-1;}
    vector<int> dist(nodeCount, INT_MAX);
    dist[src]=0;
    nodes.insert({0, src});

    while(!nodes.empty())
    {
        pair<int, int> firstNode = *(nodes.begin());
        nodes.erase(nodes.begin());
        int u = firstNode.second;
        for(int i=0;i<nodeCount;i++)
        {
            if(linkInfo.find({u, i})!=linkInfo.end())
            {
                if(param=="dist")
                {
                    if(dist[u] + linkInfo[{u, i}].first < dist[i])
                    {
                        // If node i is already present in set then remove it
                        if(dist[i]!=INT_MAX)
                        {
                            nodes.erase({dist[i], i});
                        }
                        dist[i]=dist[u]+linkInfo[{u, i}].first;
                        parent[i]=u;
                        nodes.insert({dist[i], i});
                    }
                    
                }
                // If using hop then each link has same cost of 1 
                else if(param=="hop")
                {
                    if(dist[u] + 1 < dist[i])
                    {
                        if(dist[i]!=INT_MAX)
                        {
                            nodes.erase({dist[i], i});
                        }
                        dist[i]=dist[u]+1;
                        parent[i]=u;
                        nodes.insert({dist[i], i});
                    }
                    
                    
                }
            }
        }
    } 

    getAllFirstShortestPaths(src, parent, nodeCount); 
} 

// function stores second shortest path from source to destination
void dijkstraSecond(int source, int destination, int nodeCount, string param)
{
    // Make a temporary linkInfo map after removing links (from firstShortestPath) from source to destination
    map<pair<int, int>, pair<int, int>> tempLinkInfo = linkInfo;

    vector<int> path = firstShortestPath[{source, destination}];
    int prev = source;
    for(int i=1;i<path.size();i++)
    {
        tempLinkInfo.erase({prev, path[i]});
        tempLinkInfo.erase({path[i], prev});  
        prev = path[i];
    }

    // After removing links of first shortest path use dijkshtra to compute second shortest path
    set<pair<int, int>> nodes;
    int parent[nodeCount];
    for(int i=0;i<nodeCount;i++){parent[i]=-1;}
    vector<int> dist(nodeCount, INT_MAX);
    dist[source]=0;
    nodes.insert({0, source});

    while(!nodes.empty())
    {
        pair<int, int> firstNode = *(nodes.begin());
        nodes.erase(nodes.begin());
        int u = firstNode.second;
        for(int i=0;i<nodeCount;i++)
        {
            if(tempLinkInfo.find({u, i})!=tempLinkInfo.end())
            {
                if(param=="dist")
                {
                    if(dist[u] + tempLinkInfo[{u, i}].first < dist[i])
                    {
                        if(dist[i]!=INT_MAX)
                        {
                            nodes.erase({dist[i], i});
                        }
                        dist[i]=dist[u]+tempLinkInfo[{u, i}].first;
                        parent[i]=u;
                        nodes.insert({dist[i], i});
                    }
                    
                }
                else if(param=="hop")
                {
                    if(dist[u] + 1 < dist[i])
                    {
                        if(dist[i]!=INT_MAX)
                        {
                            nodes.erase({dist[i], i});
                        }
                        dist[i]=dist[u]+1;
                        parent[i]=u;
                        nodes.insert({dist[i], i});
                    }
                    
                    
                }
            }
        }
    } 


    // Storing the second shortest path by recursively storing parents
    path.clear();
    path.push_back(destination);
    int dest = destination;
    while(parent[dest]!=-1){path.push_back(parent[dest]);dest = parent[dest];}
    reverse(path.begin(), path.end());
    secondShortestPath[{source, destination}] = path;

}

void getSecondShortestPaths(int nodeCount, string param)
{
    for(int i=0;i<nodeCount;i++)
    {
        for(int j=0;j<nodeCount;j++)
        {
            // i is the source and j is the destination
            if(i!=j)
            {
                vector<int> path = firstShortestPath[{i, j}];
                if(path.size()==1 && path[0]!=i)
                {
                    // No path between source and destination so there will be no second shortest either
                    return;
                } 
                else dijkstraSecond(i, j, nodeCount, param);
            }
        }
    }
}

// p=0 means to use optimistic approach and p=1 means to use pessimistic approach
void processConnection(int connectionId, int source, int destination, int bmin, int bave, int bmax, int p, string param)
{
    if(firstShortestPath[{source, destination}].size()==0 || (firstShortestPath[{source, destination}].size() == 1 && firstShortestPath[{source, destination}][0]!=source))
    {
        connectionsDenied++;
        cout<<"Connection with id:"<<connectionId<<" from "<<source<<" to "<<destination<<" cannot be setup."<<endl;  
        return; 
    }
    if(p==0)
    {
        int flag = 1;
        double bEquiv = min((double)bmax, (double)bave+0.25*(double)(bmax-bmin));   // ---------------CHANGE MADE

        // Get the firstshortest path and check for the condition along each link
        // If not satisfied then check for secondshortest path
        vector<int> path = firstShortestPath[{source, destination}];
        
        int prev = path[0];
        // For each link in path
        for(int i=1;i<path.size();i++)
        {
            // TODO check what does map return if key not present in map (bCummulative)
            if(bEquiv <= linkInfo[{prev, path[i]}].second - bCummulative[linkIds[{prev, path[i]}]])
            {
                prev = path[i];
                continue;
            }
            else
            {
                flag = 0;
                break;
            }
        }

        // First Shortest path worked
        if(flag)
        {
            // Change bCummulative for all links on this path and store the label list along with path cost for final output
            int prev = path[0];
            int pathCost = 0;   // TODO check if cost has to be different in hop/dist case
            for(int i=1;i<path.size();i++)
            {
                bCummulative[linkIds[{prev, path[i]}]] += bEquiv;
                pathsFileTable[connectionId].second.push_back(label);
                label++;
                if(param=="dist"){pathCost+=linkInfo[{prev, path[i]}].first;}
                else pathCost++;
                prev = path[i];
            }
            pathsFileTable[connectionId].first = pathCost;
            
            // Output connection Info
            cout<<connectionId<<"\t\t"<<source<<"\t\t"<<destination<<"\t< ";
            printVector(pathsFileTable[connectionId].second);
            cout<<">\t\t"<<pathsFileTable[connectionId].first<<endl;

            // Updating the forwarding table
            // Update table only for intermediate nodes
            
            // First entry added in forwarding table will be that of source -> {-1, -1, node 1, label}
            vector<int> temp;
            temp.push_back(-1);temp.push_back(-1);temp.push_back(path[1]);temp.push_back(pathsFileTable[connectionId].second[0]);
            forwardingTable[path[0]].push_back(temp);

            // Updating forwarding table for intermediate nodes
            for(int i=1;i<path.size()-1;i++)
            {
                vector<int> temp;
                temp.push_back(path[i-1]);  // interface in
                temp.push_back(pathsFileTable[connectionId].second[i-1]);   // label in
                temp.push_back(path[i+1]);  // interface out
                temp.push_back(pathsFileTable[connectionId].second[i]);     // label out
                forwardingTable[path[i]].push_back(temp);
            }

            // Last entry added in forwarding table will be that of destination -> {node (last -1), label, -1, -1}
            temp.clear();
            temp.push_back(path[path.size()-2]);temp.push_back(pathsFileTable[connectionId].second[pathsFileTable[connectionId].second.size()-1]);temp.push_back(-1);temp.push_back(-1);
            forwardingTable[path[path.size()-1]].push_back(temp);
        }
        // Check for second shortest path
        else
        {
            // First check if second shortest path actually exits or not
            if(secondShortestPath[{source, destination}].size()==0 || (secondShortestPath[{source, destination}].size() == 1 && secondShortestPath[{source, destination}][0]!=source))
            {
                connectionsDenied++;
                cout<<"Connection with id:"<<connectionId<<" from "<<source<<" to "<<destination<<" cannot be setup."<<endl;  
                return; 
            }

            flag = 1;
            path.clear();
            path = secondShortestPath[{source, destination}];
            prev = path[0];
            for(int i=1;i<path.size();i++)
            {
                if(bEquiv <= linkInfo[{prev, path[i]}].second - bCummulative[linkIds[{prev, path[i]}]])
                {
                    prev = path[i];
                    continue;
                }
                else
                {
                    flag = 0;
                    break;
                }
            }
            // Second shortest path worked
            if(flag)
            {
                prev = path[0];
                int pathCost = 0;
                for(int i=1;i<path.size();i++)
                {
                    bCummulative[linkIds[{prev, path[i]}]] += bEquiv;
                    pathsFileTable[connectionId].second.push_back(label);
                    label++;
                    if(param=="dist"){pathCost+=linkInfo[{prev, path[i]}].first;}
                    else pathCost++;
                    prev = path[i];
                }
                pathsFileTable[connectionId].first = pathCost;

                // Output connection Info
                cout<<connectionId<<"\t\t"<<source<<"\t\t"<<destination<<"\t< ";
                printVector(pathsFileTable[connectionId].second);
                cout<<">\t\t"<<pathsFileTable[connectionId].first<<endl;

                // Updating the forwarding table
                // Update table only for intermediate nodes

                // First entry added in forwarding table will be that of source -> {-1, -1, node 1, label}
                vector<int> temp;
                temp.push_back(-1);temp.push_back(-1);temp.push_back(path[1]);temp.push_back(pathsFileTable[connectionId].second[0]);
                forwardingTable[path[0]].push_back(temp);

                for(int i=1;i<path.size()-1;i++)
                {
                    vector<int> temp;
                    temp.push_back(path[i-1]);  // interface in
                    temp.push_back(pathsFileTable[connectionId].second[i-1]);   // label in
                    temp.push_back(path[i+1]);  // interface out
                    temp.push_back(pathsFileTable[connectionId].second[i]);     // label out
                    forwardingTable[path[i]].push_back(temp);
                }

                // Last entry added in forwarding table will be that of destination -> {node (last -1), label, -1, -1}
                temp.clear();
                temp.push_back(path[path.size()-2]);temp.push_back(pathsFileTable[connectionId].second[pathsFileTable[connectionId].second.size()-1]);temp.push_back(-1);temp.push_back(-1);
                forwardingTable[path[path.size()-1]].push_back(temp);
            }
            // Connection cannot be setup 
            else 
            {
                connectionsDenied++;
                cout<<"Connection with id:"<<connectionId<<" from "<<source<<" to "<<destination<<" cannot be setup."<<endl;
            }
        }


    }
    // Pessimistic approach
    else if(p==1)
    {
        int flag = 1;
        // Get the firstshortest path and check for the condition along each link
        // If not satisfied then check for secondshortest path
        vector<int> path = firstShortestPath[{source, destination}];
        
        int prev = path[0];
        // For each link in path
        for(int i=1;i<path.size();i++)
        {
            if(bmax <= linkInfo[{prev, path[i]}].second - bCummulative[linkIds[{prev, path[i]}]])
            {
                prev = path[i];
                continue;
            }
            else
            {
                flag = 0;
                break;
            }
        }

        // First Shortest path worked
        if(flag)
        {
            // Change bCummulative for all links on this path and store the label list along with path cost for final output
            int prev = path[0];
            int pathCost = 0;   // TODO check if cost has to be different in hop/dist case
            for(int i=1;i<path.size();i++)
            {
                bCummulative[linkIds[{prev, path[i]}]] += bmax;
                pathsFileTable[connectionId].second.push_back(label);
                label++;
                if(param=="dist"){pathCost+=linkInfo[{prev, path[i]}].first;}
                else pathCost++;
                prev = path[i];
            }
            pathsFileTable[connectionId].first = pathCost;

            // Output connection Info
            cout<<connectionId<<"\t\t"<<source<<"\t\t"<<destination<<"\t< ";
            printVector(pathsFileTable[connectionId].second);
            cout<<">\t\t"<<pathsFileTable[connectionId].first<<endl;

            // Updating the forwarding table
            // Update table only for intermediate nodes

            // First entry added in forwarding table will be that of source -> {-1, -1, node 1, label}
            vector<int> temp;
            temp.push_back(-1);temp.push_back(-1);temp.push_back(path[1]);temp.push_back(pathsFileTable[connectionId].second[0]);
            forwardingTable[path[0]].push_back(temp);
            
            for(int i=1;i<path.size()-1;i++)
            {
                vector<int> temp;
                temp.push_back(path[i-1]);  // interface in
                temp.push_back(pathsFileTable[connectionId].second[i-1]);   // label in
                temp.push_back(path[i+1]);  // interface out
                temp.push_back(pathsFileTable[connectionId].second[i]);     // label out
                forwardingTable[path[i]].push_back(temp);
            }

            // Last entry added in forwarding table will be that of destination -> {node (last -1), label, -1, -1}
            temp.clear();
            temp.push_back(path[path.size()-2]);temp.push_back(pathsFileTable[connectionId].second[pathsFileTable[connectionId].second.size()-1]);temp.push_back(-1);temp.push_back(-1);
            forwardingTable[path[path.size()-1]].push_back(temp);
        }
        // Check for second shortest path
        else
        {
            // First check if second shortest path actually exits or not
            if(secondShortestPath[{source, destination}].size()==0 || (secondShortestPath[{source, destination}].size() == 1 && secondShortestPath[{source, destination}][0]!=source))
            {
                connectionsDenied++;
                cout<<"Connection with id:"<<connectionId<<" from "<<source<<" to "<<destination<<" cannot be setup."<<endl;  
                return; 
            }

            flag = 1;
            path.clear();
            path = secondShortestPath[{source, destination}];
            prev = path[0];
            for(int i=1;i<path.size();i++)
            {
                if(bmax <= linkInfo[{prev, path[i]}].second - bCummulative[linkIds[{prev, path[i]}]])
                {
                    prev = path[i];
                    continue;
                }
                else
                {
                    flag = 0;
                    break;
                }
            }
            // Second shortest path worked
            if(flag)
            {
                prev = path[0];
                int pathCost = 0;
                for(int i=1;i<path.size();i++)
                {
                    bCummulative[linkIds[{prev, path[i]}]] += bmax;
                    pathsFileTable[connectionId].second.push_back(label);
                    label++;
                    if(param=="dist"){pathCost+=linkInfo[{prev, path[i]}].first;}
                    else pathCost++;
                    prev = path[i];
                }
                pathsFileTable[connectionId].first = pathCost;

                // Output connection Info
                cout<<connectionId<<"\t\t"<<source<<"\t\t"<<destination<<"\t< ";
                printVector(pathsFileTable[connectionId].second);
                cout<<">\t\t"<<pathsFileTable[connectionId].first<<endl;

                // Updating the forwarding table
                // Update table only for intermediate nodes

                // First entry added in forwarding table will be that of source -> {-1, -1, node 1, label}
                vector<int> temp;
                temp.push_back(-1);temp.push_back(-1);temp.push_back(path[1]);temp.push_back(pathsFileTable[connectionId].second[0]);
                forwardingTable[path[0]].push_back(temp);

                for(int i=1;i<path.size()-1;i++)
                {
                    vector<int> temp;
                    temp.push_back(path[i-1]);  // interface in
                    temp.push_back(pathsFileTable[connectionId].second[i-1]);   // label in
                    temp.push_back(path[i+1]);  // interface out
                    temp.push_back(pathsFileTable[connectionId].second[i]);     // label out
                    forwardingTable[path[i]].push_back(temp);
                }

                // Last entry added in forwarding table will be that of destination -> {node (last -1), label, -1, -1}
                temp.clear();
                temp.push_back(path[path.size()-2]);temp.push_back(pathsFileTable[connectionId].second[pathsFileTable[connectionId].second.size()-1]);temp.push_back(-1);temp.push_back(-1);
                forwardingTable[path[path.size()-1]].push_back(temp);
            }
            // Connection cannot be setup 
            else 
            {
                connectionsDenied++;
                cout<<"Connection with id:"<<connectionId<<" from "<<source<<" to "<<destination<<" cannot be setup."<<endl;
            }
        }
    }

}

int main(int argc, char** argv)
{
    string topologicalFileName, connectionFileName, routingTableFileName, forwardingTableFileName, pathsFileName, hopDist, p;
    // Process arguments
    if(argc<15)
    {
        cout<<"Input format not correct."<<endl<<"Format should be of format: ./-routing -top topologyfile -conn connectionsfile -rt routingtablefile -ft forwardingtablefile -path pathsfile - flag hopjdist - p 0|1."<<endl;
        return 0;
    }
    else
    {
        for(int i=1;i<argc;i+=2)
        {
            if(strcmp(argv[i], "-top")==0)
            {
                topologicalFileName = argv[i+1];
            }
            else if(strcmp(argv[i], "-conn")==0)
            {
                connectionFileName = argv[i+1];
            }
            else if(strcmp(argv[i], "-rt")==0)
            {
                routingTableFileName = argv[i+1];
            }
            else if(strcmp(argv[i], "-ft")==0)
            {
                forwardingTableFileName = argv[i+1];
            }
            else if(strcmp(argv[i], "-path")==0)
            {
                pathsFileName = argv[i+1];
            }
            else if(strcmp(argv[i], "-flag")==0)
            {
                hopDist = argv[i+1];
            }
            else if(strcmp(argv[i], "-p")==0)
            {
                p = argv[i+1];
            }
            else 
            {
                cout<<argv[i]<<endl;
                cout<<"Wrong arguments provided."<<endl<<"Format should be of format: ./-routing -top topologyfile -conn connectionsfile -rt routingtablefile -ft forwardingtablefile -path pathsfile - flag hopjdist - p 0|1."<<endl;
                return 0;
            }
        }
    }

    ifstream topologyFile(topologicalFileName);
    ifstream connectionFile(connectionFileName);

    if(!topologyFile)
    {
        perror("Error opening topology file.");
        return 0;
    }
    if(!connectionFile)
    {
        perror("Error opening connection file.");
        return 0;
    }

    // Read topology file
    int nodeCount, edgeCount;
    topologyFile>>nodeCount>>edgeCount;


    // Read network
    int source, destination, cost, capacity;
    for(int i=0;i<edgeCount;i++)
    {
        topologyFile>>source>>destination>>cost>>capacity;
        linkIds[{source, destination}] = i;
        linkIds[{destination, source}] = i;
        linkInfo[{source, destination}] = {cost, capacity};
        linkInfo[{destination, source}] = {cost, capacity};
    }

    // Getting first shortest paths on the basis of cost of paths/hops
    for(int i=0;i<nodeCount;i++)
    {
        dijkstra(i, nodeCount, hopDist);
    }
    
    // Get second sortest paths based on path cost/hops
    getSecondShortestPaths(nodeCount, hopDist);
    
    printRoutingTable(routingTableFileName, nodeCount, hopDist);

    // Reset connections denied
    connectionsDenied = 0;
    
    // Reading connection file
    cout<<"ConnectionId\t"<<"Source\t"<<"Destination\t"<<"Label List\t"<<"Path cost"<<endl;
    int numConnectionRequests;
    connectionFile>>numConnectionRequests;
    
    int bmin, bave, bmax;
    for(int i=0;i<numConnectionRequests;i++)
    {
        connectionFile>>source>>destination>>bmin>>bave>>bmax;
        processConnection(i, source, destination, bmin, bave, bmax, stoi(p), hopDist);
    }

    printForwardingTable(forwardingTableFileName, nodeCount);

    // output for pathFile
    ofstream pathsFile(pathsFileName);
    pathsFile<<numConnectionRequests<<" "<<numConnectionRequests - connectionsDenied<<endl;

    // Blocking probablity
    cout<<endl<<"-----------------------------------------------"<<endl;
    cout<<"Number of successful connections: "<<numConnectionRequests - connectionsDenied<<endl;
    cout<<"Number of denied connections: "<<connectionsDenied<<endl;
    cout<<"Blocking probablity (Percentage of connections denied): "<<(double)(connectionsDenied*100)/(double)numConnectionRequests<<endl;
}

// ./routing -top top14.txt -conn NSFNET_100.txt -rt routingTable.txt -ft forwardingTable.txt -path pathsFile.txt -flag dist -p 0