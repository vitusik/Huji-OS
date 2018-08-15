#ifndef _THREAD_H
#define _THREAD_H

#include <setjmp.h>
#include "uthreads.h"

// list of all thread's states:
#define RUNNING 1
#define READY 2
#define BLOCKED 3



// User Thread Class, everything is pretty straight forward, we have getters and setters
class Thread
{
    public:
        Thread();
        ~Thread();
        int get_state() const;
        const char* get_stack();
		sigjmp_buf& get_env();
		void set_state(int new_state);
		int save_env();
		void load_env();
		void qntm_inc();
		int get_qntms() const;
        void set_wait_for_sync(bool i);
        bool get_wait_for_sync() const ;
        void set_being_blocked(bool i);
        bool get_being_blocked() const ;

    private:
        char stack[STACK_SIZE];
        int state;
        int total_qntom;
        sigjmp_buf env;
        bool waiting_for_sync;
        bool being_blocked;

};

#endif
