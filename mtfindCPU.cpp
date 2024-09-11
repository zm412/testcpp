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

#if defined(_WIN32)
#include <Windows.h>

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
#include <unistd.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <time.h>

#else
#error "Unable to define getCPUTime( ) for an unknown OS."
#endif

double getCPUTime( )
{
#if defined(_WIN32)
    FILETIME createTime;
    FILETIME exitTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if ( GetProcessTimes( GetCurrentProcess( ),
        &createTime, &exitTime, &kernelTime, &userTime ) != -1 )
    {
        SYSTEMTIME userSystemTime;
        if ( FileTimeToSystemTime( &userTime, &userSystemTime ) != -1 )
            return (double)userSystemTime.wHour * 3600.0 +
                (double)userSystemTime.wMinute * 60.0 +
                (double)userSystemTime.wSecond +
                (double)userSystemTime.wMilliseconds / 1000.0;
    }

#elif defined(__unix__) || defined(__unix) || defined(unix) || (defined(__APPLE__) && defined(__MACH__))
    /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, and Solaris --------- */

#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
    /* Prefer high-res POSIX timers, when available. */
    {
        clockid_t id;
        struct timespec ts;
#if _POSIX_CPUTIME > 0
        /* Clock ids vary by OS.  Query the id, if possible. */
        if ( clock_getcpuclockid( 0, &id ) == -1 )
#endif
#if defined(CLOCK_PROCESS_CPUTIME_ID)
            /* Use known clock id for AIX, Linux, or Solaris. */
            id = CLOCK_PROCESS_CPUTIME_ID;
#elif defined(CLOCK_VIRTUAL)
            /* Use known clock id for BSD or HP-UX. */
            id = CLOCK_VIRTUAL;
#else
            id = (clockid_t)-1;
#endif
        if ( id != (clockid_t)-1 && clock_gettime( id, &ts ) != -1 )
            return (double)ts.tv_sec +
                (double)ts.tv_nsec / 1000000000.0;
    }
#endif

#if defined(RUSAGE_SELF)
    {
        struct rusage rusage;
        if ( getrusage( RUSAGE_SELF, &rusage ) != -1 )
            return (double)rusage.ru_utime.tv_sec +
                (double)rusage.ru_utime.tv_usec / 1000000.0;
    }
#endif

#if defined(_SC_CLK_TCK)
    {
        const double ticks = (double)sysconf( _SC_CLK_TCK );
        struct tms tms;
        if ( times( &tms ) != (clock_t)-1 )
            return (double)tms.tms_utime / ticks;
    }
#endif

#if defined(CLOCKS_PER_SEC)
    {
        clock_t cl = clock( );
        if ( cl != (clock_t)-1 )
            return (double)cl / (double)CLOCKS_PER_SEC;
    }
#endif

#endif

    return -1;      /* Failed. */
}

struct resultData {
    unsigned long string_number;
    unsigned short position;
    string match;
};

vector<resultData> result;
unsigned short quantity;
mutex result_mutex;
mutex quantity_mutex;

int *pre_kmp(string pattern) {
	int size = pattern.size();

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

vector<resultData> KMP(string text, string pattern, unsigned long counter) {
    static int countIn = 0;
    char ticket {'?'};
	int* pie=pre_kmp(pattern);
	int matched_pos = 0;
	int size = pattern.size();
    vector<resultData> currentResult;

	for(int current = 0; current < text.length(); current++) {
		while (matched_pos > 0 && pattern[matched_pos] != text[current] && pattern[matched_pos] != ticket) {
			matched_pos = pie[matched_pos - 1];
        }
			
		if(pattern[matched_pos] == text[current] || pattern[matched_pos] == ticket) {
			matched_pos = matched_pos+1;
        }
			
		if( matched_pos == size ) {
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

/*
#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
    FILE *fp;
    char str[128];
    if ((fp=fopen(argv[1], "r") )==NULL) {
        printf("Cannot open file.\n");
        exit (1);
    }

    unsigned long counter = 1;
    string pattern;
    pattern = argv[2];

    while(!feof (fp)) {
        if (fgets(str, 126, fp))
            printf("%s", str);
            KMP(str, pattern, counter);
            counter++;
        }
        fclose(fp);
    return 0;
}
*/

int main(int argc, char* argv []) {
    double startTime, endTime;

    vector<vector<int>> temp;
    unsigned int n = std::thread::hardware_concurrency();
    std::cout << n << " concurrent threads are supported.\n";
    
    std::map<std::string, std::vector<int>> kata;
    string line;
    string pattern;
    pattern = argv[2];
    ifstream in(argv[1]);

    if (in.is_open()) {
        unsigned long counter = 0;
        startTime = getCPUTime( );
        mutex mtx;

        for(size_t i=0; i<std::thread::hardware_concurrency(); ++i){
                
            while (getline(in, line)) {

                 future<vector<resultData>> futures = std::async(std::launch::async, [&] {
                     {
                         lock_guard<mutex> lock(mtx);
                         ++counter;
                     }

                     vector<resultData> loopResult = KMP(line, pattern, counter);

                     return loopResult;
                 });

                auto n = futures.get();
                result.insert(result.cend(), n.begin(), n.end());
            }
        }
    }
    

    cout << result.size() << endl;
    for(resultData n : result){
        cout << n.string_number << " " << n.position << " " << n.match <<"\n";    
    }

    in.close();
    endTime = getCPUTime( );
    fprintf( stderr, "CPU time used = %lf\n", (endTime - startTime) );
}
