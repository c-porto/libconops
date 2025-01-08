#include <stdint.h>
#include <stdio.h>

#include "conops.h"

enum states {
  PRINT1,
  PRINT2,
  NUM_STATES,
};

int print1(struct conops_fsm *fsm, const struct conops_event *ev,
           const uint16_t transition_to) {
  printf("CURRENT STATE: %u! GOING TO STATE -> PRINT1!\n", fsm->state);
  fsm->state = PRINT1;

  return 0;
}
int print2(struct conops_fsm *fsm, const struct conops_event *ev,
           const uint16_t transition_to) {
  printf("CURRENT STATE: %u! GOING TO STATE -> PRINT2!\n", fsm->state);
  fsm->state = PRINT2;

  return 0;
}

static transition_handler_t transition_table[NUM_STATES][NUM_STATES] = {
    {NULL, print2},
    {print1, NULL},
};

static void call(const struct conops_fsm *ctx, const struct conops_event *ev) {
  uint8_t retries = 1;

  while (retries--) {
    printf("Callback for Ev: \n\tName: %s\n\tid: %u\n\tsrc:%u\n\tCurrent "
           "State: %u\n",
           ev->ev_name, ev->ev_id, ev->src, ctx->state);
  }
}

static const struct conops_event p1 = {
    .src = 0U,
    .ev_id = 1U,
    .ev_name = "TESTE1",
    .callback = call,
};

static const struct conops_event p2 = {
    .src = 0U,
    .ev_id = 0U,
    .ev_name = "TESTE2",
    .callback = call,
};

static int32_t mapper(const struct conops_fsm *ctx,
                      const struct conops_event *ev) {
  switch (ev->ev_id) {
  case 0:
    return PRINT2;
  case 1:
    return PRINT1;
  }

  return -1;
}

int main() {
  struct conops_fsm state_machine;

  conops_fsm_init(&state_machine, &transition_table, NUM_STATES, 2U, PRINT2);
  conops_register_mapper(&state_machine, mapper);

  while (1) {
    int ev = 0;
    int err = 0;
    printf("Ev Number: ");

    scanf("%d", &ev);

    switch (ev) {
    case 1:
      if ((err = conops_fsm_process_event(&state_machine, &p1)) != 0) {
        printf("Could not process event 1! Err: %d\n", err);
      }
      break;
    case 2:
      if ((err = conops_fsm_process_event(&state_machine, &p2)) != 0) {
        printf("Could not process event 2! Err: %d\n", err);
      }
      break;
    default:
      printf("Invalid event\n");
      break;
    }
  }
  return 0;
}
