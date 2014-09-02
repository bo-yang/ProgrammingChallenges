#ifndef _POINTSSORTER_H_
#define _POINTSSORTER_H_
//
// This program reads a file of 3D points, sort the points out in order of distance 
// from the first point in the file, and finally print the sorted points out. The 3D
// points are stored in a vector, while the distances are stored in another vector 
// together with point indices(<index,distance> pair). 
//
// The Euclidean distance is used in this program, and the STL sort function is called, 
// which on average costs O(n*log(n)) time.
//
//	Bo Yang(bonny95@gmail.com), 08/01/2014
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstdlib>

struct Point3D {
	int x;
	int y;
	int z;
};

typedef std::vector<std::pair<int,double> > DistHash;

class PointsSorter {
public:
	PointsSorter();
	PointsSorter(std::string file);
	~PointsSorter();
	
	std::vector<Point3D>& LoadPoints(std::string file); // load points from file
	DistHash& CalcDist();	// calculate distance to the first point
	void SortPoints();	// sort points by distance to the first point
	void WriteSortedPoints(std::string file, bool print_to_screen=true);

protected:
	std::vector<Point3D> points; // 3D pointers
	DistHash dist; // hash table for distance, <index_of_points,distance>
};

// Constructor
PointsSorter::PointsSorter() {
}

// Constructor
PointsSorter::PointsSorter(std::string file) {
	LoadPoints(file);
	CalcDist();
}

// Destructor
PointsSorter::~PointsSorter() {
}

// Load points from file
std::vector<Point3D>& PointsSorter::LoadPoints(std::string file) {
	std::fstream fs(file, std::fstream::in);
	if(fs.is_open()) {
		std::string line;
		while(std::getline(fs,line)) {
			std::istringstream iss(line);
			Point3D pt;
			iss>>pt.x>>pt.y>>pt.z; // read coordinates of each point
			points.push_back(pt);
		}
	} else {
		std::cerr<<"Error: failed to open file "+file<<std::endl;
		abort();
	}

	fs.close();
	return points;
}

// Calculate the Euclidean distances to the first point, and store the distances by indices
DistHash& PointsSorter::CalcDist() {
	for(int i=0;i<points.size();++i) {
		double d=sqrt(pow(points[i].x-points[0].x,2)+pow(points[i].y-points[0].y,2)+pow(points[i].z-points[0].z,2));
		dist.push_back(std::pair<int,double>(i,d));
	}
	return dist;
}

// Sort points by distances to the first point
void PointsSorter::SortPoints() {
	std::sort(dist.begin(),dist.end(),
			[&](std::pair<int,double>& d1,std::pair<int,double>& d2){return d1.second < d2.second;}
			);
}

// Write sorted points to file
void PointsSorter::WriteSortedPoints(std::string file,bool print_to_screen) {
	std::fstream fs(file,std::fstream::out);
	if(fs.is_open()) {
		for(auto& d:dist) {
			fs<<points[d.first].x<<"\t"<<points[d.first].y<<"\t"<<points[d.first].z<<"\t("<<d.second<<")"<<std::endl;
			if(print_to_screen)
				std::cout<<points[d.first].x<<"\t"<<points[d.first].y<<"\t"<<points[d.first].z<<"\t("<<d.second<<")"<<std::endl;
		}
	} else {
		std::cerr<<"Error: failed to open file "+file<<std::endl;
		abort();
	}

	fs.close();
}

#endif // _POINTSSORTER_H_
