// This is the CPP file you will edit and turn in. (Remove this comment!)

#include <iostream>
#include "console.h"
#include <fstream>
#include "grid.h"
#include <string>
#include "simpio.h"
#include "strlib.h"
#include "filelib.h"
#include "lifegui.h"
#include "random.h"

using namespace std;

void printWelcomeMessage();
void initializeGrid(Grid<char>& grid, string& file, string& result);
void makeRandomGrid(Grid<char>& grid, string& result);
void makeGrid(Grid<char>& grid, string& file, string& result);
string updateResult(Grid<char>& grid, string& result);
void promptUser(Grid<char>& grid, string& file, string& result, bool& animateProgram);
void animate(Grid<char>& grid, string& result, bool& animateProgram);
void animationStartState(Grid<char>& grid);
void tick(Grid<char>& grid, string& result, bool& animateProgram);
int checkNeighbor(Grid<char>& grid, int r, int c);
int check(Grid<char>& grid, int r, int c);
bool quit();

// Initializes overall grid and the string used to display the grid in the console
// Then prints preliminary text for the assignment
// Passes in gamefile to set initial state
// Gives user agency to continue/animate/end the program

int main() {
    Grid<char> grid;
    string result;
    string file;
    bool animateProgram;
    printWelcomeMessage();
    initializeGrid(grid, file, result);
    promptUser(grid, file, result, animateProgram);
    cout << "Have a nice Life!" << endl;
    return 0;
}

// Header code to welcome user
void printWelcomeMessage() {
    cout << "Welcome to the CS 106B/X Game of Life!" << endl;
    cout << "This program simulates the lifecycle of a bacterial colony." << endl;
    cout << "Cells (X) live and die by the following rules:" << endl;
    cout << "* A cell with 1 or fewer neighbors dies." << endl;
    cout << "* Locations with 2 neighbors remain stable." << endl;
    cout << "* Locations with 3 neighbors will create life." << endl;
    cout << "* A cell with 4 or more neighbors dies." << endl;
    cout << endl;
}

//Asks the user which file they want to run, then setups up starting gridstate
void initializeGrid(Grid<char>& grid, string& file, string& result) {
    while (true) {
        file = getLine ("Grid input file name? ");
        if (file == "random") {
            makeRandomGrid(grid, result);
            break;
        } else if (!fileExists(file)) {
            cout << "Unable to open that file. Try again. " << endl;
        } else {
            makeGrid(grid,file,result);
            break;
        }
    }
}

//Makes random starting gridstate
void makeRandomGrid(Grid<char>& grid, string& result) {
    int r1 = randomInteger(3, 20);
    int r2 = randomInteger(3, 20);
    bool cell = false;
    grid.resize(r1,r2);
    for (int r = 0; r < grid.numRows(); r++) {
        for (int c = 0; c < grid.numCols(); c++) {
            cell = randomChance(.2);
            if (cell==true)
                grid[r][c] = 'X';
            else {
                grid[r][c] = '-';
            }
        }
    }
    result = updateResult(grid, result);
    cout << result;
}

//Creates starting gridstate based on file input
void makeGrid(Grid<char>& grid, string& file, string& result) {
    ifstream input;
    input.open(file);
    string line;
    getline(input, line);
    int a = stringToInteger(line);
    getline(input, line);
    int b = stringToInteger(line);
    grid.resize(a,b);
    for (int r = 0; r < grid.numRows(); r++) {
        getline(input, line);
        for (int c = 0; c < grid.numCols(); c++) {
            grid[r][c] = line[c];
        }
    }
    result = updateResult(grid, result);
    cout << result;
    input.close();
}

//Updates the string to be displayed in the console
string updateResult(Grid<char>& grid, string& result) {
    result.clear();
    for (int r = 0; r < grid.numRows(); r++) {
        for (int c = 0; c < grid.numCols(); c++) {
            result += grid[r][c];
        }
        result += '\n';
    }
    return result;
}

//Asks user what they want to do next
void promptUser(Grid<char>& grid, string& file, string& result, bool& animateProgram) {
    while (true) {
        string prompt = getLine("a)nimate, t(ick), q)uit? ");
        if (prompt == "a") {
            animateProgram = true;
            animate(grid, result, animateProgram);
        } else if (prompt == "t") {
            animateProgram = false;
            tick(grid, result, animateProgram);
        } else if (prompt == "q") {
            if (quit() == 0)
                break;
            else
                initializeGrid(grid, file, result);
        } else {
            cout << "Invalid command " << endl;
        }
    }
}


//Animates the game of life in accordances with tick rules
void animate(Grid<char>& grid, string& result, bool& animateProgram) {
    animationStartState(grid);
    int frames = getInteger("How many frames? ");
    for (int i = 0; i < frames; i++) {
        tick(grid,result, animateProgram);
        LifeGUI::clear();
        for (int r = 0; r < grid.numRows(); r++) {
            for (int c = 0; c < grid.numCols(); c++) {
                if (grid[r][c] == 'X')
                    LifeGUI::fillCell(r, c);
            }
        }
        LifeGUI::repaint();
        pause(100);
    }
}

//Initializes the animation window with starting grid state
void animationStartState(Grid<char>& grid) {
    LifeGUI::initialize();
    LifeGUI::resize(grid.numRows(), grid.numCols());
    for (int r = 0; r < grid.numRows(); r++) {
        for (int c = 0; c < grid.numCols(); c++) {
            if (grid[r][c] == 'X')
                LifeGUI::fillCell(r, c);
        }
    }
}

//Tick Function: Iterates a lifecycle of the cells,
//Kills cells if there are zero, one, or more than 4 neighbors.
//Maintians state if there are 2 neighbors
//Creates new cell if there are exactly 3 neighbors
//Prints grid to console if user is not animating
void tick(Grid<char>& grid, string& result, bool& animateProgram) {
    Grid<char> gridState = grid;
    for (int r = 0; r < gridState.numRows(); r++) {
        for (int c = 0; c < gridState.numCols(); c++) {
            if (checkNeighbor(grid, r, c) == 3) {
                gridState[r][c] = 'X';
            } else if (checkNeighbor(grid, r, c) == 2) {
                gridState[r][c] = gridState[r][c];

            } else {
                gridState[r][c] = '-';
            }
        }
    }
    grid = gridState;
    result = updateResult(grid, result);
    if (animateProgram == false)
        cout << result;
}

//Helper function to tabulate number of neighbors in the grid
int checkNeighbor(Grid<char>& grid, int r, int c) {
    int counter = 0;

    int above = r - 1;
    int below = r + 1;
    int right = c + 1;
    int left = c - 1;
    int currCol = c;
    int currRow = r;

    if (r == 0) {
        above = grid.numRows() - 1;
    }
    if (r+1 == grid.numRows()) {
        below = 0;
    }
    if (c == 0) {
        left = grid.numCols() - 1;
    }
    if (c+1 == grid.numCols()) {
        right = 0;
    }


    //Checking row above
    counter += check(grid, above, left);
    counter += check(grid, above, currCol);
    counter += check(grid, above, right);


    //Checking current row
    counter += check(grid, currRow, right);
    counter += check(grid, currRow, left);

    //Checking row below
    counter += check(grid, below, left);
    counter += check(grid, below, currCol);
    counter += check(grid, below, right);

    return counter;
}

//Returns one if value at specified grid location is occupied
int check(Grid<char>& grid, int r, int c) {
    if (grid[r][c] == 'X')
        return 1;
    else {
        return 0;
    }
}

//Quit function to ask user if they want to terminate the program
bool quit() {
    bool exist = getYesOrNo ("Load another file? (y,n) ");
    return exist;
}

