//Ethan Sandstrom

#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <climits>         //INT_MAX
#include <chrono>          //Timing
#include <algorithm>       //Only for sorting vector in kruskals()

using namespace std;
int NUM_NODES = 0;
int COMPRESS = 0;

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
int print_edge(Edge node);
vector<Edge> prims(vector<Edge> list);
vector<Edge> kruskals(vector<Edge> list);
bool full_tree(unordered_map<int, bool> mst);
Edge get_lowest_edge(vector<Edge> list, unordered_map<int, bool>& mst_map);
void print_map(unordered_map<int, bool> map);
bool is_valid_addition(Edge new_edge, unordered_map<int, bool>& mst_map);   //Prim's
bool is_valid_addition_k(Edge new_edge, unordered_map<int, int>& mst_map); //Kruskal's
bool compare_weights(Edge left, Edge right);
int get_root(int key, unordered_map<int, int>& mst_map);
int get_root_pc(int key, unordered_map<int, int>& mst_map);   //Path compression version
void merge(Edge new_edge, unordered_map<int, int>& mst_map);

int main(int argc, char* argv[])
{
    int length = 0;
    string filename = "graphs/graph_v1600_e6400.txt";
    if(argc == 2)
    {
        filename = string(argv[1]);
    }

    vector<Edge> prims_list;
    read_file(prims_list, length, filename);
    vector<Edge> kruskals_list = prims_list;
    vector<Edge> kruskals_list_pc = prims_list; //Kruskal's with path compression

    chrono::time_point<chrono::system_clock> start;
	start = chrono::system_clock::now(); // Start the system clock.

    vector<Edge> prims_mst = prims(prims_list);                           //RUN Prims()

	chrono::time_point<chrono::system_clock> end;
	end = chrono::system_clock::now();
	chrono::duration<double> total_time = end - start;
    cout << "Prim's MST is" << endl;
    print_tree(prims_mst);
    cout << endl << endl << endl;
	cerr << "Prim's computation took " << total_time.count() << " seconds " << endl;
    

    start = chrono::system_clock::now(); // Start the system clock.

    vector<Edge> kruskals_mst = kruskals(kruskals_list);

	end = chrono::system_clock::now();
	total_time = end - start;
    cout << "Kruskal's MST is" << endl;
    print_tree(kruskals_mst);
    cerr << "Kruskal's computation took " << total_time.count() << " seconds " << endl;


    COMPRESS = 1;
    start = chrono::system_clock::now(); // Start the system clock.
    vector<Edge> kruskals_mst_pc = kruskals(kruskals_list_pc);

    end = chrono::system_clock::now();
	total_time = end - start;
    cerr << "Kruskal's computation with path compression took " << total_time.count() << " seconds " << endl << endl;


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
    Edge node;

    //Parsing an individual edge
    while(getline(Instream, currentline))
    {
        index = currentline.find(',');
        n1 = stoi(currentline.substr(0, index));
        currentline = currentline.substr(index+1);
        node.node1 = n1;

        index = currentline.find(',');
        n2 = stoi(currentline.substr(0, index));
        currentline = currentline.substr(index+1);
        node.node2 = n2;

        node.weight = stoi(currentline);
        list.push_back(node);
    }
}
void print_tree(vector<Edge> list)
{
    int sum = 0;
    size_t length = list.size();
    cout << length << endl;
    for(size_t b = 0; b < length; b++)
    {
        sum += print_edge(list[b]);
    }
    cout << "MST length: " << sum << endl;
}
int print_edge(Edge node)
{
    printf("%d,%d,%d\n", node.node1, node.node2, node.weight);
    return node.weight;
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


    //Step one: Pick a node to start with. Node 1
    added_map[list[0].node1] = true;  //Add it to the map of added nodes

    //While not every node is added to the tree, keep going.
    Edge lowest;
    int edges_added = 0;

    while(edges_added < (NUM_NODES-1))
    {
        //Get lowest edge
        lowest = get_lowest_edge(list, added_map);
        if((lowest.node1 == 0) || (lowest.node2 == 0))
        {
            cout << "ABOUT TO ADD UNCONNECTED EDGE! (node returned from get_lowest)" << endl;
            exit(1);
        }
        
        //If node1 was already in the MST, add node2. Else add node2
        if(added_map[lowest.node1] == true)
            added_map[lowest.node2] = true;
        else
            added_map[lowest.node1] = true;
        mst.push_back(lowest);
        edges_added++;

        for(int e = list.size()-1; e >= 0; e--)
        {
            if(list[e] == lowest)
            {
                list.erase(list.begin() + e);
                break;
            }
        }
    }

    return mst;
}
vector<Edge> kruskals(vector<Edge> list)
{
    vector<Edge> mst;
    unordered_map<int, int> set_map;

    Edge lowest;

    //Sort by edge weight
    sort(list.begin(), list.end(), compare_weights);

    int edges_added = 0;
    while(edges_added < NUM_NODES-1)
    {
        //Find first valid edge to add, which will be the lowest
        for(size_t f = 0; f < list.size(); f++)
        {
            //Determine validity of edge per constraints of Kruskal's
            if(is_valid_addition_k(list[f], set_map))
            {
                lowest = list[f];
                list.erase(list.begin()+f);
                break;
            }
        }
        //Update hash values according to Kruskal's
        merge(lowest, set_map);

        //Add new edge to mst
        mst.push_back(lowest);
        edges_added++;
    }

    return mst;
}
bool full_tree(unordered_map<int, bool> mst_map)
{
    for(auto& element: mst_map)
        if(element.second == false)
            return false;

    return true;
}
Edge get_lowest_edge(vector<Edge> list, unordered_map<int, bool>& mst_map)
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
bool is_valid_addition(Edge new_edge, unordered_map<int, bool>& mst_map)
{
    //If one end OR the other is in the map, it can be added
    if((mst_map[new_edge.node1] == true) ^ (mst_map[new_edge.node2] == true))
        return true;

    //If BOTH ends are in the map, it will create a cycle. If neither are in the map... (shouldn't be possible)
    return false;
}
bool compare_weights(Edge left, Edge right)
{
    return left.weight < right.weight;
}
bool is_valid_addition_k(Edge new_edge, unordered_map<int, int>& mst_map)
{
    //Initialize the map
    if(mst_map.find(new_edge.node1) == mst_map.end())
        mst_map[new_edge.node1] = -1;
    if(mst_map.find(new_edge.node2) == mst_map.end())
        mst_map[new_edge.node2] = -1;


    if(COMPRESS == 1)
    {
        //If node1 has the same root as node2, connecting them will make a cycle
        if(get_root_pc(new_edge.node1, mst_map) == get_root_pc(new_edge.node2, mst_map))
            return false;
    }
    else
    {
        //If node1 has the same root as node2, connecting them will make a cycle
        if(get_root(new_edge.node1, mst_map) == get_root(new_edge.node2, mst_map))
            return false;
    }
    return true;
}
int get_root(int key, unordered_map<int, int>& mst_map)
{
    if(mst_map[key] == -1)
    {
        //root case
        return key;
    }
    else
    {
        return get_root(mst_map[key], mst_map);
    }
}
void merge(Edge new_edge, unordered_map<int, int>& mst_map)
{
    int left_root, right_root;
    if(COMPRESS == 1)
    {
        left_root = get_root_pc(new_edge.node1, mst_map);
        right_root = get_root_pc(new_edge.node2, mst_map);
    }
    else
    {
        left_root = get_root(new_edge.node1, mst_map);
        right_root = get_root(new_edge.node2, mst_map);
    }
    if(left_root < right_root)
    {
        mst_map[right_root] = left_root;
    }
    else
    {
        mst_map[left_root] = right_root;
    }
}
int get_root_pc(int key, unordered_map<int, int>& mst_map)   //Path compression version
{
    if(mst_map[key] == -1)
    {
        //root case
        return key;
    }
    else
    {
        mst_map[key] = get_root_pc(mst_map[key], mst_map);
        return mst_map[key];
    }
}
