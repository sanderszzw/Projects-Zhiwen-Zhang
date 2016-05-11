/*
 * File: boggleplay.cpp
 * ------------
 * Name: Zhiwen Zhang
 * Course: CS 106B (Section: Thursday 4:30pm)
 * Description: This is a client program that performs console UI, GUI, and work with Boggle class.
 *
 */

#include <iostream>
#include "simpio.h"
#include <string>
#include "string.h"
#include "lexicon.h"
#include "console.h"
#include "Boggle.h"
#include "bogglegui.h"
#include <cctype>
#include <cmath>

using namespace std;

// Helper Function Prototype
string getManualConfigInput();

void playOneGame(Lexicon& dictionary) {

    //(1) Board Configuration ============================================================================
    //Prompt the user to choose whether to generate a random board (y/n)
    string boardText;
    if(getYesOrNo("Do you want to generate a random board?", "Please type a word that begins with 'y' or 'n'.")) {
        // Random board configuration (string: empty)
        boardText = "";
    } else {
        // Manual configuration (string: 16 letters chosen by the user)
        boardText = getManualConfigInput();
    }

    // Load and boggle
    Boggle boggle(dictionary, boardText);

    // Initialize GUI
    int n = sqrt(boggle.getBoardSize()); // size n of a square n X n board (of size n^2)
    BoggleGUI::initialize(n, n);

    // (2) Human's turn ===================================================================================
    clearConsole();

    // Display a message that it's the human's turn and the board
    string statusMessage = "It's your turn!";
    cout << statusMessage << endl << boggle;
    BoggleGUI::setStatusMessage(statusMessage);
    BoggleGUI::labelAllCubes(boggle.getBoardText());

    string word;
    while (true) {
        // Display updated score on console
        string WordsMessage = "Your words (" + boggle.getNumFoundWords() + "): " + toUpperCase(boggle.getFoundWords());
        string ScoreMessage = "Your score: " + integerToString(boggle.getScoreHuman());
        cout << endl << WordsMessage << endl << ScoreMessage << endl;

        word = toUpperCase(getLine("Type a word (or Enter to stop): ")); // store word in upper case

        if (word == "") {
            break; // End of human's turn
        }

        if(boggle.checkWord(word)) {
            if(boggle.humanWordSearch(word)) {
                // Update console
                clearConsole();
                string statusMessage = "You found a new word! \"" + word + "\"";
                cout << statusMessage << endl << boggle;

                // Update GUI
                BoggleGUI::setStatusMessage(statusMessage);
                BoggleGUI::recordWord(word, BoggleGUI::HUMAN);
                BoggleGUI::setScore(boggle.getScoreHuman(), BoggleGUI::HUMAN);

            } else {
                clearConsole();
                string statusMessage = "That word can't be formed on this board.";
                cout << statusMessage << endl << boggle;
                BoggleGUI::setStatusMessage(statusMessage);
            }
        } else {
            clearConsole();
            string errorMessage = "You must enter an unfound 4+ letter word from the dictionary.";
            cout << errorMessage << endl << boggle;
            BoggleGUI::setStatusMessage(errorMessage);
        }
    }

    // (3) Computer's turn ==============================================================================
    BoggleGUI::clearHighlighting();

    // Display a message that it's the computer's turn
    statusMessage = "It's my turn!";
    cout << endl << statusMessage << endl;
    BoggleGUI::setStatusMessage(statusMessage);

    // Perform a search on board for all words
    Set<string> computerWords;
    computerWords = boggle.computerWordSearch();

    // Update display
    string computerMessage = "My words (" + integerToString(computerWords.size()) + ") : " + computerWords.toString();
    string computerScoreMessage = "My score: " + integerToString(boggle.getScoreComputer());
    cout << computerMessage << endl << computerScoreMessage << endl;

    // Update GUI
    for (string word: computerWords) {
        BoggleGUI::recordWord(word, BoggleGUI::COMPUTER);
    }
    BoggleGUI::setScore(boggle.getScoreComputer(), BoggleGUI::COMPUTER);

    // Display result
    if (boggle.getScoreComputer() > boggle.getScoreHuman()) {
        string loseMessage = "Ha ha ha, I destroyed you. Better luck next time, puny human!";
        cout << loseMessage << endl;
        BoggleGUI::setStatusMessage(loseMessage);
    } else {
        string winMessage = "WOW, you defeated me! Congratulations!";
        cout << winMessage << endl;
        BoggleGUI::setStatusMessage(winMessage);
    }
}

// ======================================================= Helper function(s) =======================================================
/*
 * Prompts the user for the input string of 16 letters to create a manual board.
 * Returns a 16 letter string
 */
string getManualConfigInput() {
    string input = getLine("Type the 16 letters to appear on the board: ");
    input = toUpperCase(input); // Convert the sequence to an upper case (case-insensitive)
    const int BOARD_SIZE = 16;

    for (int i = 0; i < BOARD_SIZE; i++) {
        char letter = input[i];

        // Reprompt the user if the input (1) contains non-alphabetic character or (2) is of length 16
        if (!isalpha(letter) || input.length() != BOARD_SIZE) {
            string errorMessage = "That is not a valid 16-letter board string. Try again.";
            cout << errorMessage << endl;
            return getManualConfigInput();
        }
    }
    return input;
}
