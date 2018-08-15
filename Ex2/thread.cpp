#include "thread.h"
#include <signal.h>
#include <stdlib.h>


Thread::Thread()
{
    being_blocked = false;
    waiting_for_sync = false;
    total_qntom = 0;
    state = READY;
}

Thread::~Thread()
{
}

int Thread::get_state() const
{
    return state;
}

const char* Thread::get_stack()
{
    return stack;
}

sigjmp_buf& Thread::get_env()
{
    return env;
}

void Thread::set_state(int new_state)
{
    state = new_state;
}

int Thread::save_env()
{
    //before saving the env of a thread, virtual timer signal should be unblocked
    //all the times that this func is called is inside critical code segments
    sigdelset(&env->__saved_mask, SIGVTALRM);
    sigprocmask(SIG_SETMASK, &env->__saved_mask, NULL);
    int ret_val = sigsetjmp(env, 1);

    return ret_val;
}

void Thread::load_env()
{
    siglongjmp(env, 1);
}

void Thread::qntm_inc()
{
    total_qntom++;
}

int Thread::get_qntms() const
{
    return total_qntom;
}

void Thread::set_wait_for_sync(bool i)
{
    waiting_for_sync = i;
}

bool Thread::get_wait_for_sync() const
{
    return waiting_for_sync;
}

void Thread::set_being_blocked(bool i)
{
    being_blocked = i;
}

bool Thread::get_being_blocked() const
{
    return being_blocked;
}

