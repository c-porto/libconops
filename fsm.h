#ifndef FSM_H_
#define FSM_H_

#include <stdint.h>

typedef struct fsm_s cp_fsm_t;
typedef struct event_s cp_event_t;

typedef int8_t ( *transition_handler_t )( cp_fsm_t *, cp_event_t * );
typedef int8_t ( *event_handler_t )( cp_fsm_t *, cp_event_t * );

struct fsm_s
{
    uint16_t curr_state;
    uint16_t n_states;
    uint16_t max_event_id;
    event_handler_t ev_handler;
    void * transition_table;
};

struct event_s
{
    uint16_t ev_id;
    uint16_t state_to_transition;
    void * ev_data;
};

static inline int8_t cp_fsm_set_ev_handler( cp_fsm_t * fsm,
                                            event_handler_t * ev )
{
    if( !fsm || !ev )
        return -1;

    fsm->ev_handler = *ev;

    return 0;
}

int8_t cp_fsm_conf( cp_fsm_t * fsm,
                    void * table,
                    uint16_t n_states,
                    uint16_t max_ev_id,
                    uint16_t init_state );

#endif
