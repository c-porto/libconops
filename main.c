#include <stdint.h>
#include <stdio.h>

#include "fsm.h"

enum states
{
    PRINT1,
    PRINT2,
    NUM_STATES,
};

int8_t print1( cp_fsm_t * fsm, cp_event_t * ev )
{
    printf( "CURRENT STATE: %u! GOING TO STATE -> PRINT1!\n", fsm->curr_state );
    fsm->curr_state = PRINT1;

    return 0;
}
int8_t print2( cp_fsm_t * fsm, cp_event_t * ev )
{
    printf( "CURRENT STATE: %u! GOING TO STATE -> PRINT2!\n", fsm->curr_state );
    fsm->curr_state = PRINT2;

    return 0;
}

static transition_handler_t transition_table[ NUM_STATES ][ NUM_STATES ] = {
    { NULL, print2 },
    { print1, NULL },
};

static const cp_event_t p1 = {
    .state_to_transition = PRINT1,
    .ev_id = 1U,
    .ev_data = NULL,
};

static const cp_event_t p2 = {
    .state_to_transition = PRINT2,
    .ev_id = 1U,
    .ev_data = NULL,
};

int main()
{
    cp_fsm_t state_machine;

    cp_fsm_conf( &state_machine, &transition_table, NUM_STATES, 2U, PRINT2 );

    while( 1 )
    {
        int ev = 0;
        printf( "Ev Number: " );

        scanf( "%d", &ev );

        switch( ev )
        {
            case 1:
                if( state_machine.ev_handler( &state_machine, &p1 ) != 0 )
                {
                    printf( "Could not process event 1!\n" );
                }
                break;
            case 2:
                if( state_machine.ev_handler( &state_machine, &p2 ) != 0 )
                {
                    printf( "Could not process event 2!\n" );
                }
                break;
            default:
                printf( "Invalid event" );
                break;
        }
    }
    return 0;
}
