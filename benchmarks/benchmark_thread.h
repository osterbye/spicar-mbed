/* SpiCar mbed platform
 * Copyright (c) 2017 Spiri Mobility Solutions
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef BENCHMARK_THREAD_H_
#define BENCHMARK_THREAD_H_

//#define BENCHMARK_THREAD

#ifdef BENCHMARK_THREAD
//#include "mbed.h"

static void print_thread_data(Thread *t, Serial *pc) {
    pc->printf("Thread state: %d,\tPriority: %d\r\n", t->get_state(), t->get_priority());
    pc->printf("\tStack size:\t%d bytes\r\n\tMax. use:\t%d bytes", t->stack_size(), t->max_stack());
    pc->printf("\r\n");
}
#else
static void print_thread_data(Thread *t, Serial *pc) {}
#endif // BENCHMARK_THREAD

#endif // BENCHMARK_THREAD_H_
