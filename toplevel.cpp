#include "toplevel.h"
#include <string.h>
#include <hls_stream.h>
using namespace hls;

typedef struct {
	int tileNo;
	int numbers[4]; //each of the tile numbers
	int top = 0;
	int right;
	int left;
	int bot;
	int orientation;
	bool undone[64];
	int numRotations;
	bool placed;
}tile;

//Prototypes
bool isValid(tile checkTile, int next, tile* puzzle, int size, int *colNumbs);
tile rotateTile(tile rotTile);
bool triedAll(bool *tried, int size);
void resetTried(bool *tried, tile *tiles, int size);
void resetUndone(tile *tiles, int next, int tilessize);
void getSolution(uint32 *ram, tile *puzzle, int size);
bool isLeftCol(int next, int *colNumbs);


void getSolution(uint32 *ram, tile *puzzle, int size){
	int index = 0;
	solutionLoop: for (int i=0; i<size*size; i++){
		#pragma HLS loop_tripcount min=4 max=64
		#pragma HLS pipeline off
		ram[index] = puzzle[i].top;
		ram[index+1] = puzzle[i].right;
		ram[index+2] = puzzle[i].bot;
		ram[index+3] = puzzle[i].left;
		printf("puzzle %d = %d %d %d %d \n",i, puzzle[i].top, puzzle[i].right, puzzle[i].bot, puzzle[i].left);
		printf(" %d %d %d %d \n", ram[index], ram[index+1], ram[index+2], ram[index+3] );
		index+=4;
	}
}

void resetUndone(tile* tiles, int next, int size){
	undoneLoop: for (int i = next+1 ; i < size*size; i++){ // looping for each val after next
		//loop through each tile.
		resetUndoneLoop: for (int j =0; j<size*size; j ++){
			#pragma HLS loop_tripcount min=4 max=64
			#pragma HLS loop_flatten
			#pragma HLS pipeline
			tiles[j].undone[i]  = false;
		}
	}
}

bool isLeftCol(int next, int *colNumbs){
	bool leftcol = false;
	lookupLoop: for (int i=0; i<8; i++){
		#pragma HLS unroll
		if (next==colNumbs[i]){
			leftcol = true;
		}
	}
	return leftcol;
}

bool isValid(tile checkTile, int next, tile* puzzle, int size, int *colNumbs){
	if (next == 0) {
		return true;
	}else if (next < size){ // top row (only check left
		if (puzzle[next-1].right == checkTile.left){
			return true;
		}
//	} else if (isLeftCol(next, colNumbs)){ // left col so only need to check up
	}else if (next % size == 0){
		if (puzzle[next-size].bot == checkTile.top){
			return true;
		}
	} else{ // check both left and up
		if ((puzzle[next-size].bot == checkTile.top) and (puzzle[next-1].right == checkTile.left)){
			return true;
		}
	}
	return false;
}

void resetTried(bool* tried, tile* tiles, int size){ //sets tried to false if a tile has not been placed
	resetTriedloop: for (int i =0; i<size*size; i++){
			#pragma HLS loop_tripcount min=4 max=64
			#pragma HLS unroll
			if (tiles[i].placed == true){
				tried[i] = true;
			}else{
				tried[i] = false;
			}
		}
}

bool triedAll(bool *tried, int size){
	bool triedAll = true;
	checkTriedLoop: for (int i =0; i<size*size; i++){
	#pragma HLS loop_tripcount min=4 max=64
	#pragma HLS unroll
		if (tried[i] == false){
			triedAll = false;
		}
	}
	return triedAll;
}


tile rotateTile(tile rotTile) { // rotates a tile 90 degrees
	if (rotTile.orientation == 3) {
			rotTile.orientation = 0;
	}else {
		rotTile.orientation += 1;
	}
	if (rotTile.orientation == 0){
		rotTile.top = rotTile.numbers[0];
		rotTile.right = rotTile.numbers[1];
		rotTile.bot = rotTile.numbers[2];
		rotTile.left = rotTile.numbers[3];
	}else if (rotTile.orientation == 1){
		rotTile.top = rotTile.numbers[3];
		rotTile.right = rotTile.numbers[0];
		rotTile.bot = rotTile.numbers[1];
		rotTile.left = rotTile.numbers[2];
	}else if (rotTile.orientation == 2){
		rotTile.top = rotTile.numbers[2];
		rotTile.right = rotTile.numbers[3];
		rotTile.bot = rotTile.numbers[0];
		rotTile.left = rotTile.numbers[1];
	}else if (rotTile.orientation == 3){
		rotTile.top = rotTile.numbers[1];
		rotTile.right = rotTile.numbers[2];
		rotTile.bot = rotTile.numbers[3];
		rotTile.left = rotTile.numbers[0];
	}
	return rotTile;
}

bool toplevel(uint32* ram, int size) {
	#pragma HLS INTERFACE m_axi depth=512 port=ram offset=slave bundle=MAXI
	#pragma HLS INTERFACE s_axilite port=size bundle=AXILiteS
	#pragma HLS INTERFACE s_axilite port=return bundle=AXILiteS
	tile tiles[64];
	tile puzzle[64];
	bool tried[64];
	uint32 solution[256];

	for (int i = 0; i<64; i++){
		tried[i] = false;
		solution[i] = 0;
	}

	#pragma HLS ARRAY_PARTITION variable=solution block factor=64
	int next = 0;

	int colNumbs[8] = {0,0,0,0,0,0,0,0};
	stream<int> toPlace; // queue of tile numbers that need placing
	int topCorner = 0;
	// read each tile from ram
	int index = 0; // where to read from ram,
	readLoop: for (int i=0; i<(size*size); i++){
		#pragma HLS pipeline off
		tiles[i].tileNo = i;
		tiles[i].orientation = 0;
		tiles[i].numbers[0] = ram[index];
		tiles[i].numbers[1] = ram[index+1];
		tiles[i].numbers[2] = ram[index+2];
		tiles[i].numbers[3] = ram[index+3];
		tiles[i].top = tiles[i].numbers[0];
		tiles[i].right = tiles[i].numbers[1];
		tiles[i].bot = tiles[i].numbers[2];
		tiles[i].left = tiles[i].numbers[3];
		tiles[i].placed = false;
		tiles[i].numRotations = 0;
		index += 4;
		toPlace.write(i);
		printf("tile %d = %d %d %d %d \n", i, tiles[i].top, tiles[i].right, tiles[i].bot, tiles[i].left);
		#pragma HLS ARRAY_PARTITION variable=tiles[i].undone complete
	}

	createLookup: for(int i = 0; i<size; i++){
		#pragma HLS unroll
		colNumbs[i] = size*i;
		ram[i] = 0;
	}

	int backTrackRotations = 0;
	bool found = false;
	int tileNo;
	searchLoop: while (found == false and !toPlace.empty()) {
		if (next == 0 and backTrackRotations == 0){
			tileNo = topCorner;
			topCorner += 1;
		}else{
			tileNo = toPlace.read();
		}
		tile curTile = tiles[tileNo];
		if (tiles[tileNo].undone[next] == true){
			toPlace.write(tileNo);
		} else if (tiles[tileNo].placed == false){
			//try all rotations until one found
			bool valid  = false;
			valid = isValid(tiles[tileNo], next, puzzle, size, colNumbs);
			rotationloop: for (int r =0; r < 4; r++){
				#pragma HLS pipeline off
				if (valid == false) {
					tiles[tileNo] = rotateTile(tiles[tileNo]);
					tiles[tileNo].numRotations +=1;
					valid = isValid(tiles[tileNo], next, puzzle, size, colNumbs);
				}else{
					break;
				}
			}
			if (valid == false){
				toPlace.write(tileNo);
			}else{
				puzzle[next] = tiles[tileNo];
				next += 1;
				ram[256] = next;
				//reset tried
				tiles[tileNo].placed = true;
				resetTried(tried, tiles, size);
			}
		}
		tried[tileNo] = true;
		if (next==(size*size)) {
			found = true;
			break;
		}
		if (triedAll(tried, size)) {
			next = next-1;
			tileNo = tiles[puzzle[next].tileNo].tileNo;
			if (backTrackRotations < 4 and next == 0){
					resetUndone(tiles, next, size);
					tiles[tileNo] = rotateTile(tiles[tileNo]);
					next += 1;
					//reset tried
					tiles[tileNo].placed = true;
					resetTried(tried, tiles, size);
					backTrackRotations +=1;
			}else{
				if (backTrackRotations >=4 and next == 0){
					backTrackRotations = 0;
				}
				bool valid = false;
				if (tiles[tileNo].numRotations < 4){
					backRotationloop: for (int r =tiles[tileNo].numRotations; r < 4; r++){
						#pragma HLS pipeline off
						if (valid == false) {
							tiles[tileNo] = rotateTile(tiles[tileNo]);
							tiles[tileNo].numRotations +=1;
							valid = isValid(tiles[tileNo], next, puzzle, size, colNumbs);
						}else{
							break;
						}
					}
				}
				if (valid == false){
					resetUndone(tiles, next, size);
					tiles[tileNo].numRotations = 0;
					tiles[tileNo].undone[next] = true;
					puzzle[next].tileNo = -1;
					toPlace.write(tileNo);
					//reset Tried
					tiles[tileNo].placed = false;
					resetTried(tried, tiles, size);
					tried[tileNo] = true;
				}else{
					puzzle[next] = tiles[tileNo];
					next += 1;
					//reset tried
					tiles[tileNo].placed = true;
					resetTried(tried, tiles, size);
				}
			}
		}
	}
	getSolution(ram, puzzle, size);
	return found;
}

