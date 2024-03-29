#ifndef HASH_LINE_H
#define HASH_LINE_H

#include <iostream>

using namespace std;

class HashLine {
    public:
        HashLine(long long col_id, int data, long long count);
        ~HashLine();
        void accumulate_data(int data);
        long long get_col_id();
        int get_data();
        long long get_count();
    private:
        // Map of Column IDs to Hash Requests
        long long col_id;
        int data;
        long long count;
};






#endif // !HASH_LINE_H

