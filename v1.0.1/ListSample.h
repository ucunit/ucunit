#ifndef LISTSAMPLE_H
#define LISTSAMPLE_H

typedef struct __list_struct list_t;

list_t* list_create(unsigned int element_size);
int list_init(list_t *list, unsigned int element_size);
int list_size(list_t* list);
int list_insert(list_t *list,void *element,int index);
int list_index(list_t *list, void *element);
int list_get(list_t *list, void* element, int index);
int list_rem(list_t *list, void* element, int index);
int list_destroy(list_t** list);

#endif // LISTSAMPLE_H
