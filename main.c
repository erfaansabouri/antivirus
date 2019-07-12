#include <stdio.h>
#include <string.h>
#include "tinyfiledialogs.h"
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include "md5.c"
#include "cmpdb.c"
#include "structs.c"

#define DEFAULTMD5SIZE 32



char *MD5_file (char *path, int md5_len){
    errno = 0;
	/* creating structure for using md5 */

	FILE *fp = fopen (path, "rb");
	if (fp == NULL){
		fprintf (stderr, "fopen %s failed\n", path);

        printf("Error %d \n", errno);
		return NULL;
	}

	/*
		The MD5Init(), MD5Update(), and MD5Final() functions are the core functions.
		Allocate an MD5_CTX, initialize it with MD5Init(), run over the data with MD5Update(),
		and finally extract the result using MD5Final().

		by using these :
		* MD5Context md5;
		* MD5Init(&md5);
		* MD5Update(&md5, data, datalen);
		* MD5Final(digest, &md5);
		splitting it up into that many functions is to let you stream large datasets.
		would read the file in smaller chunks
	*/
	MD5_CTX mdContext;
	MD5Init(&mdContext);

	/*
		by using fread here , every single byte from file
		will be read and put in to data array
		updating byte by byte to do the function in smaller chunks
	*/

	int bytes;
	unsigned char data[1024];
	while ((bytes = fread (data, 1, 1024, fp)) != 0){
		MD5Update(&mdContext, data, bytes);
	}

	/*
		path -> *fp -> data -> update data -> mdContext -> finilize
		finilizing md5 structure
	*/
	MD5Final (&mdContext);



	char *file_md5;
	file_md5 = (char *)malloc((md5_len + 1) * sizeof(char));
	if(file_md5 == NULL){
		printf("Allocation memory failed !\n");
		return NULL;
	}

	/* coping zero value to md_len + 1 character of file_md5 (initilization) */

	memset(file_md5, 0, (md5_len + 1));


	int i;
	if(md5_len == 16){
		for(i=4; i<12; i++){
			// writing digest of mdContext in file_md5 in hex mode
			sprintf(&file_md5[(i-4)*2], "%02x", mdContext.digest[i]);
		}
	}
	else if(md5_len == 32){
		for(i=0; i<16; i++){
			sprintf(&file_md5[i*2], "%02x", mdContext.digest[i]);
		}
	}

	else{
		fclose(fp);
		free(file_md5);
		return NULL;
	}

	fclose (fp);
	return file_md5;
}


int main( int argc , char * argv[] )
{
	int lIntValue;
	char const * lTmp;
	char const * lTheSaveFileName;
	char const * lTheOpenFileName;
	char const * lTheSelectFolderName;
	char const * lTheHexColor;
	char const * lWillBeGraphicMode;
	unsigned char lRgbColor[3];
	FILE * lIn;
	char lBuffer[1024];
	char lString[1024];
	char const * lFilterPatterns[2] = { "*.txt", "*.text" };

	tinyfd_verbose = argc - 1;
    tinyfd_silent = 1;

#ifdef _WIN32
	tinyfd_winUtf8 = 0; /* on windows, you decide if char holds 0(default): MBCS or 1: UTF-8 */
#endif

	lWillBeGraphicMode = tinyfd_inputBox("tinyfd_query", NULL, NULL);

#ifdef _MSC_VER
#pragma warning(disable:4996) /* silences warning about strcpy strcat fopen*/
#endif

	strcpy(lBuffer, "v");
	strcat(lBuffer, tinyfd_version);
	if (lWillBeGraphicMode)
	{
		strcat(lBuffer, "\ngraphic mode: ");
	}
	else
	{
		strcat(lBuffer, "\nconsole mode: ");
	}
	strcat(lBuffer, tinyfd_response);
	strcat(lBuffer, "\n");
	strcat(lBuffer, tinyfd_needs+78);
	strcpy(lString, "Welcome to Antivirus!");
	tinyfd_messageBox(lString, "An inefficient virus kills its host. A clever virus stays with it.\n~ James Lovelock\n\nA simple antivirus developed by :\n\t- Erfan Sabouri\n\t- Shayan", "ok", "info", 0);



	lTheSelectFolderName = tinyfd_selectFolderDialog(
		"Please select a directory to search for malware : ", NULL);

	if (!lTheSelectFolderName){
		tinyfd_messageBox(
			"Error",
			"Select folder name is NULL",
			"ok",
			"error",
			1);
		return 1;
	}

	int res = tinyfd_messageBox("Sure?",lTheSelectFolderName, "yesno", "question", 0);
	if(res == 0)
		return 0;
	/////////////
	char *md5;
    DIR *d;
    struct dirent *dir;
    int flag = 0;
    int i = 0 ;
	strcat(lTheSelectFolderName,"\\");
    d = opendir(lTheSelectFolderName);
    struct Node *head = NULL;
	int virus_counter = 0;
    if(d){

		// skip . and .. directory to reach files.
        while ((dir = readdir(d)) != NULL && flag != 1){
            flag++;
        }
        while((dir = readdir(d)) != NULL){
            printf("\nFile name : %s\n" , dir->d_name);
            char malwares_folder[2000] = {0};
            for(i=0 ; i<strlen(lTheSelectFolderName) ; i++){
                malwares_folder[i] = lTheSelectFolderName[i];
            }
            strcat(malwares_folder, dir->d_name);
            md5 = MD5_file(malwares_folder, DEFAULTMD5SIZE);
            printf("MD5 : %s\n" , md5);
            if(cmpdb(md5 , DEFAULTMD5SIZE) == 1){
                head = add(dir->d_name , head);
				virus_counter++;
				//record_in_table(username,sql);
                printf("*****THIS '%s' IS VIRUS!*****\n\n" , dir->d_name);
            }
    	}
    closedir(d);
    }
	if(virus_counter == 0){
		tinyfd_notifyPopup("You are safe!", "There is no malware in this directory :)", "info");
		return 0;
	}
	struct Node *temp = head;
	printf("%d malware founded : \n" , virus_counter);
	tinyfd_notifyPopup("Aww!", "I have found some Threats!", "warning");
	while(temp != NULL){
		printf("Would you like to delete this malware?\n [%s]\n" , temp->name);
		char response[100] = "Would you like to delete this malware? ";
		strcat(response , temp->name);
		res = tinyfd_messageBox("Be healthy!",response, "yesno", "question", 0);
		if (res == 1){
			char malwares_folderr[2000] = {0};
            for(i=0 ; i<strlen(lTheSelectFolderName) ; i++){
                malwares_folderr[i] = lTheSelectFolderName[i];
            }
			strcat(malwares_folderr, temp->name);
			printf("remove ret : %d" , remove(malwares_folderr));

		}
		temp = temp -> next;
	}




	///////////////


	return 0;
}

/*
OSX :
$ clang -o hello.app hello.c tinyfiledialogs.c
( or gcc )

UNIX :
$ gcc -o hello hello.c tinyfiledialogs.c
( or clang tcc owcc cc CC  )

Windows :
	MinGW needs gcc >= v4.9 otherwise some headers are incomplete:
	> gcc -o hello.exe hello.c tinyfiledialogs.c -LC:/mingw/lib -lcomdlg32 -lole32

	TinyCC needs >= v0.9.27 (+ tweaks - contact me) otherwise some headers are missing
	> tcc -o hello.exe hello.c tinyfiledialogs.c
	    -isystem C:\tcc\winapi-full-for-0.9.27\include\winapi
	    -lcomdlg32 -lole32 -luser32 -lshell32

	Borland C: > bcc32c -o hello.exe hello.c tinyfiledialogs.c

	OpenWatcom v2: create a character-mode executable project.

	VisualStudio :
	  Create a console application project,
	  it links against Comdlg32.lib & Ole32.lib.
*/
