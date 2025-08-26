/**
 * Driver (main) program for Priority RR scheduling algorithm.
 * The input file is a text file containing the process information in the following format:
 * [name] [priority] [CPU burst]
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include "scheduler_priority_rr.h"

using namespace std;

/**
 * Main function - orchestrates the Priority RR scheduling simulation
 */
int main(int argc, char *argv[]) {
    // Print program header
    cout << "CS 433 Programming assignment 3" << endl;
    cout << "Author: xxxxxx and xxxxxxx" << endl;     // TODO: add your name
    cout << "Date: xx/xx/20xx" << endl;               // TODO: add date
    cout << "Course: CS433 (Operating Systems)" << endl;
    cout << "Description: test Priority RR scheduling algorithm" << endl;
    cout << "=================================" << endl;
    
    // Validate command line arguments
    if (argc < 3) {
        cerr << "Error: Insufficient arguments provided." << endl;
        cerr << "Usage: " << argv[0] << " <input_file> <time_quantum>" << endl;
        cerr << "  <input_file>: Path to the file containing process information" << endl;
        cerr << "  <time_quantum>: Time quantum for the Round Robin algorithm" << endl;
        exit(EXIT_FAILURE);
    }
    
    // Parse and validate time quantum
    int timeQuantum;
    try {
        timeQuantum = stoi(argv[2]);
        if (timeQuantum <= 0) {
            cerr << "Error: Time quantum must be a positive integer." << endl;
            exit(EXIT_FAILURE);
        }
    } catch (const invalid_argument& e) {
        cerr << "Error: Invalid time quantum. Must be a valid integer." << endl;
        exit(EXIT_FAILURE);
    }
    
    // Read processes from input file
    ifstream inputFile(argv[1]);
    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open file '" << argv[1] << "'" << endl;
        exit(EXIT_FAILURE);
    }
    
    vector<PCB> processes;
    string line;
    int processId = 0;
    int lineNumber = 0;
    
    cout << "Reading processes from file: " << argv[1] << endl;
    cout << "----------------------------------------" << endl;
    
    while (getline(inputFile, line)) {
        lineNumber++;
        
        // Skip empty lines
        if (line.empty()) {
            continue;
        }
        
        try {
            // Parse process line
            istringstream lineStream(line);
            string token;
            
            // Parse process name (first field)
            if (!getline(lineStream, token, ',')) {
                throw runtime_error("Missing process name");
            }
            string processName = token;
            
            // Parse priority (second field)
            if (!getline(lineStream, token, ',')) {
                throw runtime_error("Missing priority value");
            }
            unsigned int priority = stoi(token);
            
            // Parse burst time (third field)
            if (!getline(lineStream, token, ',')) {
                throw runtime_error("Missing burst time value");
            }
            unsigned int burstTime = stoi(token);
            
            // Check for extra fields
            if (getline(lineStream, token, ',')) {
                throw runtime_error("Too many fields in process line");
            }
            
            // Create PCB and add to process list
            PCB process(processName, processId, priority, burstTime);
            process.print();
            processes.push_back(process);
            processId++;
            
        } catch (const exception& e) {
            cerr << "Error parsing line " << lineNumber << ": " << e.what() << endl;
            cerr << "Line content: " << line << endl;
            exit(EXIT_FAILURE);
        }
    }
    
    inputFile.close();
    
    if (processes.empty()) {
        cerr << "Error: No valid processes found in the input file." << endl;
        exit(EXIT_FAILURE);
    }
    
    cout << "Successfully loaded " << processes.size() << " processes." << endl;
    cout << "----------------------------------------" << endl;
    
    // Run the scheduler simulation
    cout << "Starting Priority RR scheduling simulation..." << endl;
    cout << "Time quantum: " << timeQuantum << endl;
    cout << "=================================" << endl;
    
    SchedulerPriorityRR scheduler(timeQuantum);
    scheduler.init(processes);
    scheduler.simulate();
    scheduler.print_results();
    
    return 0;
}