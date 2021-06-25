# Cograsim
cograsim is a command line tool that helps streamline the process of performing course grain molecular dynamics simulations of RNA/DNA using the oxDNA, cogli2, tacoxDNA packages.

# Disclaimer
I am not the author of the oxDNA, cogli2 or tacoxDNA. I have just included them here for easy processing and faster workflows for all using this repository.
The links of the original packages are 
* [oxDNA](https://dna.physics.ox.ac.uk/index.php/Main_Page)
* [cogli2](https://sourceforge.net/projects/cogli1/)
* [tacoxDNA](https://github.com/lorenzo-rovigatti/tacoxDNA)

*In case there are errors with any of the packages mentioned above, I would suggest downloading and building the required packages inside of the `lib/` directory.*

*The code has been tested on only one machine (mine) `(on a DELL G5 15 running 64-bit POP!_OS)`*

( cograsim is based on [this](https://github.com/avi1299/Coarse_Grained_RNA_Folding) project by [Avinash Sontakke](https://github.com/avi1299) )

# Usage
```
./cograsim [option] [molecule]

Options:
	sim	run simulations only, no visualization
	vis	visualize already performed simulations
	list	list the temperaures for which simulations exist
	get	Download the required PDB file
	clean	clean the required molecule's directory
```
