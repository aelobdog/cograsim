#!/bin/sh

# Script to download files from RCSB http file download services.
# Use the -h switch to get help on usage.
if ! [ `command -v curl` ]; then 
   echo "'curl' not found. Install 'curl'"
   exit 1
fi

PROGNAME=$0
BASE_URL="https://files.rcsb.org/download"

[ ! -d "./molecules" ] && mkdir molecules

if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    echo "Usage: $PROGNAME <pdb_name>"
else
   cd molecules
	url="$BASE_URL/$1.pdb"
	mkdir $1
	out="./$1/$1.pdb"
	echo "Downloading $url to $out"
	curl -s -f $url -o $out || (echo "Failed to download $url" && rmdir $1)
   cd ..
	exit 1
fi

