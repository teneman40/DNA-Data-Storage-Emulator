#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cmath>

using namespace std;

// --- BIOLOGICAL ARCHITECTURE CONSTANTS ---
// These define the physical properties of the DNA strands we are synthesizing.
const string LEFT_PRIMER = "ATGC";     // Helps the sequencing machine grab the start of the strand
const string RIGHT_PRIMER = "GCAT";    // Marks the end of the strand
const int PAYLOAD_LIMIT = 100;         // Max number of biological bases per strand (keeps synthesis error rate low)
const int INDEX_LENGTH = 8;            // 8 bases for the index gives us 4^8 (65,536) unique page numbers


// ---------------------------------------------------------
// FUNCTION 1: Binary to Base-4 Translator
// Translates a single 8-bit byte from the computer into a 4-base DNA sequence.
// Mapping: 00=A, 01=C, 10=G, 11=T
// ---------------------------------------------------------
string byteToDNA(unsigned char byte) {
    string dnaSequence = "";
    // Read the byte in 2-bit chunks, starting from the far left (most significant bits)
    for (int i = 6; i >= 0; i -= 2) {
        // Bitwise shift and mask to isolate 2 bits
        int baseValue = (byte >> i) & 0x03; 
        
        switch (baseValue) {
            case 0: dnaSequence += "A"; break;
            case 1: dnaSequence += "C"; break;
            case 2: dnaSequence += "G"; break;
            case 3: dnaSequence += "T"; break;
        }
    }
    return dnaSequence;
}

// ---------------------------------------------------------
// FUNCTION 2: The Indexer (Base-10 to Base-4 Math)
// Converts the loop counter (Page Number) into a DNA sequence.
// ---------------------------------------------------------
string generateIndexDNA(int loopCounter, int length) {
    string indexDNA = "";
    int temp = loopCounter;
    
    // Convert the integer to Base-4 using modulo arithmetic
    for (int i = 0; i < length; ++i) {
        int remainder = temp % 4;
        switch (remainder) {
            case 0: indexDNA = "A" + indexDNA; break;
            case 1: indexDNA = "C" + indexDNA; break;
            case 2: indexDNA = "G" + indexDNA; break;
            case 3: indexDNA = "T" + indexDNA; break;
        }
        temp /= 4; // Move to the next power of 4
    }
    return indexDNA;
}

// ---------------------------------------------------------
// MAIN PROGRAM EXECUTION
// ---------------------------------------------------------
int main(int argc, char* argv[]) {
    // 1. Ensure the user provided a file to encode
    if (argc < 2) {
        cerr << "Usage: ./dna_encoder <filename>" << endl;
        return 1;
    }

    string inputFilename = argv[1];
    string outputFilename = "encoded_data.fasta"; // .fasta is the standard bioinformatics text file

    // 2. Open the file in BINARY mode so we read raw 0s and 1s, not text formatting
    ifstream inputFile(inputFilename, ios::binary);
    if (!inputFile) {
        cerr << "Error: Could not open file " << inputFilename << endl;
        return 1;
    }

    // 3. Read the entire file and convert it into one massive continuous DNA string
    cout << "Translating binary file to Quaternary DNA..." << endl;
    string masterDNAString = "";
    char currentByte;
    while (inputFile.get(currentByte)) {
        // Cast to unsigned char to handle binary data correctly
        masterDNAString += byteToDNA(static_cast<unsigned char>(currentByte));
    }
    inputFile.close();

    // 4. Open the output file to write our physical strand instructions
    ofstream outputFile(outputFilename);
    if (!outputFile) {
        cerr << "Error: Could not create output file." << endl;
        return 1;
    }

    // 5. The "Assembly Line" Loop (Chunking and Indexing)
    cout << "Packaging DNA into indexed strands for synthesis..." << endl;
    int totalBases = masterDNAString.length();
    int loopCounter = 0; // This is our Page Number (Index)

    // Loop through the massive DNA string, grabbing chunks of size PAYLOAD_LIMIT
    for (int i = 0; i < totalBases; i += PAYLOAD_LIMIT) {
        
        // Grab the actual file data for this box (The Payload)
        string payload = masterDNAString.substr(i, PAYLOAD_LIMIT);
        
        // Create the mathematical label for this box (The Index)
        string index = generateIndexDNA(loopCounter, INDEX_LENGTH);

        // Stitch the physical molecule together
        string finalStrand = LEFT_PRIMER + index + payload + RIGHT_PRIMER;

        // Write to the .fasta file
        // FASTA format requires a ">" header line before the sequence
        outputFile << ">Strand_" << loopCounter << "\n";
        outputFile << finalStrand << "\n";

        loopCounter++; // Increment the page number for the next chunk
    }

    outputFile.close();

    cout << "\nSUCCESS!" << endl;
    cout << "Total raw DNA bases generated: " << totalBases << endl;
    cout << "Total physical strands to synthesize: " << loopCounter << endl;
    cout << "File saved as: " << outputFilename << endl;

    return 0;
}