//Ethan Barlow
//June 11, 2017 --- started
//This is the first try at sorting out pictures based on string contents
//
//
//



/*

	The SortModule class could be a base class with error traps and file checking
	Then a derived class could be the one doing the work of sorting and copying files
	Then a second derived class could be removing the unwanted files

	A new class could be made to take in the lists of strings to search for




*/



#include<iostream>
#include<string>
#include<boost/filesystem.hpp>
#include "SortObjectCV4.h"



#include "opencv2\core\core.hpp"
#include "opencv2\highgui\highgui.hpp"
#include <opencv2\features2d.hpp>

using namespace cv;

//Testing Purposes




using namespace std;
using namespace boost::filesystem;

//This may not be needed

void menu(); //display the menu and take input, return the value inputted

char SortSearchSubMenu(vector<char> check); //display the options for sorting, and process the sorting ---- will return a character that denotes which type of files to sort
//the vector is the vector of characters to check against to see if the input is valid

char CropSubMenu(vector<char> check); //display the options for sorting ---- will return a character that denotes which type of crop to do
//options are crop in a folder, and crop in a folder and all subfolders

string get_path(); //get input from the user

char get_char(vector<char> whichArray); //gets input from the user that is used in the menu function, the char array is used to tell the menu options apart

bool checkArray(char input, vector<char> check); //used for error trapping, first argument is user input, second is the array to check against

void detectObj(path image);

//beginning of main
int main()
{
	
	cout << "\n\n\t\tWelcome to Free Foto Filter!"
		<< endl << endl << "Thank you for choosing this software. Please contact the developer with any concerns, questions, or suggestions" << endl;

	//test object detection
	//detectObj("E:\ProductImages\sample1\test\SC-A0845-NO__2.JPG");


	menu();

	
	return 0;
}//end main



//functions here
void menu()
{
	//arrays for the various menu options
	//options for the main menu
	vector<char> MainMenuChar = { 'A', 'B', 'C', 'D', 'E', 'F' };
	vector<char> SubMenuChar = { 'A', 'B' };
	vector<char> subMenuCrop = { 'A', 'B', 'C' };
	
	char processed;

	do {
		cout << "\nWhat do you want to do with this software?" << endl << endl;

		cout << "A. Sort based on filename" << endl;
		cout << "B. Find duplicate files (based on filename) -- NOT YET SUPPORTED" << endl;
		cout << "C. Crop pictures" << endl;
		cout << "D. Copy pictures - useful for making backup folders" << endl;
		cout << "E. Find pictures that may potentially have non-white backgrounds" << endl;
		cout << "F. Quit" << endl << endl;


		processed = get_char(MainMenuChar);

		switch (processed)
		{
		case 'A':
			{
				
				char typ = NULL;

				typ = SortSearchSubMenu(SubMenuChar);
				//all error trapping is taken care of in the get_char()
				//function which is called from the SortSubMenu() function


				//cout << "\nThe file path should look something like this: " << endl
					//<< "C:\\Users\\ArbitraryFolder\\AnotherArbitraryFolder" << endl << endl;

				cout << "First, enter the path (folder\filename) where the pictures are located" << endl;
				string first = get_path();

				cout << "\nSecond, enter the path where the pictures should be placed after the sorting process" << endl;
				string second = get_path();


				SortModule item(first, second);



				//cout << "Here is where the sort function should be used" << endl << endl;
								
				//cout << "Using searchDir method, a filename should be used this time" << endl;

				cout << "Please enter the path to the file that contains the list of filenames you want to search for." << endl;
				
				item.searchDir(get_path(), typ);



				break;
			}

		case 'B':
			{//find duplicates
				
				cout << endl << "This function will find all duplicate or similar files and place them into "
					<< endl << "one big folder so that you can easily choose which to keep.";

				cout << "First, enter the path (folder\filename) where the pictures are located" << endl;
				string first = get_path();

				cout << "\nSecond, enter the path where the pictures should be placed after the duplicate cleanup" << endl;
				string second = get_path();


				SortModule item(first, second);
				item.dupeClean();



				break;
			}

		case 'C':
			{
				
				char typeOfCrop = NULL;
				typeOfCrop = CropSubMenu(subMenuCrop);
				
				cout << "\n\n\t\tCropping information\\instructions:" << endl << endl;
				//cout << "You are about to crop all the pictures in a given folder." << endl;
				cout << "Please, specify where these pictures are located" << endl;
				cout << "Located: ";
				string pre_crop = get_path();

				cout << endl;
				cout << "Please, specify where the newly cropped pictures should be saved" << endl;
				
				string post_crop = get_path();

				//use the cropping method to roll through all the pictures
				SortModule item(pre_crop, post_crop);
				item.cropDir(typeOfCrop);
				
				break;
			}

		case 'D':
			{
				char typeOfCrop = NULL;

				cout << "\n\n\t\tCopying information\\instructions:" << endl << endl;
				//cout << "You are about to copy all the pictures in a given folder." << endl;
				cout << "Please, specify where these pictures are located" << endl;
				cout << "Located: ";
				string pre_crop = get_path();

				cout << endl;
				cout << "Please, specify where the copies should be saved" << endl;

				string post_crop = get_path();

				//use the cropping method to roll through all the pictures
				SortModule item(pre_crop, post_crop);
				item.copyAll();

				break;
			}

		case 'E':
			{
				cout << "Please, specify where these pictures are located" << endl;
				cout << "Located: ";
				string pre_crop = get_path();

				cout << endl;
				cout << "Please, specify where the copies should be saved" << endl;

				string post_crop = get_path();

				//use the cropping method to roll through all the pictures
				SortModule item(pre_crop, post_crop);
				item.detectObjNonWhiteBackground();
			}

		case 'F':
			{
				break;
			}

		}//end switch
	
	} while (processed != 'F');


}//end function


//display the options for sorting and process the request
char SortSearchSubMenu(vector<char> check)
{
	char input = NULL;

	cout << endl << endl << "Within the specified folder/directory would you like to search for a specific type of file?";
	cout << endl << "A. No, search for all file types";
	cout << endl << "B. Yes, search for images only (.jpg, .png, .jpeg)";
	cout << endl;

	input = get_char(check);


	return input;
}//end function


char CropSubMenu(vector<char> check)
{
	char input = NULL;

	cout << endl << endl << "There are two methods to crop pictures within a folder. Here are the options:";
	cout << endl << "A. Crop all the pictures within a folder";
	cout << endl << "B. Crop all the pictures within a folder AND crop everything\n\t in the subfolders (dumps every photo into one destination folder)";
	cout << endl << "C. Crop all the pictures within a folder AND crop everything\n\t in the subfolders (keep the directory structure) - NOT YET SUPPORTED";
	cout << endl;

	input = get_char(check); //check is a vector used for error checking

	return input;

}//end function


//get character to be used in the menu
char get_char(vector<char> array)
{
	char choice;
	bool status;

	do
	{
		cout << "Choice: ";
		cin >> choice;
		cin.ignore(); //only recognize one character

		choice = toupper(choice); //capitalize the character for later comparison
		//error checking
		if (!isalpha(choice))//if the choice is not a letter
		{
			status = false;
			cout << "\nThat is not a valid choice. Please, try again" << endl;
		}

		//-----------------------------------------------------------------------------------------------------------------------------
		else if (isalpha(choice) && checkArray(choice, array))
		{
			status = true;
			cout << "\n\n\t\t-----Processing-----" << endl << endl;
			//this code is only run if the input from the user
			//is a letter and is equal to a menu choice
		}

		else
		{
			status = false;
			cout << "\nThat is not a valid choice. Please, try again" << endl;
		}


	} while (status == false);
	
	
	
	return choice;
}


//get the string to be used as the path
string get_path()
{
	string filepath;

	cout << "Enter the path to the folder or file that you would like to use." << endl
		<< "Path: ";
	
	getline(cin, filepath);

	
	return filepath;
	
}//end function


//check to make sure that the input is a valid menu option
bool checkArray(char input, vector<char> check)
{
	bool status;

	for (int count = 0; count < check.size(); count++)
	{
		if (input == check.at(count))
		{
			status = true;
			break; //stops the for loop
		}

		else
		{
			status = false; //input does not match the vector item -> it's false
		}

	}

	return status;
}//end function

void detectObj(path image)
{
	//Read image
	string img = image.string();
	Mat pic = imread(img, IMREAD_GRAYSCALE);

	//Set up the detector with default parameters
	SimpleBlobDetector detector;

	//Detect blobs
	std::vector<KeyPoint> keypoints;
	detector.detect(pic, keypoints);

	//Draw detected blobs as red circles
	//DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures the size of the circle corresponds to the size of the blob
	Mat pic_with_keypoints;
	drawKeypoints(pic, keypoints, pic_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);

	//show blobs
	imshow("keypoints", pic_with_keypoints);
	waitKey(0);

}//end detectObj

//end file