#include <iostream>
#include <algorithm>
#include <bitset>
#include <string>
#include <map>

using namespace std;


// REFER: https://en.wikipedia.org/wiki/Rijndael_S-box
void initialize_aes_sbox(uint8_t sbox[256]) {
#define ROTL8(x,shift) ((uint8_t) ((x) << (shift)) | ((x) >> (8 - (shift))))
	uint8_t p = 1, q = 1;
	
	/* loop invariant: p * q == 1 in the Galois field */
	do {
		/* multiply p by 3 */
		p = p ^ (p << 1) ^ (p & 0x80 ? 0x1B : 0);

		/* divide q by 3 (equals multiplication by 0xf6) */
		q ^= q << 1;
		q ^= q << 2;
		q ^= q << 4;
		q ^= q & 0x80 ? 0x09 : 0;

		/* compute the affine transformation */
		uint8_t xformed = q ^ ROTL8(q, 1) ^ ROTL8(q, 2) ^ ROTL8(q, 3) ^ ROTL8(q, 4);

		sbox[p] = xformed ^ 0x63;
	} while (p != 1);

	/* 0 is a special case since it has no inverse */
	sbox[0] = 0x63;
#undef ROTL8
}

// REFER: https://stackoverflow.com/a/1617797
template <typename Iterator>
bool next_combination(const Iterator first, Iterator k, const Iterator last)
{
   /* Credits: Mark Nelson http://marknelson.us */
   if ((first == last) || (first == k) || (last == k))
      return false;
   Iterator i1 = first;
   Iterator i2 = last;
   ++i1;
   if (last == i1)
      return false;
   i1 = last;
   --i1;
   i1 = k;
   --i2;
   while (first != i1)
   {
      if (*--i1 < *i2)
      {
         Iterator j = k;
         while (!(*i1 < *j)) ++j;
         std::iter_swap(i1,j);
         ++i1;
         ++j;
         i2 = k;
         std::rotate(i1,j,last);
         while (last != j)
         {
            ++j;
            ++i2;
         }
         std::rotate(k,i2,last);
         return true;
      }
   }
   std::rotate(first,k,last);
   return false;
}

// The Least Significant Bit is at index 0
// For Big-Endian format, the indexes will look like: {7, 6, 5, 4, 3, 2, 1, 0}
inline bool test_bit(uint8_t num, uint_fast8_t bit) {
    return (num >> bit) & 1;
}

// The Most Significant Bit is at index 0
// For Big-Endian format, the indexes will look like: {0, 1, 2, 3, 4, 5, 6, 7}
inline bool test_bit_back(uint8_t num, uint_fast8_t bit) {
    return (num >> (7 - bit)) & 1;
}

int main(){
    uint8_t aes_sbox[256] = {};
    initialize_aes_sbox(aes_sbox);
    cout << "AES S-Box is:" << endl;
    cout << hex;
    for(int i = 0; i < 16; ++i) {
        for(int j = 0; j < 16; ++j) {
            cout << static_cast<uint32_t>(aes_sbox[i * 16 + j]) << "\t";
        }
        cout << endl;
    }
    cout << dec;


    bitset<256> sbox_input[8], sbox_output[8];
    for(int i = 0; i < 256; ++i) {
        for(int j = 0; j < 8; ++j) {
            sbox_input[j].set(i, test_bit_back(i, j));
            sbox_output[j].set(i, test_bit_back(aes_sbox[i], j));
        }
    }
    cout << "AES S-Box Input and Output in binary:" << endl;
    for(int i = 0; i < 256; ++i) {
        cout << "     ";
        for(int j = 0; j < 8; ++j) {
            cout << ((sbox_input[j].test(i)==0) ? 0 : 1) << ' ';
        }
        cout << "---> ";
        for(int j = 0; j < 8; ++j) {
            cout << ((sbox_output[j].test(i)==0) ? 0 : 1) << ' ';
        }
        cout << endl;
    }
    
    // Testing whether XOR can be directly used with a column or not ---> Yes, can be used :)
    // cout << (sbox_input[0] ^ sbox_input[1]) << endl;
    int input_idx_combination[8], output_idx_combination[8];
    bitset<256> bias_calculated;
    map<string, string> bias_mapping;
    map<int32_t, int32_t> bias_count;
    for(int i = 1; i <= 8; ++i) {
        for(int j = 1; j <= 8; ++j) {
            // Reset the Index Combination array
            for(int k = 0; k < 8; ++k) {
                input_idx_combination[k] = output_idx_combination[k] = k;
            }
            bias_calculated.reset();

            do {
                do {
                    string key="", value="";
                    for(int l = 0; l < i; ++l)  {
                        bias_calculated ^= sbox_input[input_idx_combination[l]];
                        key+=('0' + input_idx_combination[l]);
                    }
                    key+=',';
                    for(int l = 0; l < j; ++l){
                        bias_calculated ^= sbox_output[output_idx_combination[l]];
                        key+=('0' + output_idx_combination[l]);
                    }
                    // string bias_str = bias_calculated.to_string();
                    const auto bias_value = bias_calculated.count();
                    bias_mapping[key] = bias_value;
                    bias_count[bias_value] += 1;
                } while(next_combination(output_idx_combination, output_idx_combination+j, output_idx_combination+8));
            } while (next_combination(input_idx_combination, input_idx_combination+i, input_idx_combination+8));
        }
    }
    cout << "** bias_count ***" << endl;
    for(auto i: bias_count) {
        cout << i.first << '=' << i.second << endl;
    }
    return 0;
}