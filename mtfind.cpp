#include <ostream>
#include <istream>
#include <fstream>
#include <string> 
#include <iostream>
using namespace std;
 
void KMP(string text, string pattern, int counter)
{
    int m = text.length();
    int n = pattern.length();
 
    if (n == 0)
    {
        cout << "The pattern occurs with shift 0";
        return;
    }
 
    if (m < n)
    {
        cout << "Pattern not found";
        return;
    }
 
    int next[n + 1];
 
    for (int i = 0; i < n + 1; i++) {
        next[i] = 0;
    }
 
    for (int i = 1; i < n; i++)
    {
        int j = next[i];
 
        while (j > 0 && pattern[j] != pattern[i]) {
            j = next[j];
        }
 
        if (j > 0 || pattern[j] == pattern[i]) {
            next[i + 1] = j + 1;
        }
    }
 
    for (int i = 0, j = 0; i < m; i++)
    {
        if (text[i] == pattern[j] || pattern[j] == '?')
        {
            if (++j == n) {
                cout << counter << " " << i - 1 << " " << pattern << endl;
            }
        }
        else if (j > 0)
        {
            j = next[j];
            i--;    
        }
    }
}
 
int main(int argc, char* argv [])
{
    std::cout << argc << std::endl;
    std::cout << argv[0] << std::endl;

    std::string line;
 
    std::ifstream in(argv[1]);

    if (in.is_open())
    {
        int counter = 1;

        while (std::getline(in, line))
        {
            KMP(line, argv[2], counter);
            counter++;
        //    std::cout << line << std::endl;
        }
    }
    
    in.close();
}
