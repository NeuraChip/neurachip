#ifndef HASH_REQUEST_H
#define HASH_REQUEST_H

#include <iostream>

#include "../counters/globalcounters.h"
#include "../instx/instx.h"

using namespace std;

// MULT_HASH_ACC(a_data_addr, b_data_addr, b_col_id_addr, count, table_id)
// A single hash request
class HashRequest: public InstX {
    public:
        // Constructor
        // @param id: Hash Request ID
        HashRequest(	GlobalCounters *gcp,
                        int src_unit_uid,
                        int dest_unit_uid,
                        long long a_data_addr,
                        long long b_data_addr,
                        long long b_col_id_addr,
                        long long count,
                        long long table_id);
        ~HashRequest();
        void print();
        long long get_id();
        bool all_ready;
        bool a_data_ready;
        bool b_data_ready;
        bool b_col_id_ready;
        bool is_all_ready();
        void set_all_ready();
        void set_a_data_ready();
        void set_b_data_ready();
        void set_b_col_id_ready();
        void set_dest_unit_uid(int dest_unit_uid);
        int get_b_col_id();
        long long get_table_id();
        int get_mult_result();
        long long get_count();
        int get_dest_unit_uid();
        int get_src_unit_uid();
        void inc_reg_file_time();
        bool is_ready_to_process();
        long long get_b_col_id_addr();
        // Hash Request State
        // 0: Not started // In Core
        // 1: LOAD a_data_addr // In Core
        // 2: LOAD b_data_addr // In Core
        // 3: LOAD b_col_id_addr // In Core
        // 4: Multiply A and B // In Core
        // 5: Hash(result, count, table_id) // In Core
        // 6: Accumulate // In SPAD
        // 7: Evict // In SPAD
        // 8: Store // In Writer
        // 9: Completed // In Writer
        int state;
        string state_in_text();
        // Hash Request ID
        long long id;
        // Global Counters
        GlobalCounters *gcp;
        // Source unit uid
        int src_unit_uid;
        // Destination unit uid
        int dest_unit_uid;
        // A Data Address
        long long a_data_addr;
        // B Data Address
        long long b_data_addr;
        // B Column ID Address
        long long b_col_id_addr;
        // Count
        long long count;
        // Table ID
        long long table_id;
        // A Data
        int a_data;
        // B Data
        int b_data;
        // B Column ID
        int b_col_id;
        // Multiplication Result
        int mult_result;
        // Time spent in Register File
        int reg_file_time;
        // Error message printed
        bool error_printed;
        // Hash Request ready to process
        bool ready_to_process;
    private:
};



#endif // !HASH_REQUEST_H