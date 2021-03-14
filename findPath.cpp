#include <bits/stdc++.h>

using namespace std;

const int SboxSize = 3;
const int totalLevels = 3;
const int plainTextSize = 9;
const int totalInputCombination = 2 << plainTextSize;  // 2 << 9 = 1024
const int Permutation[] = {0, 3, 6, 1, 4, 7, 2, 5, 8};

map<int, string> S_box;

void find_path(string inputToLevel, string ans, int level, float totalBias) {
    // base case
    if (level >= totalLevels)
        return;

    string inputToS_box;  // TODO: uninitialized - but still used. Probably this should be = ans
    string outputOfLevel;
    string outputOfS_box;
    string result;
    bool flag1, flag2;

    for (int i = 0; i < plainTextSize; i++) {
        if (inputToS_box[i] == '1') {
            result += "K(" + to_string(level) + "," + to_string(i + 1) + ") ";
        }
    }

    // "k" starts from 1 because for bias calculation, we have
    // to take "at least one input" and "at least one output"
    for (int k = 1; k < totalInputCombination; k++) {

        outputOfLevel = bitset<plainTextSize>(k).to_string(); //to binary

        for (int i = 0; i < plainTextSize; i += SboxSize) {

            inputToS_box = inputToLevel.substr(i, SboxSize);

            string xorValue = "00000000";
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

            outputOfS_box = outputOfLevel.substr(i, SboxSize);

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

                totalBias *= (bias * 2);

                if (totalBias == 0) {
                    // More Optimisation can be done here
                    // k += (2 << (plainTextSize - i - SboxSize)) - 1;
                    break;
                }
            }
        }
        if ((flag1 == false && flag2 == true) || (flag1 == true && flag2 == false) || totalBias == 0) {
            // More Optimisation can be done here
            // k += (2 << (plainTextSize - i - SboxSize)) - 1;
            continue;
        }

        for (int i = 0; i < plainTextSize; i++) {
            inputToLevel[i] = outputOfLevel[Permutation[i]];
        }

        // recursive case
        find_path(inputToLevel, ans + result, level + 1, totalBias);

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

    for (int i = 1; i < totalInputCombination; i++) {
        string inputToLevel = bitset<plainTextSize>(i).to_string(); //to binary
        cout << inputToLevel << endl;

        string ans;  // by default string is "" if uninitialized
        for (int j = 0; j < plainTextSize; j++) {
            if (inputToLevel[j] == '1') {
                ans += "P" + to_string(j + 1) + " ";
            }
        }
        cout << ans << endl;

        float totalBias = 1;
        find_path(inputToLevel, ans, 0, totalBias);
        // cout << ans << endl;
    }

    return 0;
}
