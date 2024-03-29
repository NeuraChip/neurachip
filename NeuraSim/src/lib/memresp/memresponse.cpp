// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "memresponse.h"
#include "../debugger/debugger.h"
#include "../config.h"

using namespace std;

// Constructor
// @param id: Memory Response ID
MemResponse::MemResponse(	GlobalCounters *gcp,
							int src_unit_uid,
							int dest_unit_uid,
							int resp_type,
							int src_address,
							int size,
							int payload,
							long long src_mem_req_id,
							MemRequest *src_mem_req,
							HashRequest *src_hash_req) {
	this->gcp = gcp;
	this->src_unit_uid = src_unit_uid;
	this->dest_unit_uid = dest_unit_uid;
	this->resp_type = resp_type;
	this->id = this->gcp->generate_mem_resp_id();
	this->src_address = src_address;
	this->size = size;
	this->payload = payload;
	this->src_mem_req_id = src_mem_req_id;
	this->src_mem_req = src_mem_req;
	this->src_hash_req = src_hash_req;
	this->type = INSTX_TYPE_MEM_RESP;
	this->dest_unit_uid_ptr = &this->dest_unit_uid;
}

MemResponse::~MemResponse() {
}

void MemResponse::set_operand_id(int operand_id) {
	this->operand_id = operand_id;
}

int MemResponse::get_operand_id() {
	return (this->operand_id);
}

string MemResponse::resp_type_in_text() {
	switch (this->resp_type)
	{
	case MEM_RESP_READ:
		return ("MEM_RESP_READ");
	case MEM_RESP_READ_NO_CACHE:
		return ("MEM_RESP_READ_NO_CACHE");
	case MEM_RESP_WRITE:
		return ("MEM_RESP_WRITE");
	case MEM_RESP_MOVE:
		return ("MEM_RESP_MOVE");
	case MEM_RESP_COPY:
		return ("MEM_RESP_COPY");
	case MEM_RESP_SCATTER:
		return ("MEM_RESP_SCATTER");
	case MEM_RESP_GATHER:
		return ("MEM_RESP_GATHER");
	case MEM_RESP_BCAST:
		return ("MEM_RESP_BCAST");
	case MEM_RESP_REDUCE:
		return ("MEM_RESP_REDUCE");
	default:
		return ("UNKNOWN");
	}
}

string MemResponse::operand_id_in_text() {
	switch (this->operand_id)
	{
	case HR_OPERAND_A_DATA_ADDR:
		return ("HR_OPERAND_A_DATA_ADDR");
	case HR_OPERAND_B_DATA_ADDR:
		return ("HR_OPERAND_B_DATA_ADDR");
	case HR_OPERAND_B_COL_ID_ADDR:
		return ("HR_OPERAND_B_COL_ID_ADDR");
	default:
		return ("UNKNOWN");
	}
}

void MemResponse::print() {
	cout << "*** MemResponse ***" << endl;
	cout << "ID: " << this->id << endl;
	cout << "Src Unit UID: " << this->src_unit_uid << endl;
	cout << "Dest Unit UID: " << this->dest_unit_uid << endl;
	cout << "Resp Type: " << this->resp_type_in_text() << endl;
	cout << "Src Address: " << this->src_address << endl;
	cout << "Size: " << this->size << endl;
	cout << "Payload: " << this->payload << endl;
	cout << "Src Mem Req ID: " << this->src_mem_req_id << endl;
	cout << "Src Mem Req: " << this->src_mem_req << endl;
	cout << "Src Hash Req: " << this->src_hash_req << endl;
	cout << "Operand ID: " << this->operand_id_in_text() << endl;
	cout << "*******************" << endl;
}

int MemResponse::get_src_unit_uid() {
	return (this->src_unit_uid);
}

int MemResponse::get_dest_unit_uid() {
	return (this->dest_unit_uid);
}

long long MemResponse::get_src_address() {
	return (this->src_address);
}

int MemResponse::get_payload() {
	return (this->payload);
}

long long MemResponse::get_id() {
	return (this->id);
}