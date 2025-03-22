#!/bin/sh


# This script is used to find the files in the given directory

# first we have to check if the number of arguments are less than 2
if [ $# -lt 2 ]
then
    echo "Usage: $0 <directory> <string>"
    exit 1
fi  


# check if the directory exists
if [ ! -d $1 ]
then
    echo "Directory $1 does not exist"
    exit 1
fi

# check the number of files in the directory and subdirectories
NUMFILES=$(find $1 -type f | wc -l)

# check the number of files containing the string
NUMMATCHES=$(grep -r $2 $1 | wc -l)

echo "The number of files are ${NUMFILES} and the number of matching lines are ${NUMMATCHES}"