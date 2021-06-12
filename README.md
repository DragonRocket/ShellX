<!---
	Open this file in Web Browser with Markdown File Extension or MD File Viewer Application to read properly
	Otherwise Summary of this file
	1. Extract the zip
	2. Open terminal in extracted folder
	3. Installation: make
	4. Run: make run
-->

# ShellX

## ___'Lets Command With Your Fingers'___
-------------

**Table of Contents**

[TOCM]

[TOC]

## Features
-------------
Shell with following features
> I/O Redirection
> Interactive
> Background
> Simple
> Pipes
> Mix

###### Internal Commands
- cd [DIRECTORY PATH]
- exit [OPTIONAL NUMBER]


## Tech
-------------
ShellX is completely written in C

## Installation
-------------

ShellX requires [GCC](https://www.gnu.org/software/gcc/), [GDB](https://www.gnu.org/software/gdb/) and [make](https://www.gnu.org/software/make/) to run.

Install all dependencies before installation

##### Dependencies Installation (for debian based linux)
1. open terminal
2. run the following commands on terminal
```sh
sudo apt update -y
sudo apt upgrade -y
sudo apt install make -y
sudo apt install gdb -y
sudo apt install gcc -y
```

##### ShellX Installation
1. extract the zip
2. open the terminal in the extracted folder
3. run the following command on terminal
 ```sh
make
```


## RUN
-------------

#### Execute either way
Enter following commands on terminal

##### One Way
 ```sh
make run
```
##### Another Way
 ```sh
./bin/main
```

## Additional Command
-------------
- To remove the previous build
```sh
make clean
```

## END
-------------
