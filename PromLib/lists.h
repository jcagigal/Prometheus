#ifndef AROSEXEC_LISTS_H
#define AROSEXEC_LISTS_H

/*
    Copyright � 1995-2021, The AROS Development Team. All rights reserved.
    $Id$
        
    Structures and macros for exec lists.
*/


/**************************************
		Includes
**************************************/
#ifndef EXEC_NODES_H
#   include <exec/nodes.h>
#endif



/**************************************
	       Macros
**************************************/
//#define IsListEmpty(l) (((struct Node *)((struct List *)(l))->lh_TailPred) == (struct Node *)(l))

#define IsMinListEmpty(l) ( (((struct MinList *)l)->mlh_TailPred) == (struct MinNode *)(l) )

//#define IsMsgPortEmpty(mp) ( (((struct MsgPort *)(mp))->mp_MsgList.lh_TailPred) == (struct Node *)(&(((struct MsgPort *)(mp))->mp_MsgList)) )

#ifndef __GNUC__
#define NewList(_l)                                     \
do                                                      \
{                                                       \
    struct List *__aros_list_tmp = (struct List *)(_l), \
                *l = __aros_list_tmp;                   \
                                                        \
    l->lh_TailPred = (struct Node *)l;                  \
    l->lh_Tail     = 0;                                 \
    l->lh_Head     = (struct Node *)&l->lh_Tail;        \
} while (0)
#else /* __GNUC__ */
#define NewList(_l)                                     \
do                                                      \
{                                                       \
    struct List *__aros_list_tmp = (struct List *)(_l), \
                *l = __aros_list_tmp;                   \
                                                        \
    l->lh_TailPred_= l;                                 \
    l->lh_Tail     = 0;                                 \
    l->lh_Head     = (struct Node *)&l->lh_Tail;        \
} while (0)
#endif /* __GNUC__ */

#define ADDHEAD(_l,_n)                                  \
do                                                      \
{                                                       \
    struct Node *__aros_node_tmp = (struct Node *)(_n), \
                *n = __aros_node_tmp;                   \
    struct List *__aros_list_tmp = (struct List *)(_l), \
                *l = __aros_list_tmp;                   \
                                                        \
    n->ln_Succ          = l->lh_Head;                   \
    n->ln_Pred          = (struct Node *)&l->lh_Head;   \
    l->lh_Head->ln_Pred = n;                            \
    l->lh_Head          = n;                            \
} while (0)

#define ADDTAIL(_l,_n)                                    \
do                                                        \
{                                                         \
    struct Node *__aros_node_tmp = (struct Node *)(_n),   \
                *n = __aros_node_tmp;                     \
    struct List *__aros_list_tmp = (struct List *)(_l),   \
                *l = __aros_list_tmp;                     \
                                                          \
    n->ln_Succ              = (struct Node *)&l->lh_Tail; \
    n->ln_Pred              = l->lh_TailPred;             \
    l->lh_TailPred->ln_Succ = n;                          \
    l->lh_TailPred          = n;                          \
} while (0)

#define REMOVE(_n)                                      \
({                                                      \
    struct Node *__aros_node_tmp = (struct Node *)(_n), \
                *n = __aros_node_tmp;                   \
                                                        \
    n->ln_Pred->ln_Succ = n->ln_Succ;                   \
    n->ln_Succ->ln_Pred = n->ln_Pred;                   \
                                                        \
    n;                                                  \
})

#define GetHead(_l)                                     \
({                                                      \
    struct List *__aros_list_tmp = (struct List *)(_l), \
                *l = __aros_list_tmp;                   \
                                                        \
   l->lh_Head->ln_Succ ? l->lh_Head : (struct Node *)0; \
})

#define GetTail(_l)                                              \
({                                                               \
    struct List *__aros_list_tmp = (struct List *)(_l),          \
                *l = __aros_list_tmp;                            \
                                                                 \
    l->lh_TailPred->ln_Pred ? l->lh_TailPred : (struct Node *)0; \
})

#define GetSucc(_n)                                      \
({                                                       \
    struct Node *__aros_node_tmp = (struct Node *)(_n),  \
                *n = __aros_node_tmp;                    \
                                                         \
    (n && n->ln_Succ && n->ln_Succ->ln_Succ) ? n->ln_Succ : (struct Node *)0; \
})

#define GetPred(_n)                                      \
({                                                       \
    struct Node *__aros_node_tmp = (struct Node *)(_n),  \
                *n = __aros_node_tmp;                    \
                                                         \
    (n && n->ln_Pred && n->ln_Pred->ln_Pred) ? n->ln_Pred : (struct Node *)0; \
})

#define REMHEAD(_l)                                               \
({                                                                \
    struct List *__aros_list_tmp = (struct List *)(_l),           \
                *l = __aros_list_tmp;                             \
                                                                  \
     l->lh_Head->ln_Succ ? REMOVE(l->lh_Head) : (struct Node *)0; \
})

#define REMTAIL(_l)                                                      \
({                                                                       \
    struct List *__aros_list_tmp = (struct List *)(_l),                  \
                *l = __aros_list_tmp;                                    \
                                                                         \
    l->lh_TailPred->ln_Pred ? REMOVE(l->lh_TailPred) : (struct Node *)0; \
})

#define ForeachNode(list, node)                        \
for                                                    \
(                                                      \
    *(void **)&node = (void *)(((struct List *)(list))->lh_Head); \
    ((struct Node *)(node))->ln_Succ;                  \
    *(void **)&node = (void *)(((struct Node *)(node))->ln_Succ)  \
)

#define ForeachNodeSafe(list, current, next)              \
for                                                       \
(                                                         \
    *(void **)&current = (void *)(((struct List *)(list))->lh_Head); \
    (*(void **)&next = (void *)((struct Node *)(current))->ln_Succ); \
    *(void **)&current = (void *)next                                \
)

#define ListLength(list,count)     \
do {		                   \
    struct Node * __n;	           \
    count = 0;		           \
    ForeachNode (list,__n) count ++; \
} while (0)

#endif /* AROSEXEC_LISTS_H */
