//
// A fake compiler which fails with MSVC's C1056 error a given number of times
// before creating the output file.
//
// Args: <input> <output> <numFailures> [ignored...]
//
#include <stdio.h>
#include <stdlib.h>

int main( int argc, char ** argv )
{
    if ( argc < 4 )
    {
        printf( "Bad Args!\n" );
        return 1;
    }

    const char * input = argv[ 1 ];
    const char * output = argv[ 2 ];
    const int numFailures = atoi( argv[ 3 ] );

    // Track the number of invocations in a file next to the output
    char attemptsFile[ 1024 ];
    snprintf( attemptsFile, sizeof( attemptsFile ), "%s.attempts", output );
    int attempts = 0;
    if ( FILE * f = fopen( attemptsFile, "rb" ) )
    {
        if ( fscanf( f, "%d", &attempts ) != 1 )
        {
            attempts = 0;
        }
        fclose( f );
    }
    ++attempts;
    FILE * f = fopen( attemptsFile, "wb" );
    if ( f == nullptr )
    {
        printf( "Failed to open '%s'\n", attemptsFile );
        return 1;
    }
    fprintf( f, "%d", attempts );
    fclose( f );

    if ( attempts <= numFailures )
    {
        // Same output and return code as cl.exe
        printf( "%s : fatal error C1056: cannot update the time date stamp field in '%s'; error code %%u\n", input, output );
        return 2;
    }

    f = fopen( output, "wb" );
    if ( f == nullptr )
    {
        printf( "Failed to open '%s'\n", output );
        return 1;
    }
    fwrite( &attempts, sizeof( attempts ), 1, f );
    fclose( f );
    return 0;
}
