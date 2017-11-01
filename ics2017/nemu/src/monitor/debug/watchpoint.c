#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

WP* new_wp()
{
	if(free_ == NULL) assert(0);
	WP *new = free_;
	free_ = (*free_).next;
	(*new).next = head;
	head = new;
	printf("new watchpoint:%d\n",(*new).NO);
	return new;
}

void free_wp(char *expr)
{
	WP *list1 = head;
	int num;
	sscanf(expr,"%d",&num);
	if((*list1).NO==num)
	{
		printf("Delete watchpoint %d\n",(*list1).NO);
		head = (*list1).next;
		(*list1).next = free_;
		free_ = list1;
	}
	else
	{
		WP *list2 = (*list1).next;
		while(list2!=NULL)
		{
			if((*list2).NO == num)
			{
				printf("Delete watchpoint %d\n",(*list2).NO);
				head = (*list2).next;
				(*list2).next = free_;
				free_ = list2;
			}
			list1 = list2;
			list2 = (*list2).next;
		}
	}
}

bool check()
{
	WP *list = head;
	bool Changed = false,success = true;

	while(list!=NULL)
	{
		int newvalue = expr((*list).expr,&success);
		if(newvalue != (*list).value)
		{
			printf("Stop at watchpoint %d\n",(*list).NO);
			printf("Last Value = %d \n",(*list).value);
			printf("New Value = %d \n",newvalue);
			(*list).value = newvalue;
			Changed = true;
			return Changed;
		}
		list = (*list).next;
	}
	return Changed;
}

void print_w()
{
	WP *list = head;
	while(list != NULL)
	{
		printf("Num\tAddress\tValue\n");
		printf("%3d\t%7s\t%8x\n",(*list).NO,(*list).expr,(*list).value);
		list = (*list).next;
	}
}

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}
