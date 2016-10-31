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

    string toString() const {
        stringstream buffer;
        buffer << pairToString(this->from) << " -> " << pairToString(this->to) << " = " << this->timeTaken;
        return move(buffer.str());
    }
    operator string() const {
        return move(toString());
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

    // usings BFS, find all paths to the destination
    vector<StationPath> pathsTo(const Station& dest, int maxLineSwitches) {
        EdgeToMap edgeTo;
        set<string> visited;
        queue<Edge> q;

        TransitGraph::EdgeList adj = g.adj(start);
        for(auto e : adj) {
            cout << "Seeding: " << e->toString() << endl;
            q.push(*e);
            visited.insert(e->toString());
            edgeTo.insert(EdgeToMap::value_type(e->to, e));
        }

        while(!q.empty()) {
            Edge current = q.front();
            q.pop();

            TransitGraph::EdgeList adj = g.adj(current.to);
            for(auto e : adj) {
                cout << e->toString();
                if (visited.count(e->toString()) == 0) {
                    cout << " processed" << endl;
                    visited.insert(e->toString());
                    q.push(*e);
                    edgeTo.insert(EdgeToMap::value_type(e->to, e));
                }
                else {
                    cout << " skipped" << endl;
                }
            }
        }
        // once paths are established, use dfs to build actual paths
        return printSolutions(edgeTo, dest, maxLineSwitches);
    }
private:
    vector<StationPath> printSolutions(const EdgeToMap& edgeTo, const Station& dest, int maxLineSwitches) {
        vector<vector<Edge>> result;

        auto entryPoints = g.getEntryPoints(dest);

        vector<Edge> acc;
        for(auto it : *entryPoints) {
            dfs(edgeTo, it, maxLineSwitches, acc);
        }

        return vector<StationPath>();
    }

    void printAcc(const Stop& node, const vector<Edge>& acc) {
        cout << "Node " << pairToString(node) << ": ";
        for_each(acc.begin(), acc.end(), [](auto e) {
            cout << (string)e << ", ";
        });
        cout << endl;
    }
    
    void dfs(const EdgeToMap& edgeTo, const Stop& node, int maxLineSwitches, vector<Edge> acc) {
        if (edgeTo.count(node) == 0) {
            return;
        }

        if (maxLineSwitches < 0) {
            return;
        }

        printAcc(node, acc);

        auto range = edgeTo.equal_range(node);
        for (auto it = range.first; it != range.second; ++it) {
            acc.push_back(*it->second);
            dfs(edgeTo, it->second->from, maxLineSwitches, acc);
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
    bfs.pathsTo(trip[1], 2);

    return 0;
}