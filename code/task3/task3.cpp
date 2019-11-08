#include <iostream>
#include <fstream>
#include "MAPFInstance.h"
#include "CBS.h"
#include <tuple>

int main(int argc, char *argv[]) {
    ofstream myOut("final.txt");
    for (int fileNumber = 35; fileNumber < 51; fileNumber++){
        cout << "Printing " << fileNumber << endl;
        if (fileNumber == 10 || fileNumber == 16 || fileNumber == 17 || fileNumber == 21) continue;
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
            myOut << 0 << ", ";
        }

        // print paths
        cout << "Paths:" << endl;
        int sum = 0;
        for (int i = 0; i < ins.num_of_agents; i++) {
            cout << "a" << i << ": " << paths[i] << endl;
            sum += paths[i].size();
        }
        cout << "Sum of cost: " << sum << endl;
        myOut << sum << ", ";
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
    }
    
    return 0;
}