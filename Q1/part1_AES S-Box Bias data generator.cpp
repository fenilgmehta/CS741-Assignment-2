#include <iostream>
#include <iomanip>
#include <algorithm>
#include <bitset>
#include <string>
#include <map>

using namespace std;


// REFER: https://en.wikipedia.org/wiki/Rijndael_S-box
void initialize_aes_sbox(uint8_t sbox[256]) {
#define ROTL8(x, shift) ((uint8_t) ((x) << (shift)) | ((x) >> (8 - (shift))))
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
        uint8_t xformed = q ^ROTL8(q, 1) ^ROTL8(q, 2) ^ROTL8(q, 3) ^ROTL8(q, 4);

        sbox[p] = xformed ^ 0x63;
    } while (p != 1);

    /* 0 is a special case since it has no inverse */
    sbox[0] = 0x63;
#undef ROTL8
}

// REFER: https://stackoverflow.com/a/1617797
template<typename Iterator>
bool next_combination(const Iterator first, Iterator k, const Iterator last) {
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
    while (first != i1) {
        if (*--i1 < *i2) {
            Iterator j = k;
            while (!(*i1 < *j)) ++j;
            std::iter_swap(i1, j);
            ++i1;
            ++j;
            i2 = k;
            std::rotate(i1, j, last);
            while (last != j) {
                ++j;
                ++i2;
            }
            std::rotate(k, i2, last);
            return true;
        }
    }
    std::rotate(first, k, last);
    return false;
}

// The Least Significant Bit is at index 0
// For Big-Endian format, the indexes will look like: {7, 6, 5, 4, 3, 2, 1, 0}
template<typename T>
inline bool test_bit(T num, uint_fast16_t bit) {
    return (num >> bit) & 1;
}

// The Most Significant Bit is at index 0
// For Big-Endian format, the indexes will look like: {0, 1, 2, 3, 4, 5, 6, 7}
template<typename T>
inline bool test_bit_back(T num, uint_fast16_t bit, int_fast16_t num_bits_len = -1) {
    if (num_bits_len == -1) num_bits_len = 8 * sizeof(T);
    return (num >> ((num_bits_len - 1) - bit)) & 1;
}

const int32_t N_BITS = 8;
const int32_t N_SIZE = 1 << N_BITS;  // 256
const int32_t N_SIZE_BY_2 = N_SIZE >> 1;  // 128

int main() {
    // Bias has two definitions
    // 1. Bias = (Probability of getting 0) - 1/2
    // 2. Bias = Number of occurrences of 0 in the XOR of input and output columns
    uint8_t aes_sbox[N_SIZE] = {};
    initialize_aes_sbox(aes_sbox);

    bitset<N_SIZE> sbox_input[N_BITS], sbox_output[N_BITS];
    for (int32_t i = 0; i < N_SIZE; ++i) {
        for (int32_t j = 0; j < N_BITS; ++j) {
            // cout << i << ", " << j << ", " 
            //      << static_cast<int32_t>(aes_sbox[i]) << ", " 
            //      << static_cast<int32_t>(test_bit_back(i, j, N_BITS)) << ", " 
            //      << static_cast<int32_t>(test_bit_back(aes_sbox[i], j, N_BITS)) << endl;
            sbox_input[j].set(i, test_bit_back(i, j, N_BITS));
            sbox_output[j].set(i, test_bit_back(aes_sbox[i], j, N_BITS));
        }
    }

    cout << "AES S-Box Input and Output in binary:" << endl;
    for (int32_t i = 0; i < N_SIZE; ++i) {  // row -> i
        cout << "     ";
        for (auto &j : sbox_input) {  // input column -> j
            cout << ((j.test(i) == 0) ? 0 : 1) << ' ';
        }
        cout << "---> ";
        for (auto &j : sbox_output) {  // output column -> j
            cout << ((j.test(i) == 0) ? 0 : 1) << ' ';
        }
        cout << endl;
    }

    cout << "--------------------------------------------------" << endl;
    cout << "AES S-Box is:" << endl;
    cout << hex;
    for (int32_t i = 0; i < 16; ++i) {
        for (int32_t j = 0; j < 16; ++j) {
            cout << static_cast<uint32_t>(aes_sbox[i * 16 + j]) << "\t";
        }
        cout << endl;
    }
    cout << dec;

    // Testing whether XOR can be directly used with a column or not ---> Yes, can be used :)
    // cout << (sbox_input[0] ^ sbox_input[1]) << endl;
    int input_idx_combination[N_BITS], output_idx_combination[N_BITS];
    bitset<N_SIZE> bias_calculated;

    // Columns used ---> The bias string of 0's and 1's (i.e. the XOR of all the columns used)
    map<string, string> bias_mapping;

    // stores the mapping for (Number of Zeros in the XOR of the columns ---> Number of occurrences)
    map<int32_t, int32_t> bias_count;
    for (int32_t i = 1; i <= N_BITS; ++i) {
        for (int32_t j = 1; j <= N_BITS; ++j) {
            // Reset the Index Combination array
            for (int32_t k = 0; k < N_BITS; ++k) {
                input_idx_combination[k] = output_idx_combination[k] = k;
            }

            // The columns of input and output which are to be used for bias calculation
            // are based on the values stored in input_idx_combination and output_idx_combination
            // at indices [0, i-1]   <-- both inclusive

            do {
                do {
                    bias_calculated.reset();
                    // REFER: https://stackoverflow.com/questions/17738439/value-and-size-of-an-uninitialized-stdstring-variable-in-c
                    string key, value;
                    for (int32_t l = 0; l < i; ++l) {
                        bias_calculated ^= sbox_input[input_idx_combination[l]];
                        key += "i" + to_string(input_idx_combination[l]) + " ";
                        // "i" represents input column number
                    }
                    key += ", ";
                    for (int32_t l = 0; l < j; ++l) {
                        bias_calculated ^= sbox_output[output_idx_combination[l]];
                        key += "o" + to_string(output_idx_combination[l]) + " ";
                        // "o" represents output column number
                    }

                    // we use the value "256 - x" because the probability of 0's is required
                    const int32_t bias_value = N_SIZE - bias_calculated.count();
                    bias_count[bias_value] += 1;
                    bias_mapping[key] = bias_calculated.to_string();
                } while (next_combination(output_idx_combination, output_idx_combination + j,
                                          output_idx_combination + N_BITS));
            } while (next_combination(input_idx_combination, input_idx_combination + i,
                                      input_idx_combination + N_BITS));
        }
    }

    cout << "--------------------------------------------------" << endl;
    cout << "bias -> count" << endl;
    for (auto i: bias_count) {
        cout << setw(5) << i.first << "\t=\t" << i.second << endl;
    }

    cout << "--------------------------------------------------" << endl;
    cout << "bias probability -> count" << endl;
    for (auto i: bias_count) {
        cout << "  " << setw(10) << left << (static_cast<float>(i.first - N_SIZE_BY_2) / N_SIZE) << "\t=\t" << i.second
             << endl;
    }

    // cout << "--------------------------------------------------" << endl;
    // cout << "bias_mapping" << endl;
    // for (auto i: bias_mapping) {
    //     const auto cnt = count(begin(i.second), end(i.second), '0');
    //     cout << setw(16) << i.first << "\t=\t" << cnt << " OR " << (static_cast<float>(cnt - N_SIZE_BY_2) / N_SIZE)
    //          << " OR " << i.second << endl;
    // }

    return 0;
}
