#!/usr/bin/env python
'''
Problem - print conersations(lines & buttons) with correct colors.

Example:
Hi how are you?
	Good
		Great to hear
		Since you're feeling good, do you want to eat ice cream?
			Yes
				Here's your ice cream!
			No
				Ok, some other time
	OK 
		OK
	Bad
		Sorry to hear that
		Since you're feeling bad, do you want to eat ice cream?
			Sure
				Here's your ice cream!
			No way
				Ok, some other time
    #4     
    
    
                
Hi how are you?
    Good
    Ok
    Bad
    
Good morning!



printLine("Hi how are you", Color.red/Color.green/Color.black)

# valid lines are black by default (if correct), buttons are green
# a button is invalid if it's longer than 20 chars
# a line is invalid if it's longer than 100 chars
# a line is invalid if it has more than 3 associated buttons

'''

class Color:
    red = '[RED]'
    green = '[GREEN]'
    black = '[BLACK]'

def printLine(s, color=Color.black):
    print("%s\t%s" %(s,color))

class Node:
    def __init__(self, s, isline=True, identifier=None):
        self.isline = isline # line or button
        self.child = [] # list of child node(buttons/lines) IDs
        self.s = s # string
        self.id = id # identifier, e.g. line number

    def add_child(self, identifier):
        self.child.append(identifier)

    def is_valid(self):
        if self.isline:
            return (len(self.s.strip()) <= 100) and (len(self.child) <= 3)
        return len(self.s) <= 20

    def color(self):
        if self.is_valid():
            if self.isline:
                return Color.black
            else:
                return Color.green
        return Color.red

def print_conversation(s=""):
    indent = '\t'
    lines = s.split('\n')
    # remove empty lines
    if len(lines) == 0:
        return
    if len(lines[0]) == 0:
        lines.remove('')
    if len(lines[-1]) == 0:
        lines = lines[:-1]
    # pre-process the input and build the tree
    nodes = dict() # node ID to Node map
    nodes[0] = Node(lines[0], isline=True, identifier=0) # root node
    st = [0] # stack of line numbers(Node id)
    last_level = 0
    for i in range(1, len(lines)):
        level = 0
        l = lines[i]
        if (len(l) == 0):
            continue
        while (level < len(l)) and l[level] == indent:
            level += 1
        isline = (level % 2 == 0)
        nodes[i] = Node(l, isline, i)
        # pop stack accordingly to get the correct parent node
        for j in range(last_level - level + 1):
            st.pop(0)
        parent = st[0]
        nodes[parent].add_child(i)
        st.insert(0, i) # push new node ID to stack
        last_level = level
    # print lines/buttons
    for i in range(len(lines)):
        printLine(lines[i], nodes[i].color())


conv = '''
Hi how are you?
	Good
		Great to hear
		Since you're feeling good, do you want to eat ice cream?????????????????????????????????????????????????
			Yes
				Here's your ice cream!
			No
				Ok, some other time
	OK 
		OK
	Bad
		Sorry to hear that
		Since you're feeling bad, do you want to eat ice cream?
			Sure
				Here's your ice cream!
			No way
				Ok, some other time
	#4$$$$$$$$$$$$$$$$$$$$$$$$$$
'''
print_conversation(conv)
