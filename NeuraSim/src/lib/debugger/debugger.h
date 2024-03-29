// Created by Kaustubh Shivdikar
//
// (C) All Rights Reserved

#ifndef DEBUGGER_H
#define DEBUGGER_H

// #define RED_START "\e[1;31m"
// #define GREEN_START "\e[1;32m"
// #define YELLOW_START "\e[1;33m"
// #define CYAN_START "\e[1;36m"
// #define UNDERLINE_START "\e[1;4m"
// #define COLOR_RESET "\e[0m"
// #define FATAL_ERROR "\e[1;31m\e[1;4m\e[5mFATAL ERROR:\e[25m\e[0m "
// #define SANITY_CHECK "\e[1;33m\e[1;4mSANITY CHECK:\e[0m "
// #define RED( x ) "\e[1;31m"#x"\e[0m"
// #define GREEN( x ) "\e[1;32m"#x"\e[0m"
// #define YELLOW( x ) "\e[1;33m"#x"\e[0m"
// #define CYAN( x ) "\e[1;36m"#x"\e[0m"
// #define WHITE( x ) "\e[1;37m"#x"\e[0m"

#ifdef ENABLE_COLORS
    #define RED_START "\e[1;31m"
    #define GREEN_START "\e[1;32m"
    #define YELLOW_START "\e[1;33m"
    #define CYAN_START "\e[1;36m"
    #define UNDERLINE_START "\e[1;4m"
    #define COLOR_RESET "\e[0m"
    #define FATAL_ERROR "\e[1;31m\e[1;4m\e[5mFATAL ERROR:\e[25m\e[0m "
    #define SANITY_CHECK "\e[1;33m\e[1;4mSANITY CHECK:\e[0m "
    #define RED( x ) "\e[1;31m" x "\e[0m"
    #define GREEN( x ) "\e[1;32m" x "\e[0m"
    #define YELLOW( x ) "\e[1;33m" x "\e[0m"
    #define CYAN( x ) "\e[1;36m" x "\e[0m"
    #define WHITE( x ) "\e[1;37m" x "\e[0m"
#else
    #define RED_START "<span class=\"text-danger\">"
    #define GREEN_START "<span class=\"text-success\">"
    #define YELLOW_START "<span class=\"text-warning\">"
    #define CYAN_START "<span class=\"text-info\">"
    #define UNDERLINE_START "<span class=\"text-danger\">"
    #define COLOR_RESET "</span>"
    #define FATAL_ERROR "<span class=\"text-danger\"> FATAL ERROR </span>: "
    #define SANITY_CHECK "SANITY CHECK: "
	// #define RED(x) "\"" #x "\""
	// #define GREEN(x) "\"" #x "\""
	// #define YELLOW(x) "\"" #x "\""
	// #define CYAN(x) "\"" #x "\""
	// #define WHITE(x) "\"" #x "\""
	#define RED(x) RED_START x COLOR_RESET
	#define GREEN(x) GREEN_START x COLOR_RESET
	#define YELLOW(x) YELLOW_START x COLOR_RESET
	#define CYAN(x) CYAN_START x COLOR_RESET
	#define WHITE(x) "<span class=\"text-light\">" x "</span>"



#endif


void red_start();
void green_start();
void yellow_start();
void cyan_start();
void underline_start();
void color_reset();
void set_TF_printer();
void reset_TF_printer();
void e_print(int);

#endif // DEBUGGER_H
