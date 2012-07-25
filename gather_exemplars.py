# gather_exemplars.py
# Chris Whiten - 2012
#
# Run as: 'python gather_exemplars.py'.  This should be in the root VideoParser folder, 
# and it will go back one folder to look for the 'Experiments' folder to grab the exemplars file.

"""
Parse through exemplars.txt to select and copy out the exemplars and select neighbouring shapes.
This is used for learning an optimal joint parsing model.
"""
__author__ = 'Chris Whiten'

import os
import shutil # has shutil.copyfile(src, dst)

root_folder = "../Experiments/"
shapes_folder = "/Shapes1032b/"
file_extension = ".pgm"


def prepareDirectoryStructure():
	# ensure images are accessible.
	if not os.path.isdir(root_folder + shapes_folder):
		print "Shapes folder not found.  Exiting"
		sys.exit()
	
	if not os.path.isfile(root_folder + "exemplars.txt"):
		print "Could not read exemplars file. Exiting"
		sys.exit()
		
	if not os.path.isdir(root_folder + "Clustered_Exemplars"):
		os.makedirs(root_folder + "Clustered_Exemplars")
		
def getFileNames():
	f = open(root_folder + "exemplars.txt", "r")
	file_names = []
	for line in f:
		words = line.split(",")
		class_name = words[0].strip()
		if (os.path.isfile(root_folder + shapes_folder + class_name + words[1].strip() + file_extension)):
			file_names.append(class_name + words[1].strip() + file_extension)
		elif (os.path.isfile(root_folder + shapes_folder + class_name + "0" + words[1].strip() + file_extension)):
			file_names.append(class_name + "0" + words[1].strip() + file_extension)
		else:
			print "Not sure how to handle " + str(words)
			print "Tried: " + root_folder + shapes_folder + class_name + words[1].strip() + file_extension
	f.close()
	return file_names
		
def copyFiles(files):
	for f in files:
		shutil.copyfile(root_folder + shapes_folder + f, root_folder + "/Clustered_Exemplars/" + f)

		
if __name__ == '__main__':
	prepareDirectoryStructure()
	file_names = getFileNames()
	copyFiles(file_names)
	print file_names