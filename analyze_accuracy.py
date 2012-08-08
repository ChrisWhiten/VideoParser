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
	for i in xrange(len(match_lines)):
		words = match_lines[i].split("(")
		if (len(words) > 1):
			results = []
			del words[0] # remove the prefix.
			for word in words:
				word = word.strip(")").strip(" ")
				if word.endswith("1"):
					results.append(1)
				elif word.endswith("0"):
					results.append(0)
			frames.append(results)
			
	return frames, classes

def parseRecognitionResults(spg_matching_stems, shape_context_stems):
	"""Parse object recognition results and put them into a graphable format"""
	spg_results = []
	shape_context_results = []
	classes = []
	for stem in spg_matching_stems:
		frames, classes = parseComplexMatchFile(stem)
		spg_results.append(frames)
		#spg_results.append(parseComplexMatchFile(stem))
	for stem in shape_context_stems:
		shape_context_results.append(parseSimpleMatchFile(stem))
	return (spg_results, shape_context_results, classes)

	
def computeMAPAssignments(results):
	"""Select the best matched result from SPG recognition"""
	MAP_assignments = []
	for experiment in results:
		MAP_assignment = []
		for frame in experiment:
			MAP_assignment.append(frame[0])
		MAP_assignments.append(MAP_assignment)
	return MAP_assignments
	
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
		

def comparePerClassPerformance(MAP_assignments, legends, classes):
	# Compute the statistics on a per-class basis.
	# The goal is to find out if 'a' improves over 'b' for any class.
	
	total_class_sums = {}
	data_class_sums = []
	
	for assignment in MAP_assignments:
		class_sums = {}
		data_class_sums.append(class_sums)
	
	for i in xrange(len(classes)):
		for spg in xrange(len(data_class_sums)):
			data_class_sums[spg][classes[i]] = data_class_sums[spg].get(classes[i], 0) + MAP_assignments[spg][i]
		total_class_sums[classes[i]] = total_class_sums.get(classes[i], 0) + 1
		
	for k in total_class_sums.keys():
		# store the max to display the winner on each class.
		best_spg = legends[0]
		best_spg_score = data_class_sums[0][k]
		
		print k
		print "--------------"
		for spg in xrange(len(data_class_sums)):
			print legends[spg], ": ", data_class_sums[spg][k],  "/", total_class_sums[k], " = ", data_class_sums[spg][k] / float(total_class_sums[k]) * 100, "%"
			if data_class_sums[spg][k] > best_spg_score:
				best_spg_score = data_class_sums[spg][k]
				best_spg = legends[spg]
				
		print "Best: ", best_spg
		print ""
		
if __name__ == '__main__':
	spg_matching_stems = ["recognition_results_1.txt", "recognition_results_2.txt", "recognition_results_3.txt", "recognition_results_4.txt", "recognition_results_5.txt", "recognition_results_1_nosize.txt", "recognition_results_2_nosize.txt",  "recognition_results_3_nosize.txt", "recognition_results_4_nosize.txt", "recognition_results_5_nosize.txt", "recognition_results_joint.txt"] # add to this list (and the spg_legends list) to graph multiple results against each other.
	spg_legends = [ "SPG 1", "SPG 2", "SPG 3", "SPG 4", "SPG 5", "SPG 1 no size", "SPG 2 no size", "SPG 3 no size", "SPG 4 no size", "SPG 5 no size", "SPG joint"]
	shape_context_stems = []
	shape_context_legends = []
	
	# spg_results is of the form: spg_results[i] = results for file i.  
	# spg_results[i][j] = results for the j'th frame in file i
	# spg_results[i][j][k] = kth best result for the jth frame in file i.
	#
	# shape_context_reults is of the form shape_context_results[i] = whether or not frame i's MAP assignment was correct.
	spg_results, shape_context_results, classes = parseRecognitionResults(spg_matching_stems, shape_context_stems)
	MAP_assignments = computeMAPAssignments(spg_results)
	
	comparePerClassPerformance(MAP_assignments, spg_legends, classes)
	graphRecognitionAccuracy(MAP_assignments, spg_legends, shape_context_legends, shape_context_results)