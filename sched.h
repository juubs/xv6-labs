/**
  @file sched.h
  @brief Definitions for new scheduling policies.
*/

#ifndef SCHED_H
#define SCHED_H

/**
  @brief Different scheduling policy options.
*/
enum schedpolicy_lab3 {
  SCHED_RR,   /**< Round Robin scheduling policy.*/
  SCHED_FIFO, /**< First in, First out scheduling policy.*/
  SCHED_OTHER /**< Default scheduling policy if none is specifically applied to a process.*/
};

#endif
