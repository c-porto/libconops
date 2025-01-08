#include <string.h>

#include "conops.h"

static inline state_change_callback_t
conops_get_exit_function(const struct conops_fsm *fsm, uint16_t state) {
  if (fsm->state_exit != NULL)
    return fsm->state_exit[state];

  return NULL;
}

static inline state_change_callback_t
conops_get_enter_function(const struct conops_fsm *fsm, uint16_t state) {
  if (fsm->state_enter != NULL)
    return fsm->state_enter[state];

  return NULL;
}

int conops_fsm_init(struct conops_fsm *fsm, const void *transition_table,
                    const uint16_t n_states, const uint16_t max_ev_id,
                    const uint16_t init_state) {
  if (!fsm || !transition_table)
    return -ERRNO_CONOPS_INVALID_ARG;

  if (init_state >= n_states)
    return -ERRNO_CONOPS_INVALID_STATE;

  fsm->ev_mapper_fn = NULL;
  fsm->user_data = NULL;
  fsm->state_enter = NULL;
  fsm->state_exit = NULL;
  fsm->transition_table = transition_table;
  fsm->conf.n_states = n_states;
  fsm->conf.max_ev_id = (max_ev_id != 0) ? max_ev_id : CONOPS_DEFAULT_MAX_EV_ID;
  fsm->conf.init_state = init_state;
  fsm->state = init_state;

  return 0;
}

int conops_fsm_process_event(struct conops_fsm *fsm,
                             const struct conops_event *ev) {
  state_change_callback_t change_callback = NULL;
  int err = 0;

  if (!fsm || !ev)
    return -ERRNO_CONOPS_INVALID_ARG;

  if (!fsm->transition_table)
    return -ERRNO_CONOPS_NO_TRANSITION_TABLE;

  if (ev->ev_id > fsm->conf.max_ev_id)
    return -ERRNO_CONOPS_INVALID_EV_ID;

  if (!fsm->ev_mapper_fn)
    return -ERRNO_CONOPS_NO_MAPPER_FUNCTION;

  int32_t transition_to = fsm->ev_mapper_fn(fsm, ev);

  if (transition_to < 0)
    return -ERRNO_CONOPS_MAPPING_ERROR;

  const transition_handler_t(*table)[fsm->conf.n_states] =
      fsm->transition_table;

  transition_handler_t handler = table[fsm->state][transition_to];

  if (transition_to == fsm->state)
    goto ev_callback_run;

  if (handler) {
    if ((change_callback = conops_get_exit_function(fsm, fsm->state)) != NULL)
      change_callback(fsm, fsm->state);

    err = handler(fsm, ev, transition_to);

    if (err < 0)
      return err;

    if ((change_callback = conops_get_enter_function(fsm, fsm->state)) != NULL)
      change_callback(fsm, fsm->state);
  }

ev_callback_run:
  if (ev->callback)
    ev->callback(fsm, ev);

  fsm->last_valid_ev_id = ev->ev_id;

  return 0;
}

int conops_register_mapper(struct conops_fsm *fsm, event_mapper_t mapper) {
  if (!fsm || !mapper)
    return -ERRNO_CONOPS_INVALID_ARG;

  fsm->ev_mapper_fn = mapper;

  return 0;
}

int conops_register_fsm_exit_callbacks(
    struct conops_fsm *fsm, const state_change_callback_t *callback_array) {
  if (!fsm || !callback_array)
    return -ERRNO_CONOPS_INVALID_ARG;

  fsm->state_exit = callback_array;

  return 0;
}

int conops_register_fsm_enter_callbacks(
    struct conops_fsm *fsm, const state_change_callback_t *callback_array) {
  if (!fsm || !callback_array)
    return -ERRNO_CONOPS_INVALID_ARG;

  fsm->state_enter = callback_array;

  return 0;
}

int conops_register_fsm_user_data(struct conops_fsm *fsm, void *user_data) {
  if (!fsm || !user_data)
    return -ERRNO_CONOPS_INVALID_ARG;

  fsm->user_data = user_data;

  return 0;
}

int conops_serialize_event(const struct conops_event *ev, uint8_t *buffer,
                           const uint16_t max_size,
                           const uint16_t ignore_callback_src) {
  if (!ev || !buffer)
    return -ERRNO_CONOPS_INVALID_ARG;

  if (max_size <= sizeof(*ev))
    return -ERRNO_CONOPS_INSUFFICIENT_BUFFER_SIZE;

  buffer[0] = (ev->src >> 8U) & 0xFF;
  buffer[1] = ev->src & 0xFF;
  buffer[2] = (ev->ev_id >> 8U) & 0xFF;
  buffer[3] = ev->ev_id & 0xFF;

  memcpy(&buffer[4], ev->ev_name, sizeof(ev->ev_name));

  if ((ignore_callback_src != ev->src) && (ev->callback)) {
    memcpy(&buffer[4 + sizeof(ev->ev_name)], &ev->callback,
           sizeof(ev->callback));
  } else {
    memset(&buffer[4 + sizeof(ev->ev_name)], 0U, sizeof(ev->callback));
  }

  return 0;
}

int conops_deserialize_event(struct conops_event *ev, const uint8_t *buffer,
                             const uint16_t size,
                             const uint16_t ignore_callback_src) {
  if (!ev || !buffer)
    return -ERRNO_CONOPS_INVALID_ARG;

  if (size <= sizeof(*ev) - sizeof(ev->callback))
    return -ERRNO_CONOPS_INSUFFICIENT_BUFFER_SIZE;

  ev->src = (buffer[0] << 8U) | buffer[1];

  ev->ev_id = (buffer[2] << 8U) | buffer[3];

  memcpy(ev->ev_name, &buffer[4], sizeof(ev->ev_name));

  if (ignore_callback_src != ev->src) {
    memcpy(&ev->callback, &buffer[4 + sizeof(ev->ev_name)],
           sizeof(ev->callback));
  } else {
    ev->callback = NULL;
  }

  return 0;
}
