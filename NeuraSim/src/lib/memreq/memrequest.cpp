// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "memrequest.h"
#include "../debugger/debugger.h"
#include "../config.h"
#include "../hashreq/hashrequest.h"

using namespace std;

// Constructor
// @param id: Memory Request ID
MemRequest::MemRequest(	GlobalCounters *gcp,
						int src_unit_uid,
						int dest_unit_uid,
						int req_type,
						int src_address,
						int dest_address,
						int size,
						int payload,
						HashRequest *src_hash_req) {
	this->gcp = gcp;
	this->src_unit_uid = src_unit_uid;
	this->dest_unit_uid = dest_unit_uid;
	this->req_type = req_type;
	this->id = this->gcp->generate_mem_req_id();
	this->src_address = src_address;
	this->dest_address = dest_address;
	this->size = size;
	this->payload = payload;
	this->dram_processing_time = this->gcp->DRAM_CONST_LATENCY;
	// Type is inherited from the base class InstX
	this->type = INSTX_TYPE_MEM_REQ;
	this->dest_unit_uid_ptr = &this->dest_unit_uid;
	this->src_hash_req = src_hash_req;
	this->operand_id = -1;
	// if (src_hash_req == 0)
	// {
	// 	cout << FATAL_ERROR << "MemRequest::MemRequest : src_hash_req is NULL" << endl;
	// 	this->print();
	// 	exit(1);
	// }
}

MemRequest::~MemRequest() {
}

void MemRequest::print() {
	cout << "********** Memory Request [" << this->id << "] **********" << endl;
	cout << "Source Address: " << this->src_address << endl;
	cout << "Destination Address: " << this->dest_address << endl;
	cout << "Size: " << this->size << endl;
	cout << "Payload: " << this->payload << endl;
	cout << "Source Unit: " << this->gcp->uid_to_vertex_name_map[src_unit_uid] << endl;
	cout << "Destination Unit: " << this->gcp->uid_to_vertex_name_map[dest_unit_uid] << endl;
	cout << "Type: " << this->req_type << endl;
	cout << "DRAM Processing Time: " << this->dram_processing_time << endl;
	cout << "Source Hash Request: " << this->src_hash_req << endl;
	cout << "Operand ID: " << this->operand_id << endl;
	cout << "*************************************" << endl;
}

long long MemRequest::get_id() {
	return (this->id);
}

bool MemRequest::tick() {
	this->dram_processing_time--;
	if (this->dram_processing_time == 0) {
		return (true);
	}
	return (false);
}

int MemRequest::get_src_unit_uid() {
	return (this->src_unit_uid);
}

int MemRequest::get_dest_unit_uid() {
	return (this->dest_unit_uid);
}

int MemRequest::get_req_type() {
	return (this->req_type);
}

long long MemRequest::get_src_address() {
	return (this->src_address);
}

long long MemRequest::get_size() {
	return (this->size);
}

int MemRequest::get_payload() {
	return (this->payload);
}

HashRequest *MemRequest::get_src_hash_req() {
	return (this->src_hash_req);
}

void MemRequest::set_operand_id(int operand_id) {
	this->operand_id = operand_id;
}

int MemRequest::get_operand_id() {
	return (this->operand_id);
}

void MemRequest::set_payload(int payload) {
	this->payload = payload;
}

long long MemRequest::get_address() {
	if (this->req_type == MEM_REQ_READ || this->req_type == MEM_REQ_READ_NO_CACHE) {
		return (this->src_address);
	}
	else if (this->req_type == MEM_REQ_WRITE) {
		return (this->dest_address);
	}
	else {
		cout << FATAL_ERROR << "MemRequest::get_address : Invalid request type : " << this->req_type << endl;
	}
	return (this->src_address);
}

bool MemRequest::get_is_read() {
	if (this->req_type == MEM_REQ_READ || this->req_type == MEM_REQ_READ_NO_CACHE) {
		return (true);
	}
	return (false);
}

bool MemRequest::get_is_write() {
	if (this->req_type == MEM_REQ_WRITE) {
		return (true);
	}
	return (false);
}

long long MemRequest::get_dest_address() {
	return (this->dest_address);
}