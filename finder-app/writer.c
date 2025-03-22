#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>



int main( int argc, char** argv ) {
    // if the number of arguments is not 2, print usage and return
    // first argument is the file location and the second is the content that we want to write
    if( argc != 3 ) {
        printf( "Usage: %s <file> <string>\n", argv[0] );
        return 1;
    }

    openlog ("writer", LOG_PID | LOG_CONS, LOG_USER);

    // open the file in write mode
    FILE* file = fopen( argv[1], "w" );
    if( file == NULL ) {
        printf( "Error: Could not open file %s\n", argv[1] );
        syslog (LOG_ERR, "Could not open file %s", argv[1]);
        return 1;
    }

    // print a loggin message
    syslog (LOG_DEBUG, "Writing %s to %s", argv[2], argv[1]);   

    // write the content to the file
    fprintf( file, "%s", argv[2] );
     

    fclose( file );
    return 0;
}