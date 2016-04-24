//
//  Starcounter Lisp
//
//  Vector.cpp
//  Based on Phil Bagwell original algorith and the implementation by Chae Seong Lim.
//  See HashTrie.h.
//
//  Our vector is a bitmapped hash/vector trie invented by Phil Bagwell and made popular by
//  Clojure.
//
//  Needs unit tests badly.
//
//  Created by Joachim Wester on 22/04/16.
//
//  Copyright © 2016 Starcounter AB.
//

#include "Vector.hpp"
#include "HashTrie.h"



#include <stdint.h>
#include "Vector.hpp"

//===========================================================================
// START of MurMurHash3 code
//===========================================================================

// MurMurHash3 code is under MIT license and more info can be found at
// http://code.google.com/p/smhasher/


#if defined(_MSC_VER)

#define FORCE_INLINE    __forceinline

#include <stdlib.h>

#define ROTL32(x,y)    _rotl(x,y)
#define ROTL64(x,y)    _rotl64(x,y)

#define BIG_CONSTANT(x) (x)

// Other compilers

#else    // defined(_MSC_VER)

#define FORCE_INLINE __attribute__((always_inline))

inline uint32_t rotl32( uint32_t x, int8_t r )
{
    return (x << r) | (x >> (32 - r));
}

inline uint64_t rotl64( uint64_t x, int8_t r )
{
    return (x << r) | (x >> (64 - r));
}

//#define ROTL32(x,y)    rotl32(x,y)
//#define ROTL64(x,y)    rotl64(x,y)

#define BIG_CONSTANT(x) (x##LLU)

#endif // !defined(_MSC_VER)

//-----------------------------------------------------------------------------
// Block read - if your platform needs to do endian-swapping or can only
// handle aligned reads, do the conversion here

FORCE_INLINE uint32_t getblock( const uint32_t * p, int i ) {
    return p[i];
}

FORCE_INLINE uint64_t getblock( const uint64_t * p, int i ) {
    return p[i];
}

//-----------------------------------------------------------------------------
// Finalization mix - force all bits of a hash block to avalanche

FORCE_INLINE uint32_t fmix(uint32_t h) {
    h ^= h >> 16;
    h *= 0x85ebca6b;
    h ^= h >> 13;
    h *= 0xc2b2ae35;
    h ^= h >> 16;
    
    return h;
}

//----------

FORCE_INLINE uint64_t fmix(uint64_t k) {
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xff51afd7ed558ccd);
    k ^= k >> 33;
    k *= BIG_CONSTANT(0xc4ceb9fe1a85ec53);
    k ^= k >> 33;
    
    return k;
}

uint32 MurmurHash3_x86_32(
                          const void * key,
                          int len,
                          uint32_t seed
                          ) {
    const uint8_t * data = (const uint8_t*)key;
    const int nblocks = len / 4;
    
    uint32_t h1 = seed;
    
    uint32_t c1 = 0xcc9e2d51;
    uint32_t c2 = 0x1b873593;
    
    //----------
    // body
    
    const uint32_t * blocks = (const uint32_t *)(data + nblocks * 4);
    
    for (int i = -nblocks; i; i++) {
        uint32_t k1 = getblock(blocks,i);
        
        k1 *= c1;
        k1 = ROTL32(k1,15);
        k1 *= c2;
        
        h1 ^= k1;
        h1 = ROTL32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }
    
    //----------
    // tail
    
    const uint8_t * tail = (const uint8_t*)(data + nblocks * 4);
    
    uint32_t k1 = 0;
    
    switch(len & 3) {
        case 3: k1 ^= tail[2] << 16;
        case 2: k1 ^= tail[1] << 8;
        case 1: k1 ^= tail[0];
            k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
    };
    
    //----------
    // finalization
    
    h1 ^= len;
    
    h1 = fmix(h1);
    
    return h1;
}

//===========================================================================
// END of MurMurHash3 code
//===========================================================================



#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


/**
 * These functions definitions are for all the internal APIs that are used to
 * interact with the bit-partitioned trie that backs our PersistentVector.
 */
PVecNode *pvec_node_new(void);
PVecNode *pvec_node_copy(PVecNode *node);
PersistentVector *pvec_copy(PersistentVector *vec);
PersistentVector *pvec_set(PersistentVector *vec, uint64_t key, void *data, bool insert);
void pvec_append_tail(PersistentVector *vec);


/**
 * Create a new node that is totally empty. We set all children and elements to
 * NULL. The NULL is used in order to known when we need to make a new node
 * during inserts.
 */
PVecNode *pvec_node_new(void) {
    PVecNode *neew = NULL;
    if((neew = (PVecNode*)malloc(sizeof(PVecNode))) == NULL) {
        perror("malloc");
        goto pvec_node_new_return;
    }
    for(uint64_t i = 0; i < WIDTH; i++) {
        neew->children[i] = NULL;
        neew->elements[i] = NULL;
    }
pvec_node_new_return:
    return neew;
}

/**
 * Take a copy of a single node within the trie that backs a Persistent vector.
 * We copy all pointers to children and elements into the new node.
 */
PVecNode *pvec_node_copy(PVecNode *node) {
    PVecNode *new_node = NULL;
    if((new_node = (PVecNode*)malloc(sizeof(PVecNode))) == NULL) {
        perror("malloc");
        goto pvec_node_copy_return;
    }
    if(memcpy(new_node->children, node->children, sizeof(PVecNode*) * WIDTH) == NULL) {
        perror("memcpy");
        free(new_node);
        new_node = NULL;
        goto pvec_node_copy_return;
    }
    if(memcpy(new_node->elements, node->elements, sizeof(void*) * WIDTH) == NULL) {
        perror("memcpy");
        free(new_node);
        new_node = NULL;
        goto pvec_node_copy_return;
    }
pvec_node_copy_return:
    return new_node;
}

/**
 * pvec_copy is used internally to copy the vector metadata, as well as the
 * head node of the vector during updates and inserts. We don't take a copy of
 * the tail since we won't know if we need that until we start trying to
 * update.
 */
PersistentVector *pvec_copy(PersistentVector *vec) {
    PersistentVector *copy = NULL;
    // We don't just use pvec_new since we are going to override everything in
    // it anyway.
    if((copy = (PersistentVector*)malloc(sizeof(PersistentVector))) == NULL) {
        perror("malloc");
        goto pvec_copy_return;
    }
    copy->depth = vec->depth;
    copy->length = vec->length;
    copy->tail_length = vec->tail_length;
    if((copy->head = pvec_node_copy(vec->head)) == NULL) {
        perror("pvec_node_copy");
        free(copy);
        copy = NULL;
        goto pvec_copy_return;
    }
    // yo, we don't want to copy this automatically
    // only copy it if we need to
    copy->tail = vec->tail;
pvec_copy_return:
    return copy;
}

/**
 * This function is used to take a tail node and append it to the trie in its
 * correct location. We do this by traversing the trie down to the location of
 * the node containing the tail_offset. The tail offset is
 * vector->length - vector->tail_length. This function also handles root
 * the creation of a new root node on root overflows.
 */
void pvec_append_tail(PersistentVector *vec) {
    uint64_t tail_offset = vec->length - vec->tail_length;
    uint64_t shift = BITS * (vec->depth + 1);
    uint64_t max_size = 1 << (5 * shift); //TODO(rossdylan) This needs to be verified
    
    // Check for root overflow, and if so expand the trie by 1 level
    if(vec->length == max_size) {
        PVecNode *new_root = NULL;
        if((new_root = pvec_node_new()) == NULL) {
            perror("pvec_node_new");
            return; // goto pvec_append_tail_return;
        }
        new_root->children[0] = vec->head;
        vec->head = new_root;
        vec->depth++;
        shift = BITS * (vec->depth + 1);
    }
    
    // Set up our trie traversal. We subtract BITS from level each time
    // in order to get all the way down to the level above where we want to
    // insert this tail node.
    PVecNode *cur = NULL;
    PVecNode *prev = vec->head;
    uint64_t index = 0;
    uint64_t key = tail_offset;
    for(uint64_t level = shift; level > 0; level -= BITS) {
        index = (key >> level) & MASK;
        cur = prev->children[index];
        // We are at the end of the tree time to insert our tail node
        if(cur == NULL && level - BITS == 0) {
            prev->children[index] = vec->tail;
            break;
        }
        // Found a NULL node on our way down to the bottom
        if(cur == NULL) {
            if((prev->children[index] = pvec_node_new()) == NULL) {
                perror("pvec_node_new");
                //TODO malloc failed somwhere, clean our shit up
                goto pvec_append_tail_return;
            }
            cur = prev->children[index];
            prev = cur;
            continue;
        }
        prev = cur;
    }
    // Make our new tail
    if((vec->tail = pvec_node_new()) == NULL) {
        perror("pvec_new_node");
        goto pvec_append_tail_return;
    }
    vec->tail_length = 0;
pvec_append_tail_return:
    return;
}


/**
 * This is the big one, pvec_set is used to add and update items. It does some
 * simple bounds checking. It also has all the code for putting stuff into the
 * tail of the vector, or going through the tree to find where stuff is.
 */
PersistentVector *pvec_set(PersistentVector *vec, uint64_t key, void *data, bool insert) {
    uint64_t shift = BITS * (vec->depth + 1);
    uint64_t tail_offset = vec->length - vec->tail_length;
    PersistentVector *copy = NULL;
    
    // Check our bounds for non-insert sets
    if(key > vec->length && !insert) {
        goto pvec_set_return;
    }
    // Create a copy of the root node from which we will base our insert/update
    // operations off of
    if((copy = pvec_copy(vec)) == NULL) {
        perror("pvec_copy");
        goto pvec_set_return;
    }
    // Is this an update?
    if(!insert) {
        // fuck yah it is
        //is it in the tail?
        if(key < tail_offset) {
            // nooooope
            // we just had to do this the hard way,
            // lets go find our node in the tree
            PVecNode *cur = NULL;
            PVecNode *prev = copy->head;
            uint64_t index = 0;
            for(uint64_t level = shift; level > 0; level -= BITS) {
                index = (key >> level) & MASK;
                cur = prev->children[index];
                // We are at the end of the tree time to insert our tail node
                // Found a NULL node on our way down to the bottom
                if(cur == NULL) {
                    // fuck what am I doing here?
                    // This shoulnd't happen
                    goto pvec_set_return;
                }
                prev = cur;
            }
            prev->elements[key & MASK] = data;
            goto pvec_set_return;
        }
        else {
            // Copy the tail from our original vector
            if((copy->tail = pvec_node_copy(vec->tail)) == NULL) {
                perror("pvec_node_copy");
                free(copy->head);
                free(copy);
                copy = NULL;
                goto pvec_set_return;
            }
            // If we are just updating we can just change it in the tail
            copy->tail->elements[key - tail_offset] = data;
            goto pvec_set_return;
        }
    }
    else {
        // Inserting a new item
        if(copy->tail_length == WIDTH) {
            // no more space :(
            pvec_append_tail(copy);
            tail_offset = copy->length - copy->tail_length;
        }
        copy->tail->elements[key - tail_offset] = data;
        copy->length++;
        copy->tail_length++;
        goto pvec_set_return;
    }
pvec_set_return:
    return copy;
}


/**
 * The following implementations are a part of the externally available
 * PersistentVector API. These functions might change, but their api should
 * stay the same.
 */


/**
 * Create a new PersistentVector which starts off empty.
 * This function will malloc the memory needed, and also initialie all the
 * values to their correct defaults.
 */
PersistentVector *pvec_new(void) {
    PersistentVector *vec = NULL;
    if((vec = (PersistentVector*)malloc(sizeof(PersistentVector))) == NULL) {
        perror("malloc");
        goto pvec_new_return;
    }
    if((vec->head = pvec_node_new()) == NULL) {
        perror("pvec_node_new");
        free(vec);
        vec = NULL;
        goto pvec_new_return;
    }
    if((vec->tail = pvec_node_new()) == NULL) {
        perror("pvec_node_new");
        free(vec);
        vec = NULL;
        goto pvec_new_return;
    }
    vec->depth = 0;
    vec->length = 0;
    vec->tail_length = 0;
pvec_new_return:
    return vec;
}


/**
 * A wrapper function around pvec_set that appends a single value to the end of
 * the vector.
 */
PersistentVector *pvec_cons(PersistentVector *vec, void *data) {
    return pvec_set(vec, vec->length, data, true);
}

/**
 * A Wrapper function around pvec_set that updates the value of an existing
 * key in a vector
 */
PersistentVector *pvec_assoc(PersistentVector *vec, uint64_t key, void *data) {
    return pvec_set(vec, key, data, false);
}

/**
 * Retreive a value from the given index in the vector.
 * This means we either grab it from the tail, or we traverse the backing
 * trie structure in order to find it.
 */
void *pvec_nth(PersistentVector *vec, uint64_t key) {
    uint64_t shift = BITS * (vec->depth + 1);
    PVecNode *next = NULL;
    PVecNode *prev = vec->head;
    uint64_t index = 0;
    uint64_t tail_offset = vec->length - vec->tail_length;
    void *data = NULL;
    if(key < tail_offset) {
        for(uint64_t level = shift; level > 0; level -= BITS) {
            index = (key >> level) & MASK;
            next = prev->children[index];
            prev = next;
        }
        data = prev->elements[key & MASK];
    }
    else {
        data = vec->tail->elements[key - tail_offset];
    }
    return data;
}

/**
 * Test functions. The following functions are used in order to test the
 * functionality of the Persistent Vector implementation.
 */


/**
 * Turn a Persistent Vector into an array. Useful to verify that all data in
 * the vector is still accessable after being inserted.
 */
void **pvec_to_array(PersistentVector *vec) {
    void **array = NULL;
    if((array = (void**)calloc(sizeof(void *), vec->length)) == NULL) {
        perror("calloc");
        goto pvec_to_array_return;
    }
    for(uint64_t i = 0; i < vec->length; i++) {
        array[i] = pvec_nth(vec, i);
    }
pvec_to_array_return:
    return array;
}

/**
 * Print out the contents of a Persistent Vector
 */
void print_pvec(PersistentVector *vec) {
    void **array = pvec_to_array(vec);
    printf("[");
    for(uint64_t i = 0; i < vec->length; i++) {
        printf("%s, ", (char *)array[i]);
    }
    printf("]\n");
    free(array);
}

int TestPVec(int argc, char **argv) {
    printf("PVecNode size: %lu\n", sizeof(PVecNode));
    PersistentVector **stages = NULL;
    if((stages = (PersistentVector**)calloc(sizeof(PersistentVector), argc + 1)) == NULL) {
        perror("calloc");
        return 1;
    }
    stages[0] = pvec_new();
    for(uint64_t i = 1; i < argc+1; i++) {
        stages[i] = pvec_cons(stages[i-1], argv[i-1]);
    }
    
    for(uint64_t i = 0; i < argc+1; i++) {
        print_pvec(stages[i]);
        printf("\n==============================\n");
    }
    return 0;
}