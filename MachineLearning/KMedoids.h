#ifndef KMEANS_H
#define KMEANS_H

#include <vector>
#include <iostream>
#include <ShapeMatching/ShapeMatcher.h>
#include <ShapeParsing\ShapeParseGraph.h>
#include <opencv2/core/core.hpp>
#include <algorithm>

using namespace std;

struct KMedoidsPoint
{
	KMedoidsPoint(vpl::SPGPtr s, unsigned int i) : spg_ptr(s), id(i)
	{
	};

	vpl::SPGPtr spg_ptr;
	unsigned int id;
};

/*-----------------------------------------------------------------------------------------------------------------*/
// Use Partioning Around Medoids (PAM) algorithm to compute a good clustering of the shapes.
// Used to retrieve well-differentiated exemplars.
struct KMedoids
{
	// two of these functions.
	// one returns the actual KMedoidsPoint object, the other returns its index in the pts vector.
	KMedoidsPoint randomlySamplePointWithReplacement(vector<KMedoidsPoint> &pts)
	{
		static vector<unsigned int> already_sampled;
		bool sampled = false;
		unsigned int num_pts = pts.size();

		while (true)
		{
			int index = (int)floor((double) (rand() % num_pts));
			unsigned int spg_id = pts[index].id;
			if (std::find(already_sampled.begin(), already_sampled.end(), spg_id) != already_sampled.end())
			{
				// already sampled.
				continue;
			}
			else
			{
				already_sampled.push_back(spg_id);
				return pts[index];
			}
		}
	};

	int randomlySampleIndexWithReplacement(vector<KMedoidsPoint> &pts)
	{
		static vector<unsigned int> already_sampled;
		bool sampled = false;
		unsigned int num_pts = pts.size();

		while (true)
		{
			int index = (int)floor((double) (rand() % num_pts));
			unsigned int spg_id = pts[index].id;
			if (std::find(already_sampled.begin(), already_sampled.end(), spg_id) != already_sampled.end())
			{
				// already sampled.
				continue;
			}
			else
			{
				already_sampled.push_back(spg_id);
				return index;
			}
		}
	};

	void init(vector<pair<vpl::SPGPtr, unsigned int> > data, unsigned int _k, std::shared_ptr<const vpl::ShapeMatcher> sm)
	{
		srand((unsigned) time(NULL));
		k = _k;
		shape_matcher = sm;

		if (k > data.size())
		{
			cout << "More clusters than points.  Exiting." << endl;
			exit(1);
		}

		// initialize point set
		for (auto d = data.begin(); d != data.end(); ++d)
		{
			KMedoidsPoint pt(d->first, d->second);
			points.push_back(pt);
		}

		// randomly initialize clusters.  For now, just throw them on the first k points. 
		// Consider more logical initializations later.
		for (unsigned i = 0; i < k; ++i)
		{
			clusters.push_back(randomlySampleIndexWithReplacement(points));
		}
		
		buildDistanceMatrix();
		current_cost = assignAllPointsToNearestCluster(clusters);
	};

	double distance(vpl::SPGPtr s1, vpl::SPGPtr s2)
	{
		return shape_matcher->Match(*s1, *s2);
	};

	// work with the pairwise distance matrix in k-medoids,
	// in contrast to actual points.
	void buildDistanceMatrix()
	{
		distance_matrix = new cv::Mat_<double>(points.size(), points.size());
		cluster_assignments = new cv::Mat_<double>(points.size(), points.size());

		// (r, c) = matching points[r] with points[c]
		for (int r = 0; r < distance_matrix->rows; ++r)
		{
			for (int c = 0; c < distance_matrix->cols; ++c)
			{
				distance_matrix->at<double>(r, c) = distance(points[r].spg_ptr, points[c].spg_ptr);
			}
		}
	};

	// (r, c) != 0 means r is assigned to point c (a cluster) with cost (r, c).
	double assignAllPointsToNearestCluster(vector<int> &clustering)
	{
		// reset assignments.
		cluster_assignments->release();
		cluster_assignments = new cv::Mat(cv::Mat::zeros(points.size(), points.size(), CV_64F));
		double cost = 0;

		for (unsigned i = 0; i < points.size(); ++i)
		{
			// find cluster with min distance.
			int cluster = clustering[0];
			double min_dist = numeric_limits<double>::max();

			for (auto clust = clustering.begin(); clust != clustering.end(); ++clust)
			{
				double dist = distance_matrix->at<double>(i, *clust);
				if (min_dist > dist)
				{
					min_dist = dist;
					cluster = *clust;
				}
			}

			cluster_assignments->at<double>(i, cluster) = min_dist;
			cost += min_dist;
		}
		return cost;
	};

	void run()
	{
		while (true)
		{
			vector<int> optimal_clustering = clusters;
			double optimal_cost = current_cost;

			// for each medoid m..
			for (unsigned int m = 0; m < clusters.size(); ++m)
			{
				// for each non-medoid point o
				for (unsigned int o = 0; o < points.size(); ++o)
				{
					if (std::find(clusters.begin(), clusters.end(), o) != clusters.end())
					{
						// this point is a cluster.
						continue;
					}

					// swap m and o and compute the total cost of the configuration.
					vector<int> clustering = clusters;
					clustering.erase(clustering.begin() + m);
					clustering.push_back(o);
					double cost = assignAllPointsToNearestCluster(clustering);
					//cout << cost << " versus the optimal " << optimal_cost << endl;

					if (cost < optimal_cost)
					{
						optimal_cost = cost;
						optimal_clustering = clustering;
					}
				}
			}

			if (optimal_cost == current_cost)
			{
				break;
			}
			cout << "Iteration complete.  Cost minimized from " << current_cost << " to " << optimal_cost << endl;
			clusters = optimal_clustering;
			current_cost = optimal_cost;
		}
		cout << "Clustering complete" << endl;
		cout << "Clusters: ";
		for (auto c = clusters.begin(); c != clusters.end(); ++c)
		{
			cout << points[*c].id << ", ";
		}
		cout << endl;
	};

	/* Member variables */
	/* ------------------------------------------------------------------------------------------ */

	// a matrix containing the pairwise distance between any two points
	cv::Mat *distance_matrix;

	// a matrix containing the cost each point is invoking.
	// a point only invokes a cost if it is assigned to that cluster,
	// therefore, only non-zero entries are assignments.
	cv::Mat *cluster_assignments;

	std::shared_ptr<const vpl::ShapeMatcher> shape_matcher;
	vector<KMedoidsPoint> points;

	// an element in 'clusters' is an index into the 'points' vector
	vector<int> clusters;
	unsigned int k;
	double current_cost;
};

#endif