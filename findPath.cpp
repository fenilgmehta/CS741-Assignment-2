#include <bits/stdc++.h>

using namespace std;

const int SboxSize = 3;
const int totalLevels = 3;
const int plainTextSize = 9;
const int totalInputCombination = 2 << plainTextSize;
const vector<int> Permutation = {0, 3, 6, 1, 4, 7, 2, 5, 8}; 

map<int, string> S_box; 

void find_path(string inputToLevel, string ans, int level, float totalBias) {
	// base case
	if(level >= totalLevels)
		return;
		
	string inputToS_box;
	string outputOfLevel;
	string outputOfS_box;
	bool flag1, flag2;
	
	for(int k = 1; k < totalInputCombination; k++) {
		outputOfLevel = bitset<plainTextSize>(k).to_string(); //to binary
			
		for(int i = 0; i < plainTextSize; i += SboxSize) {
		
			inputToS_box = inputToLevel.substr(i, SboxSize);
			
			string xorValue = "00000000";
			flag1 = false;
			flag2 = false;
			for(int j = 0; j < SboxSize; j++) {
				if(inputToS_box[j] == '1') {
					ans += "K" + to_string(level) + to_string(i + j + 1) + ",";
					for(int l = 0; l < 8; l++) {
						xorValue[l] = xorValue[l] ^ S_box[j][l];
					}
					flag1 = true;
				}
			}
			
			outputOfS_box = outputOfLevel.substr(i, SboxSize);
			
			
			for(int j = 0; j < SboxSize; j++) {
				if(outputOfS_box[j] == '1') {
					for(int l = 0; l < 8; l++) {
						xorValue[l] = xorValue[l] ^ S_box[SboxSize + j][l];
					}
					flag2 = true;
				}
			}
			
			if(flag1 == false && flag2 == true)
				break;
			
			int sum = 0;
			for(int j = 0; j < 8; j++) {
				if(xorValue[j] == '0')
					sum++ ;
			}
			
			float bias = sum / 8.0 - 0.5;
			
			totalBias *= (bias * 2);
		}
		if(flag1 == false && flag2 == true)
			continue;
			
		for(int i = 0; i < plainTextSize; i++) {
			inputToLevel[i] = outputOfLevel[Permutation[i]];
		}
		
		// recursive case 
		find_path(inputToLevel, ans, level + 1, totalLevels);

	}
		
}

int main() {
	  
	//S_box shown in lecture ppt
	S_box[0] = "00001111";
	S_box[1] = "00110011";
	S_box[2] = "01010101";
	S_box[3] = "00011110";
	S_box[4] = "01001011";
	S_box[5] = "00101101";
	float totalBias = 0.5; 
	
	for(int i = 1; i < totalInputCombination; i++)
	{
		string inputToLevel = bitset<plainTextSize>(i).to_string(); //to binary
		cout << inputToLevel << endl;
		string ans = "";
		for(int j = 0; j < plainTextSize; j++) {
			if(inputToLevel[j] == '1') {
				ans += "P" + to_string(j + 1) + ",";
			}
		}
		cout << ans << endl;
		find_path(inputToLevel, ans, 0, totalBias);
		cout << ans << endl;
	}
	
}
