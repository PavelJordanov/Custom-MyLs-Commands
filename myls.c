#include "secret_headers.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>

int hasI = 0;
int hasL = 0;
int hasR = 0;

void get_and_Print_Group(gid_t grpNum)
{
    struct group *grp = getgrgid(grpNum);

    if (grp) {
        printf(" %s", grp->gr_name);
    } else {
        perror("No group name found\n");
    }
}

void get_and_Print_UserName(uid_t uid)
{
    struct passwd *pw = getpwuid(uid);

    if (pw) {
        printf(" %s ", pw->pw_name);
    } else {
        perror("Hmm not found???");
    }
}

void get_and_Print_DeviceID(dev_t st_rdev) {
	printf("%d ", st_rdev); 
}

void get_and_Print_Size(off_t st_size) {
	
	printf("%*ld ",10, st_size);
}

void print_OptionI(struct stat s){
    //printf("%l0d ", s.st_ino);
	char buff[20];
	sprintf(buff, "%lu", s.st_ino);
	printf("%*s ", 18, buff);
	//printf("%*d ", 18, s.st_ino);
}

void print_Permision(struct stat s){
    //print out the permission type. reference: https://stackoverflow.com/questions/32985809/how-to-know-if-a-file-is-readable-by-other-user-in-c
    printf((S_ISDIR(s.st_mode)) ? "d" : "-");
    printf((s.st_mode & S_IRUSR) ? "r" : "-");
    printf((s.st_mode & S_IWUSR) ? "w" : "-");
    printf((s.st_mode & S_IXUSR) ? "x" : "-");
    printf((s.st_mode & S_IRGRP) ? "r" : "-");
    printf((s.st_mode & S_IWGRP) ? "w" : "-");
    printf((s.st_mode & S_IXGRP) ? "x" : "-");
    printf((s.st_mode & S_IROTH) ? "r" : "-");
    printf((s.st_mode & S_IWOTH) ? "w" : "-");
    printf((s.st_mode & S_IXOTH) ? "x" : "-");
    printf(" ");
}

void print_Time(struct stat s){
	time_t t = s.st_mtime;
	char date[100];
	
	struct tm *tm = localtime(&t);
	strftime(date, sizeof(date), "%b %e %Y %H:%M", tm);
	printf("%s ", date);
	
	
	/*
    // print out the last modified time
    time_t t = s.st_mtime;
    struct tm timeInfo;
    localtime_r(&t,&timeInfo);
    char timeStr[50];
    strftime(timeStr, sizeof timeStr, "%b %-2d %Y %H:%M", &timeInfo);
    printf("%s ", timeStr);
	*/
}

void print_OptionL(struct stat s){
    print_Permision(s);
    // print out the number of links
    printf("%3d ", s.st_nlink);
    // print out the group name
    get_and_Print_Group(s.st_gid);
    // print out the user
    get_and_Print_UserName(s.st_uid);
	// print out the device ID
	//get_and_Print_DeviceID(s.st_rdev);
	get_and_Print_Size(s.st_size);
    // print out the last modified time
    print_Time(s);
}

void read_Option(char input){
    if(input == 'i'){hasI = 1;}
	else if(input == 'l'){hasL = 1;}
	else if(input == 'R'){hasR = 1;}
}

int valid_Input(int argc, char* argv[]){
    for(int i = 1; i < argc; i++){
        // validate for supported options i, l, R
        if(argv[i][0] == '-'){
            for(int j = 1; j < strlen(argv[i]); j++){
                if(argv[i][j] != 'i'&& argv[i][j] != 'l' && argv[i][j] != 'R'){
                    perror("Error: Unsupported Option\n");
                    return 0;
                }
            }
        }
        // validate for existing directory
        if(argv[i][0] != '-'){
            if(argv[i][0] == '/'){

            }
        }
    }
    return 1;
}

/*
int bethasort(const struct dirent ** a,
              const struct dirent **b) {
	int res = stricmp((*a)->d_name, (*b)->d_name);
	return	res;
}
*/

// Traverse the directory tree recursively using Depth-first search traversal.
// Since Depth-first search is simply pre-order traversal, we perform pre-order traversal. 
// Pre-order traversal also outputs the nodes in a lexicographic order.
void pre_order(char* name) {
	if (name == NULL) return;
	
	struct stat s = {0};
	int result = stat(name, &s);
	if	(result == -1){
		if (errno == 2) {
			printf("Error: Nonexistent files or directories");
		} else {
			perror("stat() error");
		}
		exit(1);
	} else {
		if (!S_ISDIR(s.st_mode)) {
			//If 'i' is included in the option
			if(hasI){
			// print out the inode number of each file
				print_OptionI(s);
			}
			//printf("%ld ", s.st_ino);
			if(hasL){
				print_OptionL(s);                    
			}					
			// print out the name of each file
			printf("%s\n", name);
			return;
		}
	}
	
	if (hasR) {
		printf("\n%s:\n", name);
	}

	struct dirent **dir_entries;
	int n;
	n = scandir(name, &dir_entries, NULL, alphasort);
	if (n < 0) {
		if (errno == 2) {
			printf("Error: Nonexistent files or directories");
		} else {
			perror("scandir");
		}
		exit(1);
	} else {
		for (int i = 0; i < n; i++) {
			struct dirent* dir_entry = dir_entries[i];
			//printf("%s\n", dir_entry->d_name);
			// print out the group name and user name
			struct stat s = {0};
			//int result = stat(dir_entry->d_name, &s);
			
			char pathname[strlen(name) + strlen(dir_entry->d_name) + 2];
			pathname[0] = 0;
			strcat(pathname, name);
			if (strcmp(name, "/") != 0) {
				strcat(pathname, "/");	
			}
			strcat(pathname, dir_entry->d_name);
			
			int result = stat(pathname, &s);

			if	(result == -1){
				perror("stat() error");
			} else {
				//If 'i' is included in the option
				if(hasI){
					// print out the inode number of each file
					print_OptionI(s);
				}
				//printf("%ld ", s.st_ino);
				if(hasL){
					print_OptionL(s);                    
				}
				
				// print out the name of each file
				printf("%s\n", dir_entry->d_name);	
				
				if (!S_ISDIR(s.st_mode)){ //only include the normal files				
				
				} else {
					if (hasR && 
						strcmp(dir_entry->d_name, ".") != 0 && 
						strcmp(dir_entry->d_name, "..") != 0) {
						pre_order(pathname);
					}
				}
			}
			free(dir_entries[i]);
		}
	}
}

int main(int argc, char* argv[]){
	int hasDir = 0;
	
    //Check for the input options i, l, R
    if (valid_Input(argc, argv)) {
		for(int i = 1; i < argc; i++){
			if(argv[i][0] == '-'){
				for(int j = 0; j < strlen(argv[i]); j++){
					read_Option(argv[i][j]);
                }					
            } else {
				hasDir = 1;
				pre_order(argv[i]);
			}
	    }
		
		if (!hasDir) {
			pre_order(".");
		}
		
        return 0;
    } else {
		return 1;
    }
}