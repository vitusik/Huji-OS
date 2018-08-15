#include <vector>
#include <cstdlib>
#include "MapReduceFramework.h"
#include <semaphore.h>
#include <pthread.h>
#include <functional>
#include <algorithm>
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include "sys/time.h"
#include <math.h>

#define FAIL_VAL -1
typedef std::pair<k2Base*, v2Base*> k2_v2_pair;
typedef std::vector<k2_v2_pair> k2_v2_pair_vec;
// each exec_map thread writes to this type of container
typedef std::pair<pthread_t, k2_v2_pair_vec*> pthread_to_container;
// emit2 fills this vector
static std::vector<pthread_to_container> pthread_to_container_vec;


typedef std::pair<pthread_t, pthread_mutex_t> read_write_mutex;
// for each thread we have a mutex
static std::vector<read_write_mutex> read_write_mutex_vec; //

typedef std::pair<k2Base*, V2_VEC*> input_for_reduce;
static std::vector<input_for_reduce> input_for_reduce_vec;

typedef std::pair<pthread_t, OUT_ITEMS_VEC*> exec_reduce_con;
static std::vector<exec_reduce_con> exec_reduce_con_vec;

//shard index for traversing over the input vectors
int cur_index = 0;
// amount of elements each thread will read from the input vector
unsigned int jump_by = 0;

pthread_mutex_t index_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t exec_map_done_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t pthread_to_container_mutex = PTHREAD_MUTEX_INITIALIZER;
sem_t shuffle_sem;

IN_ITEMS_VEC input_vec;
OUT_ITEMS_VEC output_vec;
//bool which means that all of the execmap threads are done
bool map_th_done = false;

//the stream for the log file
std::ofstream logfile;
char buff[80];
// func that recievs a buffer a writes to it cur date and time
// in the format dd.mm.yyyy hh:mm:ss
void get_cur_date_and_time(char* buffer)
{
    time_t rawtime;
    tm* timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer,80,"[%Y.%m.%d %H:%M:%S]",timeinfo);
}

//var that is used for checking sys calls
int error_check;
// exit func in case of an error in a sys call
void error_exit(std::string func_name)
{
    std::cerr << "MapReduceFramework Failure:" << func_name<<".";
    exit(1);
}

void *execMap(void *arg)
{
    pthread_mutex_lock(&pthread_to_container_mutex);
    pthread_mutex_unlock(&pthread_to_container_mutex);
    get_cur_date_and_time(buff);
    logfile<<"Thread ExecMap created "<<buff<<"\n";
    MapReduceBase *p;
    p = (MapReduceBase*) arg;
    pthread_mutex_lock(&index_mutex);
    unsigned int i = cur_index;
    cur_index += jump_by + 1;
    pthread_mutex_unlock(&index_mutex);
    for(unsigned int j = 0;j <= jump_by;j++)
    {
        if(i + j >= input_vec.size())
        {
            break;
        }
        p->Map(input_vec[i + j].first, input_vec[i + j].second);
    }
    get_cur_date_and_time(buff);
    logfile<<"Thread ExecMap Terminated "<<buff<<"\n";
    pthread_exit(NULL);
}

void *execReduce(void *arg)
{
    pthread_mutex_lock(&pthread_to_container_mutex);
    pthread_mutex_unlock(&pthread_to_container_mutex);
    get_cur_date_and_time(buff);
    logfile<<"Thread ExecReduce created "<<buff<<"\n";
    MapReduceBase *p;
    p = (MapReduceBase*) arg;
    pthread_mutex_lock(&index_mutex);
    unsigned int i = cur_index;
    cur_index += jump_by + 1;
    pthread_mutex_unlock(&index_mutex);
    for(unsigned int j = 0;j <= jump_by;j++)
    {
        if(i + j >= input_for_reduce_vec.size())
        {
            break;
        }
        p->Reduce(input_for_reduce_vec[i + j].first,
            *input_for_reduce_vec[i + j].second);
    }
    get_cur_date_and_time(buff);
    logfile<<"Thread ExecReduce Terminated "<<buff<<"\n";
    pthread_exit(NULL);
}

void Emit2(k2Base *k, v2Base *v)
{
    pthread_t cur = pthread_self();
    std::vector<pthread_to_container>::iterator it;
    // first we find the right container
    for(it = pthread_to_container_vec.begin();
    it != pthread_to_container_vec.end(); it++)
    {
        if((*it).first == cur)
        {
            // second we look for its mutex
            std::vector<read_write_mutex>::iterator it2;
            for(it2 = read_write_mutex_vec.begin();
            it2 != read_write_mutex_vec.end(); it2++)
            {
                if((*it2).first == cur)
                {
                    // the mutex provides protection from shuffle
                    pthread_mutex_lock(&(*it2).second);
                    (*(*it).second).push_back(std::pair<k2Base*,v2Base*>(k,v));
                    pthread_mutex_unlock(&(*it2).second);
                    // waking up shuffle thread in case it fell a sleep
                    error_check = sem_post(&shuffle_sem);
                    if(error_check == FAIL_VAL)
                    {
                        error_exit("sem_post");
                    }
                    break;
                }
            }
        }
    }
}

void Emit3(k3Base *k, v3Base *v)
{
    pthread_t cur = pthread_self();
    std::vector<exec_reduce_con>::iterator it;
    for(it = exec_reduce_con_vec.begin(); it != exec_reduce_con_vec.end(); it++)
    {
        if((*it).first == cur)
        {
            (*(*it).second).push_back(std::pair<k3Base*, v3Base*>(k,v));
            return;
        }
    }
}

void *shuffle(void*)
{
    get_cur_date_and_time(buff);
    logfile<<"Thread Shuffle created "<<buff<<"\n";
    bool all_is_empty = false;
    while(true)
    {
        pthread_mutex_lock(&exec_map_done_mutex);
        // before shuffle finishes it needs to check that all of its
        // containers are empty, and that all of the execmap threads are done
        if(map_th_done && all_is_empty)
        {
            pthread_mutex_unlock(&exec_map_done_mutex);
            get_cur_date_and_time(buff);
            logfile<<"Thread Shuffle Terminated "<<buff<<"\n";
            pthread_exit(NULL);
        }
        pthread_mutex_unlock(&exec_map_done_mutex);
        // before poping up an element from the containers the semaphore
        // is brought down
        error_check = sem_wait(&shuffle_sem);
        if(error_check == FAIL_VAL)
        {
            error_exit("sem_wait");
        }
        std::vector<pthread_to_container>::iterator it;
        std::vector<read_write_mutex>::iterator it2;
        std::vector<input_for_reduce>::iterator it3;
        // finding non-empty container
        bool found_container = false, found_mutex = false, no_kbase = true;
        all_is_empty = true;
        // first we find non empty container
        for(it = pthread_to_container_vec.begin();
        it != pthread_to_container_vec.end(); it++)
        {
            if(!(*(*it).second).empty())
            {
                all_is_empty = false;
                found_container = true;
                break;
            }
        }
        // finding its mutex
        if(found_container)
        {
            for(it2 = read_write_mutex_vec.begin();
            it2 != read_write_mutex_vec.end(); it2++)
            {
                if((*it).first == (*it2).first)
                {
                    found_mutex = true;
                    break;
                }
            }
        }
        // now we can lock the mutex, pop the element and add it
        //to our input for reduce vec
        if(found_container && found_mutex)
        {
            pthread_mutex_lock(&(*it2).second);
            k2_v2_pair &pop_from_container = (*(*it).second).back();
            (*(*it).second).pop_back();
            for(it3 = input_for_reduce_vec.begin();
            it3 !=input_for_reduce_vec.end(); it3++)
            {
                if(!((*(pop_from_container.first) < *((*it3).first)))
                && (!(*((*it3).first) < *(pop_from_container.first))))
                {
                    no_kbase = false;
                    (*(*it3).second).push_back(pop_from_container.second);
                    pthread_mutex_unlock(&(*it2).second);
                    break;
                }
            }
            // in case its the first time that a base is seen by the vec
            // we need to add it
            if(no_kbase)
            {
                V2_VEC *v2_vec = new (std::nothrow) V2_VEC();
                if(!v2_vec)
                {
                    error_exit("new");
                }
                (*v2_vec).push_back(pop_from_container.second);
                input_for_reduce_vec.push_back
                (std::pair<k2Base*, V2_VEC*>(pop_from_container.first, v2_vec));
                pthread_mutex_unlock(&(*it2).second);
            }
        }
    }
}

// comperator for std::sort
bool sortf(const OUT_ITEM &item1, const OUT_ITEM &item2)
{
    return *(item1.first) < *(item2.first);
}


OUT_ITEMS_VEC
RunMapReduceFramework(MapReduceBase &mapReduce, IN_ITEMS_VEC &itemsVec,
    int multiThreadLevel, bool autoDeleteV2K2)
{
    // time meassurements
    struct timeval start, end, diff;
    logfile.open(".MapReduceFramework.log", std::ios::app);
    logfile<<"RunMapReduceFramework started with "<< multiThreadLevel
    <<" threads\n";
    // needed a global acsses to the itemsVec
    input_vec = itemsVec;
    // defined the jump to be a function of the size of the input and the amount
    // of thredas
    jump_by = 1 + (int)input_vec.size() / multiThreadLevel;
    error_check = sem_init(&shuffle_sem, 0, 0);
    if(error_check == FAIL_VAL)
    {
        error_exit("sem_init");
    }
    pthread_t thread_pool[multiThreadLevel];
    pthread_t shuffle_thread;
    pthread_mutex_t mutex_arr[multiThreadLevel];
    pthread_mutex_lock(&pthread_to_container_mutex);
    gettimeofday(&start, NULL);
    for(int t = 0; t < multiThreadLevel; t++)
    {
        k2_v2_pair_vec *k2_v2_vec = new k2_v2_pair_vec();
        if(!k2_v2_vec)
        {
            error_exit("new");
        }
        mutex_arr[t] = PTHREAD_MUTEX_INITIALIZER;
        error_check = pthread_create(&thread_pool[t], NULL,
            execMap, (void *)&mapReduce);
        if(error_check)
        {
            error_exit("pthread_create");
        }
        pthread_to_container_vec.push_back
        (std::pair<pthread_t , k2_v2_pair_vec*>(thread_pool[t],k2_v2_vec));
        read_write_mutex_vec.push_back
        (std::pair<pthread_t , pthread_mutex_t>(thread_pool[t], mutex_arr[t]));
    }
    error_check = pthread_create(&shuffle_thread, NULL, shuffle, NULL);
    if(error_check)
    {
        error_exit("pthread_create");
    }
    pthread_mutex_unlock(&pthread_to_container_mutex);

    for(int t = 0; t < multiThreadLevel; t++)
    {
        error_check = pthread_join(thread_pool[t], NULL);
        if(error_check)
        {
            error_exit("pthread_join");
        }
    }
    error_check = sem_post(&shuffle_sem);
    if(error_check == FAIL_VAL)
    {
        error_exit("sem_init");
    }
    // now the index will be used in execReduce threads
    cur_index = 0;

    pthread_mutex_lock(&exec_map_done_mutex);
    map_th_done = true;
    pthread_mutex_unlock(&exec_map_done_mutex);
    error_check = pthread_join(shuffle_thread, NULL);
    gettimeofday(&end, NULL);
    timersub(&end, &start, &diff);
    long double time_took = (diff.tv_usec * 1000 + diff.tv_sec * pow(10,9));
    logfile<<"Map and Shuffle took " << time_took <<" ns\n";
    if(error_check)
    {
        error_exit("pthread_join");
    }
    // now creating exec_red threads
    pthread_mutex_lock(&pthread_to_container_mutex);
    gettimeofday(&start, NULL);
    for(int t = 0; t < multiThreadLevel; t++)
    {
        OUT_ITEMS_VEC *v2 = new OUT_ITEMS_VEC();
        if(!v2)
        {
            error_exit("new");
        }
        error_check = pthread_create(&thread_pool[t], NULL,
            execReduce, (void*)&mapReduce);
        if(error_check)
        {
            error_exit("pthread_create");
        }
        exec_reduce_con_vec.push_back(std::pair<pthread_t,
            OUT_ITEMS_VEC*>(thread_pool[t], v2));
    }
    pthread_mutex_unlock(&pthread_to_container_mutex);
    for(int t = 0; t < multiThreadLevel; t++)
    {
        error_check = pthread_join(thread_pool[t], NULL);
        if(error_check)
        {
            error_exit("pthread_join");
        }
    }
    gettimeofday(&end, NULL);
    timersub(&end, &start, &diff);
    time_took = (diff.tv_usec * 1000 + diff.tv_sec * pow(10,9));
    logfile<<"Reduce took " <<time_took<<" ns\n";
    sem_destroy(&shuffle_sem);
    pthread_mutex_destroy(&index_mutex);
    pthread_mutex_destroy(&exec_map_done_mutex);
    pthread_mutex_destroy(&pthread_to_container_mutex);


    if(autoDeleteV2K2)
    {
        std::vector<input_for_reduce>::iterator del1;
        for(del1 = input_for_reduce_vec.begin();
        del1 != input_for_reduce_vec.end();del1++)
        {
            (*del1).second->clear();
            delete (*del1).first;

            delete (*del1).second;
        }


        std::vector<pthread_to_container>::iterator del2;
        for(del2 = pthread_to_container_vec.begin();
        del2 != pthread_to_container_vec.end(); del2 ++)
        {
            std::vector<k2_v2_pair_vec>::iterator del21;
            //for(del21 = )
            (*del2).second->clear();
            delete (*del2).second;
        }
    }

    std::vector<exec_reduce_con>::iterator it;
    for(it = exec_reduce_con_vec.begin();
    it!= exec_reduce_con_vec.end(); it++)
    {
        output_vec.insert(output_vec.end(),
        (*(*it).second).begin(), (*(*it).second).end());
    }

    std::vector<exec_reduce_con>::iterator del3;

    // another small dealloc
    for(int i = 0 ; i < multiThreadLevel; i++)
    {
        pthread_mutex_destroy(&(mutex_arr[i]));
    }

    for(del3 = exec_reduce_con_vec.begin();
    del3 != exec_reduce_con_vec.end(); del3++)
    {
        (*del3).second->clear();
        delete (*del3).second;
    }

    std::sort(output_vec.begin(), output_vec.end(), sortf);
    logfile<<"RunMapReduceFramework finished\n";
    logfile.close();
    return output_vec;
}
