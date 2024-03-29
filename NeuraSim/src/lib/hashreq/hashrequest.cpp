// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "hashrequest.h"
#include "../debugger/debugger.h"

using namespace std;

HashRequest::HashRequest(	GlobalCounters *gcp,
                        int src_unit_uid,
                        int dest_unit_uid,
                        long long a_data_addr,
                        long long b_data_addr,
                        long long b_col_id_addr,
                        long long count,
                        long long table_id) {
    this->gcp = gcp;
    this->src_unit_uid = src_unit_uid;
    this->dest_unit_uid = dest_unit_uid;
    this->id = this->gcp->generate_hash_req_id();
    this->a_data_addr = a_data_addr;
    this->b_data_addr = b_data_addr;
    this->b_col_id_addr = b_col_id_addr;
    this->count = count;
    this->table_id = table_id;
    this->state = 0;
    this->all_ready = false;
    this->a_data_ready = false;
    this->b_data_ready = false;
    this->b_col_id_ready = false;
    this->a_data = -1;
    this->b_data = -1;
    this->b_col_id = -1;
    this->mult_result = -1;
    this->reg_file_time = 0;
    // Type is inherited from the base class InstX
    this->type = INSTX_TYPE_HASH_REQ;
    this->dest_unit_uid_ptr = &this->dest_unit_uid;
    this->state = HR_NOT_STARTED;
    this->error_printed = false;
    this->ready_to_process = true;
}

HashRequest::~HashRequest() {
}

string HashRequest::state_in_text() {
    switch (this->state) {
        case HR_NOT_STARTED:
            return "HR_NOT_STARTED";
        case HR_LOAD_A_DATA_ADDR:
            return "HR_LOAD_A_DATA_ADDR";
        case HR_LOAD_B_DATA_ADDR:
            return "HR_LOAD_B_DATA_ADDR";
        case HR_LOAD_B_COL_ID_ADDR:
            return "HR_LOAD_B_COL_ID_ADDR";
        case HR_MULTIPLY:
            return "HR_MULTIPLY";
        case HR_HASH:
            return "HR_HASH";
        case HR_ACCUMULATE:
            return "HR_ACCUMULATE";
        case HR_EVICT:
            return "HR_EVICT";
        case HR_STORE:
            return "HR_STORE";
        case HR_COMPLETED:
            return "HR_COMPLETED";
        default:
            return "UNKNOWN";
    }

}

void HashRequest::print() {
    cout << "********** HashRequest **********" << endl;
    cout << "ID: " << this->id << endl;
    cout << "Src Unit UID: " << this->src_unit_uid << endl;
    cout << "Dest Unit UID: " << this->dest_unit_uid << endl;
    cout << "A Data Addr: " << this->a_data_addr << endl;
    cout << "B Data Addr: " << this->b_data_addr << endl;
    cout << "B Col ID Addr: " << this->b_col_id_addr << endl;
    cout << "Count: " << this->count << endl;
    cout << "Table ID: " << this->table_id << endl;
    cout << "State: " << this->state_in_text() << endl;
    set_TF_printer();
    cout << "All Ready: " << this->all_ready << endl;
    cout << "A Data Ready: " << this->a_data_ready << endl;
    cout << "B Data Ready: " << this->b_data_ready << endl;
    cout << "B Col ID Ready: " << this->b_col_id_ready << endl;
    reset_TF_printer();
    cout << "A Data: " << this->a_data << endl;
    cout << "B Data: " << this->b_data << endl;
    cout << "B Col ID: " << this->b_col_id << endl;
    cout << "*********************************" << endl;
}

long long HashRequest::get_id() {
    return this->id;
}

bool HashRequest::is_all_ready() {
    return this->all_ready;
}

bool HashRequest::is_ready_to_process() {
    return this->ready_to_process;
}

void HashRequest::inc_reg_file_time() {
    this->reg_file_time++;
    if (this->reg_file_time > 100000 && this->error_printed == false) {
        cout << "HashRequest spent more than 100000 cycles in core" << endl;
        this->print();
        this->error_printed = true;
        // exit(1);
    }
}

void HashRequest::set_all_ready() {
    if (this->a_data_ready == true && 
        this->b_data_ready == true && 
        this->b_col_id_ready == true) {
        this->all_ready = true;
        }
    else {
        this->all_ready = false;
        }
}

void HashRequest::set_a_data_ready() {
    this->a_data_ready = true;
    this->set_all_ready();
}

void HashRequest::set_b_data_ready() {
    this->b_data_ready = true;
    this->set_all_ready();
}

void HashRequest::set_b_col_id_ready() {
    this->b_col_id_ready = true;
    this->set_all_ready();
}

int HashRequest::get_b_col_id() {
    return this->b_col_id;
}

long long HashRequest::get_table_id() {
    return this->table_id;
}

int HashRequest::get_mult_result() {
    return this->mult_result;
}

long long HashRequest::get_count() {
    return this->count;
}

int HashRequest::get_src_unit_uid() {
    return this->src_unit_uid;
}

int HashRequest::get_dest_unit_uid() {
    return this->dest_unit_uid;
}

void HashRequest::set_dest_unit_uid(int dest_unit_uid) {
    this->dest_unit_uid = dest_unit_uid;
}

long long HashRequest::get_b_col_id_addr() {
    return this->b_col_id_addr;
}
