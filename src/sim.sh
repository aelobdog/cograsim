#! /bin/sh

if [ "$#" -ne 1 ]; then
    echo "Illegal number of parameters"
    echo "Usage: $0 <pdb_name>"
    exit 1
fi

if [ ! -d "./molecules" ]; then
   echo "ERROR : Directory "molecules" does not exist"
   exit 1
fi

cd molecules 

if [ ! -d "./$1" ]; then
   echo "ERROR : Directory "./molecules/$1" does not exist"
   echo "SUGGESTION : Use PDB_Downloader.sh to download the required files for your molecule"
   exit 1
fi

cd $1

# Step 1: Creation of topology and configuration files from the PDB
echo "\n\nCreating the topology and initial configuration file from the PDB.\n 53 indicates chain is from 5' -> 3'"
python3 ../../lib/tacoxDNA/src/PDB_oxDNA.py $1.pdb 53 

# Step 2 : Creating the configuration "input" files
python3 ../../src/create-conf.py $1

#Step 3: Running the simulation
echo "\n\nRunning the simulation according to the parameters."

# get names of all the input files
input_files=`ls | grep "inp*"` 

# run the simulation for all the files
for file in $input_files
do
   echo "file in progress : $file"
   ../../lib/oxDNA/build/bin/oxDNA $file > /dev/null &
done

cd ..
cd ..
