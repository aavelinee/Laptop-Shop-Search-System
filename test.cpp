#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

using namespace std;

int main()
{

	string temp = "2";
	// int a = atoi(temp);
	int lineInt= atoi(temp.c_str()); //convert the string to an integer
	cout << lineInt << endl;
}