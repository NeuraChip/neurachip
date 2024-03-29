// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "hashresponse.h"
#include <iostream>

using namespace std;

HashResponse::HashResponse()
{
	// Constructor
}

HashResponse::HashResponse(	GlobalCounters *gcp,
							int src_unit_uid,
							int dest_unit_uid,
							long long table_id,
							long long b_col_id,
							int mult_result,
							HashRequest *src_hash_req)
{
	// Constructor
	this->custom_constructor(gcp,
							src_unit_uid,
							dest_unit_uid,
							table_id,
							b_col_id,
							mult_result,
							src_hash_req);
}

void HashResponse::custom_constructor(	GlobalCounters *gcp,
										int src_unit_uid,
										int dest_unit_uid,
										long long table_id,
										long long b_col_id,
										int mult_result,
										HashRequest *src_hash_req)
{
	// Custom constructor
	this->gcp = gcp;
	this->id = this->gcp->generate_hash_resp_id();
	this->src_unit_uid = src_unit_uid;
	this->dest_unit_uid = dest_unit_uid;
	this->table_id = table_id;
	this->b_col_id = b_col_id;
	this->mult_result = mult_result;
	this->src_hash_req = src_hash_req;
	this->type = INSTX_TYPE_HASH_RESP;
	this->dest_unit_uid_ptr = &this->dest_unit_uid;
}

HashResponse::~HashResponse()
{
	// Destructor
}

int HashResponse::get_dest_unit_uid()
{
	return this->dest_unit_uid;
}

int HashResponse::get_src_unit_uid()
{
	return this->src_unit_uid;
}

long long HashResponse::get_table_id()
{
	return this->table_id;
}

HashRequest *HashResponse::get_src_hash_req()
{
	return this->src_hash_req;
}

int HashResponse::get_mult_result()
{
	return this->mult_result;
}