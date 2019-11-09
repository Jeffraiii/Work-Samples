#include <iostream>
#include <fstream>
#include "MAPFInstance.h"
#include "CBS.h"
#include <tuple>
#include <chrono>

int main(int argc, char *argv[]) {
    ofstream finalPathCBS("final_paths_CBS.txt");
    ofstream finalTimeCBS("final_times_CBS.txt");
    for (int fileNumber = 1; fileNumber < 51; fileNumber++){
        auto begin = chrono::high_resolution_clock::now();    
        cout << "Printing CBS " << fileNumber << endl;
        // if (fileNumber == 10 || fileNumber == 16 || fileNumber == 17 || fileNumber == 21) continue;
        MAPFInstance ins;
        string input_file = "exp3_" + to_string(fileNumber) + ".txt";
        string output_file = "exp3_" + to_string(fileNumber) + "_paths.txt";
        if (ins.load_instance(input_file)) {
            ins.print_instance();
        } else {
            cout << "Fail to load the instance " << input_file << endl;
            exit(-1);
        }

        CBS cbs(ins);
        vector<Path> paths = cbs.find_solution();
        if (paths.empty()) { // Fail to find solutions
            cout << "No solutions!" << endl;
            finalPathCBS << -1 << ",";
            auto end = chrono::high_resolution_clock::now();    
            auto dur = end - begin;
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
            finalTimeCBS << ms << ",";
            continue;
        }

        // print paths
        cout << "Paths:" << endl;
        int sum = 0;
        for (int i = 0; i < ins.num_of_agents; i++) {
            cout << "a" << i << ": " << paths[i] << endl;
            sum += paths[i].size() - 1;
        }
        cout << "Sum of cost: " << sum << endl;
        finalPathCBS << sum << ",";
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
        auto end = chrono::high_resolution_clock::now();    
        auto dur = end - begin;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        finalTimeCBS << ms << ",";
    }
    finalPathCBS.close();
    finalTimeCBS.close();

    ofstream finalPathPP("final_paths_PP.txt");
    ofstream finalTimePP("final_times_PP.txt");
    for (int fileNumber = 1; fileNumber < 51; fileNumber++){
        auto begin = chrono::high_resolution_clock::now();    
        bool skip = false;
        cout << "Printing PP " << fileNumber << endl;
        MAPFInstance ins;
        string input_file = "exp3_" + to_string(fileNumber) + ".txt";
        string output_file = "exp3_" + to_string(fileNumber) + "_paths.txt";
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
                finalPathPP << -1 << ",";
                skip = true;
                auto end = chrono::high_resolution_clock::now();    
                auto dur = end - begin;
                auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
                finalTimePP << ms << ",";
                break;
            }
        }

        if (skip){
            continue;
        }

        // print paths
        cout << "Paths:" << endl;
        int sum = 0;
        for (int i = 0; i < ins.num_of_agents; i++) {
            cout << "a" << i << ": " << paths[i] << endl;
            sum += paths[i].size() - 1;
        }
        finalPathPP << sum << ",";
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
        auto end = chrono::high_resolution_clock::now();    
        auto dur = end - begin;
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        finalTimePP << ms << ",";
    }
    finalPathPP.close();
    finalTimePP.close();
    
    return 0;
}