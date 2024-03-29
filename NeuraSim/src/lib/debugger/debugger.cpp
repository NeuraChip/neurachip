// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved


#include "debugger.h"
#include "../functions.h"

using namespace std;


void red_start () {
cout << "\e[1;31m";
}

void green_start() {
cout << "\e[1;32m";
}

void yellow_start() {
cout << "\e[1;33m";
}

void cyan_start() {
cout << "\e[1;36m";
}

void underline_start() {
cout << "\e[1;4m";
}

void color_reset() {
cout << "\e[0m";
}

void set_TF_printer() {
	cout << boolalpha;
}

void reset_TF_printer() {
	cout << noboolalpha;
}

void e_print(int error_code) {
	cout << "************** " << CYAN_START << "ERROR CODE: " << RED_START << error_code << COLOR_RESET << " **************" << endl;
	cout << YELLOW_START;
	switch (error_code)
	{
	case 1:
		cout << "Vertex UID not found in uid to port map." << endl;
		cout << "Check if neighbors list is populated." << endl;
		break;
	
	default:
		cout << "Zero error code received" << endl;
		break;
	}
	cout << COLOR_RESET;
	cout << "************** " << CYAN_START << "ERROR DESCRIPTION" << COLOR_RESET << " **************" << endl;
}