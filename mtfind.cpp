#include <iostream>
#include <string> 
#include <fstream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>

using namespace std;

struct resultData {
    unsigned long string_number;
    unsigned short position;
    string match;
};

vector<resultData> result;
atomic<unsigned short> quantity{0}; 
mutex result_mutex;

vector<resultData> greedySearch(const string& text, const string& pattern, unsigned long counter) {
    vector<resultData> results;
    int pattern_length = pattern.size();
    int text_length = text.size();

    for (int i = 0; i <= text_length - pattern_length; i++) {
        bool match = true;
        for (int j = 0; j < pattern_length; j++) {
            if (pattern[j] != '?' && pattern[j] != text[i + j]) {
                match = false;
                break;
            }
        }
        if (match) {
            resultData res = {counter, static_cast<unsigned short>(i + 1), text.substr(i, pattern_length)};
            results.push_back(res);
            quantity++;
        }
    }

    return results;
}

void process_chunk(vector<string> chunk, const string& pattern, unsigned long start_line) {
    for(unsigned long i = 0; i < chunk.size(); i++) {
        vector<resultData> currentResult = greedySearch(chunk[i], pattern, start_line + i);
        
        lock_guard<mutex> lock(result_mutex);
        result.insert(result.end(), currentResult.begin(), currentResult.end());
    }
}

int main(int argc, char* argv []) {
    if(argc < 3) {
        return 1;
    }

    string filename = argv[1];
    string pattern = argv[2];

    ifstream in(filename);
    if(!in.is_open()) {
        cout << "Cannot open file" << endl;
        return 1;
    }

    unsigned long counter = 0;
    string line;
    vector<thread> threads;
    vector<string> chunk;
    const unsigned int lines_per_thread = 2;
 
    while(getline(in, line)) {
        chunk.push_back(line);
        counter++;
        
        if(counter % lines_per_thread == 0) {
            threads.push_back(thread(process_chunk, chunk, pattern, counter - lines_per_thread));
            chunk.clear();
        }
    }

    if(!chunk.empty()) {
        threads.push_back(thread(process_chunk, chunk, pattern, counter - chunk.size()));
    }

    in.close();

    for(auto& t : threads) {
        t.join();
    }

    for(const resultData& res : result) {
        cout << "String number: " << res.string_number << ", Position: " << res.position << ", Match: " << res.match << endl;
    }

    cout << "Total matches: " << quantity << endl;

    return 0;
}
