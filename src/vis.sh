#! /bin/sh

if [ "$#" -ne 3 ]; then
    echo "Illegal number of parameters"
    echo "Usage: $0 <pdb_name> <temperature> <rna/dna>"
    exit 1
fi

mol=$1
if [ ! -d "./molecules/$1" ]; then
   echo "ERROR : ./molecules/ does not contain directory for molecule $1"
   echo "SUGGESTION : Use PDB_Downloader.sh to download the data for the molecule and try again".
   exit 1
fi

tmp=$(echo $2 | sed "s/\./_/g")
ity=$3
path="./molecules/$mol/$mol"
if [ ! -f $path-"$tmp"C-trajectory.dat ]; then
   echo "ERROR : ./molecules/$mol/ does not contain the data file for the given temperature - $2C"
   echo "SUGGESTION : use './tool sim' to first generate the required data for the given temperature"
   exit 1
fi

./lib/cogli2/build/bin/cogli2 --drums --$3 --always-centre -t $path.pdb.top $path-"$tmp"C-trajectory.dat &
