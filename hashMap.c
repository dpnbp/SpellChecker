/*
 * CS 261 Data Structures
 * HashMap Implementation
 * Name: Dipan Patel (pateldip@oregonstate.edu)
 * Date: 2020 Mar. 7
 */

#include "hashMap.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

int hashFunction1(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += key[i];
    }
    return r;
}

int hashFunction2(const char* key)
{
    int r = 0;
    for (int i = 0; key[i] != '\0'; i++)
    {
        r += (i + 1) * key[i];
    }
    return r;
}

/**
 * Creates a new hash table link with a copy of the key string.
 * @param key Key string to copy in the link.
 * @param value Value to set in the link.
 * @param next Pointer to set as the link's next.
 * @return Hash table link allocated on the heap.
 */
HashLink* hashLinkNew(const char* key, int value, HashLink* next)
{
    HashLink* link = malloc(sizeof(HashLink));
    link->key = malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(link->key, key);
    link->value = value;
    link->next = next;
    return link;
}

/**
 * Free the allocated memory for a hash table link created with hashLinkNew.
 * @param link
 */
static void hashLinkDelete(HashLink* link)
{
    free(link->key);
    free(link);
}

/**
 * Initializes a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param map
 * @param capacity The number of table buckets.
 */
void hashMapInit(HashMap* map, int capacity)
{
    map->capacity = capacity;
    map->size = 0;
    map->table = malloc(sizeof(HashLink*) * capacity);
    for (int i = 0; i < capacity; i++) {
	    map->table[i] = NULL;
    }
}

/**
 * Removes all links in the map and frees all allocated memory. You can use
 * hashLinkDelete to free the links.
 * @param map
 */
void hashMapCleanUp(HashMap* map)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert(map != NULL);
    
    int i;
    // for each bucket in the hashmap
    for(i = 0; i < map->capacity; i++) {
        // free each hashlink in bucket until all freed (until NULL reached)
        while(map->table[i] != NULL) {
            HashLink * delMe = map->table[i];
            map->table[i] = map->table[i]->next;
            hashLinkDelete(delMe);
        }
    }
    // free the hash table
    free(map->table);
    // reset remaining HashMap members to initial values
    map->size = 0;
    map->capacity = 0;
}

/**
 * Creates a hash table map, allocating memory for a link pointer table with
 * the given number of buckets.
 * @param capacity The number of buckets.
 * @return The allocated map.
 */
HashMap* hashMapNew(int capacity)
{
    HashMap* map = malloc(sizeof(HashMap));
    hashMapInit(map, capacity);
    return map;
}

/**
 * Removes all links in the map and frees all allocated memory, including the
 * map itself.
 * @param map
 */
void hashMapDelete(HashMap* map)
{
    hashMapCleanUp(map);
    free(map);
}

/**
 * Returns a pointer to the value of the link with the given key  and skip traversing as well. Returns NULL
 * if no link with that key is in the table.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return Link value or NULL if no matching link.
 */
int* hashMapGet(HashMap* map, const char* key)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert((map != NULL) && (key !=  NULL));
    //printf("DEBUG:: GETTING %c \n", *key);
    
    // find index of bucket for given key
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    // initialize pointer to head of bucket
    HashLink * ptr = map->table[hashIndex];
    // traverse linked list in search of given key
    while(ptr != NULL) {
        // if key is found, return pointer to value of key/value pair
        if (strcmp(ptr->key, key) == 0) {
            return &(ptr->value);
        }
        // else go to the next link
        ptr = ptr->next;
    }
    
    // if NULL is reached, then the given key does not exist in the hashed bucket
    return NULL;
}

/**
 * Resizes the hash table to have a number of buckets equal to the given 
 * capacity (double of the old capacity). After allocating the new table, 
 * all of the links need to rehashed into it because the capacity has changed.
 * 
 * Remember to free the old table and any old links if you use hashMapPut to
 * rehash them.
 * 
 * @param map
 * @param capacity The new number of buckets.
 */
void resizeTable(HashMap* map, int capacity)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert(map != NULL);
    //printf("DEBUG:: RESIZING TO %d\n", capacity);
    
    /* store old table and associated values */
    HashLink ** oldTable = map->table;
    int oldCapacity = map->capacity;
    
    /* initialize new table with given capacity */
    map->table = (HashLink **) malloc(capacity * sizeof(HashLink *));
    // initialize newly allocated table
    for (int i = 0; i < capacity; i++) {
	    map->table[i] = NULL;
    }
    map->capacity = capacity;
    map->size = 0;
    
    /* add key/value pairs from old table into new table via hashMapPut */
    int i;
    // for each bucket in old table
    for(i = 0; i < oldCapacity; i++) {
        // until end of bucket's linked list is reached
        while(oldTable[i] != NULL) {
            // store current link to allow for freeing memory after adding
            HashLink * delMe = oldTable[i];
            // put old table's link in map's resized table
            hashMapPut(map, oldTable[i]->key, oldTable[i]->value);
            // go to next link
            oldTable[i] = oldTable[i]->next;
            // free the old link
            hashLinkDelete(delMe);
        }
    }
    
    // free old table's memory
    free(oldTable);
}

/**
 * Updates the given key-value pair in the hash table. If a link with the given
 * key already exists, this will just update the value and skip traversing. Otherwise, it will
 * create a new link with the given key and value and add it to the table
 * bucket's linked list. You can use hashLinkNew to create the link.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket.
 * 
 * @param map
 * @param key
 * @param value
 */
void hashMapPut(HashMap* map, const char* key, int value)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert((map != NULL) && (key != NULL));
    //printf("DEBUG:: PUTTING  (%c)%d \n", *key, value);

    /* traverse map bucket's linked list in search of given key */
    // find index of bucket for given key
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    
    // initialize pointer to head of bucket
    HashLink * ptr = map->table[hashIndex];
    
    // iterate through list until at end of list
    while(ptr != NULL) {
        // if key is found, update value field with given value
        if (strcmp(ptr->key, key) == 0) {
            ptr->value = value;
            // keys are unique so get the heck outta here once key/value updated
            //printf("DEBUG:: UPDATED  (%c)%d \n", *key, value);
            return;
        }
        // else go to the next link
        ptr = ptr->next;
    }

    /* if end of list is reached, then key is not found - add new link to bucket with given key/value */
    // if load factor is more than maximum load as defined with MAX_TABLE_LOAD, double table size
    if(hashMapTableLoad(map) >= MAX_TABLE_LOAD) {
        resizeTable(map, map->capacity*2);
    }
    

    // update hashIndex in the event that table was resized
    hashIndex = HASH_FUNCTION(key) % map->capacity;

    // initialize new link and place at head of list
    //printf("DEBUG:: PUT  (%c)%d \n", *key, value);
    map->table[hashIndex] = hashLinkNew(key, value, map->table[hashIndex]);
    map->size++;
}

/**
 * Removes and frees the link with the given key from the table. If no such link
 * exists, this does nothing. Remember to search the entire linked list at the
 * bucket. You can use hashLinkDelete to free the link.
 * @param map
 * @param key
 */
void hashMapRemove(HashMap* map, const char* key)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert((map != NULL) && (key != NULL));
    //printf("DEBUG:: REMOVING %c \n", *key);

    /* traverse map bucket's linked list in search of given key */
    // find index of bucket for given key
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    
    // initialize pointer to head of bucket
    HashLink * ptr = map->table[hashIndex];
    HashLink * prevPtr = NULL;
    // iterate through list until at end of list
    while(ptr != NULL) {
        // if key is found, remove and free associated link
        if (strcmp(ptr->key, key) == 0) {
            // if this is not the first link, resolve previous and next link pointers to skip over this link
            if(prevPtr != NULL) {
                prevPtr->next = ptr->next;
            }
            // otherwise it is the first link and need to set table to point to next link as head
            else {
                map->table[hashIndex] = ptr->next;
            }
            // free this link
            hashLinkDelete(ptr);
            // decrement map size
            map->size--;
            // keys are unique so get the heck outta here once key is found and removed
            //printf("DEBUG:: REMOVED);
            return;
        }
        // else go to the next link
        prevPtr = ptr;
        ptr = ptr->next;
    }
    
    // if key is not found, nothing happens
}

/**
 * Returns 1 if a link with the given key is in the table and 0 otherwise.
 * 
 * Use HASH_FUNCTION(key) and the map's capacity to find the index of the
 * correct linked list bucket. Also make sure to search the entire list.
 * 
 * @param map
 * @param key
 * @return 1 if the key is found, 0 otherwise.
 */
int hashMapContainsKey(HashMap* map, const char* key)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert((map != NULL) && (key != NULL));
    
    // find index of bucket for given key
    int hashIndex = HASH_FUNCTION(key) % map->capacity;
    // initialize pointer to head of bucket
    HashLink * ptr = map->table[hashIndex];
    // traverse linked list in search of given key
    while(ptr != NULL) {
        // if key is found, return 1
        if (strcmp(ptr->key, key) == 0) {
            return 1;
        }
        // else go to the next link
        ptr = ptr->next;
    }
    
    // if NULL is reached, then the given key does not exist in the hashed bucket
    return 0;
}

/**
 * Returns the number of links in the table.
 * @param map
 * @return Number of links in the table.
 */
int hashMapSize(HashMap* map)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert(map != NULL);
    
    return map->size;
}

/**
 * Returns the number of buckets in the table.
 * @param map
 * @return Number of buckets in the table.
 */
int hashMapCapacity(HashMap* map)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert(map != NULL);
    
    return map->capacity;
}

/**
 * Returns the number of table buckets without any links.
 * @param map
 * @return Number of empty buckets.
 */
int hashMapEmptyBuckets(HashMap* map)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert(map != NULL);
    
    int emptyBucketCount = 0;
    
    // for each bucket in table
    int i;
    for(i = 0; i < map->capacity; i++) {
        // if list head is null, then bucket is empty - increment count
        emptyBucketCount += (map->table[i] == NULL);
    }
    
    return emptyBucketCount;
}

/**
 * Returns the ratio of (number of links) / (number of buckets) in the table.
 * Remember that the buckets are linked lists, so this ratio tells you nothing
 * about the number of empty buckets. Remember also that the load is a floating
 * point number, so don't do integer division.
 * @param map
 * @return Table load.
 */
float hashMapTableLoad(HashMap* map)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert(map != NULL);
    
    return (float) map->size / (float) map->capacity;
}

/**
 * Prints all the links in each of the buckets in the table.
 * @param map
 */
void hashMapPrint(HashMap* map)
{
    // FIXME: done
    /* ensure arguments are valid */
    assert(map != NULL);
    
    // for each bucket in table
    int i;
    HashLink * ptr;

    for(i = 0; i < map->capacity; i++) {
        ptr = map->table[i];
        printf("\n map->table[%d]: ", i);
        while(ptr != NULL) {
            printf("(%c)%d ", *(ptr->key), ptr->value);
            ptr = ptr->next;
        }

        printf("<>");
    }
    printf("\n");
}

