#!/bin/sh

# check if we have two arguments
if [ $# -lt 2 ]
then
    echo "Usage: $0 <file> <string>"
    exit 1
fi

# now we have to create the directory and file if it does not exist
mkdir -p $(dirname $1)

# print error message if we cannot create the directory
if [ $? -ne 0 ]
then
    echo "Cannot create directory"
    exit 1
fi

# now we have to write the string to the file
echo $2 > $1    

