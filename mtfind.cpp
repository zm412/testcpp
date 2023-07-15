#include <ostream>
#include <istream>
#include <fstream>
#include <string> 
#include <iostream>

using namespace std;

int *pre_kmp(string pattern, string ticket)
{
	int size = pattern.size();
	int *pie=new int [size];
	pie[0] = 0;
	int k=0;
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

void KMP(string text,string pattern, string ticket)
{
	int* pie=pre_kmp(pattern, ticket);
	int matched_pos=0;
	for(int current=0; current< text.length(); current++)
	{
		while (matched_pos > 0 && pattern[matched_pos] != text[current] && pattern[matched_pos] != ticket[0] )
			matched_pos = pie[matched_pos-1];
			
		if(pattern[matched_pos] == text[current] || pattern[matched_pos] == ticket[0])
			matched_pos = matched_pos+1;
			
		if( matched_pos == (pattern.length()) )
		{
            
			cout<<"Pattern occurs with shift "<< current - (pattern.length() -1 ) << " " << text.substr(current - (pattern.length() -1 ), pattern.length())  << "\n";
			matched_pos = pie[matched_pos-1];
		}
	}
}

int main(int argc, char* argv [])
{
    //std::cout << argc << std::endl;
    //std::cout << argv[0] << std::endl;

    std::string line;
    std::string pattern;
    std::string ticket;
    ticket = "?";
    pattern = argv[2];

    std::ifstream in(argv[1]);

    if (in.is_open())
    {
        int counter = 1;

        while (std::getline(in, line))
        {
            KMP(line, pattern, ticket);
            counter++;
            //std::cout << line << std::endl;
        }
    }

    in.close();
}

