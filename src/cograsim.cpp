#include <iostream>
#include <cstdlib>
using namespace std;

/*    This tool is a shared interface to the simulation and visualization
 *    scripts. It invokes the correct script according to the inputs
 *    provided.
 *    
 *    TODO:
 *       DECISION: handle errors regarding existence of files/directories here ?
 *       DECISION: use library funtion to get cwd ?
 */

void usage() {
   cout << "Usage:\n\t./tool [option] [molecule]";
   cout << "\nOptions:\n";
   cout << "\tsim\trun simulations only, no visualization\n";
   cout << "\tvis\tvisualize already performed simulations\n";
   cout << "\tlist\tlist the temperaures for which simulations exist\n";
   cout << "\tget\tDownload the required PDB file\n";
   cout << "\tclean\tclean the required molecule's directory\n\n";
}

int main(int argc, char** argv) {
   if (argc != 3) {
      cout << "Options not passed. Run the tool using the options listed\n";
      usage();
      return 0;
   }

   string molecule = argv[2];
   string command;
   
   if (argv[1] == string("sim")) {                     // run simulations only
      command = "sh ./src/sim.sh " + molecule;
      system(command.c_str());
   } 
   else if (argv[1] == string("vis")) {                // visualize simulation only
      string t, it;
      cout << "temperature : ";
      cin >> t;
      cout << "dna or rna : ";
      cin >> it;
      command = "sh ./src/vis.sh " + molecule + " " + t + " " + it;
      system(command.c_str());
   }
   else if (argv[1] == string("list")) {
      command = "ls ./molecules/" + molecule + "/ | grep \"input_\" | sed 's/^[^_]*_//' | sed 's/_/./g'"; // may not be the best way to do this, but it works
      system(command.c_str());
   }
   else if (argv[1] == string("get")) {
      command = "sh ./src/PDB_Downloader.sh " + molecule;
      system(command.c_str());
   }
   else if (argv[1] == string("clean")) {
      command = "cd ./molecules/" + molecule + "; rm " + molecule + "-*; rm input_*; cd .. ; cd .. ;";
      system(command.c_str());
   }
   else {
      cout << "Illegal Option \"" << argv[1] << "\"\n";
      usage();
      return 0;
   }
   
   return 0; // can be removed
}
