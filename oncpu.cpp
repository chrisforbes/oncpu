#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cstdlib>

int main( int argc, char ** argv )
{
	if (argc < 3)
	{
		printf( "oncpu: runs a command on a particular cpu.\n" );
		printf( "usage: oncpu <#> <command>\n" );
		printf( "example:\n\n\t> oncpu 0 notepad\n\nruns `notepad` restricted to CPU0.\n" );
		return 1;
	}

	int cpu = atoi( argv[1] );
	HANDLE self = GetCurrentProcess();

	if (!SetProcessAffinityMask( self, 1 << cpu ))
	{
		printf( "oncpu: failed to set process affinity.\n" );

		// report valid cpus
		DWORD current, sys;
		if (!GetProcessAffinityMask( self, &current, &sys ))
		{
			printf( "Access Denied.\n" );
			return 1;
		}

		printf( "available cpus: " );

		int i = 0;
		while( sys )
		{
			if (sys & 1)
				printf( "%d ", i );
			
			++i;
			sys >>= 1;
		}
		printf("\n");
		return 1;
	}

	char sz[2048];
	sz[0] = 0;

	for( int i = 2; i < argc; i++ )
	{
		if ( i > 2 )
			strcat( sz, " " );

		if (strstr(argv[i], " "))
		{
			strcat( sz, "\"" );
			strcat( sz, argv[i] );
			strcat( sz, "\"" );
		}
		else
			strcat( sz, argv[i] );
	}

//	printf( "oncpu: starting on cpu %d: %s\n", cpu, sz );

	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	memset( &si, 0, sizeof(si) );
	memset( &pi, 0, sizeof(pi) );

	si.cb = sizeof(si);

	if (!CreateProcess( NULL, sz, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi ))
	{
		printf("oncpu: failed to start target process.\n");
	}

	return 0;
}