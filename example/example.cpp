#include "example.h"

int main()
{
	std::cout << "Waiting..." << std::endl;

	std::string pipeName = "\\\\.\\pipe\\HMDPipe";

	hmdPipe = CreateNamedPipeA(pipeName.c_str(),
		PIPE_ACCESS_DUPLEX,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
		1,
		1024 * 16,
		1024 * 16,
		NMPWAIT_USE_DEFAULT_WAIT,
		NULL);
	if (hmdPipe != INVALID_HANDLE_VALUE)
	{
		//if pipe was successfully created wait for a connection
		if (ConnectNamedPipe(hmdPipe, NULL) != FALSE)   // wait for someone to connect to the pipe
		{
			//when pipe is connected, send number of pipes and driversmoothfactor to our connected driver
			std::cout << "Connected!" << std::endl;

		}
	}
	else
	{
		std::cout << "Could not connect!" << std::endl;
		return 1;
	}

	for (int i = 0; i < pipeNum; i++)
	{
		//create a pipe with given name and index
		std::string pipeName = "\\\\.\\pipe\\TrackPipe" + std::to_string(i);
		HANDLE pipe;
		pipe = CreateNamedPipeA(pipeName.c_str(),
			PIPE_ACCESS_DUPLEX,
			PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT,   // FILE_FLAG_FIRST_PIPE_INSTANCE is not needed but forces CreateNamedPipe(..) to fail if the pipe already exists...
			1,
			1024 * 16,
			1024 * 16,
			NMPWAIT_USE_DEFAULT_WAIT,
			NULL);
		if (pipe != INVALID_HANDLE_VALUE)
		{
			//if pipe was successfully created wait for a connection
			if (ConnectNamedPipe(pipe, NULL) != FALSE)   // wait for someone to connect to the pipe
			{
				//when pipe is connected, send number of pipes and driversmoothfactor to our connected driver
				std::string s = std::to_string(pipeNum) + " 0";

				std::cout << "Connected tracker " << i << "!" << std::endl;

				//write our data to pipe
				WriteFile(pipe,
					s.c_str(),
					(s.length() + 1),   // = length of string + terminating '\0' !!!
					&dwWritten,
					NULL);

			}
		}
		else
		{
			std::cout << "Could not connect!" << std::endl;
			return 1;
		}
		//add our pipe to our global list of pipes
		hpipe.push_back(pipe);
	}

	for (;;)
	{
		//std::cout << "frame" << std::endl;
		if (ReadFile(hmdPipe, buffer, sizeof(buffer) - 1, &dwRead, NULL) != FALSE)		//Wait untill HMD position data is availible, then read data to buffer.
		{
			buffer[dwRead] = '\0'; //add terminating zero
				//convert our buffer to string
			std::string s = buffer;

			//first three variables are a position vector
			double a;
			double b;
			double c;

			//second four are rotation quaternion
			double qw;
			double qx;
			double qy;
			double qz;

			std::cout << s << std::endl;		//print the rotation and position data to console
	
			//convert to string stream
			std::istringstream iss(s);

			//read to our variables
			iss >> a;
			iss >> b;
			iss >> c;
			iss >> qw;
			iss >> qx;
			iss >> qy;
			iss >> qz;

			//a, b, c is now headset position in meters, qw, qx, qy, qz is heatset rotation in quaternion.

			//do processing here

			if (waitFrames <= 0)		//after starting program, wait for some time
			{
				//send the position data for each tracker. In this example, i have 4 trackers (pipeNum variable) and will put one left, one right, one in front and one behind the headset.
				Send(0, a + 1, b, c, qw, qx, qy, qz);
				Send(1, a - 1, b, c, qw, qx, qy, qz);
				Send(2, a, b, c + 1, qw, qx, qy, qz);
				Send(3, a, b, c - 1, qw, qx, qy, qz);
			}
			else
			{
				waitFrames--;
			}

		}
	}

	return 0;
}

void Send(int id, double a, double b, double c, double qw, double qx, double qy, double qz)
{
	std::string s;
	s = std::to_string(a) +
		" " + std::to_string(b) +
		" " + std::to_string(c) +
		" " + std::to_string(qw) +
		" " + std::to_string(qx) +
		" " + std::to_string(qy) +
		" " + std::to_string(qz) + "\n";

	//send the string to our driver

	WriteFile(hpipe[id],
		s.c_str(),
		(s.length() + 1),   // = length of string + terminating '\0' !!!
		&dwWritten,
		NULL);
}