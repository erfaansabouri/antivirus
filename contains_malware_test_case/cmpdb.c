#include <stdio.h>

int cmpdb(char *md5 , int lenMD5){
    static const char filename[] = "db.txt";
    FILE *file = fopen ( filename, "r" );
    if ( file != NULL ){
        char line [ 128 ]; /* or other suitable maximum line size */
        int counter = 0;
        while ( fgets ( line, sizeof line, file ) != NULL ){ /* read a line */
        int c;
        //printf("line #%d\t",counter++);
        //fputs ( line, stdout ); /* write the line */
        for(c = 0 ; c < lenMD5 ; c++){
            if (line[c] == md5[c])
                continue;
            else
                break;
        }
        if (c == lenMD5 - 1)
            return 1;
      }
      fclose ( file );
   }
   else{
      perror (filename); /* why didn't the file open? */
   }
   return 0;
}
