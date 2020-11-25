#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <fstream>
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <bits/stdc++.h>
#include <sstream>
#include <string>
#include <fcntl.h> 
#include <sys/stat.h>
#include <algorithm>


using namespace std;

int sort_index;

struct info
{
	string brand, model;
	int ram, hdd, price;
};

bool sortcol_ascend( const vector<string>& v1, const vector<string>& v2 ) 
{ 
	return stoi(v1[sort_index]) < stoi(v2[sort_index]); 
}

bool sortcol_descend( const vector<string>& v1, const vector<string>& v2 ) 
{ 
	return stoi(v1[sort_index]) > stoi(v2[sort_index]); 
}

int main(int argv, char* argc[])
{
	cerr << "welcome to presenter!" << endl;

	string sort_field = argc[2];
	string sort_value = argc[3];

	string pipe_name = "presenter_pipe";
	int presenter_fd;
	mkfifo(pipe_name.c_str(), 0666);
	presenter_fd = open(pipe_name.c_str(), O_RDONLY);
	char msg [1024];
	char worker_msg[1024];
	read(presenter_fd, msg, 1024);

	string temp;
	int i=0;
	vector <string> worker_pids;
	while(msg[i] != '\0')
	{	
		while(msg[i] != '#')
		{
			temp += msg[i];
			i++;
		}
		worker_pids.push_back(temp);
		temp = "";
		i++;
	}
	vector <int> worker_pids_fd;
	for(int i=0; i<worker_pids.size(); i++)
	{
		int temp;
		cerr << "worker_pids[i] : " << worker_pids[i] << endl;
		mkfifo(worker_pids[i].c_str(), 0666);
		temp = open(worker_pids[i].c_str(), O_RDONLY);
		worker_pids_fd.push_back(temp);
	}
	vector < vector <string> > filtered_info;
	for(int i=0; i<worker_pids.size(); i++)
	{
		int read_value;
		read_value = read(presenter_fd, msg, 1024);
		while(read_value == 0)
			read_value = read(presenter_fd, msg, 1024);
			
		if(worker_pids[i] == msg)
		{
			read(worker_pids_fd[i], worker_msg, 1024);
			cerr << "worker_msg : " << worker_msg << endl;
		}


		int j=0;
		string str;
		while(worker_msg[j] != '\0')
		{
			vector <string> temp;
			while(worker_msg[j] != '*')
			{
				while(worker_msg[j] != '#')
				{
					str += worker_msg[j];
					j++;
				}
				temp.push_back(str);
				str = "";
				j++;
			}
			filtered_info.push_back(temp);
			j++;
		}

		cerr << "sort_field size " << sort_field.size() << endl;
		if(sort_field.size() != 0 && sort_value.size() != 0)
		{
			for(int j=0; j<filtered_info.size(); j++)
				for(int k=0; k<filtered_info[j].size(); k++)
				{
					if(filtered_info[j][k] == sort_field)
						sort_index = k+1; 
				}
			if(sort_value == "ascend")
				sort(filtered_info.begin(), filtered_info.end(),sortcol_ascend);
			else if(sort_value == "descend")
				sort(filtered_info.begin(), filtered_info.end(),sortcol_descend);

		}
		for(int k=0; k<filtered_info.size(); k++)
			for(int j=0; j<filtered_info[k].size(); j++)
				cerr << "%%%%%" << k << " " << j << filtered_info[k][j] << endl;
	}

}