//
//  main.cpp
//  kaprekar-numbers
//
//  Created by  Mike Emery on 11/3/16.
//  Copyright © 2016 Mike Emery. All rights reserved.
//  https://www.reddit.com/r/dailyprogrammer/comments/5aemnn/20161031_challenge_290_easy_kaprekar_numbers/

#include <iostream>
#include <string>
#include <math.h>

using namespace std;

bool isKaprekar(int value) {
    uint64_t sq = pow(value, 2);
    
    string s = to_string(sq);
    bool result = false;
    
    for(int split = 1; split < s.length(); ++split) {
        string str1 = s.substr(0, split);
        string str2 = s.substr(split);
        
        int num1 = atoi(str1.c_str());
        int num2 = atoi(str2.c_str());
        
        if (num1 + num2 == value && num1 > 0 && num2 > 0) {
            result = true;
            break;
        }
    }
    
    return result;;
}

void computeKaprekarForRange(int lower, int higher) {
    for(int i = lower; i <= higher; ++i) {
        if (isKaprekar(i)) {
            cout << i << " ";
        }
    }
    
    cout << endl;
}

int main(int argc, const char * argv[]) {
    // insert code here...
    cout << "Enter range: ";
    string line;
    
    
    if (getline(cin, line)) {
        char* str = const_cast<char *>(line.c_str());
        char* pch = strtok(str, " ");
        
        int lower(atoi(pch));
        int upper(atoi(strtok(NULL, " ")));
        computeKaprekarForRange(lower, upper);
        cout << "got it!" << endl;
    }
}
