/*
 *		cosmic.cpp
 *
 *  	Created on: Jan 31, 2013
 *  	Author: Allen Shultz
 */

#include <fstream>
#include <iostream>
#include <cstring>
#include <cerrno>
#include <cmath>
#include <iomanip>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

using namespace std;

//void printarray (int array[], int length);
void printarray2 (int array[], int length);
int printarray3 (int array[], int length);
void variance (int array[], double mean);

int main()
{
	int clock [1440];		// way to store occurrences per minute
	int cosmic = 0;			// accumulates occurrences
	int cell;			// current cell location
	int clock2 [24];		// way to store hourly occurrences
	int cell2;			// current cell location
	int cosmic2 = 0;		// accumulates occurrences

	ifstream fin;
	string dir, filepath;
	DIR *dp;
	int num;			// integer for # of files
	int benchmark = 0;		// time of Reference, set on first file
	int time;			// seconds after first file was created
	int hr;				// hour/minute of modification
	int min;			// hour/minute of modification
	int last = 0, next = 10;      // tracks previous time and compares it to next
	int last2 = 0, next2 = 10;	// tracks previous/next hr
	int jdate = 0, year = 0;	// collecting date of modification
	int date = 0;			// summation of day, month, year
	float sum = 0;			// total value of files per day
	double mean = 0;		// average of hourly occurrences
	struct dirent *dirp;
	struct stat filestat;

	// Prompt user for directory
	cout << "Welcome to cosmic.cpp! Enter '0' to EXIT, or enter directory to analyze: " << flush;
	getline( cin, dir );  			// gets everything the user ENTERS

	while ( cin != 0 )
	{
		dp = opendir( dir.c_str() );
		if ( dp == 0 )
		{
			cout << "Exiting cosmic.cpp" << endl;
			return 0;
		}

		if ( dp == NULL )
		{
			cout << "Error (" << errno << ") opening " << dir << endl;
			return errno;
		}

		while ((dirp = readdir( dp )))
		{
			filepath = dir + "/" + dirp->d_name;

			// Skips, if the file is a directory (or is in some way invalid)
			if (stat( filepath.c_str(), &filestat )) continue;
			if (S_ISDIR( filestat.st_mode ))         continue;

			// Read a single number from the file and displays it
			fin.open( filepath.c_str() );
			if (fin >> num)
			{
				// Set benchmark time of reference
				if (benchmark == 0)
					benchmark = filestat.st_mtime;

				// Collect Date
				if (jdate == 0)
				{
					// Calculating year
					year = benchmark / 86400 / 365; 		// time since epoch / sec per day / days per year
					year = ( 1970 + year ) * 1000;

					// Calculating julian day
					jdate = ((benchmark / 86400) + 1);
					jdate = jdate % 365;
					if (year == 2005000)
						jdate = jdate - 9;					// Account for leap seconds
					if (year == 2004000)
						jdate = jdate - 8;					// Account for leap seconds

					date = year + jdate;					// Sum year and jdate for date
				}

				// Counts files
//				cout << "File #("<< num <<")" << endl;

				// Print Mod Time of each individual file
//				cout << "Filepath: 		(" << filepath.c_str() << ")" << endl;
//				cout << "Mod Time: 		(" << filestat.st_mtime << ")" << endl;
//				cout << "benchmark: 		(" << benchmark << ")s" << endl;
				time = (filestat.st_mtime - benchmark);
//				cout << "Time since benchmark: 	(" << time << ")s" << endl;
				min = ( (time / 60) % 60 );
				hr = ( time / 3600 );

				// 1 min added to ToM because first file really starts at 00:01, not 00:00
				if ((min + 1) == 60)
				{
					hr = hr + 1;
					min = 0;
				}

				// Updates next/last
				last = next;
				next = ( hr * 60 ) + ( min + 1);

				// Checks if last files ToM == current files ToM
				// Increments occurrences
				if ( last == next )
					cosmic ++;

				// Starts new occurrences increment
				if (last != next )
					cosmic = 1;

				// Stores occurrences into cell
				cell = next;
				if ( cosmic == 0 )
					clock [cell] = 0;
				if ( cosmic > 0 )
					clock [cell] = cosmic;

				// Update next2/last2
				last2 = next2;
				next2 = hr;

				// Checks if last files ToM hour == current files ToM hour
				// Increments occurrences
				if ( last2 == next2 )
					cosmic2 ++;

				// Starts new occurrences increment
				if (last2 != next2 )
					cosmic2 = 1;

				// Stores occurrences into cell
				cell2 = hr;
				if ( cosmic2 == 0 )
					clock2 [cell2] = 0;
				if ( cosmic2 > 0 )
					clock2 [cell2] = cosmic2;
			}

			fin.close();
		}

		// Print date
		cout << "\n" << "Date: " << "\t" << date << "\n" << endl;

		// Print statistics per min/hour
//		printarray (clock, 1440);
		printarray2 (clock2, 24);

		// Print statistics per day
		sum = (printarray3 (clock2, 24));
		cout << "\t" << "___________" << endl;
		cout << "Sum:" << "\t" << sum << endl;
		cout << "\n" << endl;

		// Compute Mean
		mean = sum / 24;
		cout << "Mean: " << "\t" << setprecision(5) << mean << endl;
		cout << "\n";

		// Print Variance
		variance (clock2, mean);
		cout << "\n";

		// Reset variables
		cosmic = 0, last = 0, next = 10, jdate = 0, year = 0;

//		cout << "Enter '0' to EXIT, or enter directory to analyze: " << flush;
		getline( cin, dir );  				// gets everything the user ENTERS

	}

	closedir( dp );
	return 0;

}

/*
// Print occurrences per minute
void printarray ( int array[], int length )
{
	for (int n = 0; n < length; n++)
	{
		// Account for known errors
		if ( array[n] > 200 || array[n] < 0 )
			cout << n << "\t" << "0" << endl;
		else
			cout << n << "\t" << array[n] << endl;
	}
	cout << "\n";
}	*/

// Print occurrences per hour
void printarray2 (int array[], int length)
{
	cout << "Occurrences Per Hour: " << endl;
	for (int n = 0; n < length; n++)
	{
		if ( array[n] < 0 || array[n] > 10000)
			array[n] = 0;
		cout << n << "\t" << array[n] << endl;
	}
}

// Prints array total
int printarray3 (int array[], int length)
{
	int a, sum = 0 ;

	// Tally up array
	for (a = 0; a < length; a++)
		sum += array[a];

	return sum;
}

// Function for Variation
void variance (int array[], double mean)
{
	float variance[24];
	float v = 0;		// variance

	cout << "Variance Per Hour:" << endl;

	// Table Print
	for (int a = 0; a < 24; a++)
		{
			v = ((array[a] - mean) / mean);
			variance[a] = v;
			cout << a << "\t" << setprecision(4) << variance[a] << endl;
		}
}


