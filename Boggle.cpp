/*
 * File: Boggle.cpp
 * ------------
 * Name: Zhiwen Zhang
 * Course: CS 106B 
 * Description: This file implements the Boggle.h interface.
 *
 */

#include "Boggle.h"
#include "bogglegui.h"
#include <string>
#include "strlib.h"
#include <cmath>
#include "grid.h"
#include "hashset.h"
#include <cctype>

const int Boggle::BOARD_SIZE = 16;

// letters on all 6 sides of every cube
static string CUBES[16] = {
    "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
    "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
    "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
    "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};

// letters on every cube in 5x5 "Big Boggle" version (extension)
static string BIG_BOGGLE_CUBES[25] = {
   "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
   "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
   "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
   "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
   "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

Boggle::Boggle(Lexicon& dictionary, string boardText) {
    board.resize(sqrt(BOARD_SIZE), sqrt(BOARD_SIZE));// Configure initial board
    this->dictionary = dictionary;
    this->boardText = setBoard(boardText);
}

string Boggle::generateRandomBoard() {
    const int NUM_SIDES = 6; // Number of sides for a cube
    int index = 0;
    string boardText;

    shuffle(CUBES, BOARD_SIZE); // Randomize location on the board

    for (int i = 0; i < board.numRows(); i++) {
        for (int j = 0; j < board.numCols(); j++) {
            string letters = CUBES[index];
            board[i][j] = letters[randomInteger(0, NUM_SIDES - 1)]; //Randomize side of the cube
            boardText.append(charToString(board[i][j])); //Store the elements in string (used for labelAllCubes)
            index++;
        }
    }
    return boardText;
}

void Boggle::configureManualBoard(string boardText) {
    int index = 0;
    for (int i = 0; i < board.numRows(); i++) {
        for (int j = 0; j < board.numCols(); j++) {
            board[i][j] = boardText[index];
            index++;
        }
    }
}

string Boggle::setBoard(string boardText) {
    if (boardText.length() == 0){
        boardText = generateRandomBoard();
    } else {
        configureManualBoard(boardText);
    }
    return boardText;
}

int Boggle::getBoardSize() const {
    return BOARD_SIZE;
}

string Boggle::getBoardText() const {
    return boardText;
}

string Boggle::getFoundWords() const {
    return foundWords.toString();
}

char Boggle::getLetter(int row, int col) const {
    if(!board.inBounds(row, col)) {
        throw min(row, col);
    }
    return board[row][col];
}

string Boggle::getNumFoundWords() const {
    return integerToString(foundWords.size());
}

void Boggle::updateWordList(string word) {
    foundWords.add(toUpperCase(word));
}

bool Boggle::checkWord(string word) const {
    word = toLowerCase(word);
    if (word.length() >= 4 && dictionary.contains(word)
        && !foundWords.contains(word)) {
        return true;
    }
    return false;
}

bool Boggle::humanWordSearchHelper(string& word, HashSet<int>& usedIndex, int row, int col) {
    string originalWord = word;
    const int DELAY_TIME = 100; // Delay time of animation in miliseconds
    if (word == "") {
        // Base case
        usedIndex.clear();
        return true;
    }

    // Recursive case: Search the neighborhood
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++){
            // Choose
            word = originalWord;
            char first = word[0]; //first letter
            word.erase(0, 1); // rest

            int index = (i * board.numCols()) + j;

            if (board.inBounds(i, j) && !usedIndex.contains(index)
               && !(i == row && j == col)) {
                BoggleGUI::setHighlighted(i, j, true);
                BoggleGUI::setAnimationDelay(DELAY_TIME);
                BoggleGUI::setHighlighted(i, j, false);

                if (board[i][j] == first) {
                    BoggleGUI::setHighlighted(i, j, true);
                    usedIndex.add(index);

                    if (humanWordSearchHelper(word, usedIndex, i, j)) { // Explore
                        return true;
                    }
                    // Unchoose
                    BoggleGUI::setHighlighted(i, j, false);
                    word.insert(0, 1, first);
                    usedIndex.remove(index);
                }
            }
        }
    }
    return false;
}

bool Boggle::humanWordSearch(string word) {
    HashSet<int> usedIndex;
    BoggleGUI::clearHighlighting();
    string enteredWord = word;
    const int DELAY_TIME = 100; // Delay time of animation in miliseconds

    // First letter of the word (Special case for which we have to look at all board grids)
    for (int row = 0; row < board.numRows(); row++) {
        for (int col = 0; col < board.numCols(); col++) {
            string word = enteredWord;
            char first = word[0]; // first letter
            word.erase(0, 1); // rest

            BoggleGUI::setHighlighted(row, col, true);
            BoggleGUI::setAnimationDelay(DELAY_TIME);
            BoggleGUI::setHighlighted(row, col, false);

            if (board[row][col] == first) {
                int index = (row * board.numCols()) + col;
                usedIndex.add(index);

                BoggleGUI::setHighlighted(row, col, true);

                if (humanWordSearchHelper(word, usedIndex, row, col)) {
                    updateWordList(enteredWord);
                    return true;
                }
                BoggleGUI::setHighlighted(row, col, false);
            }
            usedIndex.clear();
        }
    }
    return false;
}

int Boggle::getScoreHuman() {
    int score = 0;
    for(string s: foundWords) {
        score += s.length() - 3;
    }
    return score;
}

void Boggle::computerWordSearchHelper(string& word, Set<string>& result, HashSet<int>& usedIndex,
                                      int row, int col) {
    string originalWord = word;
    if (checkWord(word) && !result.contains(word)) {
        //Base case: if the word is valid, add to the list and see whether there's any more word that contains the word as a prefix
        result.add(toUpperCase(word));
    }

    //Recursive case: Search the neighborhood and find valid words
    for (int i = row - 1; i <= row + 1; i++) {
        for (int j = col - 1; j <= col + 1; j++) {

            if (board.inBounds(i, j) && !(i == row && j == col)) {
                // Choose
                char nextLetter = board[i][j];
                word = originalWord;
                word.append(charToString(nextLetter));

                int index = (i * board.numCols()) + j;

                if (!usedIndex.contains(index) && dictionary.containsPrefix(word)) {
                    usedIndex.add(index);
                    computerWordSearchHelper(word, result, usedIndex, i, j); // Explore
                    usedIndex.remove(index); // Unchoose
                }
                // Unchoose
                word.erase(word.length() - 1, 1); // Delete the last character
            }
        }
    }
}

Set<string> Boggle::computerWordSearch() {
    // TODO: implement
    Set<string> result;
    HashSet<int> usedIndex;

    // First letter of the word (Special case for which we have to look at all board grids)
    for (int row = 0; row < board.numRows(); row++) {
        for (int col = 0; col < board.numCols(); col++) {
            // Choose the first letter
            string word = "";
            char first = board[row][col]; // Choose the first letter from the board
            word.insert(0, 1, first);
            int index = (row * board.numCols()) + col;
            usedIndex.add(index);

            // Explore
            computerWordSearchHelper(word, result, usedIndex, row, col);

            // Unchoose
            usedIndex.clear();
        }
    }
    return result;
}

int Boggle::getScoreComputer() {
    int score = 0;
    for (string s: computerWordSearch()) {
        score += s.length() - 3;
    }
    return score;
}

ostream& operator<<(ostream& out, Boggle& boggle) {
    for (int i = 0; i < boggle.board.numRows(); i++) {
        for (int j = 0; j < boggle.board.numCols(); j++) {
            out << boggle.board[i][j];
        }
        out << endl;
    }
    return out;
}
