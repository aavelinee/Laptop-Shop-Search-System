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
#include <sys/wait.h>


using namespace std;

void split(string strToSplit, vector <string> &input, char delimeter)
{
    stringstream ss(strToSplit);
    string item;
    while (getline(ss, item, delimeter))
       input.push_back(item);
}

void opening(vector <string> &header, vector <vector <string> > &each_file, string address)
{
	ifstream myfile (address);
	string line;
  	if (myfile.is_open())
  	{
  		getline(myfile, line);
  		split(line, header, ' ');
    	while ( getline (myfile,line) )
    	{	
    		vector <string> file_line;
    		split(line, file_line, ' ');
    		each_file.push_back(file_line);
      		// cout << line << '\n';
    	}
    	myfile.close();
  	}
  	else cout << "Unable to open file";
}

int main(int argc, char* argv[]) //send the filename then here create a ifstream and open that file name
{	
	cerr << "welcome to worker!!" << endl;

	int fd = atoi(argv[2]);
	
	string dir = argv[3];
	// cerr << "argv3: " << argv[3] << endl;

	vector <string> filters;

	string concat_filters;
	// cerr << "argv4: " << argv[4] << endl;
	concat_filters = argv[4];
	// cerr << "concat_filters : " << concat_filters  << "  end" << endl;

	string filt;
	for(int i=0; i<concat_filters.size(); i++)
	{
		while(concat_filters[i] != '#')
		{
			filt += concat_filters[i];
			i++;
		}
		filters.push_back(filt);
		filt = "";
	}

	// for(int i=0; i<filters.size(); i++)
	// 	cerr << "filters " << i << ": " << filters[i] << endl;



	char msg[1024];
	read(fd, msg, 1024-1);
	// cout << "*********worker message: " << msg << endl;

	int i=0;
	vector <string> directs;
	string filename;
	while(msg[i] != '\0')
	{
		while(msg[i] != '#')
		{
			filename+=msg[i];
			i++;
		}
		directs.push_back(filename);
		filename = "";
		i++;
	}

  	string pipe_name = to_string(getpid());	
	int to_presenter_fd;
	mkfifo(pipe_name.c_str(), 0666);
	cerr << "pipe_name: " << pipe_name << endl;
	to_presenter_fd = open(pipe_name.c_str(), O_WRONLY);
	// cerr << "to_presenter_fd : " << to_presenter_fd << endl;
	// cerr << "directs.size : " << directs.size() << endl;
	for(int p=0; p<directs.size(); p++)
	{
		string address = "/home/eileen/Documents/OS/CourseProjects/2/";
		address += dir;
		address += "/";
		address += directs[p];
		// cerr << "direcrrrrrr : " << p << " : " << directs[p] << endl; 

	  	string line;
	  	vector < vector<string> > each_file;
	  	vector <string> header;

	  	opening(header, each_file, address); 

	  	// for(int i=0; i<each_file.size(); i++)
	  	// 	for(int j=0; j<each_file[i].size(); j++)
	  	// 		cerr << "each_file " << i << " " << j << ": " <<  each_file[i][j] << endl;


	  	vector <int> flag;
	  	vector <string> working_each_file;
	  	// working_each_file = each_file;
	  	int flag2;

	  	string filtered_data = "";
	  	for(int i=0; i<filters.size(); i+=2)
	  		flag.push_back(0);

	  	for(int i=0; i<each_file.size(); i++)
	  	{
	  		flag2 = 1;
	  		for(int j=0; j<flag.size(); j++)
	  			flag[j] = 0;
	  		for(int j=0; j<header.size(); j++)
	  		{
	  			for(int k=0; k<filters.size(); k+=2)
	  			{
	  				if(filters[k] == header[j] && filters[k+1] == each_file[i][j])
	  				{
	  					flag[k/2] = 1;
	  				}
	  			}
	  		}
	  		for(int j=0; j<flag.size(); j++)
	  			if(flag[j] == 0)
	  			{
	  				flag2 = 0;
	  				break;
	  			}
	  		if(flag2 == 1)
	  		{
	  			for(int j=0; j<each_file[i].size(); j++)
	  			{
	  				filtered_data += header[j];
	  				filtered_data += "#";
	  				filtered_data += each_file[i][j];
	  				filtered_data += "#";
	  			}
	  			filtered_data += "*";
	  		}
	  	}
	  	filtered_data += '\0';
	  	// cerr << "filtered_data : " << filtered_data << endl;
		int n = write(to_presenter_fd, filtered_data.c_str(), strlen(filtered_data.c_str())+1);
		// cerr << "to_presenter_fd in worker : " << to_presenter_fd << endl;
		// cerr << " nnnnnnn " << n << endl;
	}
	return 0;
}
