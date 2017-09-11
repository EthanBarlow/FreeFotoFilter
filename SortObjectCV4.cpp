//This is the implimentation file for the SortObject header
//hopefully I can get this code working in a week or two, but we'll see how the cookie crumbles


//beginning of implimentation
#include <iostream>
#include<string>
#include <boost/filesystem.hpp>
#include "SortObjectCV4.h"
#include <fstream> //for input and output of files
#include <boost/algorithm/string.hpp>//for the to_lower of an entire string

#include<opencv2/core/core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include<opencv2\imgcodecs.hpp>
#include <opencv2\features2d.hpp>




using namespace cv;
using namespace std;
using namespace boost::filesystem;


//this object instance needs to be global since it is defined somewhere other than this SortModule Object
RNG rng(12345);


//constructors
//The default constructor will use "hard-coded" strings for path objects
//Later in the development process, there should be at least one more
//constructor that takes strings as arguments

//SortModule::SortModule() //current default constructor
//{
//	
//
//	//end constructor
//}

SortModule::SortModule(string fromLoc, string toLoc)
{
	/*
	fromLoc = formatStr(fromLoc); //format the path that will be pulled from
	toLoc = formatStr(toLoc); //format path that will be copied to
	*/ //This was used to format the paths of the string, but I think boost takes care of it already

	setOrigin(fromLoc);
	setDest(toLoc);

	//set up the log file for errors
	checkLogExist();

	//end second constructor
}

/*
Not actually needed because the Boost library takes care of this

//format
string SortModule::formatStr(string text)
{
	//loop through the string as a character array
	//when a "\" is encountered add in another one
	//append each character to a new string
	string new_text;

	for (int count = 0; count < text.length(); count++)
	{
		//check for a slash, double slashes are used since the first is an escape character
		if (text[count] == '\\')
		{
			new_text.push_back(text[count]); //push_back current character
			new_text.push_back('\\'); //add in the \
		
		}//end if statement

		else
		{
			new_text.push_back(text[count]); //this still adds the character that was evaluated in the current iteration
		}


	}//end for loop

	
	return new_text;
	//end formatting function
}
*/

SortModule::~SortModule()//destructor
{
	//destructor
	//make sure files are closed
	errorLog.close();
}

//set and get functions
void SortModule::setOrigin(string location)
{
	
	//uses path_check function to limit the while loop which will ensure valid values
	while (path_check(location) == false)
	{
		cout << "\n\n\tERROR: The path \"" << location << "\" does not exist" << endl;
		cout << "\tPlease enter an existing path that you would like to work with." << endl
			<< "Path: ";

		getline(cin, location);
	};
	
	
	origin = location; //another function will need to be called to add in the extra slashes required for paths

	//end function
}

path SortModule::getOrigin()
{
	return origin;
}

void SortModule::setDest(string location)
{
	
	//uses path_check function to limit the while loop which will ensure valid values
	while (path_check(location) == false)
	{
		cout << "\n\n\tERROR: The path \"" << location << "\" does not exist" << endl;


		char choice;
		cout << "Would you like to create this path or re-enter an existing path?" << endl;
		cout << "A. Create path " << endl
			<< "B. Use an existing path " << endl << endl
			<< "Choice: ";

		cin >> choice;
		cin.ignore();

		while (toupper(choice) != 'A' && toupper(choice) != 'B')//error trapping
		{
			cout << endl << endl << "Not valid" << endl
				<< "Choice: ";

			cin >> choice;
			cin.ignore();
		}//end inner while


		if (toupper(choice) == 'A')//create directory
		{
			path newDir = location;
			location = create_directory(newDir);

			cout << "\nDirectory created. Now enter that path as an \"existing path\"" << endl << endl;

		}//end if

		else if (toupper(choice) == 'B')
		{
			cout << "\tPlease enter an existing path that you would like to work with." << endl
				<< "Path: ";
			getline(cin, location);
		}//end else if

		else
		{
			//nothing happens
		}

	};//end while loop
	
	
	destination = location; //another function will need to be called to add in the extra slashes
	//end function
}

path SortModule::getDest()
{
	return destination;
	//end function
}

void SortModule::checkLogExist()//checks for the existence of a log file
{
	path logFile = getDest();

	string pathToLog = logFile.string();
	string logFileName = "\\Error Log.txt";

	pathToLog.append(logFileName);

	path newLogFile(pathToLog); //makes the string into a path so that it is formatted properly

	nameOfErrorFile = newLogFile.string();

	//check for file existence
	errorLog.open(nameOfErrorFile, ios::in);
	
	if (errorLog.fail())//if it does not exist
	{
		cout << "The log file did NOT exist, but is being created" << endl;
		errorLog.open(nameOfErrorFile, ios::out);//creates the file
		errorLog.close();
	}//end if

	else
	{
		errorLog.close();
		//reopened in order to write to the file
		errorLog.open(nameOfErrorFile, ios::app);


		cout << "Log file already exists" << endl;
		errorLog << endl << endl << endl << "RE-RUNNING OF PROGRAM" << endl << endl << endl;
		errorLog.close();
	}



}//end function

void SortModule::printToLog(string error_code, string locationInCode)
{
	static unsigned int error_count = 1; //will be used to add number lines to the file before each error code

	errorLog.open(nameOfErrorFile, ios::app);

	errorLog << error_count << ". " << error_code << endl;
	errorLog << "Location in code:" << locationInCode << endl << endl;

	errorLog.close();

	error_count++;//increments the line number

}//end function


path SortModule::checkErrorFolder()
{
	path desitnation = getDest();
	string pathRepresentation = destination.string();
	pathRepresentation.append("\\ErrorFolder");

	path errorFolder = pathRepresentation;

	if (exists(errorFolder))
	{
		//this should not do anything
	}

	else
	{
		create_directory(errorFolder); //creates an error folder if one does not exist
	}

	return errorFolder;

}//end function


void SortModule::PicErrorLog(cv::Mat *picture, string imageName)
{
	//The next four lines set up the path to write to 
	path errorDest = checkErrorFolder();
	string tempDest = errorDest.string();
	tempDest.append("\\");
	tempDest.append(imageName);

	errorDest = tempDest;

	//saves the image to the error folder
	cv::imwrite(errorDest.string(), *picture);

}//end function



bool SortModule::path_check(string p)
{
	bool check;
	path questionable = p; //the path object gets the string and will be used for error trapping/checking

	
	if (exists(questionable))
	{
		check = true;
	}

	else
	{
		check = false;
	}


	return check;
}

bool SortModule::img_check(directory_iterator current)
{
	bool stat;//reports the status of whether or not the file is an image file

	//use a simple variable for evaluation instead of the long list of methods

	string extString = current->path().filename().extension().string();
	boost::algorithm::to_lower(extString); //changes entire string, extString, to the lower case form


	if (extString == ".jpg")
	{
		stat = true;
	}

	else if (extString == ".jpeg")
	{
		stat = true;
	}

	else if (extString == ".png")
	{
		stat = true;
	}

	else
	{
		stat = false;
	}


	
	return stat;

}//end function

bool SortModule::img_check(recursive_directory_iterator current) //this will evaluate whether or not a file is an image file
{
	bool stat;//reports the status of whether or not the file is an image file

			  //use a simple variable for evaluation instead of the long list of methods

	string extString = current->path().filename().extension().string();
	boost::algorithm::to_lower(extString); //changes entire string, extString, to the lower case form


	if (extString == ".jpg")
	{
		stat = true;
	}

	else if (extString == ".jpeg")
	{
		stat = true;
	}

	else if (extString == ".png")
	{
		stat = true;
	}

	else
	{
		stat = false;
	}



	return stat;

}//end function

//used for filtering out filenames
bool SortModule::match(directory_iterator fileIt)
{
	//use the source_list vector to try and match one of its strings
	//with some text in the filename of the directory_iterator
	bool check;

	for (int count = 0; count < source_list.size(); count++)
	{
		//the long parameter before != basically takes the path object
		//and uses the filename as a string and uses the find method of strings
		//to try and find the substring held in source_list
		// string::npos - basically returns false and says that the string could not be found

		string fileRepresentation = fileIt->path().filename().string(); // "fileRepresentation" will hold the filename and be changed to lower case characters
		boost::algorithm::to_lower(fileRepresentation); //formatted to lower case


		//error seems to be right here---------------------
		if (fileRepresentation.find(source_list.at(count)) != string::npos) //is the sku in the filename?
		{
			check = true;
			break; //stops for loop iteration
		}//end if

		else
		{
			check = false;
		}//end else


	}//end for loop

	return check;
}//end match function


void SortModule::fill_source(string location) //I think this one is done, it appears to work -------- check with debug functionality
{

	//This gets the location of the file containing the search strings
	//This location is placed in the variable "source"
	
	//uses path_check function to limit the while loop which will ensure valid values
	while (path_check(location) == false)
	{
		cout << "\n\n\tERROR: The path \"" << location << "\" does not exist" << endl;
		cout << "\tPlease enter an existing path that you would like to work with in order to pull filenames from." << endl
			<< "Path: ";

		getline(cin, location);
	};


	source = location; 

	string temp;

	//Now I need to fill the source_list vector with lines of text in the path to "source"
	
	//Create a filestream object
	boost::filesystem::ifstream infile; //just typing ifstream is ambiguous because the std namespace and boost library have ifstream
	infile.open(source);

	while (!infile.eof())//reads every line, until the "end of file" bit is found
	{
		getline(infile, temp);
		source_list.push_back(temp);
	}

	infile.close(); //MUST CLOSE FILE -- originally forgot this part

	
	//end function



	

	//------------------------Next to fill out-----------------
	//read lines of text from a txt file
	//store each line in the vector, fill_source()

}

void SortModule::format_source()
{
	for (int count = 0; count < source_list.size(); )//run through the entire vector of strings, count only needs to be incremented when a line of text is found
	{
		if (source_list.at(count) == "" || source_list.at(count) == " ")
		{
			source_list.erase(source_list.begin() + count);//erase any empty elements, empty elements cause all images to be copied
		}//end if

		else 
		{
			boost::algorithm::to_lower(source_list.at(count)); //every character is made lower case
			count++; //adjusts the count so that any removals from the vector are taken into account
		}//end else
	}//end for loop


}//end function



void SortModule::searchDir(string fileLoc, char fileType)
{

	fill_source(fileLoc);
	format_source(); //lower case formatting


	//used for testing
	cout << endl << endl;
	for (int count = 0; count < source_list.size(); count++)
	{
		cout << source_list.at(count) << endl;
	}
	cout << endl;

	//now to compare the strings in the vector to the filenames
	//This next block will just find files that contain certain string
	//but it will call another function to make the copy
	path tempOrigin = getOrigin();

	directory_iterator it(tempOrigin);//beginning iterator that will be used to traverse the directory
	directory_iterator end_iter; //should be an iterator that designates the end of the directory since it has no argument passed to it

	//if (examine only pictures)
	if (fileType == 'B')
	{
		while (it != end_iter)//traverses directory
		{
			//skips any directories & non-image files (using the img_check function)
			if (is_regular_file(*it) && img_check(it))
			{
				//here is where several functions need to be called
				//check to see if the filename contains text pulled from the source_list vector
				//if so print it out for now, later we will make copies

				if (match(it))
				{
					cout << it->path().filename() << endl;
					create_copy(it);

				}//end inner if

			}//end if statement

			else if (is_directory(*it))
			{
				cout << endl << it->path().filename() << " is a directory...oops" << endl;
				cout << "No action will be taken on this item" << endl << endl;
			}//end else if


			it++;//increment the iterator
		}//end while loop
	} //end if

	else
	{
		while (it != end_iter)//traverses directory
		{
			//skips any directories & non-image files (using the img_check function)
			if (is_regular_file(*it))
			{
				//here is where several functions need to be called
				//check to see if the filename contains text pulled from the source_list vector
				//if so print it out for now, later we will make copies

				if (match(it))
				{
					cout << it->path().filename() << endl;
					create_copy(it);

				}//end inner if

			}//end if statement

			else if (is_directory(*it))
			{
				cout << endl << it->path().filename() << " is a directory...oops" << endl;
				cout << "No action will be taken on this item" << endl << endl;
			}//end else if


			it++;//increment the iterator
		}//end while loop
	}//end else





}//end function



void SortModule::create_copy(directory_iterator foundItem)
{
	//for now this will simply print out a filename or path for testing purposes
	//cout << endl << foundItem->path().filename() << " has been copied to " << getDest() << endl;
	

	//copying
	//getOrigin() returns the path where the foundItem file is currently stored
	//getDest() returns the path where the foundItem file is going to be moved to
	//foundItem.filename() is the name of the file that will be moved
	
	path goingTo = getDest();

	try
	{
		copy_file(foundItem->path(), goingTo / foundItem->path().filename());
		//first argument is the path where the file is found
		//second argument in the path where the file will be copied to
		//third item is the path to the file itself that will be copied
	}


	catch (const boost::filesystem::filesystem_error& e)
	{
		//cerr << "Error: " << e.what() << endl;
		printToLog(e.what(), " create_copy function (non-recursive) ");
	}//end catch


}//end function

void SortModule::create_copy(recursive_directory_iterator foundItem)
{
	//for now this will simply print out a filename or path for testing purposes
	//cout << endl << foundItem->path().filename() << " has been copied to " << getDest() << endl;


	//copying
	//getOrigin() returns the path where the foundItem file is currently stored
	//getDest() returns the path where the foundItem file is going to be moved to
	//foundItem.filename() is the name of the file that will be moved

	path goingTo = getDest();

	try
	{
		copy_file(foundItem->path(), goingTo / foundItem->path().filename());
		//first argument is the path where the file is found
		//second argument in the path where the file will be copied to
		//third item is the path to the file itself that will be copied
	}


	catch (const boost::filesystem::filesystem_error& e)
	{
		
		//cerr << "Error: " << e.what() << endl;
		printToLog(e.what(), " create_copy function (recursive) ");

		/*

		I tried to keep the folder structure - sub folders and all

		try
		{ 
			boost::filesystem::copy_directory(foundItem->path(), goingTo / foundItem->path().filename());
		}//end inner try

		catch(const boost::filesystem::filesystem_error& e)
		{
			cerr << "Error: " << e.what() << endl;
		}//end inner catch
		*/

	}//end catch


}//end function

void SortModule::create_copy_multifolder(directory_iterator foundItem, path dest)
{
	//for now this will simply print out a filename or path for testing purposes
	//cout << endl << foundItem->path().filename() << " has been copied to " << getDest() << endl;


	//copying
	//getOrigin() returns the path where the foundItem file is currently stored
	//getDest() returns the path where the foundItem file is going to be moved to
	//foundItem.filename() is the name of the file that will be moved

	try
	{
		copy_file(foundItem->path(), dest / foundItem->path().filename());
		//first argument is the path where the file is found
		//second argument in the path where the file will be copied to
		//third item is the path to the file itself that will be copied
	}


	catch (const boost::filesystem::filesystem_error& e)
	{
		cerr << "Error: " << e.what() << endl;
		printToLog(e.what(), " create_copy function (multi-folder) ");
	}//end catch


}//end function


void SortModule::break_down_folders(int totalNumberOfPics, path pic_destination)
{
	const int MAXPICS = 500;

	int numberFolders = 0; //will hold the number of folders that will be needed

	if (totalNumberOfPics % MAXPICS == 0)
	{
		numberFolders = (totalNumberOfPics / MAXPICS);
	}//end if

	else if (totalNumberOfPics % MAXPICS != 0)
	{
		numberFolders = (totalNumberOfPics / MAXPICS);
		numberFolders += 1; //adding an extra folder for the remainder of the pictures
	}

	create_dir(numberFolders, pic_destination);//create directories

	string move_from = pic_destination.string();

	fill_folders(move_from, MAXPICS);//move pictures, USE getDest() function to make sure that the iterators are setup properly now that there are new items in the directory

	clean_up(); //delete all the pictures from the temporary folder

}//end function

void SortModule::create_dir(int numFolders, path pic_destiny)
{

	string parentDir = pic_destiny.string();
	
	
	for (int count = 0; count < numFolders; count++)
	{
		string newDir = parentDir;
		string addString = "\\Folder " + to_string(count+1);

		newDir.append(addString);

		boost::filesystem::create_directory(newDir);

		new_folders.push_back(newDir);

	}//end for loop
} //end create_dir function

void SortModule::fill_folders(string pic_move_from, const int MAXIMUM)
{
	path fromDir = pic_move_from;
	directory_iterator iterate(pic_move_from);
	directory_iterator end_iter;

	//fill the new folders with pictures
	
	int maxPicCount = 0;
	int folderCount = 0;

	while (iterate != end_iter)
	{
		if (maxPicCount == MAXIMUM)
		{
			++folderCount; //increment the folder count, so that the next folder starts being filled
			maxPicCount = 0; //reset the picture counter to fill the next folder
		}//end if

		if(is_regular_file(*iterate) && img_check(iterate))
		{
			path newFolder = new_folders.at(folderCount);
			create_copy_multifolder(iterate, newFolder);
			maxPicCount++;
		}//end if

		iterate++;
	}//end while loop

}//end fill_folders function


void SortModule::clean_up()
{
	path tempLoc = getDest();

	directory_iterator iter(tempLoc);
	directory_iterator endIter;

	while (iter != endIter)
	{
		if (is_regular_file(*iter) && img_check(iter)) //if it is an image, delete it
		{
			boost::filesystem::remove(*iter);
		}
		iter++;
	};

}//end clean_up function


//image processing related functions

//ties it all together
void SortModule::cropDir(char cropType)
{
	/*
	-copy all pictures to destination location first - done
	-run through dir (use a path object and iterators) - done
		-crop image (take one of those iterators) - done
			-create canvas 
		-put them together
	-save each one to the destination folder
	
	
	*/

	//crops all pictures in one directory
	if (cropType == 'A')
	{

		path home_dir = getOrigin(); //this will be used to run through the directory

		//this block will run through all the pictures in a directory and use several methods to crop them

		directory_iterator home_iter(home_dir);//beginning iterator that will be used to traverse the directory
		directory_iterator end_iter; //should be an iterator that designates the end of the directory since it has no argument passed to it


		cout << "\n\nThis could take a few minutes" << endl << endl;

		//copies all pictures first
		while (home_iter != end_iter)
		{
			if (is_regular_file(*home_iter) && img_check(home_iter))
			{
				create_copy(home_iter);
			}//end if
			home_iter++;
		}//end while

		//now all pictures have been copied to the destination folder


		path dest_dir = getDest();
		directory_iterator dest_iter(dest_dir);


		int message_count = 0; //will be used to update the user of progress

		while (dest_iter != end_iter)//traverses directory
		{
			
			try
			{

				//skips any directories & non-image files (using the img_check function)
				if (is_regular_file(*dest_iter) && img_check(dest_iter))
				{
					//here is where several functions need to be called
					//crop the picture
					cv::Mat crop = crop_img(dest_iter);

					//create a canvas/background
					//I'm using a reference variable in hopes that the program will stay relatively fast
					cv::Mat canvas = create_canvas(&crop);//uses the cropped image to estimate size of background needed

					//add them together (put crop on top of canvas and center it)
					string fileName = dest_iter->path().filename().string(); //this will be used for error logging
					cv::Mat compositeImage = combo_image_canvas(&crop, &canvas, fileName);

					//save them

					string imageAndLoc = dest_iter->path().string();

					//imshow("background", canvas);
					//imshow("cropped", crop);

					//CHANGE THE SECOND ARGUMENT TO SAY "COMPOSITEIMAGE"------------------
					imwrite(imageAndLoc, compositeImage);

					if (message_count % 200 == 0)//updates user every 200 pictures
					{
						cout << "\n\n\t\t...Um...Still..Processing.....";
					}//end if

				}//end if statement

				++message_count;
				++dest_iter; //iterate through the loop

			}//end try block

			catch (exception& e)
			{
				printToLog(e.what(), " cropDir ");
			}//end catch block for a standard exception

			catch (runtime_error& e)
			{
				printToLog(e.what(), " cropDir ");
			}//end runtime_error catch

			catch (cv::Exception& e)
			{
				printToLog(e.what(), " cropDir ");
			}//end cv error catch


		}//end while loop


		cout << endl << endl;
	}//end "if" for cropType A

	//crops all pictures within a directory and subdirectories ---- DUMPS everything into one destination folder
	else if (cropType == 'B')
	{

		int pictureCount = 0; //will be used to make additional folders

		path home_dir = getOrigin(); //this will be used to run through the directory

									 //this block will run through all the pictures in a directory and use several methods to crop them

		recursive_directory_iterator home_iter(home_dir);//beginning iterator that will be used to traverse the directory
		recursive_directory_iterator end_iter; //should be an iterator that designates the end of the directory since it has no argument passed to it


		cout << "\n\nThis could take a few minutes" << endl << endl;

		//copies all pictures first
		while (home_iter != end_iter)
		{
			if (is_regular_file(*home_iter) && img_check(home_iter))
			{
				create_copy(home_iter);
			}//end if

			/*

			tried to copy the folder structure/heirarchy

			else if (is_directory(*home_iter))
			{
				create_copy(home_iter);
			}//end else if

			*/

			home_iter++;
		}//end while

		 //now all pictures have been copied to the destination folder


		path dest_dir = getDest();
		recursive_directory_iterator dest_iter(dest_dir);


		int message_count = 0; //will be used to update the user of progress

		while (dest_iter != end_iter)//traverses directory
		{
			
			try
			{

				//skips any directories & non-image files (using the img_check function)
				if (is_regular_file(*dest_iter) && img_check(dest_iter))
				{
					//here is where several functions need to be called
					//crop the picture
					cv::Mat crop = crop_img(dest_iter);

					//create a canvas/background
					//I'm using a reference variable in hopes that the program will stay relatively fast
					cv::Mat canvas = create_canvas(&crop);//uses the cropped image to estimate size of background needed

					//add them together (put crop on top of canvas and center it)
					string fileName = dest_iter->path().filename().string(); //this will be used for error logging
					cv::Mat compositeImage = combo_image_canvas(&crop, &canvas, fileName);

					//save them

					string imageAndLoc = dest_iter->path().string();

					//imshow("background", canvas);
					//imshow("cropped", crop);

					//CHANGE THE SECOND ARGUMENT TO SAY "COMPOSITEIMAGE"------------------
					imwrite(imageAndLoc, compositeImage);

					if (message_count % 40 == 0)//updates user every 40 pictures
					{
						cout << "\n\n\t\t...Um...Still..Processing.....";
					}//end if

					pictureCount++; //only incrimented if a picture is found

				}//end if statement

				++message_count;
				++dest_iter; //iterate through the loop

			}//end try block


			catch (exception& e)
			{
				printToLog(e.what(), " cropDir ");
			}//end catch block for a standard exception

			catch (runtime_error& e)
			{
				printToLog(e.what(), " cropDir ");
			}//end runtime_error catch

			catch (cv::Exception& e)
			{
				printToLog(e.what(), " cropDir ");
			}//end cv error catch

		}//end while loop



		//breaks the pictures down into additional folders so that they are more manageable
		break_down_folders(pictureCount, dest_dir);

		cout << endl << endl;
	}//end "else if" for cropType B


	 //crops all pictures within a directory and subdirectories ---- KEEPS the directory structure
	else if (cropType == 'C')
	{
		//nothing here yet
	}//end "else if" for cropType C


}//end function



//this crops closely to the object
cv::Mat SortModule::crop_img(directory_iterator current_image)//returns the cropped img
{

	//opencv does not work with path objects so it is converted to a string
	string filename = current_image->path().string();
	
	//----------------------------------------------------------
	//Maybe found a helpful tip on page 63 of the pdf in the opencv 
	//bookmark folder
	//add pixels to bounding box to give a margin/border
	//-----------------------------------------------------------
	//-------------------------------------------------------------
	//-------------------------------------------------------


	//copied function
	//original idea and code for this function from -> https://stackoverflow.com/questions/23344380/opencv-c-detect-and-crop-white-region-on-image
	
	
	
	cv::Mat in = cv::imread(filename);
	cv::Mat blurred; //this will be used to determine if a pixel is white
					//the random pixel "noise" should be cleared up by comparing
					//the blurred Mat instead of the "in" Mat
	cv::medianBlur(in, blurred, 25);//the third argument correlates to how blurry the image will be and how well it can be cropped

	// vector with all non-white point positions
	std::vector<cv::Point> nonWhiteList;
	nonWhiteList.reserve(in.rows*in.cols);

	// add all non-white points to the vector
	//TODO: there are more efficient ways to iterate through the image
	for (int j = 0; j<in.rows; ++j)
		for (int i = 0; i<in.cols; ++i)
		{
			// if not white: add to the list
			if (blurred.at<cv::Vec3b>(j, i) != cv::Vec3b(255, 255, 255))
			{
				nonWhiteList.push_back(cv::Point(i, j));
			}
		}

	// create bounding rect around those points
	cv::Rect bb = cv::boundingRect(nonWhiteList);

	//create a white background with a 20px buffer and maybe make it 1:1 ratio
	//mash these two images together making sure that the white "canvas" is the bottom layer
	


	//bb.width += 30;
	//bb.height += 10;
	//bb.x += 100;


	// display result and save it
	// testing purposes -----  cv::imshow("blurry", blurred);
	
	//cv::imshow("found rect", in(bb));
	//cv::imwrite("CropWhiteResult.png", in(bb));


	cv::waitKey(-1);
	
	return in(bb);

	//end copied function

	//displaying an image
	/*
	cv::Mat img = imread("D:\\ProductImages\\SC-X7891__2.JPG", CV_LOAD_IMAGE_UNCHANGED); //reads the picture into "img"

	if (img.empty())//check for the loading of the img
	{
		cout << "Error: Image cannot be loaded..!!!!!" << endl;
	}


	//namedWindow("MyWindow", CV_WINDOW_AUTOSIZE); //create a window with name "MyWindow"

												 //actually display the image
	imshow("Mywindow", img);

	waitKey(0);//wait forever for a keypress

	destroyWindow("MyWindow");//releases memory used for the window and closes the window
	*/



}//end function

 //this crops closely to the object
cv::Mat SortModule::crop_img(recursive_directory_iterator current_image)//returns the cropped img
{
	const int BLUR_AMOUNT = 25;
	//opencv does not work with path objects so it is converted to a string
	string filename = current_image->path().string();

	//----------------------------------------------------------
	//Maybe found a helpful tip on page 63 of the pdf in the opencv 
	//bookmark folder
	//add pixels to bounding box to give a margin/border
	//-----------------------------------------------------------
	//-------------------------------------------------------------
	//-------------------------------------------------------


	//copied function
	//original idea and code for this function from -> https://stackoverflow.com/questions/23344380/opencv-c-detect-and-crop-white-region-on-image



	cv::Mat in = cv::imread(filename);
	cv::Mat blurred; //this will be used to determine if a pixel is white
					 //the random pixel "noise" should be cleared up by comparing
					 //the blurred Mat instead of the "in" Mat
	cv::medianBlur(in, blurred, BLUR_AMOUNT);//the third argument correlates to how blurry the image will be and how well it can be cropped

									// vector with all non-white point positions
	std::vector<cv::Point> nonWhiteList;
	nonWhiteList.reserve(in.rows*in.cols);

	// add all non-white points to the vector
	//TODO: there are more efficient ways to iterate through the image
	for (int j = 0; j<in.rows; ++j)
		for (int i = 0; i<in.cols; ++i)
		{
			// if not white: add to the list
			if (blurred.at<cv::Vec3b>(j, i) != cv::Vec3b(255, 255, 255))
			{
				nonWhiteList.push_back(cv::Point(i, j));
			}
		}

	// create bounding rect around those points
	cv::Rect bb = cv::boundingRect(nonWhiteList);

	//create a white background with a 20px buffer and maybe make it 1:1 ratio
	//mash these two images together making sure that the white "canvas" is the bottom layer



	//bb.width += 30;
	//bb.height += 10;
	//bb.x += 100;


	// display result and save it
	// testing purposes -----  cv::imshow("blurry", blurred);

	//cv::imshow("found rect", in(bb));
	//cv::imwrite("CropWhiteResult.png", in(bb));


	cv::waitKey(-1);

	return in(bb);

	//end copied function

	//displaying an image
	/*
	cv::Mat img = imread("D:\\ProductImages\\SC-X7891__2.JPG", CV_LOAD_IMAGE_UNCHANGED); //reads the picture into "img"

	if (img.empty())//check for the loading of the img
	{
	cout << "Error: Image cannot be loaded..!!!!!" << endl;
	}


	//namedWindow("MyWindow", CV_WINDOW_AUTOSIZE); //create a window with name "MyWindow"

	//actually display the image
	imshow("Mywindow", img);

	waitKey(0);//wait forever for a keypress

	destroyWindow("MyWindow");//releases memory used for the window and closes the window
	*/



}//end function

//creates the white background which will allow the image to have a border/margin
cv::Mat SortModule::create_canvas(cv::Mat *image_to_fit)
{

	//will be used to add to the height and/or width of an image
	const int MARGIN = 60;
	
	
	
	int canvasHeight;
	int canvasWidth;

	int imgHeight(image_to_fit->size().height); //only calling the size().height member variable once per image_to_fit
	int imgWidth(image_to_fit->size().width);

	//if the picture is wider than it is tall
	if (imgWidth > imgHeight)
	{
		//make a square
		canvasWidth = imgWidth + MARGIN;
		canvasHeight = canvasWidth;
	}//end if

	//if the picture is taller than it is wide
	else if (imgWidth < imgHeight)
	{
		canvasHeight = imgHeight + MARGIN;
		canvasWidth = canvasHeight;
	}//end else if

	else if (imgWidth == imgHeight)
	{
		canvasHeight = canvasWidth = imgWidth;
	}//end else if

	else
	{
		cout << "\n\n\t\t-------------There is no logical reason that you should be seeing this message------------"
			<< "\t\t-------------Please contact the developer, if you are seeing this----------" << endl << endl;
	}//end else
	
	//make a canvas of a designated height and width that has a solid white background
	cv::Mat canv(canvasHeight, canvasWidth, CV_8UC3, cv::Scalar(255, 255, 255));



	//https://stackoverflow.com/questions/4337902/how-to-fill-opencv-image-with-one-solid-color




	return canv;
}//end canvas function


//hopefully using pointers will keep the software fast
cv::Mat SortModule::combo_image_canvas(cv::Mat *image, cv::Mat *background, string filename)
{
	//used to determine where the upper right corner of the image will be located
	int offsetX;
	int offsetY;

	offsetX = ((background->size().width - image->size().width)/2);
	offsetY = ((background->size().height - image->size().height) / 2);

	//size of image to be overlayed, used in creating the composite Mat
	int imgX = image->rows;
	int imgY = image->cols;

	//sets up the background and the Rect is like the "frame" for where the image will go
	//cv::Mat composite(*background, Rect(offsetX, offsetY, imgY, imgX));
	try
	{
		image->copyTo((*background)(cv::Rect(offsetX, offsetY, imgY, imgX)));
	}//end try

	catch (cv::Exception& e)
	{
		printToLog(e.what(), " combo_image_canvas ");
		PicErrorLog(image, filename);
	}//end catch

	//combine the images
	//image->copyTo(composite);

	//imshow("Combined", composite);



	return (*background);

}//end combination function


void SortModule::copyAll()
{
	//get the location to pull from
	path pullFrom = getOrigin();

	directory_iterator iter(pullFrom);
	directory_iterator end_iter;
	
	while (iter != end_iter)
	{
		create_copy(iter);
		iter++;
	}//end while loop

}//end function


void SortModule::dupeClean() //-------Not yet supported
{
	//code copied from and based on that in the "cropDir" function

	vector<std::pair<string, path>> fileList;

	path home_dir = getOrigin(); //this will be used to run through the directory

								 //this block will run through all the pictures in a directory 

	recursive_directory_iterator home_iter(home_dir);//beginning iterator that will be used to traverse the directory
	recursive_directory_iterator end_iter; //should be an iterator that designates the end of the directory since it has no argument passed to it


	cout << "\n\nThis could take a while. I'd suggest getting some coffee, taking a walk, or if you have a lot of files, take a nap." << endl << endl;

	//should fill the vector
	while (home_iter != end_iter)
	{
		if (is_regular_file(*home_iter) && img_check(home_iter))
		{
			//add to the vector
			//use just the filename for the first parameter/key
			//use the path object for the second parameter/value
			fileList.push_back(std::pair<string, path>(home_iter->path().stem().string(), home_iter->path()));
		}//end if
				
		home_iter++;
	}//end while

	 //now all pictures/files have been loaded into the vector

	//-----------------------------------------
	//Sort the vector based on first item in the pair
	//This sorts in ascending order
	
	//This sort may be unecessary 
	std::sort(fileList.begin(), fileList.end());

	//move everything referenced by the vector into the dump folder
	//account for duplicate filenames by appending "copy" to each one - done in changeFileLocation function

	auto vecIter = fileList.begin();
	auto endVecIter = fileList.end();

	int count;
	//----------------------------------------The problem is here somewhere
	while (vecIter != endVecIter)
	{

		//move file and delete old one
		changeFileLocation(&(vecIter->second));
		//change file name of second piece of the pair which is the path that the file is located in
		

		//change file location

		//testing purposes
		cout << endl << vecIter->first;
		cout << endl << vecIter->second << endl << endl;

		//increment the iterator
		vecIter++;
		
	}//end while loop


	//this should be deleted later
	/*
	path dest_dir = getDest();
	recursive_directory_iterator dest_iter(dest_dir);

	
	int message_count = 0; //will be used to update the user of progress

	while (dest_iter != end_iter)//traverses directory
	{

		try
		{

			//skips any directories & non-image files (using the img_check function)
			if (is_regular_file(*dest_iter) && img_check(dest_iter))
			{
				//here is where several functions need to be called
				//crop the picture
				cv::Mat crop = crop_img(dest_iter);

				//create a canvas/background
				//I'm using a reference variable in hopes that the program will stay relatively fast
				cv::Mat canvas = create_canvas(&crop);//uses the cropped image to estimate size of background needed

													  //add them together (put crop on top of canvas and center it)
				string fileName = dest_iter->path().filename().string(); //this will be used for error logging
				cv::Mat compositeImage = combo_image_canvas(&crop, &canvas, fileName);

				//save them

				string imageAndLoc = dest_iter->path().string();

				//imshow("background", canvas);
				//imshow("cropped", crop);

				//CHANGE THE SECOND ARGUMENT TO SAY "COMPOSITEIMAGE"------------------
				imwrite(imageAndLoc, compositeImage);

				if (message_count % 40 == 0)//updates user every 40 pictures
				{
					cout << "\n\n\t\t...Um...Still..Processing.....";
				}//end if

				pictureCount++; //only incrimented if a picture is found

			}//end if statement

			++message_count;
			++dest_iter; //iterate through the loop

		}//end try block


		catch (exception& e)
		{
			printToLog(e.what(), " cropDir ");
		}//end catch block for a standard exception

		catch (runtime_error& e)
		{
			printToLog(e.what(), " cropDir ");
		}//end runtime_error catch

		catch (cv::Exception& e)
		{
			printToLog(e.what(), " cropDir ");
		}//end cv error catch

	}//end while loop



	 //breaks the pictures down into additional folders so that they are more manageable
	break_down_folders(pictureCount, dest_dir);
	*/

	cout << endl << endl;

}//end the duplicate clean up function


//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------
//FIGURE THIS OUT FOR YOUR OWN SANITY AND PEACE OF MIND

void SortModule::changeFileLocation(path *currentFile)
{
	path goingTo = getDest();

	//copy file

	bool fileExistStatus = false;

	do 
	{
		cout << endl << "in the do while" << endl;

		try
		{
			
			fileExistStatus = false;
			copy_file(*currentFile, goingTo / currentFile->filename());
			boost::filesystem::remove(*currentFile);
			//boost::filesystem::remove(*currentFile); //deletes the old file
			//first argument is the path where the file is found
			//second argument in the path where the file will be copied to
			//third item is the path to the file itself that will be copied
			
		}


		catch (const boost::filesystem::filesystem_error& e)
		{
			//rename file
			/*if (e.code == system_error::code)
			{*/
			string nameOf = currentFile->stem().string(); //name of file
			string nameOfOriginal = nameOf;
			nameOf.append("copy"); //new name of the file

			string nameOfExt = currentFile->extension().string();

			string location = currentFile->parent_path().string(); //location of file
			//new file name in location now
			location.append("\\" + nameOf + nameOfExt);

			path renamedPic = location;

			// .c_str() converts a string to a const char*
			//rename in current folder
			std::rename(nameOfOriginal.c_str(), location.c_str());

			/*
			try
			{
				boost::filesystem::rename(*currentFile, renamedPic);
			}

			catch (const boost::filesystem::filesystem_error &e)
			{
				printToLog(e.what(), " changeFileLocation - renaming ");
			}

			try
			{
				//deletes old paths
				boost::filesystem::remove(*currentFile);
				boost::filesystem::remove(renamedPic);
			}


			catch (const boost::filesystem::filesystem_error &e)
			{
				printToLog(e.what(), " changeFileLocation - deleting");
			}

			//}

			//try again

			*/

			printToLog(e.what(), " changeFileLocation ");

			fileExistStatus = true;

		}//end catch


		//delete from original spot

	} while (fileExistStatus == true);


	//delete the file's old location
	

}//end the changeFileLocation


/*
Object Detection functions

void SortModule::detectObjNonWhiteBackground();
	This will be used to scan for pictures without white backgrounds
	Such pictures were skipped during the photoshop process
	
	Also, the pictures will be moved to a new folder


*/

//public function to tie it all together
//will manage the traversal of the filesystem
void SortModule::detectObjNonWhiteBackground()
{

	vector<std::pair<string, char>> listOfPaths;

	//will manage the traversal of the filesystem
	int messageCount = 0;
	cout << endl << endl << "\t\t--------Searching--------";

	path searchPath = getOrigin();
	recursive_directory_iterator imgExam(searchPath);
	recursive_directory_iterator end_iter;
	

	while (imgExam != end_iter)//loops through every file
	{
		if (is_regular_file(*imgExam) && img_check(imgExam))
		{
			//neededVars.filename = imgExam->path().string();
			//objectDetectMain(imgExam, &neededVars); ////used for the object detection
			cv::Mat possibleBadPic = crop_img(imgExam);

			//if the corners are not white copy it to the destination folder and remove it from the current folder
			if (check_corners_using_crop(&possibleBadPic))
			{
				std::pair<string, char> currentPair;
				currentPair.first = imgExam->path().string();
				currentPair.second = 'R';

				listOfPaths.push_back(currentPair);
				
				create_copy(imgExam);
				//boost::filesystem::remove(*imgExam);
				//maybe decrement pointer --------------------------------------------------
			}//end if

		}//end outer if
		//waitKey(0); //for testing
		messageCount++;
		imgExam++;

		if ((messageCount % 300) == 0)
		{
			cout << endl << endl << "\t\t--------Searching--------";
		}

	}//end while loop


	//delete the images that were copied
	for (int count = 0; count < listOfPaths.size(); count++)
	{
		if (listOfPaths.at(count).second == 'R')
		{
			boost::filesystem::remove(listOfPaths.at(count).first);
		}
	}

}


//Object Detection functions
void SortModule::objectDetectMain(recursive_directory_iterator current_image, struct objDet *vars)
{
	/// Load source image and convert it to gray
	vars->src = imread(vars->filename, 1);


	try 
	{
		/// Convert image to gray and blur it
		cvtColor(vars->src, vars->src_gray, CV_BGR2GRAY);
		blur(vars->src_gray, vars->src_gray, Size(3, 3));
	}//end catch

	catch (cv::Exception& e)
	{
		printToLog(e.what(), " objectDetectMain ");
		//PicErrorLog(&(vars->src), vars->filename);
	}//end catch

	/// Create Window
	//For testing
	
	/*
	char* source_window = "Source";
	namedWindow(source_window, CV_WINDOW_AUTOSIZE);
	imshow(source_window, vars->src);
	*/

	thresh_callback(0, 0, vars, current_image);
	
	//waitKey(0);
}

void SortModule::thresh_callback(int, void*, struct objDet *moreVars, recursive_directory_iterator currentItem)
{
	Mat canny_output;
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	/// Detect edges using canny
	Canny(moreVars->src_gray, canny_output, moreVars->thresh, (moreVars->thresh) * 2, 3);
	/// Find contours
	findContours(canny_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

	/// Approximate contours to polygons and get the bounding rectangles
	//All the vectors are given the same dimensions as the "contours" vector
	vector<vector<Point>> contours_poly(contours.size());
	vector<Rect> boundRect(contours.size());
	vector<Point2f> center(contours.size());
	vector<float> radius(contours.size());

	//-----------------------------------------------------------------
	// --------------- DO NOT MESS WITH THIS FOR LOOP -----------------
	//-----------------------------------------------------------------
	for (int i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true); //approximate a polygon
		boundRect[i] = boundingRect(Mat(contours_poly[i])); //creates a bounding rectangle for the polygon
	}

	Rect biggestBound;
	//find biggest bounding box
	for (int x = 0; x < boundRect.size(); x++)
	{
		//passes in the current boundRect to find the largest one
		biggestBound = find_largest_boundBox(&boundRect[x], x);
	}

	//bool needsFixing = false;
	//needsFixing = check_corners(&biggestBound, moreVars);

	if (backgroundCheck(&biggestBound,moreVars))
	{
		//rename file

		create_copy(currentItem);

		try
		{
			boost::filesystem::remove(*currentItem);
		}

		catch (const boost::filesystem::filesystem_error& e)
		{
			//cerr << "Error: " << e.what() << endl;
			printToLog(e.what(), " thresh_callback function trying to remove bad pics from their folders ");
		}//end catch


	}

	/// Draw contours and draw the bounding rectangles
	Mat drawing = Mat::zeros(canny_output.size(), CV_8UC3);
	for (int i = 0; i< contours.size(); i++)
	{
		//Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		//drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
		// .tl() returns the member variable corresponding to the top left corner, and .br() corresponds to the bottom right corner
		//rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}

	rectangle(drawing, biggestBound.tl(), biggestBound.br(), Scalar(0, 0, 255), 2, 8, 0);

	/// Show in a window
	
	//for testing
	//namedWindow("Contours", CV_WINDOW_AUTOSIZE);
	//imshow("Contours", drawing);
	
}//end thresh_callback

Rect SortModule::find_largest_boundBox(Rect *current, int count)
{
	static Rect* largestBox;
	//first iteration
	if (count == 0)
	{
		largestBox = current;
	}

	else //compare box sizes
	{
		int largerArea = largestBox->area();
		int currentArea = current->area();
		//if largestBox is bigger than the "current" box - do nothing
		if (largerArea > currentArea)
		{
			//this should do nothing
		}//end if

		 //else make "current" the largestBox
		else
		{
			largestBox = current;
		}//end else

	}//end else

	return *largestBox;
}//end find_largest_boundBox function

bool SortModule::check_corners(Rect *largeBox, struct objDet *variableStruct)
{
	bool status = false;

	Mat corner = imread(variableStruct->filename);
	Mat cornerCopy = corner.clone();
	variableStruct->src_small = cornerCopy(*largeBox);//new try
	//corner(*largeBox);
	//imshow("title", corner(*largeBox)); //for testing
	//waitKey(0);
	
	//find the top left corner of rectangular box
	cv::Point2i cornerPixel = largeBox->tl();
	int xCoordinateTL = cornerPixel.x;
	int yCoordinateTL = cornerPixel.y;

	//find the bottom right corner of rectangular box
	cv::Point2i cornerPixel2 = largeBox->tl();
	int xCoordinateBR = cornerPixel2.x;
	int yCoordinateBR = cornerPixel2.y;


	try 
	{
		if (nonWhiteWithErrorCatch(&cornerCopy, xCoordinateTL, yCoordinateTL, xCoordinateBR, yCoordinateBR))
		{
			//std::cout << "This picture needs fixin\n\n";
			status = true;

		}//end if


		else
		{
			//cout << "This picture is ready to publish" << endl << endl; --Do nothing
		}//end else

	}//end try block

	catch (cv::Exception& e)
	{
		printToLog(e.what(), " check_corners - if((cornerCopy.at<cv::Vec3b>... ");
		//PicErrorLog(&(vars->src), vars->filename);
	}//end catch

	/* 
	else if ((cornerCopy.at<cv::Vec3b>(xCoordinateTL-3, yCoordinateTL-3) != cv::Vec3b(255, 255, 255)) || (cornerCopy.at<cv::Vec3b>(xCoordinateBR+3, yCoordinateBR+3) != cv::Vec3b(255, 255, 255)))
	{
		std::cout << "This picture needs fixin\n\n";
		status = true;

	}//end if
	

	//inside the box
	
	else if ((cornerCopy.at<cv::Vec3b>(xCoordinateTL+3, yCoordinateTL+3) != cv::Vec3b(255, 255, 255)) || (cornerCopy.at<cv::Vec3b>(xCoordinateBR-3, yCoordinateBR-3) != cv::Vec3b(255, 255, 255)))
	{
		std::cout << "This picture needs fixin\n\n";
		status = true;
	}
	*/


	return status;
}//end check_corners function


bool SortModule::check_corners_using_crop(Mat *pic)
{
	
	bool status = false;
	//cout << "hi" << endl;
	cv::Point endPointBR; //bottom Corner
	endPointBR.x = pic->rows;
	endPointBR.x -= 1; //to account for array counting
	//cout << "rows: " << endPoint << endl;
	endPointBR.y = pic->cols;
	endPointBR.y -= 1;
	//cout << "cols: " << endPoint << endl;
	/*long int numRows = endPoint.y;
	numRows--;
	long int numCols = endPoint.x;
	numCols--;*/

	//the following three blocks of code should ensure that the x-axes and y=axes line up
	if (endPointBR.x > endPointBR.y)
	{
		//int largerNum;
		//if (pic->rows > pic->cols)
		//{
		//	largerNum = pic->rows;
		//}


		//else if (pic->cols > pic->rows)
		//{
		//	largerNum = pic->cols;
		//}

		//else
		//{
		//	//nothing should happen here
		//}

		swap(endPointBR.x, endPointBR.y);
	}

	else if(endPointBR.y > endPointBR.x)
	{
		//int largerNum;
		//if (pic->rows > pic->cols)
		//{
		//	largerNum = pic->rows;
		//}


		//else if (pic->cols > pic->rows)
		//{
		//	largerNum = pic->cols;
		//}

		//else
		//{
		//	//nothing should happen here
		//}

		swap(endPointBR.x, endPointBR.y);
	}

	else
	{
		//nothing should happen here
	}

	try
	{
		//checks the upper left corner and the bottom right corner
		if (nonWhiteWithErrorCatch(pic, endPointBR))
		{
			status = true; //colored pixel
		}

		/*
		if (!status) //if a colored pixel still has not been found, check the bottom border
		{
			status = bottomBorderCheck(pic); //checking for a colored pixel, returns true if one is found
		}
		*/
	}

	catch (std::exception &e)
	{
		printToLog(e.what(), " check_corners_using crop function ");
	}



	return status;
}//end check_corners_using crop function


bool SortModule::colored_border(Rect *box, struct objDet *variableStruct)
{
	const int MINIBOXSIZE = 1;

	bool coloredPixel = false;

	int topLx = (box->width) / 2;
	topLx -= MINIBOXSIZE;
	int topLy = box->height;
	topLy -= MINIBOXSIZE;

	cv::Point2i topLeft(topLx, topLy);

	int bottomRx = (box->width) / 2;
	bottomRx += MINIBOXSIZE;
	int bottomRy = box->height;

	cv::Point2i bottomRight(bottomRx, bottomRy);

	cv::Rect littleRec(topLeft, bottomRight);
	Mat borderBox = variableStruct->src_small;
	cv::Mat littleMat = borderBox(littleRec);

	//imshow("little box for border check", borderBox(littleRec)); //for test
//	waitKey(0);

	for (int j = 0; j < littleMat.rows; ++j)
	{
		if (coloredPixel)
		{
			break;
		}//if there is a colored pixel this will break the outer loop
		
		else
		{
			for (int i = 0; i < littleMat.cols; ++i)
			{
				// if not white: add to the list
				if (littleMat.at<cv::Vec3b>(j, i) != cv::Vec3b(255, 255, 255))
				{
					//cout << "\nFound a colored pixel" << endl;
					coloredPixel = true;
					break;//if there is a colored pixel this will break the inner loop
				}//end if
			}//end inner for
		}//end else
	}//end outer for

	return coloredPixel;

}

bool SortModule::nonWhiteWithErrorCatch(Mat * copy, int xTL, int yTL, int xBR, int yBR)
{
	bool checkVal = false;
	//int minThresh = 40;

	/*while (checkVal == false || thresh != minThresh)
	{*/
	try
	{
		if ((copy->at<cv::Vec3b>(xTL, yTL) != cv::Vec3b(255, 255, 255)) || (copy->at<cv::Vec3b>(xBR, yBR) != cv::Vec3b(255, 255, 255)))
		{
			checkVal = true;
		}
	}

	catch (std::exception &e)
	{
		printToLog(e.what(), " nonWhiteWithErrorCatch ");
		//	minThresh -= 10;
		//cerr << endl << e.what();
	}
	//}//end while

	return checkVal;
	
}
//end nonWhiteWithErrorCatch function

bool SortModule::nonWhiteWithErrorCatch(Mat *copy, cv::Point endPt)
{
	bool checkVal = false;
	//int minThresh = 40;

	/*while (checkVal == false || thresh != minThresh)
	{*/
	try
	{
		
		try 
		{
			if ((copy->at<cv::Vec3b>(cv::Point(0, 0)) != cv::Vec3b(255, 255, 255)) || (copy->at<cv::Vec3b>(endPt) != cv::Vec3b(255, 255, 255)))
			{
				checkVal = true;
				return checkVal;
			}
		}
	
		catch (std::exception &e)
		{
			printToLog(e.what(), " First if -- nonWhiteWithErrorCatch - std error, with the long ints ");
			//	minThresh -= 10;
			//cerr << endl << e.what();
		}

		catch (cv::Exception &e)
		{
			printToLog(e.what(), " First if -- nonWhiteWithErrorCatch - opencv error, with the long ints ");
			//	minThresh -= 10;
			//cerr << endl << e.what();
		}

		catch (std::system_error &e)
		{
			printToLog(e.what(), " First if -- nonWhiteWithErrorCatch - system error, with the long ints ");
		}
		
		
		if(!checkVal)
		{
			cv::Point2i bottomCorn;
			bottomCorn.y = copy->rows;
			bottomCorn.x = copy->cols;

			//compensates for array counting
			bottomCorn.x -= 1;
			bottomCorn.y -= 1;

			int topLx = bottomCorn.x - 3;
			int topLy = bottomCorn.y - 3;

			cv::Point2i upperCorn(topLx, topLy);

			cv::Rect littleRec(upperCorn, bottomCorn);
			Mat borderBox = copy->clone();
			/*imshow("Check borderBox", borderBox);
			imshow("Original", *copy);
			imshow("little Region check", borderBox(littleRec));
			waitKey(0);*/
			cv::Mat littleMat = borderBox(littleRec);//here is where the issue is occurring ----------------------- FIX THIS THING RIGHT HERE

			for (int j = 0; j < littleMat.rows; ++j)
			{
				if (checkVal)
				{
					break;
				}//if there is a colored pixel this will break the outer loop

				else
				{
					for (int i = 0; i < littleMat.cols; ++i)
					{
						// if not white: add to the list
						if (littleMat.at<cv::Vec3b>(j, i) != cv::Vec3b(255, 255, 255))
						{
							//cout << "\nFound a colored pixel" << endl;
							//if a colored pixel is found in this bottom right corner, check the upper left to guarantee a non-white background
							//checkVal = upperLeftCornerCheck(copy);
							checkVal = true;
							return checkVal;
							//break;//if there is a colored pixel this will break the inner loop
						}//end if
					}//end inner for
				}//end else
			}//end outer for

		}//end if




	}//end try

	catch (std::exception &e)
	{
		printToLog(e.what(), " nonWhiteWithErrorCatch - std error, with the long ints ");
		//	minThresh -= 10;
		//cerr << endl << e.what();
	}

	catch (cv::Exception &e)
	{
		printToLog(e.what(), " nonWhiteWithErrorCatch - opencv error, with the long ints ");
		//	minThresh -= 10;
		//cerr << endl << e.what();
	}

	catch (std::system_error &e)
	{
		printToLog(e.what(), " nonWhiteWithErrorCatch - system error, with the long ints ");
	}

	//}//end while

	return checkVal;

}
//end nonWhiteWithErrorCatch function

bool SortModule::upperLeftCornerCheck(Mat* copy)
{
	bool color = false;

	cv::Point2i TLcorner; //top left corner of the image
	TLcorner.x = 0; //origin
	TLcorner.y = 0;

	cv::Point2i bottomBound; //bottom right corner of the small bounding box that will be made
	bottomBound.x = 10;
	bottomBound.y = 10;

	cv::Rect smallRect(TLcorner, bottomBound);
	Mat borderBox = copy->clone();

	Mat tinyMat = borderBox(smallRect);

	//check this new, tiny box for a color pixel
	
	for (int j = 0; j < tinyMat.rows; ++j)
	{
		if (color)
		{
			break;
		}//if there is a colored pixel this will break the outer loop

		else
		{
			for (int i = 0; i < tinyMat.cols; ++i)
			{
				// if not white: add to the list
				if (tinyMat.at<cv::Vec3b>(j, i) != cv::Vec3b(255, 255, 255))
				{
					//cout << "\nFound a colored pixel" << endl;
					color = true;
					return color;
					//break;//if there is a colored pixel this will break the inner loop
				}//end if
			}//end inner for
		}//end else
	}//end outer for

	//if no color was found, color still remains false

	return color;
}

bool SortModule::bottomBorderCheck(Mat *pic)
{
	bool colorCheck = false;

	cv::Point leftMidPoint; //holding upper left corner of a small bounding box
	cv::Point rightMidPoint; //holding bottom right corner of the small bounding box

	int bottomY = pic->cols;
	int rightX = pic->rows;

	if (rightX > bottomY)
	{
		swap(rightX, bottomY);
	}

	else if (bottomY > rightX)
	{
		swap(rightX, bottomY);
	}

	else
	{
		//nothing should happen here
	}

	rightMidPoint.x = ((rightX / 2)+2);
	rightMidPoint.y = bottomY;

	leftMidPoint.x = ((rightX / 2) - 2);
	leftMidPoint.y = bottomY - 2;

	cv::Rect tinyRect(leftMidPoint, rightMidPoint);
	Mat borderBox = pic->clone();
	Mat miniMat = borderBox(tinyRect);


	for (int j = 0; j < miniMat.rows; ++j)
	{
		if (colorCheck)
		{
			break;
		}//if there is a colored pixel this will break the outer loop

		else
		{
			for (int i = 0; i < miniMat.cols; ++i)
			{
				// if not white: add to the list
				if (miniMat.at<cv::Vec3b>(j, i) != cv::Vec3b(255, 255, 255))
				{
					//cout << "\nFound a colored pixel" << endl;
					//if a colored pixel is found in this bottom right corner, check the upper left to guarantee a non-white background
					colorCheck = true;
					return colorCheck;
					//break;//if there is a colored pixel this will break the inner loop
				}//end if
			}//end inner for
		}//end else
	}//end outer for


	return colorCheck;

}

//this tries to look for colored pixels near the border
cv::Mat SortModule::edge_detect(recursive_directory_iterator current_image)//returns the cropped img
{
	const int BLUR_AMOUNT = 25;
	//opencv does not work with path objects so it is converted to a string
	string filename = current_image->path().string();

	//----------------------------------------------------------
	//Maybe found a helpful tip on page 63 of the pdf in the opencv 
	//bookmark folder
	//add pixels to bounding box to give a margin/border
	//-----------------------------------------------------------
	//-------------------------------------------------------------
	//-------------------------------------------------------


	//copied function
	//original idea and code for this function from -> https://stackoverflow.com/questions/23344380/opencv-c-detect-and-crop-white-region-on-image



	cv::Mat in = cv::imread(filename);
	//cv::Mat blurred; //this will be used to determine if a pixel is white
					 //the random pixel "noise" should be cleared up by comparing
					 //the blurred Mat instead of the "in" Mat
	//cv::medianBlur(in, blurred, BLUR_AMOUNT);//the third argument correlates to how blurry the image will be and how well it can be cropped

											 // vector with all non-white point positions
	std::vector<cv::Point> nonWhiteList;
	nonWhiteList.reserve(in.rows*in.cols);

	// add all non-white points to the vector
	//TODO: there are more efficient ways to iterate through the image
	for (int j = 0; j<in.rows; ++j)
		for (int i = 0; i<in.cols; ++i)
		{
			// if not white: add to the list
			if (in.at<cv::Vec3b>(j, i) != cv::Vec3b(255, 255, 255))
			{
				nonWhiteList.push_back(cv::Point(i, j));
			}
		}

	// create bounding rect around those points
	cv::Rect bb = cv::boundingRect(nonWhiteList);

	// display result and save it
	// testing purposes -----  cv::imshow("blurry", blurred);

	//cv::imshow("found rect", in(bb));
	//cv::imwrite("CropWhiteResult.png", in(bb));


	cv::waitKey(-1);

	return in(bb);


}//end function


bool SortModule::backgroundCheck(Rect *largeBox, struct objDet *variableStruct)
{
	bool check = false;
	
	if (check_corners(largeBox, variableStruct) || colored_border(largeBox, variableStruct))
	{
		check = true;
	}


	return check;
}

//end file