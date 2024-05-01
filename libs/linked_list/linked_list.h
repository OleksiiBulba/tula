#ifndef LINKED_LIST_H_
#define LINKED_LIST_H_

#include <stdlib.h>
#include <string.h>

#define LINKED_LIST_TYPE(item_type)         linked_list_##item_type
#define CREATE_LIST_NODE(item_type)         create_list_node_##item_type
#define INSERT_AT_HEAD(item_type)           insert_at_head_##item_type
#define FIND_LIST_ITEM(item_type)           find_list_item_##item_type
#define FREE_LIST(item_type)                free_list_##item_type
#define SEARCH_CRITERIA_TYPE(item_type)     search_criteria_##item_type
#define MATCH_TYPE(item_type)               match_##item_type
#define CONTEXT_CRITERIA_TYPE(item_type)    context_criteria_##item_type

#define LINKED_LIST(item_type)                                      \
    typedef struct LINKED_LIST_TYPE(item_type) {                    \
        item_type* item;                                            \
        struct LINKED_LIST_TYPE(item_type)* next;                   \
    } LINKED_LIST_TYPE(item_type);                                  \
                                                                                            \
    typedef bool (*SEARCH_CRITERIA_TYPE(item_type))(item_type* element, void* context);     \
                                                                                            \
    LINKED_LIST_TYPE(item_type)* CREATE_LIST_NODE(item_type)(item_type* item);              \
    void INSERT_AT_HEAD(item_type)(LINKED_LIST_TYPE(item_type)** head, item_type* item);    \
    item_type* FIND_LIST_ITEM(item_type)(                                                   \
        LINKED_LIST_TYPE(item_type)* head,                                                  \
        SEARCH_CRITERIA_TYPE(item_type) criteria,                                           \
        void* context                                                                       \
    );                                                                                      \
    void FREE_LIST(item_type)(LINKED_LIST_TYPE(item_type)* head);                           \

#define LINKED_LIST_DEFINITIONS(item_type)                                                  \
    LINKED_LIST_TYPE(item_type)* CREATE_LIST_NODE(item_type)(item_type* item)               \
    {                                                                                       \
        LINKED_LIST_TYPE(item_type)* newNode =                                              \
            (LINKED_LIST_TYPE(item_type)*)malloc(sizeof(LINKED_LIST_TYPE(item_type)));      \
        if (newNode) {                                                                      \
            newNode->item = item;                                                           \
            newNode->next = NULL;                                                           \
        }                                                                                   \
        return newNode;                                                                     \
    }                                                                                       \
                                                                                            \
    void INSERT_AT_HEAD(item_type)(LINKED_LIST_TYPE(item_type)** head, item_type* item)     \
    {                                                                                       \
        LINKED_LIST_TYPE(item_type)* newNode = CREATE_LIST_NODE(item_type)(item);           \
        if (newNode) {                                                                      \
            newNode->next = *head;                                                          \
            *head = newNode;                                                                \
        }                                                                                   \
    }                                                                                       \
                                                                                            \
    item_type* FIND_LIST_ITEM(item_type)(                                                   \
        LINKED_LIST_TYPE(item_type)* head,                                                  \
        SEARCH_CRITERIA_TYPE(item_type) criteria,                                           \
        void* context                                                                       \
    ) {                                                                                     \
        LINKED_LIST_TYPE(item_type)* current = head;                                        \
        while (current != NULL) {                                                           \
            if (criteria(current->item, context)) {                                         \
                return current->item;                                                       \
            }                                                                               \
            current = current->next;                                                        \
        }                                                                                   \
        return NULL;                                                                        \
    }                                                                                       \
                                                                                            \
    void FREE_LIST(item_type)(LINKED_LIST_TYPE(item_type)* head)                            \
    {                                                                                       \
        LINKED_LIST_TYPE(item_type)* tmp;                                                   \
        while (head != NULL) {                                                              \
            tmp = head;                                                                     \
            head = head->next;                                                              \
            free(tmp);                                                                      \
        }                                                                                   \
    }                                                                                       \

#define DEFINE_MATCH_TYPE(item_type, context_cast, comparison)      \
    bool MATCH_TYPE(item_type)(item_type* element, void* context)   \
    {                                                               \
        context_cast;                                               \
        return comparison;                                          \
    }

#endif // LINKED_LIST_H_

