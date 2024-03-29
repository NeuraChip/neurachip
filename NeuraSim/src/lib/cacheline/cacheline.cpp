// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "cacheline.h"


using namespace std;

CacheLine::CacheLine(	int payload,
						long long address,
						long long accessed_at_cycle) {
	// Constructor
	this->payload = payload;
	this->address = address;
	this->accessed_at_cycle = accessed_at_cycle;
}

CacheLine::~CacheLine() {
	// Destructor
}
