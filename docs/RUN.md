# Run from shell
From Windows
```
grapa.exe [parameters]
```

From Linux
```
./grapa [parameters]
```

To exit while in the console, enter a '.' character.

If using paste to enter a block of code to run, ensure each line is not larger than the maximum line line for the console - break up the code into shorter lines.

If using paste to enter multiple lines that need to be run at the same time (such as when a single command spans multiple lines), enter `$[` sequence first and `$]` sequence at the end. This will trigger the lexical engine to signal to the grammar engine to pause grammar analysis until after all the tokens have been generated for the input stream. 

# Command line options

Once copied into a location (and PATH updated if needed), you'll be able to run the tool using "grapa" from a terminal or command shell from any location. Running the tool with "grapa --env" will display a few of the directories where the tool will look for information.

Other command line options:
```
        -h,--help       :Show this help message
        -v,--version    :Show version
        -q,--quite      :Suppress header
        -env            :Show environment details
        -ccmd script    :Run from script
        -cfile file     :Run from file
	-w		:Editor
	-wfile file	:Editor from file
        -ccin           :Run from stdin
        -argcin         :Places std:cin into $ARGCIN environment variable (use with -ccmd|-cfile)
        -argv           :Places proceeding args into $ARGV environment variable
```

Example: Performs a SHAKE256 hash of the string "thisisatest".
```
grapa -ccmd "'thisisatest'.encode('SHAKE256')" -q
0x94B3D49AF1B6396CD186876793A5C4405A1BBFD12C7341521ABD62AA26E3E852B06B345D82126B1D864DFA885B6DC791D21A318259D307D76D7946D1EFF9DA54
```

Example: Same, but takes input from cin. 
```
echo "thisisatest".encode('SHAKE256') | grapa -ccin -q
0x94B3D49AF1B6396CD186876793A5C4405A1BBFD12C7341521ABD62AA26E3E852B06B345D82126B1D864DFA885B6DC791D21A318259D307D76D7946D1EFF9DA54
```
