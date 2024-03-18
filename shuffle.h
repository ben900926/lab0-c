#ifndef LAB0_SHUFFLE_H
#define LAB0_SHUFFLE_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "list.h"


/**
 * q_shuffle() - Shuffle the queue using Fisher-Yates Shuffle
 * @head: header of queue
 */
void q_shuffle(struct list_head *head);

#endif