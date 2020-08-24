#include <unistd.h>
#include <stdio.h>
#include "smalloc.h"
#include <string.h>

sm_container_t sm_head = {
    0,
    &sm_head,
    &sm_head,
    0
} ;

static
void *
_data (sm_container_ptr e)
{
    return ((void *) e) + sizeof(sm_container_t) ;
}

static
void
sm_container_split (sm_container_ptr hole, size_t size)
{
    sm_container_ptr remainder = (sm_container_ptr) (_data(hole) + size) ;

    remainder->dsize = hole->dsize - size - sizeof(sm_container_t) ;
    remainder->status = Unused ;
    remainder->next = hole->next ;
    remainder->prev = hole ;
    hole->dsize = size ;
    hole->next->prev = remainder ;
    hole->next = remainder ;
}

static
void *
retain_more_memory (int size)
{
    sm_container_ptr hole ;
    int pagesize = getpagesize() ;
    int n_pages = 0 ;

    n_pages = (sizeof(sm_container_t) + size + sizeof(sm_container_t)) / pagesize  + 1 ;
    hole = (sm_container_ptr) sbrk(n_pages * pagesize) ;
    if (hole == 0x0)
        return 0x0 ;
    
    hole->status = Unused ;
    hole->dsize = n_pages * getpagesize() - sizeof(sm_container_t) ;
    return hole ;
}

void *
smalloc (size_t size)
{
    size_t temp_size;
    int context = 0;
    sm_container_ptr hole = 0x0, itr = 0x0 ;

    for (itr = sm_head.next ; itr != &sm_head ; itr = itr->next) {
        //find the smalles possible remaining object
        if (itr->status == Busy)
            continue ;
        if ((itr->dsize == size) || (size + sizeof(sm_container_t) < itr->dsize)) {
            if(temp_size>itr->dsize||(context==0))
            {
                context = 1;
                temp_size = itr->dsize;
                hole = itr ;
            }
        }
    }
    if (hole == 0x0) {
        hole = retain_more_memory(size) ;
        if (hole == 0x0)
            return 0x0 ;
        hole->next = &sm_head ;
        hole->prev = sm_head.prev ;
        (sm_head.prev)->next = hole ;
        sm_head.prev = hole ;
    }
    if (size < hole->dsize)
        sm_container_split(hole, size) ;
    hole->status = Busy ;
    return _data(hole) ;
}

void
sfree (void * p)
{
    sm_container_ptr itr ;
    size_t size;
    
    for (itr = sm_head.next ; itr != &sm_head ; itr = itr->next) {
        if (p == _data(itr)) {
            sm_container_ptr temp;
            itr->status = Unused;
            temp = itr;
            while(1)
            {
                if(temp->next->status == Unused)
                {
                    temp = temp->next;
                    itr->next = temp->next;
                    temp->next->prev = itr;
                    printf("NEXT : %d %d %d\n",(int)itr->dsize,(int)sizeof(sm_container_t),(int)temp->dsize);
                    itr->dsize = itr->dsize + sizeof(sm_container_t) + temp->dsize;
                }
                else{
                    break;
                }
            }
            temp = itr;
            while(1)
            {
                if(temp->prev->status == Unused)
                {
                    temp = temp->prev;
                    itr->prev = temp->prev;
                    temp->prev->next = itr;
                    printf("PREV : %d %d %d\n",(int)itr->dsize,(int)sizeof(sm_container_t),(int)temp->dsize);
                    itr->dsize = itr->dsize + sizeof(sm_container_t) + temp->dsize;
                }
                else{
                    break;
                }
            }
            break ;
        }
    }
}

void
print_sm_containers ()
{
    sm_container_ptr itr ;
    int i ;

    printf("==================== sm_containers ====================\n") ;
    for (itr = sm_head.next, i = 0 ; itr != &sm_head ; itr = itr->next, i++) {
        printf("%3d:%p:%s:", i, _data(itr), itr->status == Unused ? "Unused" : "  Busy") ;
        printf("%8d:", (int) itr->dsize) ;

        int j ;
        char * s = (char *) _data(itr) ;
        for (j = 0 ; j < (itr->dsize >= 8 ? 8 : itr->dsize) ; j++)
            printf("%02x ", s[j]) ;
        printf("\n") ;
    }
    printf("\n") ;

}

void print_mem_uses()
{
    sm_container_ptr itr ;
    size_t size_total = 0;
    size_t size_used = 0;
    int count = 0;
    for (itr = sm_head.next; itr != &sm_head ; itr = itr->next) {
        count ++;
        size_total = size_total + itr->dsize + sizeof(sm_container_t);
        if(itr->status==Busy) size_used = size_used + itr->dsize; //exclude meta data
    }
    
    printf("Retained Memory : %d\n",(int)size_total);
    printf("Used Memory : %d\n",(int)size_used);
    printf("Retained but not used Memory : %d\n",(int)size_total-(int)size_used-(int)sizeof(sm_container_t)*count);
}

void * srealloc (void * p, size_t newsize)
{
    sm_container_ptr itr ;
    size_t possible_size;
    for (itr = sm_head.next ; itr != &sm_head ; itr = itr->next) {
        if (p == _data(itr)) {
            possible_size = itr->dsize;
            if(itr->next->status==Unused) possible_size = possible_size + itr->next->dsize;
            if(newsize<possible_size)
            {
                //fisrt combine
                itr->next = itr->next->next;
                itr->dsize = possible_size;
                sm_container_split (itr, newsize);
                //pointer maintain
            }
            else{
                void * p1;
                sfree(p);
                p1 = smalloc((int)newsize);
                memcpy(p1,_data(itr),newsize);
                p = p1;
            }
            break;
        }
    }
    return p;
}

void sshrink()
{
    sm_container_ptr itr ;
    sm_container_ptr itr2 ;
    for (itr = sm_head.next ; itr != &sm_head ; itr = itr->next)
    {
        if(itr->status==Busy) itr2 = itr;
    }
    printf("\n%p",_data(itr2->next));
    if(!brk(_data(itr2->next))) printf("break point set\n");
}
