#include<bits/stdc++.h>
using namespace std;

vector<bool> vector_xor(const vector<bool> &a, const vector<bool> &b) {
    if(a.size() != b.size()) return vector<bool>(0);
    vector<bool> res(a.size());
    for(size_t i = 0; i < a.size(); ++i) {
        res[i] = a[i] ^ b[i];
    }
    return res;
}

template<typename T, typename U> std::vector<T> MatrixVector(int n, U v){ return std::vector<T>(n, v);}
template<typename T, class... Args> auto MatrixVector(int n, Args... args){auto val = MatrixVector<T>(args...); return std::vector<decltype(val)>(n, move(val));}

int main(){
    uint32_t b2=123456789;  // '0b111010110111100110100010101'
    bitset<19> b1 = b2;  //      '--------0111100110100010101'
    cout << b1 << endl;

    return 0;
    auto m = MatrixVector<bool>(4, 10, false);
    cout << m[0].size() << endl;

    vector<vector<bool>> arr(4, vector<bool>(10));
    cout << arr[0].size() << endl;
    return 0;
    vector<bool> a1(5), a2(5);
    
    a1[0] = 0;
    a1[1] = 0;
    a1[2] = 1;
    a1[3] = 1;
    
    a2[0] = 0;
    a2[1] = 1;
    a2[2] = 0;
    a2[3] = 1;

    vector<bool> a3 = vector_xor(a1, a2);
    
    for(int i = 0; i < 4; ++i) {
        cout << a3[i] << " ";
    }
    cout << endl;

    return 0;
}