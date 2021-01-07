#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

//function prototypes
const char *getFilenameExt(const char *);
void DFS_lineCount(char *, char *, int);

//global counter
int totalLines=0;

int main(int argc, char* argv[]){
    char *root, *ext;
    //command line arguments
    //if the command line arguments are out of our bounds give error message
    if (argc != 3){
      printf("Make sure to enter in the executable, then the directory, then the file extension without the dot!:\n ./executable (directory) (filextension)");
      return 0;
    }
    else{
      //if the command line arguments are in bounds set the root directory and file extension
      root=argv[1];
      ext=argv[2];
    }

    printf("Directory scan of %s\n",root);
    DFS_lineCount(root, ext, 0);
    printf("You have written %d lines of %s code!\n", totalLines, ext);

    return 0;
}

/*This function takes a pointer to a string that contains a filename and figures out
what the file extension is.*/
const char *getFilenameExt(const char *filename){
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return "";
  return dot + 1;
}

/*This function uses depth first search traversal over a filesystem tree.
The function is given a root directory, a target file extension and
a depth (for indentation). It will recursively traverse the tree
and print out each directory and file within the tree. Once it knows
it has encountered a file it will check to see if the target file
extension is the same as the current file extension for said file.
If they are the same it will then open the file and count the number
of lines within the file using a global variable "totalLines".*/
void DFS_lineCount(char *rootDir, char *targetFileExt, int depth){
  DIR *myDir;
  struct dirent *current;
  struct stat status;
  int spaces = depth*4;

  //check to see if the directory exists
  if((myDir = opendir(rootDir)) == NULL){
    fprintf(stderr,"cannot open directory: %s\n", rootDir);
    return;
  }

  //change the working directory
  chdir(rootDir);

  //while the current directory exists
  while((current = readdir(myDir)) != NULL){

    //get link status
    lstat(current->d_name,&status);

    //is this a directory
    if(S_ISDIR(status.st_mode)){
      //We found a directory
      //Ignore the dots
      if(strcmp(".",current->d_name) == 0 || strcmp("..",current->d_name) == 0){
        continue;
      }
      //print directory
      printf("%*s%s/\n",spaces,"",current->d_name);
      //Call function recursively to open the directory we just found
      DFS_lineCount(current->d_name, targetFileExt, depth+1);
    }
    else {
      //We found a file
      //print file
      printf("%*s%s\n",spaces,"", current->d_name);

      //get the extension of the current file
      char *currentFileExt = getFilenameExt(current->d_name);

      //if the current file extension matches the target extension
      if(strcmp(currentFileExt, targetFileExt)==0){
        //open the file for reading
        FILE *fPtr = fopen(current->d_name, "r");

        //traverse the file
        while(!feof(fPtr)){
          //if there is a new line character
          int temp = fgetc(fPtr);
          if(temp=='\n'){
            //add a line to the total amount of lines
            totalLines++;
          }
        }

        //close the file
        fclose(fPtr);
        fPtr=NULL;
      }
    }
  }

  //close the directory
  chdir("..");
  closedir(myDir);
}
