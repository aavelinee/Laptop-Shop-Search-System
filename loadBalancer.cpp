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

struct format
{
	string field;
	string data;
};

bool has_sorting(int inputSize, vector<string> input)
{

	bool has_sort = false;
	for (int i=0; i<inputSize; i++)
	{
		cout << "argv " << i <<  ":" << input[i] << endl;
		if(input[i] == "ascend" || input[i] ==  "descend" )
		{
			has_sort = true;
			break;
		}
	}
	return has_sort;
}

void print_file(ifstream &fileIn)
{
	string s;
	while(!fileIn.eof())
	{
   		fileIn >> s;
   		cout << s << endl;
	}
}

int getworkers (vector < string > input)
{
	int num_of_workers;
	int inputSize = sizeof(input);
	cout<< inputSize << endl;

	for(int i=0; i<inputSize; i++)
		if( input[i] == "prc_cnt")
		{
			num_of_workers = stoi(input[i + 2]);
			break;
		}
	return num_of_workers;
}

void split(string strToSplit, vector <string> &input, char delimeter)
{
    stringstream ss(strToSplit);
    string item;
    while (getline(ss, item, delimeter))
       input.push_back(item);
}

void fill(vector <string> input, string &dir, string &prc_cnt, 
	string &sort, string &sort_field, vector <string> &filter, vector <string> &filter_field)
{
	for(int i=0; i<input.size(); i++)
		if(input[i] == "dir")
		{
			dir = input[i+2];
			input.erase(input.begin()+i, input.begin() + i + 3);
			break;
		}

	for(int i=0; i<input.size(); i++)
		if(input[i] == "prc_cnt")
		{
			prc_cnt = input[i+2];
			input.erase(input.begin()+i, input.begin() + i + 4);
			break;
		}

	for(int i=0; i<input.size(); i++)
		if(input[i] == "ascend" || input[i] ==  "descend" )
		{
			sort = input[i];
			sort_field = input[i-2];
			input.erase(input.begin()+i-2, input.begin() + i+2);
			break;
		}
	for(int i=0; i<input.size(); i+=4)
	{
		// struct format temp;

		// temp.field = input[i];
		// temp.data = ;
		filter_field.push_back(input[i]);
		filter.push_back(input[i+2]);
	}
}

void get_directory( vector <string> &dirNames, string dir)
{
	DIR *directory;
	struct dirent *ent;
	string strAddr;
	const char* charAddr = "/home/eileen/Documents/OS/CourseProjects/2/";
	strAddr = charAddr + dir;
	const char* address = strAddr.c_str();

	if ((directory = opendir (address)) != NULL)
	{
	/* print all the files and directories within directory */
		while ((ent = readdir (directory)) != NULL)
		{
			if(strcmp(ent->d_name,".") && strcmp(ent->d_name,".."))
			{
				dirNames.push_back(ent->d_name);
				cout << "dirnames : " << ent->d_name << endl;
			}
		}
		closedir (directory);
	}
	else
	{
	/* could not open directory */
		perror ("could not open directory");
		// return EXIT_FAILURE;
		exit(1);
	}
}

int main(int argc, char* argv[])
{
    vector <string> tokens;

    stringstream check;
    string line;

    vector <string> input;
    int inputSize;
	bool has_sort;
	int num_of_workers;

	vector < vector <int> > unnamedPipes;

	string dir;
	string prc_cnt;
	string sort;
	string sort_field;
	vector <string> filter;
	vector <string> filter_field;

	vector <string> dirNames;


    while(true) 
	{
		getline(cin, line);
		split(line, input, ' ');

		inputSize = input.size();
		// has_sort = has_sorting(inputSize, input);


		num_of_workers = getworkers(input);

		fill(input, dir, prc_cnt, sort, sort_field, filter, filter_field);

		vector < vector <const char*> > divided_dirNames;
		vector <pid_t> workerId;


		for(int i=0 ; i < atoi(prc_cnt.c_str()); i++)
		{
			vector <const char*> temp;
			// temp.push_back("");
			divided_dirNames.push_back(temp);
		}

		char* execArg [100];

		// for(int i=0; i<filter.size(); i++)
		// 	cout << "filter " << i << ": " << filter[i] << endl;


		// for(int i=0; i<filter_field.size(); i++)
		// 	cout << "filter_field " << i << ": " << filter_field[i] << endl;

		if(inputSize <= 0)
		{
			perror("error in arguments: negegive number of arguments");
			return 0;
			// exit(1);
		}

		else if(input[0] == "q")
			break;

		else if(inputSize < 7)
		{
			perror("error in arguments: too few arguments");
			return 0;
			// exit(1);		
		}

		// else if(inputSize == 7)
		// {
		// 	// no sorting no filtering
		// 	ifstream inFile;
		// 	inFile.open("sampleDataset.txt");
		// 	print_file(inFile);
		// }

		else
		{
			pid_t presenterId = fork();
			if(presenterId == 0)
			{
				// for sorting -> exec presenter
				execl("/home/eileen/Documents/OS/CourseProjects/2/presenter", 
					"presenter", "g++", sort_field.c_str(), sort.c_str(), (char*)0);
			}
			else
			{
				// for other works
				get_directory(dirNames, dir);

				for(int i=0; i<dirNames.size(); i++)
					divided_dirNames[i%atoi(prc_cnt.c_str())].push_back(dirNames[i].c_str());


				for(int i=0; i<num_of_workers; i++)
				{
					int fd[2];
					if(pipe(fd) < 0)
					{
						perror("error in pipe");
						return 0;
					}
					else
					{
						pid_t pid = fork();
						vector <int> vtemp;
						for(int j=0; j<2; j++)
							vtemp.push_back(fd[j]);
						unnamedPipes.push_back(vtemp);

						if(pid == 0)
						{
							//for filtering -> exec worker
							string exec_msg;
							for(int j=0; j<filter.size(); j++)
							{
								exec_msg += filter_field[j];
								exec_msg += "#";
								exec_msg += filter[j];
								exec_msg += "#";
							}
							cerr << "exec_msg : " << exec_msg << endl;

							close(fd[1]);
							execl("/home/eileen/Documents/OS/CourseProjects/2/worker", 
								"worker", "g++", to_string(fd[0]).c_str(), dir.c_str(), 
								exec_msg.c_str(), (char*)0);
						}

						else if(pid > 0)
						{
							cerr << "piiiiiiiiiiid: " << pid << endl;
							workerId.push_back(pid);
							close(fd[0]);
							string pipe_msg;

							for(int j=0; j<divided_dirNames[i].size(); j++)
							{
								pipe_msg += divided_dirNames[i][j];
								pipe_msg += "#";
							}
							pipe_msg += "\0";
							write(fd[1], pipe_msg.c_str(), strlen(pipe_msg.c_str())+1);
						}
						else
							perror("worker fork error");
					}
				}

				string pipe_name = "presenter_pipe";
				int for_presenter_fd;
				// mkfifo(pipe_name.c_str(), 0666);
				string worker_pids;
				for(int i=0; i<workerId.size(); i++)
				{
					worker_pids += to_string(workerId[i]);
					worker_pids += "#";
				}
				worker_pids += '\0';

				for_presenter_fd = open(pipe_name.c_str(), O_WRONLY);
				int n = write(for_presenter_fd, worker_pids.c_str(), strlen(worker_pids.c_str())+1);
				for(int i=0; i<workerId.size(); i++)
				{
					int pid = wait(NULL);
					write(for_presenter_fd, to_string(pid).c_str(), strlen(to_string(pid).c_str())+1);
				}
				
			}
		}
		input.clear();
		filter.clear();
	}
}
