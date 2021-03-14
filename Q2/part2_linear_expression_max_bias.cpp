#include <iostream>
#include <iomanip>
#include <algorithm>
#include <bitset>
#include <string>
#include <map>
#include <vector>
#include "MyDebugger.hpp"

using namespace std;


// REFER: https://github.com/fenilgmehta/Misc-Programming/blob/master/src/Range%20Iteration.cpp
namespace NumericRange {
    template<typename S>
    struct urange_iterator {
        typedef std::random_access_iterator_tag iterator_category;
        typedef S value_type;       // int
        typedef S difference_type;  // int
        typedef urange_iterator<S> *pointer;         // urange_iterator<int>*
        typedef urange_iterator<S> &reference;       // urange_iterator<int>&
        typedef const urange_iterator<S> &const_reference;       // const urange_iterator<int>&

        S current;
        const S step;

        inline urange_iterator(const S &f1, const S &s1) : current{f1}, step{s1} {}

        inline bool operator!=(const_reference zz) const { return current < zz.current; }

        inline reference operator++() {
            current += step;
            return *this;
        }

        inline reference operator+=(const difference_type &val) {
            current += step * val;
            return *this;
        }

        inline difference_type operator-(const_reference zz) const {
            return (current - zz.current + (step - 1)) / step;
        }

        inline const value_type &operator*() const { return current; }

        inline pointer operator->() { return this; }

        inline reference operator=(const_reference zz) {
            current = zz.current;
            return *this;
        }
    };

    // This is similar to Python3's "range" function
    // IMPORTANT: "step" has to always be +ve
    template<typename T>
    struct urange {
        T f1, l1, s1;

        inline explicit urange(T last) : f1{0}, l1{last}, s1{1} {}

        inline explicit urange(T first, T last, T step = 1) : f1{first}, l1{last}, s1{step} {}

        [[nodiscard]] inline urange_iterator<T> begin() const { return urange_iterator<T>(f1, s1); }

        [[nodiscard]] inline const urange_iterator<T> end() const { return urange_iterator<T>(l1, s1); }
    };
}
using namespace NumericRange;


// REFER: https://stackoverflow.com/questions/22910563/fast-stdvectorbool-reset
template<size_t BITS_LEN>
struct DynamicBitset {
    // static const unsigned BITS_LEN = 64; // or 32, or more?
    typedef std::bitset<BITS_LEN> Bits;
    std::vector<Bits> data;
    size_t data_size_in_bits;

    explicit DynamicBitset(size_t len = 0) : data_size_in_bits{0} {
        resize(len);
    }

    // reset all bits to false
    void reset() {
        for (auto it = data.begin(); it != data.end(); ++it) {
            it->reset(); // we can use the fast bitset::reset :)
        }
    }

    // reset the whole bitset belonging to bit i
    inline void reset_approx(size_t i) {
        data[i / BITS_LEN].reset();
    }

    // make room for len bits
    void resize(size_t len) {
        data_size_in_bits = len;
        data.resize((len + BITS_LEN - 1) / BITS_LEN);
    }

    [[nodiscard]] inline size_t size() const {
        return data_size_in_bits;
    }

    [[nodiscard]] inline size_t size_in_mem() const {
        return BITS_LEN * data.size();
    }

    // access a bit
    typename Bits::reference operator[](size_t i) {
        size_t k = i / BITS_LEN;
        return data[k][i % BITS_LEN];
    }

    bool operator[](size_t i) const {
        size_t k = i / BITS_LEN;
        return data[k][i % BITS_LEN];
    }

    [[nodiscard]] auto c_xor(const DynamicBitset<BITS_LEN> &b) const {
        // DynamicBitset<BITS_LEN> res(this->)
        DynamicBitset<BITS_LEN> res(this->size());
        for (size_t i = 0, i_end = this->data.size(); i < i_end; ++i) {
            res.data[i] = this->data[i] ^ b.data[i];
        }
        return res;
    }

    void c_xor_equals(const DynamicBitset<BITS_LEN> &b) {
        // DynamicBitset<BITS_LEN> res(this->)
        for (size_t i = 0, i_end = this->data.size(); i < i_end; ++i) {
            data[i] ^= b.data[i];
        }
    }

    auto begin() { return data.begin(); }

    auto end() { return data.end(); }
};


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


// REFER: https://github.com/fenilgmehta/Misc-Programming/blob/master/src/Competitive%20Programming%20Template%20(Big).cpp#L64
template<typename T, typename U>
std::vector<T> MatrixVector(int n, U v) { return std::vector<T>(n, v); }

template<typename T, class... Args>
auto MatrixVector(int n, Args... args) {
    auto val = MatrixVector<T>(args...);
    return std::vector<decltype(val)>(n, move(val));
}


// The Least Significant Bit is at index 0
// For Big-Endian format, the indexes will look like: {7, 6, 5, 4, 3, 2, 1, 0}
template<typename T>
inline bool test_bit(T num, uint_fast16_t bit) {
    return (num >> bit) & 1;
}

// The Most Significant Bit is at index 0
// For Big-Endian format, the indexes will look like: {0, 1, 2, 3, 4, 5, 6, 7}
// ASSUMPTION: bit <= num_bits_len
template<typename T>
inline bool test_bit_back(T num, uint_fast16_t bit, int_fast16_t num_bits_len = -1) {
    if (num_bits_len == -1) num_bits_len = 8 * sizeof(T);
    return (num >> ((num_bits_len - 1) - bit)) & 1;
}

template<typename T>
inline void set_bit(T &num, uint_fast16_t bit) {
    num |= (1 << bit);
}

// ASSUMPTION: bit <= num_bits_len
template<typename T>
inline void set_bit_back(T &num, uint_fast16_t bit, int_fast16_t num_bits_len = -1) {
    if (num_bits_len == -1) num_bits_len = 8 * sizeof(T);
    num |= (1 << (num_bits_len - 1 - bit));
}


uint32_t column_idx_to_uint32_t(uint16_t arr_in[], int arr_in_len, uint16_t arr_out[], int arr_out_len) {
    uint32_t result = 0;
    for (auto i: urange<int32_t>(arr_in_len)) {
        set_bit_back(result, arr_in[i], 32);
    }
    for (auto i: urange<int32_t>(arr_out_len)) {
        set_bit_back(result, arr_out[i], 16);
    }
    return result;
}

uint32_t column_idx_to_uint32_t(bitset<64> b, int32_t idx_start, int32_t idx_end) {
    uint64_t result = (b >> idx_start).to_ullong();
    result &= (static_cast<uint64_t>(1) << (idx_end - idx_start + 1)) - 1;
    return static_cast<uint32_t>(result);
}

uint32_t column_idx_to_uint32_t_len(bitset<64> b, int32_t idx_start, int32_t len_bits) {
    uint64_t result = (b >> idx_start).to_ullong();
    result &= (static_cast<uint64_t>(1) << len_bits) - 1;
    return static_cast<uint32_t>(result);
}

/* N_BITS = sbox_bits */
map<uint32_t, float> generate_sbox_bias_mapping(const vector<uint32_t> &sbox_arr, const int32_t N_BITS) {
    //                                           IF (N_BITS is 8), then:
    const int32_t N_SIZE = 1 << N_BITS;  //          N_SIZE = 256
    const int32_t N_SIZE_BY_2 = N_SIZE >> 1;  //     N_SIZE_BY_2 = 128

    // rows = N_BITS, cols = N_SIZE;
    // vector[i] is column "i" of the input/output
    vector<DynamicBitset<128>> sbox_input(N_BITS, DynamicBitset<128>(N_SIZE));
    vector<DynamicBitset<128>> sbox_output(N_BITS, DynamicBitset<128>(N_SIZE));

    for (auto i: urange<int32_t>(N_SIZE)) {
        for (auto j: urange<int32_t>(N_BITS)) {
            // cout << i << ", " << j << ", "
            //      << static_cast<int32_t>(sbox_arr[i]) << ", "
            //      << static_cast<int32_t>(test_bit_back(i, j, N_BITS)) << ", "
            //      << static_cast<int32_t>(test_bit_back(sbox_arr[i], j, N_BITS)) << endl;
            sbox_input[j][i] = test_bit_back(i, j, N_BITS);
            sbox_output[j][i] = test_bit_back(sbox_arr[i], j, N_BITS);
        }
    }

    // // DEBUG
    cout << "--------------------------------------------------" << endl;
    cout << "S-Box Input and Output in binary:" << endl;
    for (auto i: urange<int32_t>(N_SIZE)) {  // Y = row -> i
        cout << "     ";
        for (auto j: urange<int32_t>(N_BITS)) {  // X = input column -> j
            cout << ((sbox_input[j][i] == 0) ? 0 : 1) << ' ';
        }
        cout << "---> ";
        for (auto j: urange<int32_t>(N_BITS)) {  // X = output column -> j
            cout << ((sbox_output[j][i] == 0) ? 0 : 1) << ' ';
        }
        cout << endl;
    }

    // Testing whether XOR can be directly used with a column or not ---> Yes, can be used :)
    uint16_t input_idx_combination[N_BITS], output_idx_combination[N_BITS];
    DynamicBitset<128> bias_calculated(N_SIZE);

    // Key (Columns used) ---> Value ((probability of 0) - 1/2)
    //     Most Significant 16 bits ---> Input columns
    //     Least Significant 16 bits ---> Output columns
    map<uint32_t, float> bias_probability_mapping;

    // stores the mapping for: Key (Number of Zeros in the XOR of the columns) ---> Value (Number of occurrences)
    map<int32_t, int32_t> bias_count;

    for (auto i: urange<int32_t>(1, N_BITS + 1)) {  // Number of Input columns to take
        for (auto j: urange<int32_t>(1, N_BITS + 1)) {  // Number of Output columns to take
            // Reset the Index Combination array
            for (auto k: urange<int32_t>(N_BITS)) {
                input_idx_combination[k] = output_idx_combination[k] = k;
            }

            // The columns of input and output which are to be used for bias calculation
            // are based on the values stored in "input_idx_combination" and "output_idx_combination"
            // at indices [0, i-1]   <--- both inclusive

            do {
                do {
                    bias_calculated.reset();

                    for (auto l: urange<int32_t>(i)) {
                        bias_calculated.c_xor_equals(sbox_input[input_idx_combination[l]]);
                    }
                    for (auto l: urange<int32_t>(j)) {
                        bias_calculated.c_xor_equals(sbox_output[output_idx_combination[l]]);
                    }

                    // we use the value "256 - x" because the probability of 0's is required
                    int32_t zeros_count = 0;
                    // count() return number of bits set
                    for (auto l: bias_calculated) zeros_count += l.size() - l.count();
                    // Below statement is very important, because, the trailing 0's are also counted in the above loop
                    zeros_count -= (bias_calculated.size_in_mem() - bias_calculated.size());

                    // this is signed because "bias_value - N_SIZE_BY_2" can be -ve
                    const int32_t bias_value = zeros_count;

                    // cout << "DEBUG: "
                    //      << bitset<32>(column_idx_to_uint32_t(input_idx_combination, i, output_idx_combination, j))
                    //      << ", N_SIZE = " << N_SIZE
                    //      << ", zeros_count = " << zeros_count
                    //      << ", bias_value = " << bias_value
                    //      << ", N_SIZE = " << N_SIZE << endl;
                    bias_probability_mapping[
                            column_idx_to_uint32_t
                                    (
                                            input_idx_combination, i,
                                            output_idx_combination, j
                                    )
                    ] = static_cast<float>(bias_value - N_SIZE_BY_2) / N_SIZE;
                    bias_count[bias_value] += 1;
                } while (next_combination(output_idx_combination,
                                          output_idx_combination + j,
                                          output_idx_combination + N_BITS));
            } while (next_combination(input_idx_combination,
                                      input_idx_combination + i,
                                      input_idx_combination + N_BITS));
        }
    }

    // DEBUG
    cout << "--------------------------------------------------" << endl;
    cout << "bias_count" << endl;
    for (auto i: bias_count) {
        cout << setw(5) << i.first << "\t=\t" << i.second << endl;
    }

    // DEBUG
    cout << "--------------------------------------------------" << endl;
    cout << "bias probability count" << endl;
    for (auto i: bias_probability_mapping) {
        cout << std::bitset<32>(i.first) << " -> " << i.second << endl;
    }
    cout << "--------------------------------------------------" << endl;

    return bias_probability_mapping;
}


struct MaxBiasedPath {
    int32_t numStages, numPlainTextBits, numSboxBits, totalInputOutputCombinations;
    vector<int32_t> arrPermutation, arrSbox;

    // Each element of the vector is of size numPlainTextBits
    // pair(A, B) ---> A (Input Columns), B (Output Columns)
    vector<pair<DynamicBitset<64>, DynamicBitset<64>>> result;
    double maxBias_into2;
    double finalMaxBias;
    string finalAns;

    MaxBiasedPath(int32_t mNumStages, int32_t mNumPlainTextBits, int32_t mNumSboxBits) :
            numStages{mNumStages}, numPlainTextBits{mNumPlainTextBits}, numSboxBits{mNumSboxBits},
            arrPermutation(mNumPlainTextBits), arrSbox(1 << mNumSboxBits),
            result(), maxBias_into2{0.0}, finalMaxBias{0.0}, finalAns(),
            totalInputOutputCombinations{1 << mNumPlainTextBits} {
        result.reserve(20);
    }

    string generate_ans_string() {
        string ans;
        ans += " ";
        // TODO
        return ans;
    }

    // IMPORTANT: fill arrPermutation and arrSbox before calling this function
    void find_path(string inputToLevel, string ans, const int32_t level, double currentBias_into2) {
        // base case
        if (level > numStages) {
            if (abs(currentBias_into2) > abs(maxBias_into2)) {
                maxBias_into2 = currentBias_into2;

                finalMaxBias = maxBias_into2 / 2;
                finalAns = ans;  // Add cipher bits here

                // DEBUG
                cout << endl;
                db(finalAns, finalMaxBias)
            }
            return;
        }

        bool flag1, flag2;

        for (int i = 0; i < numPlainTextBits; i++) {
            if (inputToLevel[i] == '1') {
                if (level == numStages) {
                    ans += "C(" + to_string(i) + ") ";
                } else {
                    ans += "K(" + to_string(level) + "," + to_string(i) + ") ";
                }
            }
        }

        // "k" starts from 1 because for bias calculation, we have
        // to take "at least one output"
        for (int k = 1; k < totalInputOutputCombinations; k++) {
            double thisOutputCombinationBias_into2 = currentBias_into2;
            string outputOfLevel = bitset<numPlainTextBits>(k).to_string(); // to binary

            for (int i = 0; i < numPlainTextBits; i += numSboxBits) {
                string inputToS_box = inputToLevel.substr(i, numSboxBits);
                string outputOfS_box = outputOfLevel.substr(i, numSboxBits);

                string xorValue = "00000000";  // bin(48)='0b110000', bin(49)='0b110001'
                flag1 = false;
                flag2 = false;

                for (int j = 0; j < numSboxBits; j++) {
                    if (inputToS_box[j] == '1') {
                        for (int l = 0; l < 8; l++) {
                            xorValue[l] = (xorValue[l] == arrSbox[j][l]) ? '0' : '1';
                        }
                        flag1 = true;
                    }
                }

                for (int j = 0; j < numSboxBits; j++) {
                    if (outputOfS_box[j] == '1') {
                        for (int l = 0; l < 8; l++) {
                            xorValue[l] = (xorValue[l] == arrSbox[numSboxBits + j][l]) ? '0' : '1';
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

                    double bias = (sum / 8.0) - 0.5;
                    thisOutputCombinationBias_into2 *= (bias * 2);

                    if (thisOutputCombinationBias_into2 == 0.0) {
                        // More Optimisation can be done here
                        // k += (2 << (plainTextSize - i - SboxSize)) - 1;
                        break;  // TODO: try a different output combination
                    }
                }
            }

            if ((flag1 == false && flag2 == true) || (flag1 == true && flag2 == false) || thisOutputCombinationBias_into2 == 0) {
                // More Optimisation can be done here
                // k += (2 << (plainTextSize - i - SboxSize)) - 1;
                continue;
            }

            if (level < numStages - 1) {
                string temp = outputOfLevel;
                for (int i = 0; i < numPlainTextBits; i++) {
                    outputOfLevel[i] = temp[arrPermutation[i]];
                }
            }

            // recursive case
            find_path(outputOfLevel, ans, level + 1, thisOutputCombinationBias_into2);
        }
    }

    void find_path() {
        for (auto i: urange<int32_t>(1, totalInputOutputCombinations)) {
            string inputToLevel = bitset<64>(i).to_string(); // to binary
            // db(inputToLevel)
            cout << "\r" << "Progress = " << inputToLevel << " , " << i << " / " << totalInputOutputCombinations;

            string ans;  // by default string is "" if uninitialized
            for (int j = 0; j < numPlainTextBits; j++) {
                if (inputToLevel[j] == '1') {
                    ans += "P(" + to_string(j) + ") ";
                }
            }
            // db(ans);

            float totalBias = 1;
            find_path(inputToLevel, ans, 0, totalBias);
        }
    }
};

int main() {
/*
    For understanding the concept:
        REFER: http://www.csc.kth.se/utbildning/kth/kurser/DD2448/krypto11/handouts/lec04.pdf
        REFER: https://en.wikipedia.org/wiki/Substitution%E2%80%93permutation_network

    CONVENTION Followed:
        All indexing starts from 0

    ASSUMPTIONS:

        INPUT:
            Num of stages (T)
            Size of plaintext (N)
            Permutation (of size N, space separated)
            Size of S-box (S)
            S-box (of size power(2, S))

        INPUT Limits:
            2 <= number_of_stages
            2 <= size_of_plain_text <= 64      <--- IMPORTANT
            1 <= sbox_bits <= 16               <--- IMPORTANT
                AND   sbox_bits <= size_of_plain_text   
                AND   "sbox_bits" is a multiple of "size_of_plain_text"

        OUTPUT:
            Print the bits which give the maximum bias
            Value of the bias

    Sample Input:
        4
        9
        0 3 6 1 4 7 2 5 8   ## 0 mapped to 0, 1 mapped to 3, 2 mapped to 6 ...
        3
        0 2 4 6 3 1 7 5     ## 0->0, 1->2, 2->4, ... substitution

*/

    int32_t number_of_stages, size_of_plain_text;  // T, N
    cin >> number_of_stages >> size_of_plain_text;

    vector<int32_t> permutation_arr(size_of_plain_text);
    for (auto i: urange<int32_t>(size_of_plain_text)) cin >> permutation_arr[i];

    int32_t sbox_bits;  // S
    cin >> sbox_bits;
    vector<uint32_t> sbox_arr(1 << sbox_bits);
    for (auto i: urange<int32_t>(sbox_arr.size())) cin >> sbox_arr[i];

    cout << "number_of_stages = " << number_of_stages << endl;
    // Bias has two definitions
    // 1. Bias = (Probability of getting 0) - 1/2
    // 2. Bias = Number of occurrences of 0 in the XOR of input and output columns

    // Fox "sbox_bits = 16", size of "sbox_mapper = pow(pow(2,n)-1, 2) = 4294836225"
    //     higher 16 bits = input columns
    //     lower 16 bits = output columns
    map<uint32_t, float> sbox_bias_mapper = generate_sbox_bias_mapping(sbox_arr, sbox_bits);

    struct MaxBiasedPath maxBiasedPath(number_of_stages, size_of_plain_text, sbox_bits);
    // TODO: fill the vectors
    maxBiasedPath.find_path()

    // cout << "AES S-Box is:" << endl;
    // cout << hex;
    // for (int32_t i = 0; i < 16; ++i) {
    //     for (int32_t j = 0; j < 16; ++j) {
    //         cout << static_cast<uint32_t>(aes_sbox[i * 16 + j]) << "\t";
    //     }
    //     cout << endl;
    // }
    // cout << dec;

    return 0;
}
