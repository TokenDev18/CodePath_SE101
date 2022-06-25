#include <stdio.h>
#include <math.h>
#include "oslabs.h"

#define QUEUEMAX 10
#define MAPMAX 10
#define TABLEMAX 10
#define POOLMAX 10
#define REFERENCEMAX 20
#define MAX( a, b ) ( ( a > b) ? a : b ) 
#define MIN( a, b ) ( ( a > b) ? b : a ) 

struct MEMORY_BLOCK NULLBLOCK() {
    struct MEMORY_BLOCK nullblock;
    nullblock.start_address = 0; 
    nullblock.end_address = 0; 
    nullblock.segment_size = 0; 
    nullblock.process_id = 0; 
    return nullblock;
};

struct MEMORY_BLOCK best_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    //definind structs
    struct MEMORY_BLOCK nullblock = NULLBLOCK();
    //defining variables for the free memory map entry
    int end_address, segment_size, free_block, pos;
    int mcnt = *map_cnt; //storing map count
    //loop through memory map array
    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].process_id == 0){ //checking for free memory block
            free_block = i; //storing the index of the free memory block
            pos = free_block + 1;//insert position for free space of memory
            if(memory_map[free_block].segment_size == request_size){
                memory_map[free_block].process_id = process_id;
                return memory_map[free_block];
            }else if(memory_map[i].segment_size > request_size){
                segment_size = memory_map[free_block].segment_size; //storing original segment size
                end_address = memory_map[free_block].end_address; //storing orginal end_address

                memory_map[free_block].end_address = request_size - 1;
                memory_map[free_block].segment_size = request_size;
                memory_map[free_block].process_id = process_id;

                mcnt++;

                for(int i = mcnt-1; i >= pos; i--){
                    memory_map[i] = memory_map[i-1];
                }
                memory_map[pos-1].start_address = request_size;
                memory_map[pos-1].end_address = memory_map[pos-1].start_address + memory_map[pos-1].segment_size -1 ;
                memory_map[pos-1].segment_size = segment_size - request_size;
                memory_map[pos-1].process_id = 0;
                (*map_cnt)++;
                return memory_map[free_block];
            }else {
                return nullblock;
            }
        }else {
            continue;
        }
    }
} 
struct MEMORY_BLOCK first_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    struct MEMORY_BLOCK nullblock = NULLBLOCK();
    return nullblock;
} 
struct MEMORY_BLOCK worst_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id){
    struct MEMORY_BLOCK nullblock = NULLBLOCK();
    int mcnt = *map_cnt; //storing map count
    int free_block, pos, segment_size;    
    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].segment_size == request_size && memory_map[i].process_id == 0){
            memory_map[i].process_id = process_id;
            return memory_map[i];
        }
        if(memory_map[i].segment_size > request_size && memory_map[i].process_id == 0){
            free_block = i;
            pos = free_block + 1;

            segment_size = memory_map[free_block].segment_size; //storing original segment size

            memory_map[free_block].end_address = request_size - 1;
            memory_map[free_block].segment_size = request_size;
            memory_map[free_block].process_id = process_id;

            mcnt++;

            for(int i = mcnt-1; i >= free_block; i--){
                memory_map[i] = memory_map[i-1];
            }
            memory_map[pos-1].start_address = request_size;
            memory_map[pos-1].end_address = memory_map[pos-1].start_address + memory_map[pos-1].segment_size -1 ;
            memory_map[pos-1].segment_size = segment_size - request_size;
            memory_map[pos-1].process_id = 0;
            (*map_cnt)++;
            return memory_map[free_block];
        }
    }
    return nullblock;
}
struct MEMORY_BLOCK next_fit_allocate(int request_size, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt, int process_id, int last_address){
    struct MEMORY_BLOCK nullblock = NULLBLOCK();
    return nullblock;
}
void release_memory(struct MEMORY_BLOCK freed_block, struct MEMORY_BLOCK memory_map[MAPMAX],int *map_cnt){
    int free_block_pos; 
    int mcnt = *map_cnt;

    for(int i = 0; i < *map_cnt; i++){
        if(memory_map[i].process_id == freed_block.process_id){
            memory_map[i].process_id = 0;
            free_block_pos = i;
            break;
        }
    }
    if(free_block_pos > 0 && free_block_pos + 1 < mcnt){
        if(memory_map[free_block_pos-1].process_id == 0){
            memory_map[free_block_pos-1].segment_size = memory_map[free_block_pos].segment_size + memory_map[free_block_pos-1].segment_size;
            memory_map[free_block_pos-1].end_address = memory_map[free_block_pos].start_address + memory_map[free_block_pos].segment_size - 1;
            (*map_cnt)--;

            for(int i = free_block_pos; i < *map_cnt; i++){
                memory_map[i] = memory_map[i+1];   
            } 
            return;
        }
        if(memory_map[free_block_pos].process_id == 0){
            memory_map[free_block_pos-1].segment_size = memory_map[free_block_pos].segment_size + memory_map[free_block_pos-1].segment_size;
            memory_map[free_block_pos-1].end_address = memory_map[free_block_pos].start_address + memory_map[free_block_pos].segment_size - 1;
            (*map_cnt)--;
            for(int i = free_block_pos; i < *map_cnt; i++){
                memory_map[i] = memory_map[i+1];   
            }
            return;
        }
        if(memory_map[free_block_pos + 1].process_id == 0){
            memory_map[free_block_pos+1].segment_size = memory_map[free_block_pos].segment_size + memory_map[free_block_pos+1].segment_size;
            memory_map[free_block_pos+1].process_id = memory_map[free_block_pos].process_id;
            memory_map[free_block_pos+1].start_address = memory_map[free_block_pos].start_address;
            (*map_cnt)--;

            for(int i = free_block_pos; i < *map_cnt; i++){
                memory_map[i] = memory_map[i+1];   
            }
            return;
        }
    } else if(free_block_pos == 0){
        memory_map[free_block_pos+1].segment_size = memory_map[free_block_pos].segment_size + memory_map[free_block_pos+1].segment_size;
        memory_map[free_block_pos+1].process_id = memory_map[free_block_pos].process_id;
        memory_map[free_block_pos+1].start_address = memory_map[free_block_pos].start_address;
        (*map_cnt)--;

        for(int i = free_block_pos; i < *map_cnt; i++){
            memory_map[i] = memory_map[i+1];
        }
        return; 
    }else {
        memory_map[free_block_pos-1].segment_size = memory_map[free_block_pos].segment_size + memory_map[free_block_pos-1].segment_size;
        memory_map[free_block_pos-1].end_address = memory_map[free_block_pos].start_address + memory_map[free_block_pos].segment_size - 1;
        (*map_cnt)--;

        for(int i = free_block_pos; i < *map_cnt; i++){
            memory_map[i] = memory_map[i+1];   
        }
        return; 
    }
}      