#include <ostream>
#include <istream>
#include <fstream>
#include <string> 
#include <iostream>

using namespace std;

int *pre_kmp(string pattern) {
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

void KMP(string text, string pattern, int counter) {
    static int countIn = 0;
    char ticket {'?'};
	int* pie=pre_kmp(pattern);
	int matched_pos = 0;
	int size = pattern.size();

	for(int current = 0; current < text.length(); current++) {
		while (matched_pos > 0 && pattern[matched_pos] != text[current] && pattern[matched_pos] != ticket) {
			matched_pos = pie[matched_pos - 1];
        }
			
		if(pattern[matched_pos] == text[current] || pattern[matched_pos] == ticket) {
			matched_pos = matched_pos+1;
        }
			
		if( matched_pos == size ) {
            //куррент 6 (от нуля), минус (длина паттерна - 1) (получаем символ, на котором начинается совпадение). Добавляем 1 (дабы получить позицию с 1)
            int position = current - (size -1 ) + 1; 
            string subStr = text.substr(current - (size - 1), size);
			cout << "quantity: " << ++countIn << " " << counter << " " << position << " " << subStr << endl;
			matched_pos = pie[0];
		}
	}
}

int main(int argc, char* argv []) {
    string line;
    string pattern;
    pattern = argv[2];
    ifstream in(argv[1]);

    if (in.is_open()) {
        int counter = 1;

        while (getline(in, line)) {
            KMP(line, pattern, counter);
            counter++;
        }
    }

    in.close();
}
