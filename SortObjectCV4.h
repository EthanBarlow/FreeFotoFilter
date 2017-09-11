#pragma once

#ifndef SORTOBJECT_H
#define SORTOBJECT_H


#include<iostream>
#include<string>
#include<fstream>
#include<boost/filesystem.hpp>
#include "opencv2\highgui\highgui.hpp"

using namespace cv;
using namespace std;
using namespace boost::filesystem;

//this will do all of the sorting and copying of files
//another object will be used to delete any pictures that are unwanted


class SortModule 
{
private:
	
	//variables
	path origin;
	path destination;
	path source;
	std::fstream errorLog; //any errors will be logged here
	string nameOfErrorFile;

	vector<string> source_list; //this will hold the list of strings that will be used to search from
	vector<path> new_folders; //this will hold a list of new directories that will be filled with pictures

	//object detection variables


	//set and get functions
	void setOrigin(string location);
	path getOrigin();

	void setDest(string location);
	path getDest();

	void checkLogExist(); //checks for the existence of an error log, and creates one if need be
	void printToLog(string error_code, string locationInCode); //prints errors into the log file

	path checkErrorFolder(); //checks for the existence of an "Error Folder", and creates one if needed
	void PicErrorLog(cv::Mat *picture, string imageName); //copies pictures that cause an "openCV assertion failed error" to an "error" folder

	bool path_check(string p); //makes sure that the path to the file or folder exists
	//will be used in do-while loops and if statements to evaluate the condition of the path
	bool img_check(directory_iterator current); //this will evaluate whether or not a file is an image file
	bool img_check(recursive_directory_iterator current); //this will evaluate whether or not a file is an image file

	bool match(directory_iterator fileIt); //will try to find a string within the filename 
	//that comes from the source_list

	void fill_source(string location); //will be used to fill the entire vector, source_list
	void format_source(); //changes all the strings in the source_list vector to lower case

	//This function will accept a string and return the string
	//formatted in the way that c++ will understand (with the double \\)
	
	/* --------No longer needed....oh well--------
	string formatStr(string text); 
	*/

	void create_copy(directory_iterator foundItem);
	void create_copy(recursive_directory_iterator foundItem);

	void create_copy_multifolder(directory_iterator foundItem, path dest);

	//randomly puts pictures in subfolders if there are lots of pictures
	void break_down_folders(int totalPics, path pic_destination);
	void create_dir(int numFolders, path pic_destiny);
	void fill_folders(string pic_move_from, const int MAXIMUM);

	void changeFileLocation(path *currentFile);

	void clean_up(); //BE VERY careful using this function since it deletes files
	//In this version of the program, it will be used to delete files from a temporary holding folder

	//Image processing functions
	cv::Mat crop_img(directory_iterator current_image);
	cv::Mat crop_img(recursive_directory_iterator current_image);

	cv::Mat create_canvas(cv::Mat *image_to_fit);

	cv::Mat combo_image_canvas(cv::Mat *image, cv::Mat *background, string filename);


	//Object Detection functions and variables
	struct objDet
	{
		Mat src;
		Mat src_gray;
		Mat src_small; //this will hold a mat of the object detected
		const int thresh = 100;
		const int max_thresh = 255;
		string filename = " ";
	};

	void objectDetectMain(recursive_directory_iterator current_image, struct objDet *vars);

	void thresh_callback(int, void*, struct objDet *moreVars, recursive_directory_iterator currentIter);

	Rect find_largest_boundBox(Rect *current, int count);

	bool check_corners(Rect *largeBox, struct objDet *variableStruct);
	bool colored_border(Rect *box, struct objDet *variableStruct);

	bool check_corners_using_crop(Mat *pic);
	bool nonWhiteWithErrorCatch(Mat *copy, cv::Point endPt);
	bool upperLeftCornerCheck(Mat* copy); //not currently in use, because it missed too many pictures
	bool bottomBorderCheck(Mat *copy);//not currently in use, it is commented out because it returned unwanted results

	bool nonWhiteWithErrorCatch(Mat *copy, int xTL, int yTL, int xBR, int yBR);

	cv::Mat edge_detect(recursive_directory_iterator current_image); //basically the same thing as the crop, but I won't blur the image before checking for colored pixels

	bool backgroundCheck(Rect *largeBox, struct objDet *variableStruct);

public:
	//constructors
	//SortModule();

	//second constructor --- allows user input
	SortModule(string fromLoc, string toLoc);

	//destructor
	~SortModule();

	//will be the one big function to tie other, smaller ones together
	//will be used to search the directory for certain files containing a string
	//char fileType will be used to determine if the file being searched for is a picture, txt, pdf, etc
	void searchDir(string fileLoc, char fileType);

	void cropDir(char cropType);

	void copyAll();

	void dupeClean(); //function to clean up all the duplicate files

	void detectObjNonWhiteBackground();


};







#endif

//end class definition


//end file
