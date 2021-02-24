#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

using namespace std;

int mod(const int& a, const int& b)
{
    int r = a % b;
    return r < 0 ? r + b : r;
}

// modulo exponentation
int mod_exp(const int& num, const int& exp, const int& modulo)
{
    int result = 1;

    if(modulo == 1) return 0;

    for(int i = 0; i < exp; i++){
        result = mod((result * num), modulo);
    }

    return result;
}

// It is used because sometimes the number is too big.
int mod_str(const string& num_str, const int& modulo){
    if(modulo == 1) return 0;

    int digits_num  = num_str.size();

    int mod_exp_res = 1;
    int result      = mod(num_str[digits_num - 1] - '0', modulo);

    for(int i = digits_num - 2; i >= 0; i--){
        mod_exp_res = mod((mod_exp_res * 10), modulo);
        int factor2 = mod(num_str[i] - '0', modulo);

        result += mod_exp_res * factor2;
    }

    return mod(result, modulo);
}

// because its big-endian, thus, it needs reverse
int reverseInt (int i)
{
    unsigned char c1, c2, c3, c4;

    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

// just getting the ceil(arr.size / 2) element of the 
// sorted array
const double findMedian(const vector<double>& arr){
    vector<double> arr_copy = arr;

    if(arr.size() == 1) return arr[0];
    
    sort(arr_copy.begin(), arr_copy.end());

    return arr_copy[(int) ceil(((double) arr_copy.size()) / 2.0)];
}
