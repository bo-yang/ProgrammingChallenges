import sys
import os.path
import getopt
import random

class WordGenerator:
	"""
	Generate mis-spelling words based on words in input file.
	"""
	infile=''
	outfile=''
	inwords=[]
	outwords=[]
	vowels='aeiou'
	word_num=5	# by default generate 5 different variations for each input word
	repeat_letters=3	# max repeated letters

	def __init__(self,inputfile,outputfile=''):
		self.infile=inputfile
		self.outfile=outputfile
		if os.path.isfile(self.infile):
			try:
				f=open(self.infile,'r')
				for w in f:
					w=w.rstrip() # remove '\n'
					self.inwords.append(w)
			except IOError:
				print ('Error: failed to open input file ' + self.infile)
			else:
				f.close()
		else:
			print "Error: no valid input file specified!"

	def gen_words(self):
		for wd in self.inwords:
			for i in range(self.word_num):
				nwd=wd
				# mutate vowels
				for j in range(len(nwd)):
					if nwd[j] in self.vowels:
						vowel_idx=random.randint(0,len(self.vowels)-1)
						#nwd=nwd[:j]+self.vowels[vowel_idx]+nwd[j+1:]
						nwd=nwd.replace(nwd[j],self.vowels[vowel_idx])

				# repeat letters
				for j in range(random.randint(0,max(len(wd)/2,0))):
					repeat_nums=random.randint(0,self.repeat_letters)
					repeat_idx=random.randint(0,max(len(wd)-1,0))
					nwd=nwd.replace(wd[repeat_idx],wd[repeat_idx]*repeat_nums)
				
				# change some letters to upper case
				for j in range(random.randint(0,int(len(wd)/3))):
					length=random.randint(0,min(int(len(nwd)/3),3))
					start=random.randint(0,max(len(nwd)-length,0))
					end=start+length
					tmp=nwd[start:end]
					tmp.upper()
					nwd=nwd.replace(nwd[start:end],tmp)
				self.outwords.append(nwd)
		return self.outwords

	def show_generated_words(self):
		for wd in self.outwords:
			print wd

	def write_words(self):
		if(len(self.outfile)>0):
			fout=open(self.outfile,'w')
			for wd in self.outwords:
				fout.write(wd+'\n')
			fout.close()
		else:
			print "Error: no output file specified!"

def main(argv):
	inputfile=''	# file of words
	outputfile=''	# output spelling suggestions
	usage="""
gen_words.py -i <inputfile> -o <outputfile> 
where
	- inputfile:	words to be checked, optional
	- outputfile: 	file to store spelling suggestions, optional
		"""

	try:
		opts, args = getopt.getopt(argv,"hi:o:",["ifile=","ofile="])
	except getopt.GetoptError:
		print usage
		sys.exit(2)

	for opt, arg in opts:
		if opt == '-h':
			print usage
			sys.exit()
		elif opt in ("-i", "--ifile"):
			inputfile = arg
		elif opt in ("-o", "--ofile"):
			outputfile = arg

	if(len(outputfile)>0):
		generator=WordGenerator(inputfile,outputfile)
	else:
		generator=WordGenerator(inputfile)
	generator.gen_words()
	if(len(outputfile)>0):
		generator.write_words()
	else:
		generator.show_generated_words()

if  __name__ =='__main__':
	main(sys.argv[1:])
