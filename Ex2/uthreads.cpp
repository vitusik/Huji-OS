#include "uthreads.h"
#include "thread.h"
#include <iostream>
#include <sys/time.h>
#include <vector>
#include <map>
#include <signal.h>
#include <stdlib.h>


#define SYS_ERR true
#define LIB_ERR false
#define FAILURE_VAL -1
#define MICRO_TO_SECOND 1000000
using namespace std;

#ifdef __x86_64__
/* code for 64 bit Intel arch */
typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
		"rol    $0x11,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
		"rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#endif

// global variables for the library
static struct itimerval timer;
static struct sigaction sig_a;
// the key to each thread is its pid
static map<int, Thread*> all_threads;
static vector<int> ready_list;
static vector<int> blocked_list;
// list of pairs, the first int is the pid of the thread that's called the sync
// func, the second int is the the arg in that sync func
static vector<pair<int, int> > synced_list;
static int cur_running;
static int total_qntm;

/**
 * func that find the next id for a thread
 * @return valid thread id
 */
int get_next_pid()
{
    for (int id = 1; id < MAX_THREAD_NUM; id++)
    {
        // the func always will return valid id, because before its called,
        // there is a check for valid amount of threads
		if (all_threads.find(id) == all_threads.end())
        {
			return id;
		}
	}
    // this return statement was added to avoid warning from the compiler
    return FAILURE_VAL;
}

/**
 * func that clears the map that holds all the threads
 */
void clear_map()
{
    Thread* tmp;
    map<int, Thread*>::iterator it;
    for(it = all_threads.begin(); it != all_threads.end(); it++)
    {
        tmp = it->second;
        delete tmp;
    }
    all_threads.clear();
}

/**
 * function that outputs error to stderr
 * @param msg the error that has occurred
 * @param is_system bool balue that represents whether the error is a system
 *  error, or a library error
 */
void error_handler(string msg, bool is_system)
{
    if(is_system)
    {
        clear_map();
        cerr<<"system error: " <<msg<<endl;
        exit(1);
    }
    else
    {
        cerr<<"thread library error: " <<msg<<endl;
    }
}

/**
 * function that initializes the signal mask for the library
 */
void init_mask()
{
    if (sigemptyset(&sig_a.sa_mask)) {
        error_handler("failed to empty signal mask", SYS_ERR);
        exit(1);
    }
    // adds virtual timer signal to the mask
    if (sigaddset(&sig_a.sa_mask, SIGVTALRM)) {
        error_handler("failed to add signal to mask", SYS_ERR);
        exit(1);
    }
    sig_a.sa_flags = 0;
    // initializess a sigaction struct fro SIGVTALRM
    if (sigaction(SIGVTALRM, &sig_a, NULL)) {
        error_handler("failed to create sigaction", SYS_ERR);
        exit(1);
    }
}

/**
 * function that blocks SIGVTALARM
 */
void sig_block()
{
    if(sigprocmask(SIG_BLOCK, &sig_a.sa_mask, NULL))
    {
        error_handler("failed to block signal mask", SYS_ERR);
        exit(1);
    }
}

/**
 * function that unblocks SIGVTALARM
 */
void sig_unblock()
{
    if (sigprocmask(SIG_UNBLOCK, &sig_a.sa_mask, NULL))
    {
        error_handler("failed to unblock signals", SYS_ERR);
        exit(1);
    }
}

/**
 * function that removes thread with id pid, from vector v
 * @param pid the id of the thread we want to remove
 * @param v the vector from which the thread will be deleted from
 */
void thread_remove(int pid, vector<int>& v)
{
    vector<int>::iterator it;
    for(it = v.begin(); it != v.end(); it++)
    {
        if(*it == pid)
        {
            v.erase(it);
            return;
        }
    }
}

/**
 * function that removes thread with id pid from the synced_list vector
 * @param pid the id of the thread we wish to delete
 */
void remove_from_sync_list(int pid)
{
    // I've tried to merge this func with the previous one, but I was'nt succesfull
    vector<pair<int, int> >::iterator it;
    for(it = synced_list.begin(); it != synced_list.end(); it++)
    {
        if(it->first == pid)
        {
            synced_list.erase(it);
            return;
        }
    }
}

/**
 * handler function fro the event of a thread has finished its run
 * (or just its running quanta) or have been blocked, that checks if there are
 * threads with sync dependencies to a given thread, if so it removes them
 * @param pid the id of the thread that has finished its run
 */
void sync_unblock(int pid)
{
    if(synced_list.empty())
    {
        return;
    }
    vector<pair<int, int> >::iterator it;
    for(it = synced_list.begin(); it != synced_list.end(); it++)
    {
        if(it->second == pid)
        {
            Thread* finished = all_threads[it->first];
            // if the synced thread was also blocked it can't return to ready
            if(!finished->get_being_blocked())
            {
                finished->set_state(READY);
                ready_list.push_back(it->first);
            }
            finished->set_wait_for_sync(false);
            it = synced_list.erase(it);
            // after erase 'it' advances to the next element,
            // in case the 'it' is the list end, we need to break
            // otherwise will lead to segfault
            if(it == synced_list.end())
            {
                break;
            }
        }
    }

}

/**
 * function that runs the next thread in the ready list
 */
void run_next()
{
    cur_running = ready_list.front();
    ready_list.erase(ready_list.begin());

    all_threads[cur_running]->set_state(RUNNING);
    all_threads[cur_running]->qntm_inc();
    all_threads[cur_running]->load_env();
}

/**
 * function that sets the virtual timer
 */
void set_timer()
{
    // each time, when it time to set the timer, it means another qunta has
    // passed
    total_qntm++;
    if(setitimer(ITIMER_VIRTUAL, &timer, NULL) == -1)
    {
        error_handler("could not set timer", SYS_ERR);
        exit(1);
    }
}

/**
 * the signal handler for SIGVTALRM
 * @param signum the number that represents the signal
 */
void timer_handler(int signum)
{
    set_timer();
    // if the return value diff from 0, it means we got here from siglongjmp()
    if(all_threads[cur_running]->save_env())
    {
        return;
    }
    // unblocking possible synced threads
    sync_unblock(cur_running);
    all_threads[cur_running]->set_state(READY);
    ready_list.push_back(cur_running);
    run_next();
}

//----------------------------------------------------------------------------------------------------------------------
int uthread_init(int quantum_usecs)
{
    if(quantum_usecs <= 0)
    {
        error_handler("quantum_usecs must be positive number", LIB_ERR);
        return FAILURE_VAL;
    }
    sig_a.sa_handler = timer_handler;
    init_mask();

    total_qntm = 0;
    cur_running = 0;
    // the no throw variant of new, returns NULL in case of failure,
    // thus removing the need in try and catch blocks
    Thread* main_th = new (nothrow)Thread();
    if(main_th == NULL)
    {
        error_handler("failed to alloc mem for main thread", SYS_ERR);
        exit(1);
    }
    all_threads[0] = main_th;
    all_threads[0]-> set_state(RUNNING);
    all_threads[0]-> qntm_inc();

    timer.it_value.tv_sec = quantum_usecs / MICRO_TO_SECOND;
    timer.it_interval.tv_sec = quantum_usecs / MICRO_TO_SECOND;
    timer.it_value.tv_usec = quantum_usecs % MICRO_TO_SECOND;
    timer.it_interval.tv_usec = quantum_usecs % MICRO_TO_SECOND;
    set_timer();
    return 0;
}

int uthread_spawn(void (*f)(void))
{
    sig_block();
    if(all_threads.size() == MAX_THREAD_NUM)
    {
        error_handler("exceeds amount of threads", LIB_ERR);
        sig_unblock();
        return FAILURE_VAL;
    }
    int pid = get_next_pid();
    Thread* new_th = new (nothrow)Thread();
    if(new_th == NULL)
    {
        error_handler("failled to alloc mem for new thread", SYS_ERR);
        exit(1);
    }
    all_threads[pid] = new_th;
    ready_list.push_back(pid);
    address_t sp, pc;
    sp = (address_t) new_th->get_stack() + STACK_SIZE - sizeof(address_t);
    pc = (address_t)*f;
    sigjmp_buf& env = new_th->get_env();
    (env->__jmpbuf)[JB_SP] = translate_address(sp);
    (env->__jmpbuf)[JB_PC] = translate_address(pc);
    if(sigemptyset(&env->__saved_mask) == FAILURE_VAL)
    {
        delete new_th;
        clear_map();
        error_handler("failed to empty signal mask", SYS_ERR);
    }
    sig_unblock();
    return pid;
}

int uthread_terminate(int tid)
{
    sig_block();
    // the main thread ended its run
    if(tid == 0)
    {
        clear_map();
        exit(0);
    }
    if(all_threads.find(tid) == all_threads.end())
    {
        error_handler("trying to delete non-existing thread", LIB_ERR);
        sig_unblock();
        return FAILURE_VAL;
    }
    Thread* to_del = all_threads[tid];
    sync_unblock(tid);
    if(to_del->get_state() == BLOCKED)
    {
        remove_from_sync_list(tid);
        to_del->set_wait_for_sync(false);
        to_del->set_being_blocked(false);
        thread_remove(tid, blocked_list);
    }
    else
    {
        thread_remove(tid, ready_list);
    }
    all_threads.erase(tid);
    delete to_del;
    if(tid == cur_running)
    {
        set_timer();
        run_next();
    }
    sig_unblock();
    return 0;
}

int uthread_block(int tid)
{
    sig_block();
    if(tid == 0)
    {
        error_handler("cant block the main thread", LIB_ERR);
        sig_unblock();
        return FAILURE_VAL;
    }
    if(all_threads.find(tid) == all_threads.end())
    {
        error_handler("cant block non-existing thread", LIB_ERR);
        sig_unblock();
        return FAILURE_VAL;
    }

    Thread* to_block = all_threads[tid];
    if(to_block->get_state() != BLOCKED)
    {
        thread_remove(tid, ready_list);
        to_block->set_state(BLOCKED);
        blocked_list.push_back(tid);
        sync_unblock(tid);
        to_block->set_being_blocked(true);
        if(tid == cur_running)
        {
            if(to_block->save_env())
            {
                sig_unblock();
                return 0;
            }
            set_timer();
            run_next();
        }
    }
    // the thread is blocked because of sync dependency, but not because it
    // was blocked by other thread
    else if (to_block->get_state() == BLOCKED && to_block->get_wait_for_sync())
    {
        // thread that's being synced can be blocked by other thread
        if(!to_block->get_being_blocked())
        {
            to_block->set_being_blocked(true);
            blocked_list.push_back(tid);
        }
    }
    // there is one extra case that was'nt handled and its the case when a
    // thread was blocked by other thread and then calling sync on some thread,
    // however its impossible since it's blocked, therefore cant run and use
    // sync func
    sig_unblock();
    return 0;
}

int uthread_resume(int tid)
{
    sig_block();
    if(all_threads.find(tid) == all_threads.end())
    {
        error_handler("cant resume non-existing thread", LIB_ERR);
        sig_unblock();
        return FAILURE_VAL;
    }
    Thread* to_be_resumed = all_threads[tid];
    if(to_be_resumed->get_state() == BLOCKED)
    {
        if(to_be_resumed->get_being_blocked())
        {
            thread_remove(tid, blocked_list);
            to_be_resumed->set_being_blocked(false);
        }
        // if the thread being synced it cant return to the ready list
        if(!to_be_resumed->get_wait_for_sync())
        {
            to_be_resumed->set_state(READY);
            ready_list.push_back(tid);
        }
    }
    sig_unblock();
    return 0;
}

int uthread_sync(int tid)
{
    sig_block();
    if(cur_running == tid)
    {
        error_handler("a thread cant sync itself", LIB_ERR);
        return FAILURE_VAL;
    }
    if(cur_running == 0)
    {
        error_handler("cant use sync func of main thread", LIB_ERR);
        sig_unblock();
        return FAILURE_VAL;
    }
    if(all_threads.find(tid) == all_threads.end())
    {
        error_handler("cant sync non-existing thread", LIB_ERR);
        sig_unblock();
        return FAILURE_VAL;
    }
    Thread* synced_th = all_threads[cur_running];
    synced_list.push_back(pair<int, int>(cur_running, tid));
    synced_th->set_state(BLOCKED);
    sync_unblock(cur_running);
    synced_th->set_wait_for_sync(true);
    if(synced_th->save_env())
    {
        sig_unblock();
        return 0;
    }
    set_timer();
    run_next();
    sig_unblock();
    return 0;
}

int uthread_get_tid()
{
    return cur_running;
}

int uthread_get_total_quantums()
{
    return total_qntm;
}

int uthread_get_quantums(int tid)
{
    sig_block();
    int quantums;
    if(all_threads.find(tid) == all_threads.end())
    {
        error_handler("cant get quantums of non-existing thread", LIB_ERR);
        quantums = FAILURE_VAL;
    }
    else
    {
        quantums = all_threads[tid]->get_qntms();
    }
    sig_unblock();
    return quantums;
}
