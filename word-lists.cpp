/*
 * Word Lists.cpp
 * 7/18/17
 * Autumn Teague
 * 
 * Purpose: To demonstrate C style I/O and string manipulation by processing
 * large text files.
 * This program processes each entry in a WordNet 3.1 index file and retrieves
 * all definitions from the corresponding data file. The WordNet database files
 * included are distributed as per the license contained at the top of each
 * file.
 * 
 * I appreciate feedback! Is my code efficient, safe, and readable?
 * Let me know!
 * ateague063@gmail.com
 */
#include <string>
#include <vector>
//change PATH to wherever you have stored your database files
#define PATH "C:/Users/default.default-PC/Desktop/words/"
//change EXT to the abbreviation of the part of speech to be processed.
//possible values are ADJ, ADV, NOUN, and VERB
#define EXT VERB
//probably don't change these
#define WORD_FILE(type, ext) (PATH## #type##ext)
#define OUTPUT_FILE(type) (PATH##"output"##type##".txt")
#define ADJ ".adj"
#define ADV ".adv"
#define NOUN ".noun"
#define VERB ".verb"
#define INDEX_START 1740
#define WORD_SIZE 64
#define LOC_STR_SIZE 768
#define WORD_DEF_SIZE 256

//recursively parse tokens once context has been established. DO NOT CALL.
inline void _parse_locations_rec(wchar_t** context, std::vector<int>& out)
    {
    //try to get the next token. If result is null, no more tokens exist.
    auto result = wcstok_s(nullptr, L" ", context);
    if (result == nullptr) return;
    //otherwise, parse and store token and repeat
    out.push_back(std::stoi(result));
    _parse_locations_rec(context, out);
    }

//Parses locations from a space-delimited string
void parse_locations(wchar_t* str, std::vector<int>& out)
    {
    wchar_t* context;
    //get the first token, parse as int, and store it
    out.push_back(std::stoi(wcstok_s(str, L" ", &context)));
    //pass context and vector ref to recursive function
    _parse_locations_rec(&context, out);
    }

int main()
    {
    printf_s("Press any key to process files...");
    getchar();
    std::vector<int> locations;
    FILE* index_file, *data_file, *output_file;
    //open index, data, and output files.
    fopen_s(&index_file, WORD_FILE(index, EXT), "r");
    fopen_s(&data_file, WORD_FILE(data, EXT), "r");
    fopen_s(&output_file, OUTPUT_FILE(EXT), "w");
    //seek to the first index entry
    fseek(index_file, INDEX_START, SEEK_SET);

    while (!feof(index_file))
        {
        wchar_t word[WORD_SIZE];
        wchar_t loc_str[LOC_STR_SIZE];

        /* scan next index entry. Word is at the beginning of the line,
         * definitions are at the end. All fields are delimited by spaces,
         * and most of them we do not need, so they are discarded. Only the
         * first and last format fields are stored. Because the secure version
         * of this function is used, the size of the target arguments also
         * need to be passed.
         */
        fwscanf_s(index_file, L"%s %*c%*d%*d%*[^0-9]%*d%*d%[^a-z\n]",
            word, WORD_SIZE, loc_str, LOC_STR_SIZE);
        //write the word to the output file and the std output
        fwprintf_s(output_file, L"%ls :\n", word);
        wprintf_s(L"%ls\n", word);
        //parse locations into vector of ints
        parse_locations(loc_str, locations);

        for (auto it = locations.begin(); it != locations.end(); ++it)
            {
            wchar_t def[WORD_DEF_SIZE];
            //seek to definition
            fseek(data_file, *it, SEEK_SET);
            //read the line. The defintion starts at the | character.
            fwscanf_s(data_file, L"%*[^|]| %[^\n]", def, WORD_DEF_SIZE);
            //print the def into the output file
            fwprintf_s(output_file, L"    %ls\n", def);
            } //for it = locations.begin(); it != locations.end(); ++it

        //clear location vector
        locations.clear();
        } //while !feof(index)

    //close all files and wait for user input
    _fcloseall();
    wprintf_s(L"File finished processing. Press any key to continue...");
    getchar();
    return 0;
    }