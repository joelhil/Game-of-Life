
#include <SDL.h>
#include <cmath>
#include <iostream>
#include <time.h>
#include <string>

// Game of life with Random initial states for all cells

// SDL2 framework class
class Framework{
public:
    // Contructor which initialize the parameters.
    SDL_Renderer *renderer = NULL;      // Pointer for the renderer
    SDL_Window *window = NULL;
    Framework(int height_, int width_): height(height_), width(width_){
        SDL_Init(SDL_INIT_VIDEO);       // Initializing SDL as Video
        SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);      // setting draw color
        SDL_RenderClear(renderer);      // Clear the newly created window
        SDL_RenderPresent(renderer);    // Reflects the changes done in the

                                        //  window.
    }

    // Destructor
    ~Framework(){
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

private:
    int height;     // Height of the window
    int width;      // Width of the window
// Pointer for the window
};


// make a 2dindex into a 1d index
// first column then row
int index1D (int column, int row, int columns){
    return row*columns + column;
}

//When drawing square, use -1 on both positions and -2 on width for grid net
void draw_square_white(SDL_Renderer *renderer,int blCorner_x, int blCorner_y, int width_){
    // Setting the color to be RED with 100% opaque (0% trasparent).
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    // Drawing square
    for(int x=blCorner_x; x<=blCorner_x+width_; x++){
        for(int y=blCorner_y; y<=blCorner_y+width_; y++){
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

}

void draw_square_black(SDL_Renderer *renderer,int blCorner_x, int blCorner_y, int width_){
    // Setting the color to be RED with 100% opaque (0% trasparent).
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    // Drawing square
    for(int x=blCorner_x; x<=blCorner_x+width_; x++){
        for(int y=blCorner_y; y<=blCorner_y+width_; y++){
            SDL_RenderDrawPoint(renderer, x, y);
        }
    }

}

// Counts neightbours according to moores neighourhood periodically
int countNeighboursPadded(int iColumn,int iRow, int cellMatrix[], int cols,int rows){
    int nNeighbours = 0;
    //The elements with all eigth neighbours // col0, row 2
    for (int i=-1;i<2;i++){
        nNeighbours += cellMatrix[index1D((iColumn+i),(iRow+1),cols+2)];
        nNeighbours += cellMatrix[index1D((iColumn+i), (iRow-1), cols+2)];
    }
    nNeighbours += cellMatrix[index1D((iColumn-1),(iRow),cols+2)];
    nNeighbours += cellMatrix[index1D((iColumn+1), (iRow), cols+2)];
    return nNeighbours;
}


// Calculates if a cell is dead or alive in next timestep
int nextState(int currentState , int nNeighbours){
    int nextState=0;

    if (currentState==0){
        if (nNeighbours == 3){
            nextState = 1;
        }
    } else if (currentState==1){
        if (nNeighbours == 2 || nNeighbours == 3) {
            nextState = 1;
        }
    }

    return nextState;
}



// Make padding mirroring opposite sides of array
// Is used to make the neightbourhood funtion easier to calculate
void fillPadding(int arr[],int cols, int rows){
    for (int i = 1; i<cols+2;i++){
        arr[index1D(i,0,cols+2)]=arr[index1D(i,rows,cols+2)];
        arr[index1D(i,rows+1,cols+2)]=arr[index1D(i,1,cols+2)];
    }
    for (int j = 1; j<rows+2;j++){
        arr[index1D(0,j,cols+2)]=arr[index1D(cols,j,cols+2)];
        arr[index1D(cols+1,j,cols+2)]=arr[index1D(1,j,cols+2)];
    }
    arr[index1D(0,0,cols+2)]=arr[index1D(cols,rows,cols+2)];
    arr[index1D(cols+1,0,cols+2)]=arr[index1D(1,rows,cols+2)];
    arr[index1D(0,rows+1,cols+2)]=arr[index1D(cols,1,cols+2)];
    arr[index1D(cols+1,rows+1,cols+2)]=arr[index1D(1,1,cols+2)];
}


void initRandom(SDL_Renderer *renderer, int cellArray[], int cols, int rows, int squareWidth){
    for (int i = 1; i<(cols+1);i++) {
        for (int j = 1; j<(rows+1);j++){
            if ((float) rand()/RAND_MAX > 0.5) {
                cellArray[index1D(i,j,cols+2)]=1;
                draw_square_white(renderer,(i-1)*squareWidth-1, (j-1)*squareWidth-1, squareWidth-2);
            } else {
                cellArray[index1D(i,j,cols+2)]=0;
            }

        }
    }
    fillPadding(cellArray,cols,rows);
    SDL_RenderPresent(renderer);
}

int main(int argc, char * argv[]){
    // Creating the object by passing Height and Width value.
    // Global variables
    int height = 900;
    int width = 900;
    int squareWidth = 8;

    //Create graphic window
    Framework fw(height, width);
    SDL_Event event;

    //int generations = 40;
    int cols = width/squareWidth;
    int rows = height/squareWidth;
    // Create matrix of positions and draw original squares
    // with uniformly random squares
    int cellArray[cols*rows+2*cols+2*rows+4];
    srand( (unsigned)time( NULL ) );

    // Initialize with random states at t = 0
    initRandom(fw.renderer,cellArray,cols,rows,squareWidth);


    int timeStep = 0;
    SDL_SetWindowTitle(fw.window, "Time = 0");
    const Uint8* state = SDL_GetKeyboardState(nullptr);
    while (!state[SDL_SCANCODE_Q]) {
    // For title of window
    char title[64] = "Time = "; // WE need enough memory
        int newcellArray[cols*rows + 2*cols + 2*rows + 4];
        for (int i = 1; i<=cols ;i++){
            for (int j = 1; j<=rows ;j++){
                // Change cell and update windows
                int i1D = index1D(i,j,cols+2);
                int nNeightbour = countNeighboursPadded(i,j,cellArray,cols,rows);
                int newCell = nextState(cellArray[i1D],nNeightbour);
                newcellArray[i1D]=newCell;
                //Cover up square if cell has died
                if (newcellArray[i1D]==0 && cellArray[i1D]!=0) {
                    draw_square_black(fw.renderer,(i-1)*squareWidth-1, (j-1)*squareWidth-1, squareWidth-2);
                //Draw square if cell has been born
                } else if (newcellArray[i1D]==1) {
                    draw_square_white(fw.renderer,(i-1)*squareWidth-1, (j-1)*squareWidth-1, squareWidth-2);
                }
            }
        }
        // Put corners into padding
        fillPadding(newcellArray,cols,rows);
        memcpy(cellArray, newcellArray, sizeof(cellArray)); // Make row = newRow
        SDL_RenderPresent(fw.renderer);      // Update rendering
        SDL_PumpEvents();                 // Check if 'q' was pressed
        SDL_Delay(20);

        // Update title with timestep
        timeStep++;
        char integer_string[32];
        sprintf(integer_string, "%d", timeStep);
        strcat(title, integer_string);
        SDL_SetWindowTitle(fw.window, title);

    }


    while (1) {
        if (SDL_PollEvent(&event) && event.type == SDL_QUIT)
            break;
        SDL_Delay(2);
    }

}


