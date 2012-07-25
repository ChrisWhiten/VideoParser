#ifndef KMEANS_H
#define KMEANS_H

#include <vector>
#include <iostream>
#include <ShapeMatching/ShapeMatcher.h>
#include <ShapeParsing\ShapeParseGraph.h>

using namespace std;

struct KMeansPoint
{
	KMeansPoint(vpl::SPGPtr s, unsigned int i) : spg_ptr(s), id(i)
	{
	};

	vpl::SPGPtr spg_ptr;
	unsigned int id;
};

// Templated implementation of K-Means clustering, designed for use with SPGs
// Data is assumed to be in format pair<data_piece, unique id>, where data_piece is likely an SPG or SPGPtr
// distance function between two data pieces is required for the run() function.  
//
// Things didn't work as planned since everything is so tightly coupled.
// This is half-templated half-hardcoded for SPGPtr objects.
struct KMeans
{
	void init(vector<pair<vpl::SPGPtr, unsigned int> > data, unsigned int _k)
	{
		k = _k;

		if (k > data.size())
		{
			cout << "More clusters than points.  Exiting." << endl;
			exit(1);
		}

		// initialize point set
		for (auto d = data.begin(); d != data.end(); ++d)
		{
			KMeansPoint pt(d->first, d->second);
			points.push_back(pt);
		}

		// randomly initialize clusters.  For now, just throw them on the first k points. 
		// Consider more logical initializations later.
		for (unsigned i = 0; i < k; ++i)
		{
			clusters.push_back(&(points[i]));
		}
	};

	void run()
	{
		cout << "Number of points: " << points.size() << endl;
		cout << "Number of clusters: " << clusters.size() << endl;
	};

	double distance(vpl::SPGPtr s1, vpl::SPGPtr s2)
	{
		return shape_matcher->Match(*s1, *s2);
	}

	std::shared_ptr<const vpl::ShapeMatcher> shape_matcher;
	vector<KMeansPoint> points;
	vector<KMeansPoint *> clusters;
	unsigned int k;
};

#endif