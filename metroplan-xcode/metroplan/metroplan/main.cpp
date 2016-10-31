#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <utility>
#include <algorithm>
#include <cassert>
#include <queue>
#include <stack>
#include <string.h>
#include <limits>
#include <set>

using namespace std;

typedef string Station;
typedef string Line;
typedef pair<Station, Line> Stop;

string pairToString(const pair<string, string>& p) {
    stringstream buffer;
    buffer << "(" << p.first << ", " << p.second << ")";
    return move(buffer.str());
}

struct Edge {
    Stop from;
    Stop to;
    double timeTaken;
public:
    Edge(const Stop& from, const Stop& to, double timeTaken)
    :from(from), to(to), timeTaken(timeTaken) {}
    
    Edge swap() const{
        return Edge(to, from, timeTaken);
    }
    
    string toString() const {
        stringstream buffer;
        buffer << pairToString(this->from) << " -> " << pairToString(this->to) << " = " << this->timeTaken;
        return move(buffer.str());
    }
    
    bool operator<(const Edge& other) const {
        return this->toString().compare(other.toString()) < 0;
    }
};

class TransitGraph {
private:
    typedef multimap<Stop, shared_ptr<Edge>> EdgeMap;
    EdgeMap edges;
    
    map<Station, shared_ptr<set<Stop>>> entryPoints;
    
    void addEntryPoint(const Stop& e) {
        if (entryPoints.count(e.first) == 0) {
            entryPoints[e.first] = make_shared<set<Stop>>();
        }
        
        entryPoints[e.first]->insert(e);
    }
public:
    typedef vector<shared_ptr<Edge>> EdgeList;
    shared_ptr<set<Stop>> getEntryPoints(const Station& s) {
        if (entryPoints.count(s) == 0) {
            return make_shared<set<Stop>>();
        }
        
        return entryPoints[s];
    }
    
    void assoc(const Stop& s1, const Stop& s2, double timeTaken) {
        auto value = make_shared<Edge>(s1, s2, timeTaken);
        cout << value->toString() << endl;
        addEntryPoint(s1);
        addEntryPoint(s2);
        edges.insert(EdgeMap::value_type(s1, value));
    }
    
    EdgeList adj(const Station& s) {
        EdgeList result;
        
        if (entryPoints.count(s) > 0) {
            shared_ptr<set<Stop>> stops = entryPoints[s];
            for(auto it = stops->begin(); it != stops->end(); ++it) {
                EdgeList current = adj(*it);
                result.insert(result.end(), current.begin(), current.end());
            }
        }
        
        return move(result);
    }
    
    EdgeList adj(const Stop& s) {
        EdgeList result;
        auto range = edges.equal_range(s);
        for (auto it = range.first; it != range.second; ++it) {
            result.push_back(it->second);
        }
        return move(result);
    }
};

class StationPath {
    vector<Edge> path;
    double time;
public:
    StationPath(const vector<Edge>& calcPath): path(calcPath) {
        double time = 0;
        for_each(path.begin(), path.end(), [&time](auto e) {
            time += e.timeTaken;
        });
        this->time = time;
    }
    double getTotalTime() { return time; }
};

class BFS {
    typedef multimap<Stop, shared_ptr<Edge>> EdgeToMap;
    
    TransitGraph g;
    Station start;
    
public:
    BFS(const TransitGraph& g, const Station& start): g(g), start(start) {}
    
    void process(shared_ptr<Edge> e, queue<Stop>& q, EdgeToMap& edgeTo, set<Edge>& visited) {
        q.push(e->to);
        visited.insert(*e);
        edgeTo.insert(EdgeToMap::value_type(e->to, e));
        cout << "EdgeTo: " << edgeTo.size() << endl;
    }
    
    // usings BFS, find all paths to the destination
    vector<StationPath> pathsTo(const Station& dest, int maxLineSwitches) {
        EdgeToMap edgeTo;
        set<Edge> visited;
        queue<Stop> q;
        
        TransitGraph::EdgeList adj = g.adj(start);
        for(auto e : adj) {
            cout << "Seeding: " << e->toString() << endl;
            q.push(e->from);
        }
        int iter = 0;
        
        while(!q.empty()) {
            iter++;
            Stop current = q.front();
            q.pop();
            
            TransitGraph::EdgeList adj = g.adj(current);
            cout << "----------------" << pairToString(current) << "----------------" << endl;
            for(auto e : adj) {
                cout << e->toString();
                if (visited.count(*e) == 0) {
                    cout << ", Processed" << endl;
                    process(e, q, edgeTo, visited);
                }
                else {
                    cout << ", Skipped" << endl;
                }
            }
        }
        
        cout << "Solved?" << endl;
        // once paths are established, use dfs to build actual paths
        return printSolutions(edgeTo, dest, maxLineSwitches);
    }
private:
    vector<StationPath> printSolutions(const EdgeToMap& edgeTo, const Station& dest, int maxLineSwitches) {
        vector<vector<Edge>> result;
        
        auto entryPoints = g.getEntryPoints(dest);
        set<Edge> visited;
        
        for(auto it : *entryPoints) {
            solutionDfs(edgeTo, it, it, maxLineSwitches, visited, move(vector<Edge>()));
        }
        
        return vector<StationPath>();
    }
    
    void printAcc(const Stop& node, const vector<Edge>& acc) {
        cout << "Node " << pairToString(node) << ": " << endl;
        for_each(acc.begin(), acc.end(), [](auto e) {
            cout << "\t" << e.swap().toString() << endl;
        });
        cout << endl;
    }
    
    void solutionDfs(const EdgeToMap& edgeTo, const Stop& goal, const Stop& node, int maxLineSwitches, set<Edge>& visited, vector<Edge> acc) {
        auto range = edgeTo.equal_range(node);
        
        int count = 0;
        
        for (auto it = range.first; it != range.second; ++it) {
            shared_ptr<Edge> current = it->second;
            if (visited.count(*current) == 0) {
                count++;
                visited.insert(*current);
                acc.push_back(*current);
                solutionDfs(edgeTo, goal, current->from, maxLineSwitches, visited, acc);
            }
        }
        
        if (count == 0) {
            printAcc(goal, acc);
        }
    }
};

int main(int argc, char** argv) {
    TransitGraph g;
    cout << "Loading map from " << argv[1] << endl;
    
    string line;
    ifstream infile(argv[1]);
    const char* delim = string(" ,\n").c_str();
    while(getline(infile, line)) {
        char* cstr = new char[line.length()+1];
        strcpy(cstr, line.c_str());
        char* pch = strtok(cstr, delim);
        if (pch != NULL) {
            string s1(pch);
            string l1(strtok(NULL, delim));
            string s2(strtok(NULL, delim));
            string l2(strtok(NULL, delim));
            double timeTaken = atof(strtok(NULL, delim));
            
            g.assoc(Stop(s1, l1), Stop(s2, l2), timeTaken);
            g.assoc(Stop(s2, l2), Stop(s1, l1), timeTaken);
        }
    }
    
    vector<string> trip;
    for(auto i = 0; i < 2; ++i) {
        cout << "Enter stop " << i << ": ";
        if(!std::getline(std::cin, line)) {
            return 1;
        }
        trip.push_back(line);
    }
    
    cout << "Trip: " << trip[0] << " -> " << trip[1] << endl;
    
    BFS bfs(g, trip[0]);
    bfs.pathsTo(trip[1], 1);
    
    return 0;
}
