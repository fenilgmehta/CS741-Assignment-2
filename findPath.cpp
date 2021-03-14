#include <bits/stdc++.h>

using namespace std;

// DEBUG macros, to set the flag from commnad line, comment the below line and use `g++ name.cpp -DPRINT_DEBUG`
#define PRINT_DEBUG
#ifdef PRINT_DEBUG
#define printFunction(outStream, functionName, argDelimiter, lineDelimiter) template <typename Arg, typename... Args> inline void functionName(Arg&& arg, Args&&... args) { outStream << arg; (void)(int[]){0, (void(outStream << argDelimiter << args),0)...}; outStream << lineDelimiter; }
printFunction(cerr, printErr, " "<<"\033[1;31m"<<"●"<<"\033[0m"<<" ", '\n');
#define db(...) dbg(__LINE__, #__VA_ARGS__, __VA_ARGS__);
template<class T, class... U> void dbg(int32_t lineNo, const char *sdbg, T h, U... a) {cerr<<"\033[1;31m"<<"Debug (" << lineNo << ") : "<<"\033[0m"; cerr<<sdbg; cerr<<" "<<"\033[1;41m"<<"="<<"\033[0m"<<" "; printErr(h, a...); cout.flush(); cerr.flush();}
template <class S, class T>ostream& operator <<(ostream& os, const pair<S, T>& p) {return os << "pair(" << p.first << "\033[1;31m" << ", " << "\033[0m" << p.second << ")";}
template <class T>ostream& operator <<(ostream& os, const vector<T>& p) {os << "\033[1;32m" << "vector[ " << "\033[0m"; for (const auto& it : p) os << it << "\033[1;31m" << ", " << "\033[0m"; return os << "\033[1;32m" << "]" << "\033[0m";}
template <class T>ostream& operator <<(ostream& os, const set<T>& p) {os << "\033[1;32m" << "set[ "; for (const auto& it : p) os << it << "\033[1;31m" << ", " << "\033[0m"; return os << "\033[1;32m" << "]" << "\033[0m";}
template <class S, class T>ostream& operator <<(ostream& os, const map<S, T>& p) {os << "\033[1;32m" << "map[ " << "\033[0m"; for (const auto& it : p) os << it << "\033[1;31m" << ", " << "\033[0m"; return os << "\033[1;32m" << "]" << "\033[0m";}
#define dbiter(...) dbgIter(#__VA_ARGS__, __VA_ARGS__);
template <class T> void dbgIter(const char *sdbg, T a, T b) {cerr<<"\033[1;31m"<<"Debug: "<<"\033[0m"; cerr<<sdbg; cerr<<"\033[1;31m"<<" = "<<"\033[0m"; cerr << "["; for (T i = a; i != b; ++i) {if (i != a) cerr << ", "; cerr << *i;} cerr << "]\n"; cout.flush(); cerr.flush();}
#else
#define db(...) ;
    #define dbiter(...) ;
#endif


const int SboxSize = 3;
const int totalLevels = 3;
const int plainTextSize = 9;
const int totalInputOutputCombinations = 1 << plainTextSize;  // 2 << 9 = 1024
const int Permutation[] = {0, 3, 6, 1, 4, 7, 2, 5, 8};
string finalAns;
float maxBias = 0.0;

map<int, string> S_box;

void find_path(string inputToLevel, string ans, int level, float totalBias) {
    // base case
    if (level >= totalLevels) {
        if (abs(totalBias) > abs(maxBias)) {
            maxBias = totalBias;
            finalAns = ans;     // Add cipher bits here
            db(finalAns, maxBias)
        }
        return;
    }

    string inputToS_box;  // TODO: uninitialized - but still used. Probably this should be = inputToLevel
    string outputOfLevel;
    string outputOfS_box;
    bool flag1, flag2;

    for (int i = 0; i < plainTextSize; i++) {
        if (inputToLevel[i] == '1') {
            ans += "K(" + to_string(level) + "," + to_string(i + 1) + ") ";
        }
    }

    // "k" starts from 1 because for bias calculation, we have
    // to take "at least one output"
    for (int k = 1; k < totalInputOutputCombinations; k++) {
        float thisOutputCombinationBias = totalBias;
        outputOfLevel = bitset<plainTextSize>(k).to_string(); //to binary

        for (int i = 0; i < plainTextSize; i += SboxSize) {
            inputToS_box = inputToLevel.substr(i, SboxSize);
            outputOfS_box = outputOfLevel.substr(i, SboxSize);

            string xorValue = "00000000";  // bin(48)='0b110000', bin(49)='0b110001'
            flag1 = false;
            flag2 = false;

            for (int j = 0; j < SboxSize; j++) {
                if (inputToS_box[j] == '1') {
                    for (int l = 0; l < 8; l++) {
                        xorValue[l] = xorValue[l] ^ S_box[j][l];
                    }
                    flag1 = true;
                }
            }

            for (int j = 0; j < SboxSize; j++) {
                if (outputOfS_box[j] == '1') {
                    for (int l = 0; l < 8; l++) {
                        xorValue[l] = xorValue[l] ^ S_box[SboxSize + j][l];
                    }
                    flag2 = true;
                }
            }

            if ((flag1 == false && flag2 == true) || (flag1 == true && flag2 == false))
                break;

            if (flag1 == true && flag2 == true) {
                int sum = 0;
                for (int j = 0; j < 8; j++) {
                    if (xorValue[j] == '0')
                        sum++;
                }

                float bias = sum / 8.0 - 0.5;
                thisOutputCombinationBias *= (bias * 2);

                if (thisOutputCombinationBias == 0.0) {
                    // More Optimisation can be done here
                    // k += (2 << (plainTextSize - i - SboxSize)) - 1;
                    break;  // TODO: try a different output combination
                }
            }
        }

        if ((flag1 == false && flag2 == true) || (flag1 == true && flag2 == false) || thisOutputCombinationBias == 0) {
            // More Optimisation can be done here
            // k += (2 << (plainTextSize - i - SboxSize)) - 1;
            continue;
        }

        if (level < totalLevels - 1) {
            string temp = outputOfLevel;
            for (int i = 0; i < plainTextSize; i++) {
                outputOfLevel[i] = temp[Permutation[i]];
            }
        }

        // recursive case
        find_path(outputOfLevel, ans, level + 1, thisOutputCombinationBias);
    }
}

int main() {

    //S_box shown in lecture ppt
    S_box[0] = "00001111";  // This is column 0 of input
    S_box[1] = "00110011";  //         column 1 of input
    S_box[2] = "01010101";  //         column 2 of input
    S_box[3] = "00011110";  // This is column 0 of output
    S_box[4] = "01001011";  //         column 1 of output
    S_box[5] = "00101101";  //         column 2 of output

    for (int i = 1; i < totalInputOutputCombinations; i++) {
        string inputToLevel = bitset<plainTextSize>(i).to_string(); //to binary
        // db(inputToLevel)
        cout << "\r" << "Progress = " << inputToLevel << " , " << i << " / " << totalInputOutputCombinations;

        string ans;  // by default string is "" if uninitialized
        for (int j = 0; j < plainTextSize; j++) {
            if (inputToLevel[j] == '1') {
                ans += "P" + to_string(j + 1) + " ";
            }
        }
        // db(ans);

        float totalBias = 1;
        find_path(inputToLevel, ans, 0, totalBias);
    }

    return 0;
}
