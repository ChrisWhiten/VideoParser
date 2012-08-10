# analyze_accuracy.py
# Chris Whiten - 2012
#
# Dependent on 'matplotlib', which is used for displaying the graphs.
#
# Run as: 'python analyze_accuracy.py'.  This should be in the root VideoParser folder, 
# and it will go back one folder to look for the 'Experiments' folder to grab the results files.

"""
Parses through the results of object recognition and outputs the recognition accuracy.
Also graphs the accuracy over frames with matplotlib's graphing functionality
[TODO]: Add precision/recall and other relevant statistics.
"""
__author__ = 'Chris Whiten'

import matplotlib.pyplot as plt
import collections
from os import path

"""
class Match:
	# This class is a wrapper for a single query and its matches.
	# It contains the information on the matches, the MAP assignment,
	# And the actual class name.
	results = []
	def __init__(self, name, id):
		self.name = name
		self.id = id
		
	def addResult(self, result):
		self.results.append(result)
		
	def display(self):
		print self.name, ": ", self.id
		print "------------------"
		for r in self.results:
			print r
"""

def parseSimpleMatchFile(stem):
	"""Parse simple results file printed by Shape Context recognition"""
	results = []
	file_path = path.relpath("../Experiments/" + stem)
	
	with open(file_path) as f:
		line = f.readline()
		while (line != ""):
			# this assumes a format of: ([query frame], [matched frame], [matching score], [1 if match])
			individual_pieces = line.replace("(", "").replace(")", "").strip().split(",")
			# and all we need right now is the last piece...
			results.append((int)(individual_pieces[-1].strip()))
			line = f.readline()
	return results
	
def parseComplexMatchFile(stem):
	"""Parse the more complex results file printed by SPG recognition"""
	match_lines = []
	info_lines = []
	classes = []
	file_path = path.relpath("../Experiments/" + stem)
	
	# first, parse through the file and get all lines containing matches.
	with open(file_path) as f:
		line = f.readline()
		while (line != ""):
			words = line.split(" ")
			if words[0].startswith("frame") and words[0].endswith("matches"):
				match_lines.append(line)
			if words[0].startswith("frame") and words[0].endswith("info"):
				info_lines.append(line)
			line = f.readline()
		
	# Now, pull out the class for each frame.
	for info in info_lines:
		words = info.split("(")
		if (len(words) > 1):
			del words[0] # remove prefix.
			words = words[0].split(",")
			name = words[0]
			id = words[1] # not using id right now...
			
			classes.append(name)
	
	# next, go through and append the matching result from each line.
	# here we actually get the results for all recorded matches,
	# not just the first one.  This is just part of the transition
	# to including precision/recall... for now, we only end up 
	# using the first one.
	frames = []
	frame_matches = []
	for i in xrange(len(match_lines)):
		words = match_lines[i].split("(")
		if (len(words) > 1):
			results = []
			matches = []
			del words[0] # remove the prefix.
			
			for word in words:
				word = word.replace(")", "").strip(" ").strip()
				parsed = word.split(",")
				
				if parsed[-1] is "1":
					results.append(1)
				elif parsed[-1] is "0":
					results.append(0)
				matches.append(int(parsed[1]))
				
			frames.append(results)
			frame_matches.append(matches)
	return frames, frame_matches, classes

def parseRecognitionResults(spg_matching_stems, shape_context_stems):
	"""Parse object recognition results and put them into a graphable format"""
	spg_results = []
	spg_matches = []
	shape_context_results = []
	classes = []
	
	for stem in spg_matching_stems:
		frames, frame_matches, classes = parseComplexMatchFile(stem)
		spg_results.append(frames)
		spg_matches.append(frame_matches)
		
	for stem in shape_context_stems:
		shape_context_results.append(parseSimpleMatchFile(stem))
		
	return (spg_results, spg_matches, shape_context_results, classes)

	
def computeMAPAssignments(results, matches):
	"""Select the best matched result from SPG recognition"""
	MAP_assignments = []
	MAP_matches = []

	for experiment in xrange(len(results)):
		MAP_assignment = []
		MAP_match_assignment = []
		for frame in xrange(len(results[experiment])):
			MAP_assignment.append(results[experiment][frame][0])
			MAP_match_assignment.append(matches[experiment][frame][0])

		MAP_assignments.append(MAP_assignment)
		MAP_matches.append(MAP_match_assignment)
	return MAP_assignments, MAP_matches
	
def computeGraphableAccuracy(experiment):
	"""Construct graphable list from parsed object recognition results"""
	results = []
	total_matches = 0
	correct_matches = 0
	
	for match in experiment:
		total_matches += 1
		if match == 1:
			correct_matches += 1
		results.append(float(correct_matches)/float(total_matches)*100)
	return results

def graphRecognitionAccuracy(spg_results, spg_legends, sc_legends, shape_context_results):
	"""Graph object recognition accuracy with matplotlib"""
	# first, get the accuracy to graph at each y-coordinate.
	graphable_results = []
	for experiment in spg_results:
		graphable_result = computeGraphableAccuracy(experiment)
		graphable_results.append(graphable_result)
		
	# add shape context to results.
	results = graphable_results
	legends = spg_legends
	for sc in zip(shape_context_results, sc_legends):
		graphable_shapecontext_result = computeGraphableAccuracy(sc[0])
		results.append(graphable_shapecontext_result)
		legends.append(sc[1])
	
	# report results in the console
	for result in zip(legends, results):
		print "-----------------------"
		print result[0]
		print "-----------------------"
		print "Final accuracy:", result[1][-1], "%"
	
	# now graph it
	for y_data, name in zip(results, legends):
		plt.plot(y_data, label = name)
	plt.legend(loc = 'best')
	plt.ylabel('Recognition accuracy (percentage)')
	plt.xlabel('Frame number')
	plt.show()
		

def comparePerClassPerformance(MAP_assignments, legends, classes, MAP_matches):
	""" 
	Compute statistics on a per-class basis.  We aim to find out where the successes
	and shortcomings are for each experiment, and hopefully find a mixture of the
	experiments that produces favourable results.  

	[TODO] The lists of dictionaries of dictionaries idea is pretty convoluted and hard to follow.
	Perhaps python has a better construct for these, or we should organize everything into objects.
	"""
	# Compute the statistics on a per-class basis.
	# The goal is to find out if 'a' improves over 'b' for any class.

	total_class_sums = {}
	experiment_sums = []
	experiment_match_sums = []
	
	# initialize empty dictionary for each experiment.
	for assignment in MAP_assignments:
		class_sums = {}
		match_sums = {}
		experiment_sums.append(class_sums)
		experiment_match_sums.append(match_sums)
	
	# gather sums of matches for each class, over each experiment.
	for experiment in xrange(len(experiment_sums)):
		experiment_match_sums[experiment] = collections.defaultdict(dict)
		for i in xrange(len(classes)):
			experiment_match_sums[experiment][classes[i]][classes[MAP_matches[experiment][i]]] = experiment_match_sums[experiment][classes[i]].get(classes[MAP_matches[experiment][i]], 0) + 1
			experiment_sums[experiment][classes[i]] = experiment_sums[experiment].get(classes[i], 0) + MAP_assignments[experiment][i]

	# total counts.
	for i in xrange(len(classes)):
		total_class_sums[classes[i]] = total_class_sums.get(classes[i], 0) + 1

	"""for i in xrange(len(classes)):
		for experiment in xrange(len(experiment_sums)):
			experiment_sums[experiment][classes[i]] = experiment_sums[experiment].get(classes[i], 0) + MAP_assignments[experiment][i]

			# Oy.
			experiment_match_sums[experiment][classes[i]] = collections.defaultdict(int)
			#print classes[MAP_matches[experiment][i]]
			experiment_match_sums[experiment][classes[i]][classes[MAP_matches[experiment][i]]] = experiment_match_sums[experiment][classes[i]].get(classes[MAP_matches[experiment][i]], 0) + 1
			print classes[i], "->", classes[MAP_matches[experiment][i]]
			#experiment_match_sums[experiment][classes[i]][MAP_matches[experiment][classes[i]]].get(classes[i], 0) + MAP_matches[experiment][i]
		total_class_sums[classes[i]] = total_class_sums.get(classes[i], 0) + 1"""
		
	"""for k in experiment_match_sums[0].keys():
		print k, ": ", experiment_match_sums[0][k]
	print experiment_match_sums[0]["cat"]
	exit()"""



	# print some general statistics, determine the best SPG, the class accuracies, etc..
	for k in total_class_sums.keys():
		# store the max to display the winner on each class.
		best_experiment = legends[0]
		best_experiment_score = experiment_sums[0][k]
		
		print k
		print "--------------"
		for experiment in xrange(len(experiment_sums)):
			# Overall accuracy of this class on this experiment.
			print legends[experiment], ": ", experiment_sums[experiment][k],  "/", total_class_sums[k], " = ", experiment_sums[experiment][k] / float(total_class_sums[k]) * 100, "%"

			# This shows what classes this experiment matched members of this class against.  Good for determining which classes are getting mixed up
			for key in experiment_match_sums[experiment][k].keys():
				print "-> ", key, ":", experiment_match_sums[experiment][k][key]

			if experiment_sums[experiment][k] > best_experiment_score:
				best_experiment_score = experiment_sums[experiment][k]
				best_experiment = legends[experiment]
				
		print "Best: ", best_experiment
		print ""

	# determine which classes caused the most mismatches for each experiment.
	for experiment in xrange(len(experiment_sums)):
		mismatch_sums = collections.defaultdict(int)

		for k in total_class_sums.keys():
			for key in experiment_match_sums[experiment][k].keys():
				if k != key:
					mismatch_sums[key] += experiment_match_sums[experiment][k][key]

		# print results.
		print legends[experiment], "mismatch sums"
		print "---------------------------"
		for k in mismatch_sums.keys():
			print k, ":", mismatch_sums[k]
		
if __name__ == '__main__':
	"""spg_matching_stems = ["recognition_results_1.txt", "recognition_results_2.txt", "recognition_results_3.txt", "recognition_results_4.txt", 
	"recognition_results_5.txt", "recognition_results_1_nosize.txt", "recognition_results_2_nosize.txt",  "recognition_results_3_nosize.txt", 
	"recognition_results_4_nosize.txt", "recognition_results_5_nosize.txt", "recognition_results_joint_1.txt", "recognition_results_joint_2.txt",
	"recognition_results_joint_3.txt"] # add to this list (and the spg_legends list) to graph multiple results against each other.
	
	spg_legends = [ "SPG 1", "SPG 2", "SPG 3", "SPG 4", "SPG 5", "SPG 1 no size", "SPG 2 no size", "SPG 3 no size", "SPG 4 no size", "SPG 5 no size", 
	"SPG joint 1", "SPG joint 2", "SPG joint 3"]"""
	spg_matching_stems = ["recognition_results_joint_3.txt", "recognition_results_joint_4.txt", "recognition_results_joint_5.txt", "recognition_results_joint_6.txt", "recognition_results_joint_7.txt", "recognition_results_joint_8.txt"]
	spg_legends = ["SPG joint 3", "SPG joint 4", "SPG joint 5", "SPG joint 6", "SPG joint 7", "SPG joint 8"]
	shape_context_stems = []
	shape_context_legends = []
	
	# spg_results is of the form: spg_results[i] = results for file i.  
	# spg_results[i][j] = results for the j'th frame in file i
	# spg_results[i][j][k] = kth best result for the jth frame in file i.
	#
	# shape_context_reults is of the form shape_context_results[i] = whether or not frame i's MAP assignment was correct.
	spg_results, spg_matches, shape_context_results, classes = parseRecognitionResults(spg_matching_stems, shape_context_stems)
	MAP_assignments, MAP_matches = computeMAPAssignments(spg_results, spg_matches)

	comparePerClassPerformance(MAP_assignments, spg_legends, classes, MAP_matches)
	graphRecognitionAccuracy(MAP_assignments, spg_legends, shape_context_legends, shape_context_results)