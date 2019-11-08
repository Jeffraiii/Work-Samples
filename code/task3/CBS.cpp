#include "CBS.h"
#include <iostream>
#include <queue>

vector<Path> CBS::find_solution() {
    priority_queue<CBSNode*, vector<CBSNode*>, CompareCBSNode> open; // open list

    /* generate the root CBS node */
    auto root = new CBSNode();
    all_nodes.push_back(root);  // whenever generating a new node, we need to
                                 // put it into all_nodes
                                 // so that we can release the memory properly later in ~CBS()

    // find paths for the root node
    root->paths.resize(a_star.ins.num_of_agents);
    for (int i = 0; i < a_star.ins.num_of_agents; i++) {
        // TODO: if you change the input format of function find_path()
        //  you also need to change the following line to something like
        //  root->paths[i] = a_star.find_path(i, list<Constraint>());
        root->paths[i] = a_star.find_path(i);
        if (root->paths[i].empty()) {
            cout << "Fail to find a path for agent " << i << endl;
            return vector<Path>(); // return "No solution"
        }
    }
    // compute the cost of the root node
    for (const auto& path : root->paths)
        root->cost += path.size();

    // put the root node into open list
    open.push(root);

    while (!open.empty()) {
        // TODO: implement the high-level of CBS
    }

    return vector<Path>(); // return "No solution"
}

void CBS::find_collision(vector<Path> paths){
    for (int i = 0; i < paths.size(); i++){
        for (int j = i + 1; j < paths.size(); i++){
            int totalTime = std::min(paths[i].size(), paths[j].size());
            for (int t = 0; t < totalTime; t++){
                if (paths[i][t] == paths[j][t]){
                    // vertex constraint
                }
                if (t > 0){
                    if (paths[i][t] == paths[j][t - 1] && paths[i][t - 1] == paths[j][t]){
                        // edge constraint
                    }
                }
            }
        }
    }
}


CBS::~CBS() {
    // release the memory
    for (auto n : all_nodes)
        delete n;
}