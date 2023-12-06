//Ethan Sandstrom

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <climits>
#include <chrono>

using namespace std;
int NUM_NODES = 0;

struct Edge
{
    int node1;
    int node2;
    int weight;
    Edge()
    {
        node1 = node2 = weight = 0;
    }
    bool operator ==(Edge rhs)
    {
        if(node1 == rhs.node1 && node2 == rhs.node2 && weight == rhs.weight)
        {
            return true;
        }
        if(node1 == rhs.node2 && node2 == rhs.node1 && weight == rhs.weight)
        {
            return true;
        }
        return false;
    }
};

void read_file(vector<Edge>& list, int& length, string filename);
void print_tree(vector<Edge> list);
void print_edge(Edge node);
vector<Edge> prims(vector<Edge> list);
vector<Edge> kruskals(vector<Edge> list);
bool full_tree(unordered_map<int, bool> mst);
Edge get_lowest_edge(vector<Edge> list, unordered_map<int, bool> mst_map);
void print_map(unordered_map<int, bool> map);
bool is_valid_addition(Edge new_edge, unordered_map<int, bool> mst_map);

int main()
{
    int length = 0;
    string filename = "graphs/graph_v400_e1200.txt";
    //string filename = "small.txt";
    vector<Edge> prims_list;
    read_file(prims_list, length, filename);
    vector<Edge> kruskals_list = prims_list;

    // print_tree(list);
    // cout << endl;
    //exit(1);

    /*
    chrono::time_point<chrono::system_clock> start;
	start = chrono::system_clock::now(); // Start the system clock.
    vector<Edge> prims_mst = prims(prims_list);
	chrono::time_point<chrono::system_clock> end;
	end = chrono::system_clock::now();
	chrono::duration<double> total_time = end - start;
    cout << "Prim's FINAL MST IS" << endl;
    print_tree(prims_mst);
	cerr << "Base computation took " << total_time.count() << " seconds " << endl;
    */

   vector<Edge> kruskals_mst = kruskals(kruskals_list);
   print_tree(kruskals_mst);


    return 0;
}

void read_file(vector<Edge>& list, int& length, string filename)
{
    string currentline;
    ifstream Instream;
    int n1, n2, weight;
    n1 = n2 = weight = 0;
    size_t index = 0;

    Instream.open(filename);
    if(Instream.fail())
    {
        cout << "Couldn't open file " << filename << endl;
        exit(1);
    }
    if(!getline(Instream, currentline))
    {
        cout << "Empty file" << endl;
        exit(0);
    }
    NUM_NODES = stoi(currentline);
    if(NUM_NODES == 0)
    {
        cout << "Empty graph" << endl;
        exit(0);
    }
    //cout << "Num nodes " << NUM_NODES << endl;
    Edge node;

    while(getline(Instream, currentline))
    {
        index = currentline.find(',');
        //printf("_%s_\n", (currentline.substr(0, index)).c_str());
        n1 = stoi(currentline.substr(0, index));
        currentline = currentline.substr(index+1);
        node.node1 = n1;

        index = currentline.find(',');
        n2 = stoi(currentline.substr(0, index));
        //printf("_%s_\n", (currentline.substr(0, index)).c_str());
        currentline = currentline.substr(index+1);
        node.node2 = n2;

        //printf("_%s_\n", currentline.c_str());
        node.weight = stoi(currentline);
        list.push_back(node);
    }
}
void print_tree(vector<Edge> list)
{
    size_t length = list.size();
    cout << length << endl;
    for(size_t b = 0; b < length; b++)
    {
        print_edge(list[b]);
    }
}
void print_edge(Edge node)
{
    printf("%d,%d,%d\n", node.node1, node.node2, node.weight);
}
vector<Edge> prims(vector<Edge> list)
{
    vector<Edge> mst;
    if(NUM_NODES == 1)
    {
        mst.push_back(list[0]);
        return mst;
    }

    //Create a map to keep track of which nodes have been added to the MST
    //<Node number, added or not>
    unordered_map<int, bool> added_map;
    // for(size_t c = 0; c < list.size(); c++)
    // {
    //     added_map[list[c].node1] = false;
    //     added_map[list[c].node2] = false;
    // }

    //Step one: Pick a node to start with. Node 1
    added_map[list[0].node1] = true;  //Add it to the map of added nodes

    //While not every node is added to the tree, keep going.
    Edge lowest;
    int edges_added = 0;
    //while(full_tree(added_map) == false)
    while(edges_added < (NUM_NODES-1))
    {
        // cout << endl << "MAP:" << endl;
        // print_map(added_map);
        // cout << "MST:" << endl;
        // print_tree(mst);
        // cout << "List:" << endl;
        // print_tree(list);

        //Get lowest edge
        lowest = get_lowest_edge(list, added_map);
        if(lowest.node1 == 0 || lowest.node2 == 0)
            cout << "ABOUT TO ADD A 0 HERE!!! (node returned from get_lowest)" << endl;
        
        //If node1 was already in the MST, add node2. Else add node2
        if(added_map[lowest.node1] == true)
            added_map[lowest.node2] = true;
        else
            added_map[lowest.node1] = true;
        mst.push_back(lowest);
        edges_added++;
        //for(size_t e = 0; e < list.size(); e++)
        for(int e = list.size()-1; e >= 0; e--)
        {
            if(list[e] == lowest)
            {
                list.erase(list.begin() + e);
                break;
            }
        }
    }
    // cout << endl << "MAP:" << endl;
    // print_map(added_map);
    return mst;
}
vector<Edge> kruskals(vector<Edge> list)
{
    
}
bool full_tree(unordered_map<int, bool> mst_map)
{
    for(auto& element: mst_map)
        if(element.second == false)
            return false;

    return true;
}
Edge get_lowest_edge(vector<Edge> list, unordered_map<int, bool> mst_map)
{
    int lowest_weight = INT_MAX;
    int index_of_lowest = -1;
    for(size_t d = 0; d < list.size(); d++)
    {
        //If one node of the current edges IS already added in the mst_map, the edge can be examined, otherwise it's not a candidate
        //XOR because only one can be connected already
        if(is_valid_addition(list[d], mst_map))
        {
            //If the edge's weight is lower than current lowest, make it the current lowest
            if(list[d].weight < lowest_weight)
            {
                lowest_weight = list[d].weight;
                index_of_lowest = d;
            }
        }
    }
    if(index_of_lowest == -1)
    {
        cout << "get_lowest_edge(). Index was -1 after algorithm finished" << endl;
        exit(1);
    }
    return list[index_of_lowest];
}
void print_map(unordered_map<int, bool> map)
{
    for(auto& element: map)
    {
        cout << "[" << element.first << ", " << element.second << "]" << endl;
    }
}
bool is_valid_addition(Edge new_edge, unordered_map<int, bool> mst_map)
{
    //If one end OR the other is in the map, it can be added
    if((mst_map[new_edge.node1] == true) ^ (mst_map[new_edge.node2] == true))
        return true;

    //If BOTH ends are in the map, it will create a cycle. If neither are in the map... (shouldn't be possible)
    return false;
}