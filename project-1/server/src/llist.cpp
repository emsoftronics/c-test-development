#include "llist.h"

int llist::count = 0;
llist* llist::start = (llist *)NULL;
llist* llist::prevlnode = (llist *)NULL;

llist::llist(void) 
{	
	prev = next = this;
	type = TYPE_NODE;
	if (count) { 
		prev = prevlnode;
		next = prevlnode->next;
		prevlnode->next=this;
	}
	else {
		start = this;
	}
	prevlnode = this;
	count++;
}
		
llist::~llist(void)
{
	prev->next = next;
	next->prev = prev;
	count--;
	prevlnode = prev;
	type = 0;
	if (start == this) 
		start = next;
	if (!count)
		start = (llist *)NULL;
}
unsigned long int llist::getType(void)
{
	return type;
}

llist* llist::getNext(void)
{
	return next;
}

llist* llist::getPrev(void)
{
	return prev;
}

llist* llist::swap(void)
{
	if (next == start)
		return this;
	prev->next = next;
	next->prev = prev;
	prev = next;
	next = next->next;
	prev->next = this;
	next->prev = this;
	if (this == start)
		 start = prev;
	return prev;
}

int llist::getCount(void)
{
	return count;
}	

llist* llist::getList(void) 
{
	return start;
}
		
void llist::deleteList(void)
{
	if (!count)
		return;
	
	llist *tmp = start->next;
	while (  tmp != start) {
		llist *t = tmp;
		tmp = tmp->next;
		delete t;
	}
	delete start;
}
			
llist* llist::getLnode(unsigned long id)
{
	llist *tmp = (llist *)id;
	if (id == 0) 
		return (llist *)NULL;
	else
		return (tmp->type & NODE_MASK == TYPE_NODE) ? tmp : (llist *)NULL; 
}

llist* llist::getHead(void)
{
	return start;
}

