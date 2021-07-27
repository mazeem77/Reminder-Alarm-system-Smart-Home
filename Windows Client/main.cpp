#include<iostream>
using namespace std;
#include<string>
#include<iomanip>
#include<stdlib.h>
#include<Windows.h>
#include"SerialPort.h"

char incomingData[MAX_DATA_LENGTH];

const char* port = "\\\\.\\COM5";       //port name "////.//<portname>"

int main() {
	ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
	system("color B0");
	char data[100], dec, dec2;
	SerialPort arduino(port);

	cout << "Still Searching...";
	while (!arduino.isConnected())
	{
		cout << '.';
		Sleep(500);
	}

	//checking the status of connection whether the connection is established or not
	if (arduino.isConnected()) {
		cout << "\nConnection is established!" << endl << endl;
	}
	else {
		cout << "Choose correct port name" << endl << endl;
	}
	cout << "Do you want to proceed (Y/N): ";
	dec = getchar();
	if (dec == 'y' && arduino.isConnected())
	{
		do
		{
			system("CLS");
			cout << setw(80) << "Welcome to Smart Systems\n" << endl;
			cout << "Following are the commands with their operations:\n" << endl;
			cout << setw(60) << "(1) led1_on: Turns LED 1 ON" << setw(40) << "(2) led1_off: Turns LED 1 OFF\n" << endl;
			cout << setw(60) << "(3) led2_on: Turns LED 2 ON" << setw(40) << "(4) led1_off: Turns LED 2 OFF\n" << endl;
			cout << "\nEnter the command you want to do: ";
			cin >> data;

			char* serialarray = new char[101];
			copy(data, data + 100, serialarray);
			serialarray[strlen(data)] = '\n';

			arduino.writeSerialPort(serialarray, MAX_DATA_LENGTH);

			delete[] serialarray;
			cout << endl << "\nSending";
			for (int i = 0; i < 10; i++)
			{
				cout << '.';
				Sleep(700);
			}
			cout << endl << "\nSent";
			cout << endl <<"\nDo you want to do it again (Y/N): ";
			getchar();
			dec2 = getchar();

		} while (arduino.isConnected() && dec2 != 'n');
	}
	if (dec == 'y' && !arduino.isConnected())
		cout << "\nConnection is not established!\n";
	cout << setw(50) << endl <<"Exiting";
	for (int i = 0; i < 10; i++)
	{
		cout << '.';
		Sleep(200);
	}
	return 0;
}
