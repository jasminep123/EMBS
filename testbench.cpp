#include "toplevel.h"
#define NUMDATA 1000
uint32 mainmemory[1000];

bool isValid(uint32 (*solution)[8][4], int size){
	    for (int row = 0; row<size; row++){
	    	for (int col=0; col<size; col++){
	    		if (row == 0){
	    			if (col == 0){ //top left corner so check bot and right
	    				// check right
	    				if (!(solution[row][col][1] == solution[row][col+1][3])){
	    					return false;
	    				}
	    				// check bot
	    				if (!(solution[row][col][2] == solution[row+1][col][0])){
	    					return false;
	    				}
	    			}else if (col == size-1){ // top right corner so check bot and left
	    				// check left
	    				if (!(solution[row][col][3] == solution[row][col-1][1])){
	    					return false;
	    				}
	    				// check bot
	    				if (!(solution[row][col][2] == solution[row+1][col][0])){
	    					return false;
	    				}
	    			}
	    		}else if (row == size-1){
	    			if (col == 0){ // bottom left check up and right
	    				// check right
	    				if (!(solution[row][col][1] == solution[row][col+1][3])){
	    					return false;
	    				}
	    				// check up
	    				if (!(solution[row][col][0] == solution[row-1][col][2])){
	    					return false;
	    				}
	    			}else if (col == size-1){ // bot right corner so check top and left
	    					// check left
	    				if (!(solution[row][col][3] == solution[row][col-1][1])){
	    					return false;
	    				}
	    				// check top
	    				if (!(solution[row][col][0] == solution[row-1][col][0])){
	    					return false;
	    				}
	    			}
	    		}else {
	    			//check all
	    			//check left
	    			if (!(solution[row][col][3] == solution[row][col-1][1])){
	    				return false;
	    			}
	    			// check top
	    			if (!(solution[row][col][0] == solution[row-1][col][0])){
	    				return false;
	    			}
	    			// check right
	    			if (!(solution[row][col][1] == solution[row][col+1][3])){
	    				return false;
	    			}
	    			// check bot
	    			if (!(solution[row][col][2] == solution[row+1][col][0])){
	    				 return false;
	    			}
	    		}
	    	}
	    }
	    return true;
}


int main() {
    //Create input data
	//4x4 puzzle
	uint32 mainmemory[64];
	mainmemory[0] = 3;
	mainmemory[1] = 4;
	mainmemory[2] = 1;
	mainmemory[3] = 3;
	mainmemory[4] = 3;
	mainmemory[5] = 1;
	mainmemory[6] = 8;
	mainmemory[7] = 1;
	mainmemory[8] = 5;
	mainmemory[9] = 1;
	mainmemory[10] = 8;
	mainmemory[11] = 4;
	mainmemory[12] = 7;
	mainmemory[13] = 5;
	mainmemory[14] = 8;
	mainmemory[15] = 0;
	mainmemory[16] = 6;
	mainmemory[17] = 6;
	mainmemory[18] = 1;
	mainmemory[19] = 7;
	mainmemory[20] = 7;
	mainmemory[21] = 6;
	mainmemory[22] = 9;
	mainmemory[23] = 3;
	mainmemory[24] = 1;
	mainmemory[25] = 5;
	mainmemory[26] = 6;
	mainmemory[27] = 0;
	mainmemory[28] = 3;
	mainmemory[29] = 3;
	mainmemory[30] = 3;
	mainmemory[31] = 1;
	mainmemory[32] = 3;
	mainmemory[33] = 9;
	mainmemory[34] = 6;
	mainmemory[35] = 5;
	mainmemory[36] = 3;
	mainmemory[37] = 9;
	mainmemory[38] = 4;
	mainmemory[39] = 0;
	mainmemory[40] = 5;
	mainmemory[41] = 8;
	mainmemory[42] = 4;
	mainmemory[43] = 7;
	mainmemory[44] = 5;
	mainmemory[45] = 3;
	mainmemory[46] = 3;
	mainmemory[47] = 1;
	mainmemory[48] = 6;
	mainmemory[49] = 4;
	mainmemory[50] = 5;
	mainmemory[51] = 3;
	mainmemory[52] = 3;
	mainmemory[53] = 8;
	mainmemory[54] = 3;
	mainmemory[55] = 1;
	mainmemory[56] = 8;
	mainmemory[57] = 8;
	mainmemory[58] = 5;
	mainmemory[59] = 8;
	mainmemory[60] = 8;
	mainmemory[61] = 5;
	mainmemory[62] = 8;
	mainmemory[63] = 3;


    int size = 4;
    uint32 solution[128];
    //Set up the inputs to the hardware
    uint32 sol[8][8][4];
    //Run the hardware
    uint32 ram[514];
    toplevel(mainmemory, size);
    printf("checking solution... ");
    int index = 0;
     for (int x=0; x<size; x++){ //read into a puzzle board (x,y, piece)
    	for (int y=0 ; y<size; y++){
    		for (int p=0; p<4; p++){
    			sol[x][y][p] = mainmemory[index];
    			printf(" %d ", mainmemory[index]);
    			index+=1;
    		}
    		printf(" ");
    	}
    	printf("\n");
     }

     printf("printing solution .......... \n");
     for (int x = 0; x<size; x++){
    	 for (int y = 0; y<size; y++){
    		 printf("   %d   \n", sol[x][y][0]);
    		 printf(" %d  %d \n", sol[x][y][3], sol[x][y][1]);
    		 printf("   %d    \n", sol[x][y][2]);
    	 }
     }

    if (isValid(sol, size)){
    	printf("VALID SOLUTION FOUND!!!!!! \n");
    	return 0;
    }else{
    	return 0;
    }
}

