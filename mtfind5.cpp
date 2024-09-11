#include <ostream>
#include <istream>
#include <fstream>
#include <string> 
#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>
#include <vector>
#include <map>
#include <future>
#include <algorithm>

using namespace std;

struct resultData {
    unsigned long string_number;
    unsigned short position;
    string match;
};

vector<resultData> result;
atomic<unsigned short> quantity{0}; 
mutex result_mutex;

int *pre_kmp(string pattern) {
    cout << pattern << "LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLl" << endl;

	int size = pattern.size();

    /*
    for(int i=0; i<size; i++){
        char t = pattern[i];
        cout << t << " TTTT" << endl;
    }
    */

    if(size > 1000){
        cout << "Pattern is too long" << endl;
        exit(0);
    }

	int *pie = new int [size];
	pie[0] = 0;
	int k = 0;
    char signalS { '\n' };

    if (pattern.find(signalS) < size) {
        cout << "ERROR" << endl;
        exit(0);
    }

	for(int i = 1; i < size; i++) {
		while(k>0 && pattern[k] != pattern[i]) {
			k = pie[k-1];
		} 

        if(pattern[k] == pattern[i]) {
			k = k+1;
		}

		pie[i] = k;
	}

	return pie;
}

vector<resultData> KMP(string text, string pattern, unsigned long counter, int* pie) {
    cout << text << "line" << endl;
    int matched_pos = 0;
    int size = pattern.size();
    vector<resultData> currentResult;

    for(int current = 0; current < text.length(); current++) {
        cout << *pie << "pattern" << endl;
        while (matched_pos > 0 && pattern[matched_pos] != text[current]) {
            matched_pos = pie[matched_pos - 1];
        }
        
        if(pattern[matched_pos] == text[current]) {
            matched_pos++;
        }
        
        if(matched_pos == size) {
            unsigned short position = current - (size - 1) + 1; 
            string subStr = text.substr(current - (size - 1), size);
            resultData temp{ counter, position, subStr };
            currentResult.push_back(temp);
            quantity++;
            matched_pos = pie[0];
        }
    }

    return currentResult;
}

void process_chunk(vector<string> chunk, string pattern, unsigned long start_line) {
    int* pie = pre_kmp(pattern);

    for(unsigned long i = 0; i < chunk.size(); i++) {
        cout << chunk[i] << "CHANK" << endl;
        vector<resultData> currentResult = KMP(chunk[i], pattern, start_line + i, pie);
        
        lock_guard<mutex> lock(result_mutex);
        result.insert(result.end(), currentResult.begin(), currentResult.end());
    }

    delete[] pie;
}

int main(int argc, char* argv []) {
    if(argc < 3) {
        cout << "Usage: ./program <filename> <pattern>" << endl;
        return 1;
    }

    string filename = argv[1];
    string pattern = argv[2];

    ifstream in(filename);
    if(!in.is_open()) {
        cout << "Cannot open file." << endl;
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
