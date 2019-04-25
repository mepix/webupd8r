#include <fstream>
#include <iostream>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

//using namespace std;
bool checkIfHTMLfile(std::string file);
void replaceDiv(std::string fileToEdit, std::string fileToAdd, std::string tagStart, std::string tagEnd);

//GLOBALS
std::string PATH_TO_HTML = "~/Workspace/webUpd8ter/testHTML/";

int main(int argc, char* argv[])
{
    //Validate Input Arguments
    for (int i=0; i<argc; i++) {
        std::cout << "ARGV[" << i << "]=" << argv[i] << std::endl;
    }

    if(argc < 2){
        std::cout << "Path to HTML required." << std::endl;
        return -1;
    } else {
        PATH_TO_HTML = std::string(argv[1]);
    }

    //Assumes there is a "nav.html" and a "footer.html"
    std::string nav = PATH_TO_HTML + "nav.html";
    std::string footer = PATH_TO_HTML + "footer.html";

    //Loop through the files in the directory
    for(auto& p: fs::directory_iterator(PATH_TO_HTML)){
        //Convert the directory iterator to a nice std::string
        std::string file = p.path().string();

        //We don't want to overwrite the header or footer
        if(p == nav || p == footer){
            std::cout << "Skipping Header or Footer" << std::endl;
            continue;
        }

        //We only care about HTML files
        if(!checkIfHTMLfile(file)){
            continue;
        }
        std::cout << file << std::endl;

        //Replace the header
        replaceDiv(file, nav,"<nav","</nav>");

        //Replace the footer
        replaceDiv(file, footer,"<footer","</nav>");

    }
    return 0;
}

/**
 * @brief checkIfHTMLfile Checks if the given file has a ".html" extension
 * @param file The complete path to the file to inspect
 * @return [T] if the sent file is a file with a ".html" extension
 */
bool checkIfHTMLfile(std::string file){
    std::size_t extPos = file.find_last_of(".");
    //Check if ANY extension is found
    if (extPos == std::string::npos){return false;}

    std::string extension = file.substr(extPos);
//    std::cout << extension << std::endl;
    if(extension == ".html"){return true;}
    else {return false;}
}

/**
 * @brief replaceDiv This function does the heavy lifting of opening the fileToEdit and replacing
 * the section of code bounded by the <tag>.
 * @param fileToEdit The original file that will be modified with the tag
 * @param fileToAdd A second file with the section to be replaced.  Note this file should only
 * contain information within "<tag" and "</tag>"
 * @param tagStart The opening tag in of the division to replace in the form "<tag" with no closing "."
 * @param tagEnd The ending tag of the division to replaced in the form "</tag>"
 */
void replaceDiv(std::string fileToEdit, std::string fileToAdd, std::string tagStart, std::string tagEnd){
    std::ifstream toRead(fileToEdit);
    std::ifstream toAdd(fileToAdd);
    std::string line;

    std::string tempFile = PATH_TO_HTML + "temp.html";
    std::ofstream toWrite;
    toWrite.open(tempFile);

    //find first instance of <nav>
    bool foundTagOpen = false;
    bool foundTagClose = false;
    while(std::getline(toRead,line)){

        if(!foundTagOpen){ //Looking for start <tag>
            //Look for the open tag
            std::size_t tagPosOpen = line.find(tagStart);
            if(tagPosOpen != std::string::npos){
                foundTagOpen = true;
                //Read in the replacement file.
                while(std::getline(toAdd,line)){
                    toWrite << line << std::endl;
                }
            } else {
                toWrite << line << std::endl;
            }

        } else if(foundTagOpen && !foundTagClose){ //Looking for end </tag>
            std::size_t tagPosClose = line.find(tagEnd);
            if(tagPosClose != std::string::npos){
                foundTagClose = true;
            }
            continue; //Ignore the current line b/c it is in between <tag> & </tag>
        } else { //retrun to printing the main body
            toWrite << line << std::endl;
        }
    }

    //Rename the temp file to the actual file
    fs::rename(tempFile,fileToEdit);
    fs::remove(tempFile);

    //Close stream files
    toRead.close();
    toAdd.close();
    toWrite.close();
}
