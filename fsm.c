#include "fsm.h"

static int8_t default_ev_handler(cp_fsm_t *fsm, cp_event_t *ev)
{
    if (!fsm || !ev)
        return -1;

    if (!fsm->transition_table)
        return -2;

    if (ev->ev_id > fsm->max_event_id || ev->state_to_transition >= fsm->n_states)
        return -3;

    transition_handler_t (*table)[fsm->n_states] = fsm->transition_table;

    transition_handler_t handler = table[fsm->curr_state][ev->state_to_transition];
    
    if (!handler)
        return -4;

    return handler(fsm, ev);
}

int8_t cp_fsm_conf(cp_fsm_t *fsm, void *table, uint16_t n_states, uint16_t max_ev_id, uint16_t init_state)
{
    if (!fsm || !table)
        return -1;

    fsm->n_states = n_states;
    fsm->max_event_id = max_ev_id;
    fsm->transition_table = table;
    fsm->curr_state = init_state;
    fsm->ev_handler = default_ev_handler;

    return 0;
}
