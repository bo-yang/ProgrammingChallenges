import sys
import signal
import getopt
from collections import defaultdict

class SpellChecker:
	"""
	This class checks the spelling of given words based on dictionary, and it 
	can give suggestions for a correct spelling if errors detected.
	
	All words loaded from dictionary are first transformed into patterns
	(such p**pl* for people), and then stored into a hash table(dict).Since
	multiple words may mapped into the same pattern, *defaultdict* is used to
	store the words with identical patterns in a list. 
	
	Given a (mis-spelled) word, it will also be transformed into a pattern, 
	then search the pattern in the hash table. Since mis-spelled consequtive 
	vowels could be mapped into one star(*)(i.e. peepple => p*pl*, but 
	people => p**pl*),this program will also try to repeat the stars so as to
	enlarge candidates.

	Based on the patterns, the candidate words could be found in the hash table
	(defaultdict). In order to determine the closest match, Levenshtein distance
	is used to calculate the distance between a candidate word and the given (mis-spelled) 
	word. In order to guaranttee the suggestions are reasonable, a English word 
	frequency(stored in a hash table) is adopted to choose the more popular
	candidate by default.

	Time complexity: O(n*m) for loading dictionary and O(1 + m) for word checking,
	where n is the number of entries in the dictionary, and m is the number of 
	characters in the string. Finding a word in hash table usually takes constant 
	time O(1), and word operations costs O(m).

	"""
	dict_file='/usr/share/dict/words'
	freq_file='word_freq.txt'
	p2w_dict=defaultdict(list) # the pattern to word dictionary
	word_freq={} # word frequency
	num_repeat_letters=3 # max number of sequentially repeated letters
	vowels='aeiou'
	use_word_freq=True

	def __init__(self,dict_file='/usr/share/dict/words',use_word_freq=True,freq_file='word_freq.txt'):
		self.dict_file=dict_file
		self.load_dict()
		self.use_word_freq=use_word_freq
		self.freq_file=freq_file
		if self.use_word_freq:
			self.load_freq()

	#
	# Load dictionary from disk
	#
	def load_dict(self):
		print ('Loading dictionary from ' + self.dict_file + '...')
		try:
			f=open(self.dict_file,'r')
			for w in f:
				w=w.rstrip() # remove '\n'
				w=w.lower()
				pattern=self.remove_repeat_letters(w)
				pattern=self.replace_vowels(pattern)
				self.p2w_dict[pattern].append(w)
		except IOError:
			print ('Error: failed to open dictionary ' + self.dict_file)
		else:
			f.close()
	
	#
	# Load word frequency statistics, which is downloaded from 
	# http://en.wiktionary.org/wiki/Wiktionary:Frequency_lists#English
	# 
	def load_freq(self):
		print ('Loading word frequency from ' + self.freq_file + '...')
		try:
			f=open(self.freq_file,'r')
			for line in f:
				line=line.rstrip() # remove '\n'
				word=line.split('\t')
				self.word_freq[word[1]]=int(word[0])
		except IOError:
			print ('Error: failed to open frequency ' + self.freq_file)
		else:
			f.close()

	#
	# Remove the sequentially repeated letters in a word and return a list of
	# posssible words. 
	def remove_repeat_letters(self,word):
		nwd=word[0]
		pre=0
		cur=1
		while(cur<len(word)):
			if word[pre]!=word[cur]:
				nwd+=word[cur]
				pre=cur
			cur+=1
		return nwd
	
	#
	# Replace vowels in a given word with star.
	#
	def replace_vowels(self,word):
		for i in range(len(word)):
			if word[i] in self.vowels:
				word=word.replace(word[i],'*')
		return word

	#
	# Calculate the Levenshtein distance between two words, please refer to 
	# http://en.wikipedia.org/wiki/Levenshtein_distance
	#
	def word_dist(self,s,t):
		# degenerate cases
		if len(s)==0 or len(t)==0:
			return max(len(s),len(t))
		# create two work vectors of integer distances
		v0=[0]*(len(t)+1)
		v1=[0]*(len(t)+1)
		# initialize v0 (the previous row of distances)
    	# this row is A[0][i]: edit distance for an empty s
    	# the distance is just the number of characters to delete from t
		for i in range(len(v0)):
			v0[i]=i
		for i in range(len(s)):
			# calculate v1 (current row distances) from the previous row v0
 			# first element of v1 is A[i+1][0]
        	# edit distance is delete (i+1) chars from s to match empty t
			v1[0]=i+1
			# use formula to fill in the rest of the row
			for j in range(len(t)):
				if s[i]==t[j]:
					cost=0
				else:
					cost=1
				v1[j+1]=min(v1[j]+1,v0[j+1]+1,v0[j]+cost)
			# copy v1 (current row) to v0 (previous row) for next iteration
			for j in range(len(v0)):
				v0[j]=v1[j]

		return v1[len(t)]

	#
	# Match repeated vowels
	#
	def pattern_to_words(self,pattern,word):
		word_list={}
		pat_list=[]
		if self.p2w_dict.has_key(pattern):
			pat_list.append(pattern)
		# match more vowels
		for i in range(len(pattern)):
			if pattern[i]=='*':
				new_pat=pattern[:i]+'*'+pattern[i:]
				if self.p2w_dict.has_key(new_pat):
					pat_list.append(new_pat)
		# find possible words
		for pat in pat_list:
			for wd in self.p2w_dict[pat]:
				word_list[wd]=self.word_dist(wd,word)
				# tune the weights of candidate words based on frequencies
				if self.use_word_freq:
					if (not self.word_freq.has_key(wd)):
						word_list[wd]+=0.5
					else:
						word_list[wd]+=self.word_freq[wd]/len(self.word_freq)*0.4

		return sorted(word_list,key=word_list.get)

	#
	# Signal handler
	#
	def signal_handler(self,signal, frame):
		sys.exit(0)

	#
	# Read words from stdin and check the spellings
	#
	def check_words_stdin(self):
		signal.signal(signal.SIGINT, self.signal_handler)
		print 'Please enter a word(press Ctrl+C to exit):'
		while True:
			try:
				w=raw_input('>')
				w=w.lower()
				p=self.remove_repeat_letters(w)
				p=self.replace_vowels(p)
				word_list=self.pattern_to_words(p,w)
				if len(word_list)>0:
					print word_list[0]
				else:
					print 'NO SUGGESTION'
			except (EOFError):
				break
	
	#
	# Read words from stdin and check the spellings
	#
	def check_words_file(self,infile,outfile):
		if len(outfile)==0:
			print ('Error: output file is not specified!')
			sys.exit(1)
		try:
			fin=open(infile,'r')
			fout=open(outfile,'w')
			for w in fin:
				w=w.rstrip() # remove '\n'
				w=w.lower()
				p=self.remove_repeat_letters(w)
				p=self.replace_vowels(p)
				word_list=self.pattern_to_words(p,w)
				if len(word_list)>0:
					fout.write(word_list[0]+'\n')
				else:
					fout.write('NO SUGGESTION'+'\n')
		except IOError:
			print ('Error: failed to open input file ' + infile)
		else:
			fin.close()
			fout.close()


def main(argv):
	dict_file='/usr/share/dict/words'
	freq_file='word_freq.txt'
	inputfile=''	# file of words
	outputfile=''	# output spelling suggestions
	no_word_freq=False	# by default use word frequency to help spelling check
	usage="""
spelling_checker.py -i <inputfile> -o <outputfile> -d <dictfile> -f <freqfile> -n
where
	- inputfile:	words to be checked, optional
	- outputfile: 	file to store spelling suggestions, optional
	- dictfile: 	dicitonary file, optional
	- freqfile: 	frequency file, optional
	- n:	do not use word frequency to enhance spelling check, optional
		"""

	try:
		opts, args = getopt.getopt(argv,"hi:o:d:f:n",["ifile=","ofile=","dfile=","ffile="])
	except getopt.GetoptError:
		print usage
		sys.exit(2)

	for opt, arg in opts:
		if opt == '-h':
			print usage
			sys.exit(0)
		elif opt in ("-i", "--ifile"):
			inputfile = arg
		elif opt in ("-o", "--ofile"):
			outputfile = arg
		elif opt in ("-d", "--dfile"):
			dict_file = arg
		elif opt in ("-f", "--ffile"):
			dict_file = arg
		elif opt == '-n':
			no_word_freq = True

	checker=SpellChecker(dict_file,not no_word_freq, freq_file)
	if(len(inputfile)>0):
		checker.check_words_file(inputfile,outputfile)
	else:
		checker.check_words_stdin()

if  __name__ =='__main__':
	main(sys.argv[1:])
