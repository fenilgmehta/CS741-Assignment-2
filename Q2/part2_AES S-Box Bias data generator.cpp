#include <iostream>
#include <iomanip>
#include <algorithm>
#include <bitset>
#include <string>
#include <map>
#include <vector>

using namespace std;


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
template<typename T>
inline bool test_bit(T num, uint_fast8_t bit) {
    return (num >> bit) & 1;
}

// The Most Significant Bit is at index 0
// For Big-Endian format, the indexes will look like: {0, 1, 2, 3, 4, 5, 6, 7}
template<typename T>
inline bool test_bit_back(T num, uint_fast8_t bit, uint_fast8_t num_bits_len=-1) {
    if (num_bits_len == -1) num_bits_len = 8 * sizeof(T);
    return (num >> ((num_bits_len - 1) - bit)) & 1;
}

// REFER: https://github.com/fenilgmehta/Misc-Programming/blob/master/src/Range%20Iteration.cpp
namespace NumericRange{
    template<typename S>
    struct range_iterator{
        typedef std::random_access_iterator_tag iterator_category;
        typedef S value_type;       // int
        typedef S difference_type;  // int
        typedef range_iterator<S>* pointer;         // range_iterator<int>*
        typedef range_iterator<S>& reference;       // range_iterator<int>&
        typedef const range_iterator<S>& const_reference;       // const range_iterator<int>&
        
        S current;
        const S step;

        inline range_iterator(const S &f1, const S &s1): current{f1}, step{s1} {}
        inline bool operator!=(const_reference zz) const {return current < zz.current;}
        inline reference operator++(){ current += step; return *this; }  // current = (current <= l) ? current : l;
        inline reference operator+=(const difference_type& val) { current+=step*val; return *this; }
        inline difference_type operator-(const_reference zz) const { return (current-zz.current + (step-1))/step; }
        inline const value_type& operator*() const { return current; }
        inline pointer operator->() { return this; }
        inline reference operator=(const_reference zz) { current = zz.current; return *this; }
    };

    template<typename T>
    struct urange{
        T f1, l1, s1;
        inline explicit urange(T last): f1{0}, l1{last}, s1{1} {}
        inline explicit urange(T first, T last, T step=1): f1{first}, l1{last}, s1{step} {}
        inline range_iterator<T> begin() const { return range_iterator<T>(f1, s1); }
        inline const range_iterator<T> end() const { return range_iterator<T>(l1, s1); }
    };
}
using namespace NumericRange;

// REFER: https://github.com/fenilgmehta/Misc-Programming/blob/master/src/Competitive%20Programming%20Template%20(Big).cpp#L64
template<typename T, typename U> std::vector<T> MatrixVector(int n, U v){ return std::vector<T>(n, v);}
template<typename T, class... Args> auto MatrixVector(int n, Args... args){auto val = MatrixVector<T>(args...); return std::vector<decltype(val)>(n, move(val));}

map<uint32_t, float> generate_sbox_bias_mapping(vector<uint32_t> &sbox_arr, const int N_BITS) {
    // vector[i] is column "i" of the input/output
    auto sbox_input = MatrixVector<bool>(N_BITS, N_SIZE, false);
    auto sbox_output = MatrixVector<bool>(N_BITS, N_SIZE, false);

    // TODO
    for(int32_t i = 0; i < N_SIZE; ++i) {
        for(int32_t j = 0; j < N_BITS; ++j) {
            // cout << i << ", " << j << ", " 
            //      << static_cast<int32_t>(sbox_arr[i]) << ", " 
            //      << static_cast<int32_t>(test_bit_back(i, j, N_BITS)) << ", " 
            //      << static_cast<int32_t>(test_bit_back(sbox_arr[i], j, N_BITS)) << endl;
            sbox_input[j].set(i, test_bit_back(i, j, N_BITS));
            sbox_output[j].set(i, test_bit_back(sbox_arr[i], j, N_BITS));

            // sbox_input[j].set(i, test_bit_back(i, j, N_BITS));
            // sbox_output[j].set(i, test_bit_back(sbox_arr[i], j, N_BITS));
        }
    }

    cout << "AES S-Box Input and Output in binary:" << endl;
    for(int32_t i = 0; i < N_SIZE; ++i) {  // row -> i
        cout << "     ";
        for(int32_t j = 0; j < N_BITS; ++j) {  // input column -> j
            cout << ((sbox_input[j].test(i)==0) ? 0 : 1) << ' ';
        }
        cout << "---> ";
        for(int32_t j = 0; j < N_BITS; ++j) {  // output column -> j
            cout << ((sbox_output[j].test(i)==0) ? 0 : 1) << ' ';
        }
        cout << endl;
    }
    
    // Testing whether XOR can be directly used with a column or not ---> Yes, can be used :)
    // cout << (sbox_input[0] ^ sbox_input[1]) << endl;
    int input_idx_combination[N_BITS], output_idx_combination[N_BITS];
    bitset<N_SIZE> bias_calculated;

    // Columns used ---> The bias string of 0's and 1's (i.e. the XOR of all the columns used)
    map<string, string> bias_mapping;
    
    // stores the mapping for (Number of Zeros in the XOR of the columns ---> Number of occurrences)
    map<int32_t, int32_t> bias_count;
    for(int32_t i = 1; i <= N_BITS; ++i) {
        for(int32_t j = 1; j <= N_BITS; ++j) {
            // Reset the Index Combination array
            for(int32_t k = 0; k < N_BITS; ++k) {
                input_idx_combination[k] = output_idx_combination[k] = k;
            }

            // The columns of input and output which are to be used for bias calculation
            // are based on the values stored in input_idx_combination and output_idx_combination
            // at indices [0, i-1]   <-- both inclusive

            do {
                do {
                    bias_calculated.reset();
                    string key="", value="";
                    for(int32_t l = 0; l < i; ++l)  {
                        bias_calculated ^= sbox_input[input_idx_combination[l]];
                        key+="i" + to_string(input_idx_combination[l]) + " ";
                        // "i" represents input column number
                    }
                    key+=", ";
                    for(int32_t l = 0; l < j; ++l){
                        bias_calculated ^= sbox_output[output_idx_combination[l]];
                        key+="o" + to_string(output_idx_combination[l]) + " ";
                        // "o" represents output column number
                    }

                    // we use the value "256 - x" because the probability of 0's is required
                    const auto bias_value = N_SIZE - bias_calculated.count();
                    bias_count[bias_value] += 1;
                    bias_mapping[key] = bias_calculated.to_string();
                } while(next_combination(output_idx_combination, output_idx_combination+j, output_idx_combination+N_BITS));
            } while (next_combination(input_idx_combination, input_idx_combination+i, input_idx_combination+N_BITS));
        }
    }

    cout << "*** ***" << endl;
    cout << "bias_count" << endl;
    for(auto i: bias_count) {
        cout << setw(5) << i.first << "\t=\t" << i.second << endl;
    }

    cout << "*** ***" << endl;
    cout << "bias probability count" << endl;
    for(auto i: bias_count) {
        cout << setw(15) << left << (static_cast<float>(i.first - N_SIZE_BY_2) / N_SIZE) << "\t=\t" << i.second << endl;
    }

    cout << "*** ***" << endl;
    cout << "bias_mapping" << endl;
    for(auto i: bias_mapping) {
        const auto cnt = count(begin(i.second), end(i.second), '0');
        cout << setw(16) << i.first << "\t=\t" << cnt << " OR " << (static_cast<float>(cnt - N_SIZE_BY_2) / N_SIZE) << " OR " << i.second << endl;
    }
}

const int32_t N_BITS = 8;
const int32_t N_SIZE = 1 << N_BITS;  // 256
const int32_t N_SIZE_BY_2 = static_cast<float>(N_SIZE >> 1);  // 128

// For understanding the concept:
//     REFER: http://www.csc.kth.se/utbildning/kth/kurser/DD2448/krypto11/handouts/lec04.pdf
//     REFER: https://en.wikipedia.org/wiki/Substitution%E2%80%93permutation_network
int main(){
/*
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
            2 <= size_of_plain_text
            1 <= sbox_bits <= 16   
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
        0 2 4 6 3 1 7 8     ## 0->0, 1->2, 2->4, ... substitution

*/
    uint32_t number_of_stages, size_of_plain_text;  // T, N
    cin>>number_of_stages >> size_of_plain_text;
    
    vector<uint32_t> permutation_arr(size_of_plain_text);
    for(auto i: urange<int32_t>(size_of_plain_text)) cin>>permutation_arr[i];

    uint32_t sbox_bits;  // S
    cin>>sbox_bits;
    vector<uint32_t> sbox_arr(1 << sbox_bits);
    
    // Bias has two definitions
    // 1. Bias = (Probability of getting 0) - 1/2
    // 2. Bias = Number of occurrences of 0 in the XOR of input and output columns

    // Fox "sbox_bits = 16", size of "sbox_mapper = pow(pow(2,n)-1, 2) = 4294836225"
    //     higher 16 bits = input columns
    //     lower 16 bits = output columns
    map<uint32_t, float> sbox_bias_mapper = generate_sbox_bias_mapping();


    // TODO: reuse/remove the below things
    uint8_t aes_sbox[N_SIZE] = {};


    cout << "AES S-Box is:" << endl;
    cout << hex;
    for(int32_t i = 0; i < 16; ++i) {
        for(int32_t j = 0; j < 16; ++j) {
            cout << static_cast<uint32_t>(aes_sbox[i * 16 + j]) << "\t";
        }
        cout << endl;
    }
    cout << dec;

    return 0;
}
