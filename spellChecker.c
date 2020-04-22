/**
 * CS 261 Data Structures
 * SpellChecker
 * Name: Dipan Patel (pateldip@oregonstate.edu)
 * Date: 2020 Mar. 7
 */
 
#include "hashMap.h"
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/**
 * SOURCES:
 * https://en.wikipedia.org/wiki/Levenshtein_distance#cite_note-5
 * I ended up taking the pseudocode from the Wikipedia article and building the following from that.
 */
int levDistance(const char* s, const char* t) {
    // get length of each string
    int lenS = strlen(s);
    int lenT = strlen(t);

    // initialize two vectors to hold integer distance values, initialized to zero
    int ** matrix = malloc((lenS+1) * sizeof(int *));
    for(int i=0; i<lenS+1; i++) {
        matrix[i] = malloc((lenT+1) * sizeof(int));
    }

    // valgrind kept yelling until I did this - uninitialized memory
    for(int i = 0; i < lenS+1; i++) {
        for(int j = 0; j < lenT+1; j++) {
            matrix[i][j] = 0;
        }
    }
   
    // initialize first row of matrix - compare target to empty string
    for (int i = 1; i<lenS+1; i++) {
        matrix[i][0] = i;
        //printf("Initializing row index: %d \n", i);
    }
    
    // initialize first column of matrix - compare starting string to empty string
    for (int i = 1; i<lenT+1; i++) {
        matrix[0][i] = i;
        //printf("Initializing col index: %d \n", i);
    }
    // fill in the remaining distances
    int a = 0;
    int b = 0;
    int c = 0;
    for (int i = 1; i<lenS+1; i++) {
        for (int j = 1; j<lenT+1; j++) {
            // Option A: Delete character
            a = matrix[i-1][j] + 1;
            // Option B: Add character (by deleting from target)
            b = matrix[i][j-1] + 1;
            // Option C: Substitute Character (only costs 1 if not matching)
            if(s[i-1]!=t[j-1]) {
                c = matrix[i-1][j-1] + 1;
            }
            else {
                c = matrix[i-1][j-1];
            }

            // Find option with smallest cost
            if (b < a) {
                a = b;
            }
            if (c < a) {
                a = c;
            }

            // store smallest cost option at current index
            matrix[i][j] = a;
        }
    }

    // store smallest distance and free allocated memory
    int ret = matrix[lenS][lenT];
    for(int i=0; i<lenS+1; i++) {
        matrix[i]=0;
        free(matrix[i]);
    }
    matrix = 0;
    free(matrix);

    //printf("%s (%d) | %s (%d)| %d \n", s, lenS, t, lenT, ret);
    return ret;
};

/**
 * Manage array of closest matches to a given word
 * Idea is to allow this program to manage capturing the lowest 5 distance words
 * When a value is found to be lower than an index, move it there and shift the rest up
 * @return used for debugging to alert when a word is found that is considered close
 */
int closest(HashLink ** table, HashLink * ptr) {
    HashLink * temp = NULL;
    int ret = 0;
    for(int i = 0; i < 5; i++) {
        if (table[i] == NULL) {
            table[i] = ptr;
            return 1;
        }
        if (ptr->value < table[i]->value) {
            temp = table[i];
            table[i] = ptr;
            ptr = temp;
            ret = 1;
        }
    }
    return ret;
};

/**
 * Allocates a string for the next word in the file and returns it. This string
 * is null terminated. Returns NULL after reaching the end of the file.
 * @param file
 * @return Allocated string or NULL.
 */
char* nextWord(FILE* file)
{
    int maxLength = 16;
    int length = 0;
    char* word = malloc(sizeof(char) * maxLength);
    while (1)
    {
        char c = fgetc(file);
        if ((c >= '0' && c <= '9') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            c == '\'')
        {
            if (length + 1 >= maxLength)
            {
                maxLength *= 2;
                word = realloc(word, maxLength);
            }
            word[length] = c;
            length++;
        }
        else if (length > 0 || c == EOF)
        {
            break;
        }
    }
    if (length == 0)
    {
        free(word);
        return NULL;
    }
    word[length] = '\0';
    return word;
}

/**
 * Loads the contents of the file into the hash map.
 * @param file
 * @param map
 */
void loadDictionary(FILE* file, HashMap* map)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert((file != NULL) && (map != NULL));
    // get next dictionary word from file
    char * key = nextWord(file);
    // until NULL terminator is reached in file, add each word from file to HashMap
    while(key != NULL) {
        // place key into hashmap with placeholder value of -1
        hashMapPut(map, key, -1);
        // since we allocate memory via nextWord and return pointer to it, free it after we add it
        free(key);
        // go on to next word in file
        key = nextWord(file);
    }
}

/**
 * Checks the spelling of the word provided by the user. If the word is spelled incorrectly,
 * print the 5 closest words as determined by a metric like the Levenshtein distance.
 * Otherwise, indicate that the provded word is spelled correctly. Use dictionary.txt to
 * create the dictionary.
 * @param argc
 * @param argv
 * @return
 */
int main(int argc, const char** argv)
{
    // FIXME: implement
    HashMap* map = hashMapNew(1000);

    FILE* file = fopen("dictionary.txt", "r");
    clock_t timer = clock();
    loadDictionary(file, map);
    timer = clock() - timer;
    printf("Dictionary loaded in %f seconds\n", (float)timer / (float)CLOCKS_PER_SEC);
    fclose(file);

    char inputBuffer[256];
    int quit = 0;

    while (!quit)
    {
        printf("Enter a word or \"quit\" to quit: ");
        scanf("%s", inputBuffer);
        
        // Implement the spell checker code here... 
        for(int i = 0; i < strlen(inputBuffer); i++) {
            inputBuffer[i] = tolower(inputBuffer[i]);
        }
        // if key is found, output as such
        if(hashMapContainsKey(map, inputBuffer)) {
            printf("The inputted word '%s' is spelled correctly. \n\n", inputBuffer);
        }
        // otherwise, go through dictionary and identify closest matches and store in closestTable
        else {
            // table to hold closest values, initialize to zero cus you have a ton of bugs baby
            HashLink ** closestTable = malloc(sizeof(HashLink *) * 5);
            for(int i=0; i<5; i++) {
                closestTable[i] = NULL;
            }
             printf("The inputted word '%s' is spelled incorrectly. \n", inputBuffer);
             for(int i = 0; i < map->capacity; i++) {
                 HashLink * ptr = map->table[i];
                 while(ptr != NULL) {
                     // calculate distance between input and dictionary word
                    int distance = levDistance(ptr->key, inputBuffer);
                    ptr->value = distance;

                    // used for debugging but need to run closest() to update table of closest matches
                    if (closest(closestTable, ptr)) {
                        //printf("LEV DISTANCE: %s & %s is %d | len: %d, %d \n", inputBuffer, ptr->key, distance, strlen(inputBuffer), strlen(ptr->key));
                        //printf("LEV DISTANCE: %s & %s \n", inputBuffer, ptr->key);
                    }
                    
                    // traverse to next link
                    ptr = ptr->next;
                 }
                 // idk why I have this but I added it during debugging and i'm afraid to get rid of it
                 ptr = NULL;
             }
             printf("Did you mean...: ");
             // output the 5 closest matches
             for(int i=0; i<5; i++) {
                 printf("%s ", closestTable[i]->key);
             }
             printf("? \n \n");

             // idk why I have this but I added it during debugging and i'm afraid to get rid of it
            for(int i=0; i<5; i++) {
                closestTable[i] = NULL;
            }
            free(closestTable);
        }
        if (strcmp(inputBuffer, "quit") == 0)
        {
            quit = 1;
        }
        
    }
    hashMapDelete(map);
    return 0;
}
