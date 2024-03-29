// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#include "hashline.h"

#include <iostream>

using namespace std;

HashLine::HashLine(long long col_id, int data, long long count)
{
    this->col_id = col_id;
    this->data = data;
    this->count = count;
}

HashLine::~HashLine()
{
    // Destructor
}

void HashLine::accumulate_data(int data)
{
    this->data += data;
}

long long HashLine::get_col_id()
{
    return this->col_id;
}

int HashLine::get_data()
{
    return this->data;
}

long long HashLine::get_count()
{
    return this->count;
}


