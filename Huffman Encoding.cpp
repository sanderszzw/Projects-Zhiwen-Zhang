/*
 * File: encoding.cpp
 * ------------
 * Name: Zhiwen Zhang
 * Course: CS 106B
 * Description: Implements compression(encoding) and decompression(decoding) using Huffman's encoding algorithm.
 *
 */

#include <iostream>
#include <string>
#include "bitstream.h"
#include "encoding.h"
#include "filelib.h"
#include "HuffmanNode.h"
#include "map.h"
#include "pqueue.h"

using namespace std;

/*
 * Method: buildFrequencyTable
 * Usage: Map<int, int> freqTable = buildFrequencyTable(input);
 * -------------------------
 * Accepts an input istream
 * Returns a map from each character represented as an integer variable to its frequency.
 * Returns a map with 1 occurence of PSEUDO_EOF if the file is empty (i.e. {EOF: 1}).
 * Assumes that the input file exists and can be read.
 */
Map<int, int> buildFrequencyTable(istream& input) {
    Map<int, int> freqTable;
    while (true) {
        int ch = input.get();
        if (ch == -1) {
            // End of file
            freqTable.put(PSEUDO_EOF, 1);
            break;
        }
        freqTable[ch]++;
    }
    return freqTable;
}

/*
 * Method: buildEncodingTree
 * Usage: HuffmanNode* encodingTree = buildEncodingTree(freqTable);
 * -------------------------
 * Accepts a frequency table that maps a character represented as an integer variable to its frequency
 * Returns a pointer to the root of the Huffman encoding tree
 * Builds a Huffman encoding tree based on the provided frequency table.
 * Assumes that the provided frequency table is valid.
 */
HuffmanNode* buildEncodingTree(const Map<int, int>& freqTable) {
    // Use a priority queue to keep track of which nodes to process next
    PriorityQueue<HuffmanNode*> pq;
    HuffmanNode* node;
    HuffmanNode* node0;
    HuffmanNode* node1;

    // Put all the elements in the map into the priority queue
    for (int key: freqTable) {
        node = new HuffmanNode(key, freqTable[key]);
        pq.enqueue(node, node->count);
    }

    // Repeatedly removes the two nodes from the front of the queue and joins them into a new node whose frequency is their sum
    while(pq.size() > 1) {
        node0 = pq.dequeue();
        node1 = pq.dequeue();
        node = new HuffmanNode(NOT_A_CHAR, (node0->count + node1->count), node0, node1);
        pq.enqueue(node, node->count);
    }
    return node; //Pointer to the root of the encoding tree
}

/*
 * Method: buildEncodingMapHelper
 * Usage: buildEncodingMapHelper(encodingTree, encodingMap, binaryStr);
 * -------------------------
 * *** Helper function to buildEncodingMap.
 * Accepts a pointer to the root of a Huffman tree and a binaryString as values, and encodingMap as a reference parameter.
 * Traverses the encoding Tree using recursion to create a Huffman encoding map.
 */
void buildEncodingMapHelper(HuffmanNode* encodingTree, Map<int, string>& encodingMap, string binaryStr) {
    if (encodingTree->isLeaf()) {
         // Base case: leaf
        encodingMap[encodingTree->character] = binaryStr;
    } else {
        // Recursive case
        buildEncodingMapHelper(encodingTree->zero, encodingMap, binaryStr + "0");
        buildEncodingMapHelper(encodingTree->one, encodingMap, binaryStr + "1");
    }
}

/*
 * Method: buildEncodingMap
 * Usage: Map<int, string> encodingMap = buildEncodingMap(encodingTree);
 * -------------------------
 * Accepts a pointer to the root of a Huffman tree.
 * Returns a map from each character represented as an integer variable to its binary representation.
 * Returns an empty map if the encoding tree is NULL.
 */
Map<int, string> buildEncodingMap(HuffmanNode* encodingTree) {
    Map<int, string> encodingMap;
    string binaryStr;
    // Return an empty map if the encoding tree is NULL
    if (encodingTree == NULL) {
        return encodingMap;
    }

    // Create a Huffman encoding map based on the tree's structure
    buildEncodingMapHelper(encodingTree, encodingMap, binaryStr);

    return encodingMap;
}

/*
 * Method: writeBinaryStrAsBit
 * Usage: writeBinaryStrAsBit(output, binaryStr);
 * -------------------------
 * *** Helper function to encodeData
 * Accepts a binaryString as a value and output obitstream as a reference parameter.
 * Writes the binary representation of the character as bits in the output stream.
 */
void writeBinaryStrAsBit (obitstream& output, string binaryStr) {
    for (char bit: binaryStr) {
        output.writeBit(bit - '0');
    }
}

/*
 * Method: encodeData
 * Usage: encodeData(input, encodingMap, output);
 * -------------------------
 * Accepts an input istream, encodingMap, and output obitstream as reference parameters.
 * Encodes each character to binary representation and write the binary bits to the given out bitstream.
 */
void encodeData(istream& input, const Map<int, string>& encodingMap, obitstream& output) {
    string binaryStr;
    while (true) {
        int ch = input.get();
        // Read one character at a time from a given input file
        if (ch == -1) {
            // End of file: Write a single occurence of the binary encoding for PSUEDO_OEF into the output
            binaryStr = encodingMap[PSEUDO_EOF];
            writeBinaryStrAsBit(output, binaryStr);
            break;
        } else {
            // Use the provided encodingMap to encode each character into binary string
            binaryStr = encodingMap[ch];
            writeBinaryStrAsBit(output, binaryStr);
        }
    }
}

/*
 * Method: decodeDataHelper
 * Usage: decodeDataHelper(input, encodingTree->zero, output, root);
 * -------------------------
 * *** Helper function to decodeData
 * Accepts a pointer to a node in the encodingTree and a pointer to the root of the encodingTree as values,
 *  and the input ibitstream and output ostream as reference parameters.
 * Recursively traverses the encoding tree to write the original uncompressed contents of the file to the given output stream.
 */
void decodeDataHelper (ibitstream& input, HuffmanNode* encodingTree, ostream& output, HuffmanNode* root) {
    if (encodingTree->isLeaf()) {
        if (encodingTree->character == PSEUDO_EOF) {
            // End of file: Stop
            return;
        }
        // Base case: Hit a leaf node: output the character
        output.put(encodingTree->character);
        encodingTree = root;
    }

    // Recursive case: Read more bits & move down the tree until reaching a leaf node
     int bit = input.readBit();
     if (bit == -1) {
         // Corrupted file reaches the end
         return;
     }
     if (bit == 0) {
         decodeDataHelper(input, encodingTree->zero, output, root); // Move left
     } else {
         decodeDataHelper(input, encodingTree->one, output, root); // bit == 1; Move right
     }
}

/*
 * Method: decodeData
 * Usage: decodeData(input, encodingTree, output);
 * -------------------------
 * Accepts a pointer to a node in the encodingTree as a value parameter and
 *   the input ibitstream and output ostream as reference parameters.
 * Recursively traverses the encoding tree to write the original uncompressed contents of the file to the given output stream.
 */
void decodeData(ibitstream& input, HuffmanNode* encodingTree, ostream& output) {
    // Store the pointer to the root of the tree so that we can go back to the root of the tree
    // once we reach a leaf node
    HuffmanNode* root = encodingTree;
    decodeDataHelper(input, encodingTree, output, root);

    // Free the memory associated with the Huffman tree
    freeTree(root);
}

/*
 * Method: compress
 * Usage: compress(input, output)
 * -------------------------
 * Accepts an input istream and output obitstream as reference parameters.
 * Compresses the given input file and writes the compressed contents of the file to output.
 */
void compress(istream& input, obitstream& output) {
    // Compress the input file using a Huffman's algorithm
    Map<int, int> freqTable = buildFrequencyTable(input);
    HuffmanNode* encodingTree = buildEncodingTree(freqTable);
    Map<int, string> encodingMap = buildEncodingMap(encodingTree);

    // Print frequency table to output
    output << freqTable;

    // Rewind the stream and encode the file to output
    rewindStream(input);
    encodeData(input, encodingMap, output);

    // Free the memory associated with the Huffman tree
    freeTree(encodingTree);
}

/*
 * Method: decompress
 * Usage: decompress(input, output)
 * -------------------------
 * Accepts an input ibitstream and output ostream as reference parameters.
 * Decompresses the given input file and writes the original contents of the file to output.
 */
void decompress(ibitstream& input, ostream& output) {
    // Read the frequency table
    Map<int, int> freqTable;
    input >> freqTable;

    // Build tree
    HuffmanNode* encodingTree = buildEncodingTree(freqTable);

    // Decode file
    decodeData(input, encodingTree, output);

    // Free the memory associated with the Huffman tree
    freeTree(encodingTree);
}

/*
 * Method: freeTree
 * Usage: freeTree(encodingTree);
 * -------------------------
 * Accepts a pointer to the root of the encoding tree.
 * Recursively traverse the encoding tree to delete the nodes and free the associated memory.
 */
void freeTree(HuffmanNode* node) {
    if (node == NULL) {
        // No effect if the tree passed is NULL
        return;
    }

    if (node->isLeaf()) {
        // Base case
        delete node;
    }

    // Recursive case
    freeTree(node->zero);
    freeTree(node->one);
}
