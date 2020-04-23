# grapa
grapa - a grammar parser language / environment for processing data for ETL workflows, or experimenting with new language constructs. 

## Introduction
The unique approach of grapa is that it includes native syntax for JSON/XML and BNF style grammers for defining data processing rules. The same grammer engine used to assist with parsing data is the same engine used to process the language. The syntax of the language is fully customizable (even at runtime if that's of interest) as the language rules are stored as global variables. If for example, you'd like to extend the langue to include SQL style syntax, that could be done and the new syntax would function as native syntax. 

Underneith the syntax, there are several libraries that provide the lower level heavy lifting for strings, execution flow, files, networking, threads, classes with inheritance, ecryiption (RSA, AES, SHA, etc), math, database, etc. The grammer syntax you define can either invoke routines written in the existing syntax, or call lower level libraries. The native integer and float support uses unlimited percision (or limited by CPU/memory). 

Execution of all scripts (from code or compiled) run the same accross all platforms, including the unlimted floating point routines. And will work the same on either big endian or little endian systems. 

## Setup
### Installation
Suported platforms include Win10, MacOS, Linux.

To instally copy the zip for the target platform, unzip, and copy to the exectutable to an apprpriate directory. If needed, update the system PATH. The binaries are located in the "bin" directory in this GitHub repository.

There will be an installer for each of the platforms...but for now, drag and drop of the binary works.
Linux: Place in "/usr/bin" (or somewhere accessable). 
Windows: Place in a location that is in the PATH, or update PATH to the location the application is copied to.
Mac: Place in "/usr/local/bin" - first time running the tool will take about 20 seconds as the OS scans the executable. 

### Command line options

Once copyed into a location (and PATH updated if needed), you'll be able to run the tool using "grapa" from a terminal or command sheel from any location. Running the tool with "grapa --env" will display a few of the directories where the tool will look for information.

Other command line options:
```
        -h,--help       :Show this help message
        -v,--version    :Show version
        -q,--quite      :Suppress header
        -env            :Show environment details
        -ccmd script    :Run from script
        -cfile file     :Run from file
        -ccin           :Run from stdin
        -argcin         :Places std:cin into $ARGCIN environment variable (use with -ccmd|-cfile)
        -argv           :Places proceeding args into $ARGV environment variable
```

Example: Performas a SHAKE256 hash of the string "thisisatest".
```
grapa -ccmd "'thisisatest'.encode(SHAKE256)" -q
94B3D49AF1B6396CD186876793A5C4405A1BBFD12C7341521ABD62AA26E3E852B06B345D82126B1D864DFA885B6DC791D21A318259D307D76D7946D1EFF9DA54
```

Example: Same, but takes input from cin. 
```
echo "thisisatest".encode(SHAKE256) | grapa -ccin -q
94B3D49AF1B6396CD186876793A5C4405A1BBFD12C7341521ABD62AA26E3E852B06B345D82126B1D864DFA885B6DC791D21A318259D307D76D7946D1EFF9DA54
```

# Data Types

## Native Types

### $FLOAT

Supports both fix and float format. Fix will keep the number centered around the decimal, rounding to the precision specified. Float will not restrict to the decimal, but will roun to the the  precision specified. Each number maintains it's own precision, which is used in math operations with numbers of other precision. Some math operations may convert an input from one format to another. For example, passing in a float to a trig function will produce a fix format result. Floats also support specifying "extra" bits to apply to the calcuations to reduce error propagation (the default is 7 bits). 

Example:
<pre><code>> 5.13
5.13
</code></pre>

You can also create using hex format:
<pre><code>> -0x4.0x5
-4.3125
</code></pre>

Or binaary format:
<pre><code>> 0b101.11
5.625
</code></pre>

By default, all floats are "float" format. To change formats use the fix() and float() routines, where both the precision and the "extra" bits can also be specified. The parsing engine will set the precision to the system default, which is 128 bits. There are two ways to change this. First, change the system defualt using the setfloat and setfix routines. Second, pass in a $STR to the fix or float routines. For example:

<pre><code>> "3.5".float(300,6)
3.5
</code></pre>

To verify the float is being created properly, use the decode routine with FLOAT as the parameter. For example:

<pre><code>> "30.75".float(300,6).decode(FLOAT)
{"sign":false,"trunc":false,"fix":false,"exp":4,"max":300,"prec":6,"data":123}
> (30.75).decode(FLOAT).data
123
> (30.75).decode(FLOAT).data.hex()
7B
</code></pre>

Convert to other formats:
<pre><code>> (30.75).hex()
1E.C
> (30.75).bin()
11110.0011
> (30.75).int()
30
> (30.75).raw()
00048100077B
</code></pre>

Bit shifts:
<pre><code>> (30.75) >> 4
1.921875
> (30.75) << 4
492
</code></pre>

### $INT

### $STR

### $BOOL

### $ID

### $LIST

### $ARRAY

### $TIME

### $TABLE

### $RAW

### XML

## System Types

### $SYSID

### $SYSINT

### $SYSSTR

### $ERR

## Custom Types
Create custome types using the class routine. The underlying structure is a $LIST where variables are stored, and the class can inherit other classes, including system types/classes (each system type is initiated as a class instance).

The following examle defines a class with a single method. The method setdiv2 updates data to the parameter divided by 2.
<pre><code>> myClass = class {data = 0; setdiv2 = op(p){data = @p/2;}; get = op(){@data;};};
> test = myClass();
> @test.setdiv2(5);
> @test.get();
2
> @test
{"data":2}
</code></pre>

# Loops

## IF

## WHILE

## MAP

## REDUCE

## FILTER

# Commands


