#pragma warning(disable : 4996)
#include <assert.h>
#include <iostream>
#include <queue>	
#include <list>
#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <unistd.h>
#include <getopt.h>
using namespace std;

#define MAX_WORD 2478	
typedef unsigned char BYTE;
class tree
{
public:
	tree( ) {}
	tree( unsigned char name1_, unsigned char name2_, int freq_ )
	{
		name[0] = name1_;
		name[1] = name2_;
		freq = freq_;
		left_child = list<tree>( );
		right_child = list<tree>( );
	}
	unsigned char name[2];
	int freq;
	list<tree>  left_child;
	list<tree>  right_child;
};
class code
{
public:
	unsigned char name[2];
	string huffcode;
};
struct Mtree {
	bool operator()( const tree& l, const tree& r ) {
		return l.freq > r.freq;
	}
};
struct Mcode {
	bool operator()( const code& l, const code& r ) {
		return l.huffcode > r.huffcode;
	}
};
bool decode( string );
bool search( vector<code>&, string&, BYTE[2] );
bool sorting( const code&, const code& );
bool frequency( char*, int[][94], int* );
void makeNode( int[][94], int*, priority_queue<tree, vector<tree>, Mtree>& );
bool huffmanEncode( string, priority_queue<code, vector<code>, Mcode>& );
void convertBinary( string, priority_queue<code, vector<code>, Mcode>& );
void makeTree( priority_queue<tree, vector<tree>, Mcode>& );
void inorder( const tree&, priority_queue<code, vector<code>, Mcode>&, string );
bool makeCode( const priority_queue<tree, vector<tree>, Mtree>&, priority_queue<code, vector<code>, Mcode>& );

const bool ERROR = false;

int main(int argc, char** argv)
{
	char option;

	if (argc != 3)
	{
		printf("number of arguments must be three.\n");
		return -1;
	}


	while ((option = getopt(argc, argv, "c:d:")) != -1)
	{
		string sArg = "";
		priority_queue<code, vector<code>, Mcode> huffcode;

		switch(option)
		{
		case 'c':
			sArg = optarg;
			assert( huffmanEncode( sArg, huffcode ) != false );	//error
			cout << "Then, your output file should be \"" << argv[2] << ".zz\"" << endl;
			break;
		case 'd':
			sArg = optarg;
			assert( decode( sArg ) != false );
			cout << "Then, your output file should be \"" << argv[2] << ".yy\"" << endl;
			break;
		default:
			printf("unknown arg: %s\n", option);
			break;
		}
	}
	return 0;
}

bool sorting( const code& l, const code& r )
{
	return l.huffcode < r.huffcode;
}

bool search(
	vector<code>& v,
	string& s,
	BYTE word[2] )
{
	int left = 0;
	int right = v.size( ) - 1;

	while ( left <= right ) {
		int mid = ( left + right ) / 2;
		if ( v[mid].huffcode < s )
			left = mid + 1;
		else if ( s < v[mid].huffcode )
			right = mid - 1;
		else {
			word[0] = v[mid].name[0];
			word[1] = v[mid].name[1];
			return true;
		}
	}
	return false;
}

bool decode( string name )
{
	vector<code> v;

	FILE* file = fopen( name.c_str( ), "rb" );
	if ( file == NULL ) {
		return false;
	}
	string outfilename = name;
	outfilename.pop_back( );
	outfilename.pop_back( );
	outfilename.pop_back( );
	outfilename += ".zz.yy";
	FILE* decoded = fopen( outfilename.c_str( ), "wt" );
	char msb;
	int codenum;

	fscanf( file, "%c", &msb );
	fscanf( file, "%d", &codenum );
	for ( int i = 0; i < codenum; ++i ) {
		code item;
		char valLo;
		fscanf( file, "%c", &item.name[0] );
		if ( item.name[0] > 127 )
			fscanf( file, "%c", &item.name[1] );
		else
			item.name[1] = 0;
		fscanf( file, "%c", &valLo );
		BYTE buffer = 0;
		while ( valLo > 0 ) {
			fscanf( file, "%c", &buffer );

			for ( int j = 7; j >= 0; --j ) {
				if ( valLo <= 0 )
					break;
				char bitdata = ( buffer >> j ) & 1;

				item.huffcode.push_back( bitdata + '0' );
				--valLo;
			}
		}
		v.push_back( item );
	}

	sort( v.begin( ), v.end( ), sorting );
	BYTE buffer, EOFcheck;
	int cnt = 0;
	string huffcode;
	while ( fscanf( file, "%c", &buffer ) != EOF ) {
		if ( fscanf( file, "%c", &EOFcheck ) == EOF ) {
			for ( int i = 7; i >= 7 - msb; --i ) {
				char bitdata = ( buffer >> i ) & 1;
				huffcode.push_back( bitdata + '0' );
				BYTE write_word[2] = { 0 };
				bool found = false;
				found = search( v, huffcode, write_word );
				if ( found ) {
					fprintf( decoded, "%c", write_word[0] );
					if ( write_word[0] > 127 ) {
						fprintf( decoded, "%c", write_word[1] );
					}
					huffcode.clear( );
					break;
				}
			}
		}
		else {
			fseek( file, -1, SEEK_CUR );
			for ( int i = 7; i >= 0; --i ) {
				char bitdata = ( buffer >> i ) & 1;
				huffcode.push_back( bitdata + '0' );
				BYTE write_word[2] = { 0 };
				bool found = false;
				found = search( v, huffcode, write_word );

				if ( found )
				{
					fprintf( decoded, "%c", write_word[0] );
					if ( write_word[0] > 127 ) {
						fprintf( decoded, "%c", write_word[1] );
					}
					huffcode.clear( );
				}
			}
		}
	}

	fclose( file );
	fclose( decoded );

	return true;
}

string search_code(
	vector<code>& v,
	BYTE buffer[2] )
{
	if ( buffer[0] > 127 )
	{
		for ( code item : v ) {
			if ( item.name[0] == buffer[0] && item.name[1] == buffer[1] )
				return item.huffcode;
		}
	}
	else
	{
		for ( code item : v ) {
			if ( item.name[0] == buffer[0] )
				return item.huffcode;
		}
	}
	return string( "NULL" );
}

bool frequency(
	string filename,
	int  freq[][94],
	int* freqascii )
{
	FILE* file = fopen( filename.c_str( ), "rt" );

	if ( file == NULL ) {
		return ERROR;
	}

	unsigned char index[2] = { 0 };

	while ( fscanf( file, "%c", &index[0] ) != EOF )
	{
		if ( index[0] > 127 )
		{
			fscanf( file, "%c", &index[1] );
			++freq[index[0] - 0xB0][index[1] - 0xA1];
		}
		else
		{
			++freqascii[index[0]];
		}

	}

	fclose( file );
	return true;
}

void makeNode(
	int freq[][94],
	int* freqascii,
	priority_queue<tree,
	vector<tree>,
	Mtree>& pq )
{
	for ( int i = 0; i < 25; i++ )
		for ( int j = 0; j < 94; j++ )
			if ( freq[i][j] != 0 )
			{
				tree item( i + 0xB0, j + 0xA1, freq[i][j] );
				pq.push( item );
			}

	for ( unsigned char i = 0; i < 128; i++ )
		if ( freqascii[i] != 0 )
		{
			tree item( i, 0, freqascii[i] );
			pq.push( item );
		}

	return;
}

void makeTree(
	priority_queue<tree, vector<tree>,
	Mtree>& pq )
{
	while ( pq.size( ) >= 2 )
	{
		tree left = pq.top( );
		pq.pop( );
		tree right = pq.top( );
		pq.pop( );

		tree parent;
		parent.left_child.push_back( left );
		parent.right_child.push_back( right );
		parent.freq = left.freq + right.freq;

		pq.push( parent );
	}
	return;
}

bool makeCode(
	const priority_queue<tree, vector<tree>, Mtree>& pq,
	priority_queue<code, vector<code>,
	Mcode>& huffcode )
{

	inorder( pq.top( ), huffcode, "" );

	return true;
}

void inorder(
	const tree& root,
	priority_queue<code,
	vector<code>, Mcode>& huffcode,
	string cur_code )
{

	if ( !root.left_child.empty( ) )
		inorder( root.left_child.front( ), huffcode, cur_code + "0" );

	//is leaf,
	if ( root.left_child.empty( ) && root.right_child.empty( ) ) {
		code item;

		if ( root.name[0] > 127 )
		{
			item.name[0] = root.name[0];
			item.name[1] = root.name[1];
			item.huffcode = cur_code;
			huffcode.push( item );
		}
		else
		{
			item.name[0] = root.name[0];
			item.name[1] = 0;
			item.huffcode = cur_code;
			huffcode.push( item );
		}
	}

	if ( !root.right_child.empty( ) )
		inorder( root.right_child.front( ), huffcode, cur_code + "1" );

	return;
}

bool huffmanEncode(
	string file,
	priority_queue<code,
	vector<code>,
	Mcode>& huffcode )
{
	int freq[25][94] = { 0 };
	int freq_ascii[128] = { 0 };
	priority_queue<tree, vector<tree>, Mtree>  pq;

	if ( frequency( file, freq, freq_ascii ) == false )
		return false;
	makeNode( freq, freq_ascii, pq );
	makeTree( pq );
	makeCode( pq, huffcode );
	convertBinary( file, huffcode );
	return true;
}

void convertBinary(
	string filename,
	priority_queue<code,
	vector<code>, Mcode>& huffcode )
{
	FILE* readfile = fopen( filename.c_str( ), "rt" );

	filename.erase( filename.length( ) - 4, 4 );
	string save = filename + ".txt.zz";

	FILE* write = fopen( save.c_str( ), "wb" );

	int dummy = 0;
	fprintf( write, "%c", dummy );
	fprintf( write, "%d", (int)huffcode.size( ) );
	int idx = 0;
	vector<code> v( huffcode.size( ) );

	while ( !huffcode.empty( ) )
	{
		code item = huffcode.top( );
		huffcode.pop( );
		v[idx++] = item;

		if ( item.name[0] > 127 ) {
			fprintf( write, "%c%c", item.name[0], item.name[1] );
		}
		else {
			fprintf( write, "%c", item.name[0] );
		}
		fprintf( write, "%c", (char)item.huffcode.length( ) );
		BYTE buffer = 0;
		int mm = -1;
		for ( int i = 0; i < item.huffcode.length( ); ++i ) {
			if ( mm == 7 ) {
				fprintf( write, "%c", buffer );
				buffer = 0;
				mm = -1;
			}
			buffer = buffer << 1;
			buffer = buffer | item.huffcode[i] - '0';
			++mm;
		}

		if ( mm != -1 ) {
			while ( mm != 7 ) {
				buffer = buffer << 1;
				mm++;
			}
			fprintf( write, "%c", buffer );
		}
	}
	BYTE word[2];
	BYTE buffer = 0;
	int mm = -1;
	while ( fscanf( readfile, "%c", &word[0] ) != EOF ) {
		if ( word[0] > 127 )
			fscanf( readfile, "%c", &word[1] );
		string write_code = search_code( v, word );
		for ( int i = 0; i < write_code.length( ); ++i ) {
			if ( mm == 7 ) {
				fprintf( write, "%c", buffer );
				buffer = 0;
				mm = -1;
			}
			buffer = buffer << 1;
			buffer = buffer | write_code[i] - '0';
			++mm;
		}
	}
	int lastbit = mm;
	while ( lastbit != 7 ) {
		buffer = buffer << 1;
		lastbit++;
	}
	fprintf( write, "%c", buffer );
	fseek( write, 0, SEEK_SET );
	fprintf( write, "%c", (char)mm );
	fclose( readfile );
	fclose( write );
	return;
}
