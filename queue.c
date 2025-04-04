#include "queue.h"
#include "tile_game.h"
#include <stdlib.h>

void enqueue(struct queue *q, struct game_state state) 
{
    size_t serialized = serialize(state);
    insert_at_tail(&q->data, serialized);
}

struct game_state dequeue(struct queue *q) 
{ 
    size_t val = remove_from_head(&q->data);
    return deserialize(val); 
}

bool is_goal(struct game_state *state) 
{
    int expected = 1;
    for (int row = 0; row < 4; row++) 
    {
        for (int col = 0; col < 4; col++) 
        {
            if (row == 3 && col == 3) 
            {
                if (state->tiles[row][col] != 0) 
                {    
                    return false;
                }
            } 
            else 
            {
                if (state->tiles[row][col] != expected++) 
                {
                    return false;
                }
            }
        }
    }
    return true;
}

#define MAX_STATES 100000

int number_of_moves(struct game_state start) 
{ 
    struct queue q = {.data = {.head = NULL}};
    size_t visited[MAX_STATES] = {0};
    size_t visited_count = 0;

    enqueue(&q, start);
    visited[visited_count++] = serialize(start);

    while(q.data.head) 
    {
        struct game_state curr = dequeue(&q);

        if(is_goal(&curr)) 
        {
            free_list(q.data);
            return curr.num_steps;
        }

        void (*moves[4])(struct game_state *) = {
            move_up, move_down, move_left, move_right
        };

        for(int i = 0; i < 4; i++) 
        {
            struct game_state next = curr;
            moves[i](&next);

            // skip if nothign happens
            if(serialize(next) == serialize(curr)) 
            {
                continue;
            }

            next.num_steps++;

            size_t encoded = serialize(next);
            bool already_seen = false;
            for(size_t j = 0; j < visited_count; j++) 
            {
                if(visited[j] == encoded) 
                {
                    already_seen = true;
                    break;
                }
            }

            if(!already_seen) 
            {
                enqueue(&q, next);
                if(visited_count < MAX_STATES) 
                {
                    visited[visited_count++] = encoded;
                }
            }
        }
    }

    free_list(q.data);
    return -1;
}
