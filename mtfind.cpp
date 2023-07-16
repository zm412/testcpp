#include <ostream>
#include <istream>
#include <fstream>
#include <string> 
#include <iostream>

using namespace std;

int *pre_kmp(string pattern, string ticket)
{
	int size = pattern.size();

    for(int i=0; i<size; i++){
        char t = pattern[i];
        cout << t << " TTTT" << endl;
    }
    if(size > 1000){
        cout << "Pattern is too long" << endl;
        return 0;
    }

	int *pie=new int [size];
	pie[0] = 0;
	int k=0;
    char signalS { '\n' };
    cout << size << "size" << endl;
    cout << pattern.find(signalS) << "signal" << endl;
    if (pattern.find(signalS) < size)
    {
        cout << "ERROR" << endl;
        exit(0);
    }

	for(int i=1;i<size;i++)
	{
		while(k>0 && pattern[k] != pattern[i])
		{
			k=pie[k-1];
		} 
        if( pattern[k] == pattern[i])
		{
			k=k+1;
		}
		pie[i]=k;
	}

	return pie;
}

void KMP(string text,string pattern, string ticket, int counter)
{
    cout << pattern << endl;
    static int countIn = 0;
	int* pie=pre_kmp(pattern, ticket);
	int matched_pos=0;

	for(int current=0; current< text.length(); current++)
	{
		while (matched_pos > 0 && pattern[matched_pos] != text[current] && pattern[matched_pos] != ticket[0] )
        {
			matched_pos = pie[matched_pos-1];
        }
			
		if(pattern[matched_pos] == text[current] || pattern[matched_pos] == ticket[0])
        {
			matched_pos = matched_pos+1;
        }
			
		if( matched_pos == (pattern.length()) )
		{
            //куррент 6 (от нуля), минус (длина паттерна - 1) (получаем символ, на котором начинается совпадение). Вычитаем 1 (дабы получить позицию с 1)
			cout << "quantity: " << ++countIn <<" "<< counter << " " << current - (pattern.length() -1 ) + 1 << " " << text.substr(current - (pattern.length() -1 ), pattern.length())  << endl;
			matched_pos = pie[0];
		}
	}
}

int main(int argc, char* argv [])
{
    //std::cout << argc << std::endl;
    //std::cout << argv[0] << std::endl;

    string line;
    string pattern;
    string ticket;
    ticket = "?";
    pattern = argv[2];
    ifstream in(argv[1]);

    if (in.is_open())
    {
        int counter = 1;

        while (getline(in, line))
        {
            KMP(line, pattern, ticket, counter);
            counter++;
            //std::cout << line << std::endl;
        }
    }

    in.close();
}

