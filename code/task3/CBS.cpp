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
        root->paths[i] = a_star.find_path(i, list<Constraint>());
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
        CBSNode* curr = open.top();
        open.pop();
        list<Constraint> constraints = find_collision(curr->paths);
        if (constraints.size() == 0){
            return curr->paths;
        }
        for (Constraint c : constraints){
            CBSNode* child = new CBSNode(*curr);
            child->constraints.push_back(c);
            int agent = get<0>(c);
            Path path = a_star.find_path(agent, child->constraints);
            if (!path.empty()){
                child->paths[agent] = path;
                for (const auto& p : child->paths)
                    child->cost += p.size();
                all_nodes.push_back(child);
                open.push(child);
            }
        }
    }

    return vector<Path>(); // return "No solution"
}

list<Constraint> CBS::find_collision(vector<Path> paths){
    list<Constraint> result;
    for (int i = 0; i < paths.size(); i++){
        for (int j = i + 1; j < paths.size(); j++){
            int totalTime = std::min(paths[i].size(), paths[j].size());
            int l = -1;
            int s = -1;
            if (totalTime == paths[i].size()){
                l = j;
                s = i;
            }
            else{
                l = i;
                s = j;
            }
            for (int t = 0; t < totalTime; t++){
                if (paths[i][t] == paths[j][t]){
                    // vertex constraint
                    result.push_back(make_tuple(i, paths[i][t], -1, t));
                    result.push_back(make_tuple(j, paths[j][t], -1, t));
                    i = paths.size();
                    j = paths.size();
                    break;
                }
                if (t > 0){
                    if (paths[i][t] == paths[j][t - 1] && paths[i][t - 1] == paths[j][t]){
                        // edge constraint
                        result.push_back(make_tuple(i, paths[i][t - 1], paths[i][t], t));
                        result.push_back(make_tuple(j, paths[j][t - 1], paths[j][t], t));
                        i = paths.size();
                        j = paths.size();
                        break;
                    }
                }
            }
            for (int t = totalTime; t < paths[l].size(); t++){
                if (paths[l][t] == paths[s][totalTime - 1]){
                    result.push_back(make_tuple(l, paths[s][totalTime - 1], -2, totalTime - 1));
                    result.push_back(make_tuple(s, paths[l][t], -1, t));
                    i = paths.size();
                    j = paths.size();
                    break;
                }
            }
        }
    }
    return result;
}


CBS::~CBS() {
    // release the memory
    for (auto n : all_nodes)
        delete n;
}
