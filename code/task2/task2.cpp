#include <iostream>
#include <fstream>
#include "MAPFInstance.h"
#include "AStarPlanner.h"
#include <tuple>

int main(int argc, char *argv[]) {
    MAPFInstance ins;
    string input_file = argv[1];
    string output_file = argv[2];
    if (ins.load_instance(input_file)) {
        ins.print_instance();
    } else {
        cout << "Fail to load the instance " << input_file << endl;
        exit(-1);
    }

    AStarPlanner a_star(ins);
    vector<Path> paths(ins.num_of_agents);

    // assign priority ordering to agents
    // By default, we use the index ordering of the agents where
    // the first always has the highest priority.
    vector<int> priorities;
    for (int i = 0; i < ins.num_of_agents; i++) {
        priorities.push_back(i);
    }

    // plan paths
    for (int i = 0; i < priorities.size(); i++) {
        // TODO: Transform already planned paths into constraints
        list<Constraint> constraints;
        if (i > 0){
            for (int j = 0; j < i; j++){
                for (int index = 0; index < paths[priorities[j]].size(); index++){
                    constraints.push_back(make_tuple(i, paths[priorities[j]][index], -1, index));
                    
                }
                for (int index = 0; index < paths[priorities[j]].size() - 1; index++){
                    constraints.push_back(make_tuple(i, paths[priorities[j]][index + 1], paths[priorities[j]][index], index + 1));
                }
                constraints.push_back(make_tuple(i, paths[priorities[j]][paths[priorities[j]].size() - 1], -2, paths[priorities[j]].size() - 1));
            }
        }
        //  Replace the following line with something like paths[i] = a_star.find_path(i, constraints);
        paths[i] = a_star.find_path(i, constraints);

        if (paths[i].empty()) {
            cout << "Fail to find any solutions for agent " << i << endl;
            return 0;
        }
    }

    // print paths
    cout << "Paths:" << endl;
    int sum = 0;
    for (int i = 0; i < ins.num_of_agents; i++) {
        cout << "a" << i << ": " << paths[i] << endl;
        sum += paths[i].size() - 1;
    }
    cout << "Sum of cost: " << sum << endl;

    // save paths
    ofstream myfile (output_file.c_str(), ios_base::out);
    if (myfile.is_open()) {
        for (int i = 0; i < ins.num_of_agents; i++) {
            myfile << paths[i] << endl;
        }
        myfile.close();
    } else {
        cout << "Fail to save the paths to " << output_file << endl;
        exit(-1);
    }
    return 0;
}
