
#ifndef __LLIST_H__
#define __LLIST_H__

#ifndef NULL
#define NULL 	(void *)0
#endif
#define TYPE_NODE	0xaa0000
#define NODE_MASK	0xff0000

#define llist_foreach_node(obj)						\
		obj = llist::getHead();					\
		for ( bool _lflag = false;				\
		obj && ((!_lflag) || (obj != llist::getHead())); 	\
		_lflag = true, obj = obj->getNext())		

class llist 
{
        protected:
                unsigned long int type;
                llist *prev;
                llist *next;
                static int count;
		static llist *start;
                static llist *prevlnode;
		llist* swap(void);
        public:

                llist(void);
                virtual ~llist(void);
		llist* getNext(void);
		llist* getPrev(void);
                unsigned long int getType(void);
                static int getCount(void);
                static llist* getList(void);
                static void deleteList(void);
                static llist* getLnode(unsigned long id);
		static llist* getHead(void);
};

#endif

