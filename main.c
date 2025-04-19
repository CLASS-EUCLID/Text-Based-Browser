#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_DESC_LEN 200
#define MAX_URL_LEN 55
#define MAX_SITES 52
#define MAX_QUERY_LEN 30

typedef struct stack_t stack_t;
struct stack_t
{
	struct doubly_linked_list_t *list;
};

typedef struct {
    int id;
    char url[55];   
    char *description;
} page;

typedef struct {
    int id;
    page *curr_page;
    stack_t *back;
    stack_t *forw;
}tab;

typedef struct dll_node_t dll_node_t;
struct dll_node_t
{
    void* data;
    dll_node_t *prev, *next;
};

typedef struct doubly_linked_list_t doubly_linked_list_t;
struct doubly_linked_list_t
{
    dll_node_t* head;
    int data_size;
    int size;
};  

typedef struct browser {
    dll_node_t *sen;
    dll_node_t *curr;
    int next_id;
} browser;

doubly_linked_list_t*
dll_create(unsigned int data_size)
{
	doubly_linked_list_t *list = malloc(sizeof(doubly_linked_list_t));
    list->head = NULL;
    list->data_size = data_size;
    list->size = 0;
    return list;
}

dll_node_t*
dll_get_nth_node(doubly_linked_list_t* list, int n)
{
	n %= list->size;
    int cnt = 0;dll_node_t *p;
    if(!list->head)
         return NULL;
    p = list->head;
    if(!n)
    {
        return p;
    }
    while(cnt != n)
    {
        p=p->next;
        cnt++;
    }
    return p;
}

void
dll_add_nth_node(doubly_linked_list_t* list, int n, const void* new_data)
{
	int cnt = 0;
    dll_node_t *p = malloc(sizeof(dll_node_t));
    p->data = malloc(list->data_size);
    memcpy(p->data, new_data,list->data_size);
    if(!list->head)
    {
        list->head = p;
        list->size++;
        p->next = p;
        p->prev = p;
        return;
    }
    if (n == 0)
    {
        p->prev = list->head->prev;
        p->next = list->head;
    
        list->head->prev->next = p;
        list->head->prev = p; 
    
        list->head = p; 
        list->size++;
        return;
    }
    dll_node_t *trv = list->head;
    if(n >= list->size)
    {
        while(trv->next != list->head)
        {
            trv = trv->next;
        }
        p->next = trv->next;
        p->prev = trv;
        trv->next = p;
        list->head->prev = p;
        list->size++;
        return;
    }
    cnt++;
    while(cnt != n)
    {
        trv = trv->next;
        cnt++;
    }
    p->next = trv->next;
    trv->next = p;
    p->prev = trv;
    p->next->prev = p;
    list->size++;
    return;
}

int dll_get_size(doubly_linked_list_t* list)
{
    return list->size;
}

dll_node_t*
dll_remove_nth_node(doubly_linked_list_t* list, int n)
{
    dll_node_t *rm = list->head;
	if(list->size == 1)
    {
        rm = list->head;
        list->head = NULL;
        list->size--;
        return rm;
    }
    if(n == 0)
    {
        rm->next->prev = rm->prev;
        list->head = rm->next;
        list->size--;
        return rm;
    }
    if(n >= list->size - 1)
    {
        rm = list->head->prev;

        list->head->prev = rm->prev;
        rm->prev->next = list->head;
        list->size--;

        return rm;
    }

    int cnt = 0;
    while(cnt != n )
    {
        rm = rm->next;
        cnt++;
    }
    rm->prev->next = rm->next;
    rm->next->prev = rm->prev;
    list->size--;
    return rm;
}


void
dll_free(doubly_linked_list_t** pp_list)
{
	if(!(*pp_list)->head)
    {
        (*pp_list)->data_size = 0;
        (*pp_list)->size = 0;
        free(*pp_list);
        return;
    }
	dll_node_t *p = (*pp_list)->head;
    p = p->next;
    while(p != (*pp_list)->head)
    {
        dll_node_t *t = p->next;
        free(p->data);
        p->next = NULL;
        p->prev = NULL;
        free(p);
        p = t;
    }
    p->next = NULL;
    p->prev = NULL;
    free(p->data);
    free(p);
    (*pp_list)->data_size = 0;
    (*pp_list)->head = NULL;
    (*pp_list)->size = 0;
    free((*pp_list));
}

void
dll_print_int_list(doubly_linked_list_t* list, FILE *out)
{
	if(!list->head)
        return;
    dll_node_t *p  = list->head;
    int cnt = 0;
    while(cnt != list->size)
    {
        fprintf(out,"%d ",*(int *)p->data);
        p = p->next;
        cnt++;
    }
    fprintf(out,"\n");
}

void
dll_print_string_list(doubly_linked_list_t* list, FILE *out)
{
	if(!list->head)
        return;
    dll_node_t *p  = list->head;
    int cnt = 0;
    while(cnt != list->size)
    {
        fprintf(out,"%s ",(char *)p->data);
        p = p->next;
        cnt++;
    }
    fprintf(out,"\n");
}


stack_t *
st_create(unsigned int data_size)
{
	stack_t *stk = malloc(sizeof(stack_t));
    if (!stk) 
        return NULL;
    stk->list = dll_create(data_size);
    return stk;
}

unsigned int
st_get_size(stack_t *st)
{
	return dll_get_size(st->list);
}

unsigned int
st_is_empty(stack_t *st)
{
    if(!st || !st->list || !st->list->size){
        return 1;
    }
	return 0;
}

/*
 * Functia intoarce elementul din vârful stivei al carei pointer este trimis
 * ca parametru fără a efectua nicio modificare asupra acesteia.
 */
void *
st_peek(stack_t *st)
{
    if(!st || !st->list || !st->list->head || !st->list->size ){
        return NULL;
    }
	return st->list->head->data;
}

/*
 * Functia elimina elementul din vârful stivei al carei pointer este trimis
 * ca parametru. Atentie! Este responsabilitatea acestei functii de a elibera
 * memoria pentru elementul eliminat.
 */
void
st_pop(stack_t *st)
{
   if(!st || st_is_empty(st))
        return;
   dll_node_t *fr = dll_remove_nth_node(st->list,0);
   free(fr->data); // suspect
   free(fr);
}

/*
 * Pe baza datelor trimise prin pointerul new_data, se adauga un nou element in
 * vârful stivei al carei pointer este trimis ca parametru.
 */
void
st_push(stack_t *st, void *new_data)
{
    if(!st){
        return;
    }
	dll_add_nth_node(st->list,0,new_data);
}

/*
 * Functia elimina toate elementele stivei al carei pointer este trimis
 * ca parametru.
 */
void
st_clear(stack_t *st)
{
	while(!st_is_empty(st))
    {
        st_pop(st);
    }
}

/*
 * Functia elibereaza memoria folosita de toate elementele, iar la
 * sfarsit elibereaza memoria folosita de structura stivei.
 */
void
st_free(stack_t *st)
{
    if (!st) return;
    if (st->list) {
        st_clear(st);
        dll_free(&st->list);
    }
    free(st);
}

browser* browser_create(page *def) {
    browser *br = malloc(sizeof(browser));

    br->sen = malloc(sizeof(dll_node_t));
    br->sen->data = NULL;

    tab *initial_tab = malloc(sizeof(tab));
    initial_tab->id = 0;
    initial_tab->curr_page = &def[0];
    initial_tab->back = st_create(sizeof(page *));
    initial_tab->forw = st_create(sizeof(page *));

    dll_node_t *first = malloc(sizeof(dll_node_t));
    first->data = initial_tab;

    first->next = br->sen;
    first->prev = br->sen;

    br->sen->next = first;
    br->sen->prev = first;

    br->curr = first;
    br->next_id = 1;

    return br;
}


// PAGE <ID>
// the error check for this function should be 
// done before its called
void open_page(browser *br, int e, page *pges) {
    tab *curr = ((tab *)(br->curr->data));
    st_push(curr->back, &curr->curr_page); // always bugs here
    st_clear(curr->forw);
    curr->curr_page = &pges[e];
}

// BACKWARD
void go_down(browser *br, FILE *out) {
    tab *curr = ((tab *)(br->curr->data));
    if(st_is_empty(curr->back)) {
        fprintf(out,"403 Forbidden\n");
        return;
    }
    st_push(curr->forw, &curr->curr_page); // always bugs
    page *p = *(page **)st_peek(curr->back);
    curr->curr_page = p;
    st_pop(curr->back);
}

// FORWARD
void go_up(browser *br, FILE * out) {
    tab *curr = ((tab *)(br->curr->data));
    if(st_is_empty(curr->forw)) {
        fprintf(out,"403 Forbidden\n");
        return;
    }
    st_push(curr->back, &curr->curr_page); // more bugs
    page *p = *(page **)st_peek(curr->forw);
    curr->curr_page = p;
    st_pop(curr->forw);
}

// PRINT
void print_all(browser *br, FILE *out)
{
    if(!br->curr)
    {
        fprintf(out,"403 Forbidden\n");
        return;
    }
    if(br->curr == br->sen)
    {
        fprintf(out,"403 Forbidden\n");
        return;
    }
    dll_node_t *it = br->curr;
    fprintf(out,"%d ",((tab*)(br->curr->data))->id);
    it = it->next;
    while(it != br->curr)
    {
        if(it == br->sen)
        {
            it = it->next;
            continue;
        }
            
        fprintf(out,"%d ",((tab*)(it->data))->id);
        it = it->next;
    }
    fprintf(out,"\n%s\n",((page *)(((tab *)(br->curr->data))->curr_page))->description);
}

// PRINT_HISTORY <ID>
void print_history(browser *br, int e, FILE *out)
{
    //tab * temp = (tab *)(br->curr->data);
    dll_node_t *q = br->sen;
    q = q->next;
    while(((tab *)(q->data))->id != e)
    {
        q = q->next;
    }
    tab *temp  = (tab *)(q->data);
    stack_t *mv = st_create(sizeof(page *));
    while(!st_is_empty(temp->forw))
    {
        st_push(mv,st_peek(temp->forw));
        st_pop(temp->forw);
    }
    while(!st_is_empty(mv))
    {
        page *p;
        memcpy(&p, st_peek(mv), sizeof(p));
        fprintf(out, "%s\n", p->url);

        st_push(temp->forw,st_peek(mv));
        st_pop(mv);
    }

    fprintf(out,"%s\n",(((tab *)(q->data))->curr_page)->url);

    while(!st_is_empty(temp->back))
    {
        page *p;
        memcpy(&p, st_peek(temp->back), sizeof(p));
        fprintf(out, "%s\n", p->url);
        st_push(mv,st_peek(temp->back));
        st_pop(temp->back);
    }
    while(!st_is_empty(mv))
    {
        st_push(temp->back,st_peek(mv));
        st_pop(mv);
    }
    st_free(mv);
}

int read_sites(FILE *f, page sites[]) {
    int sites_number;
    if (fscanf(f, "%d", &sites_number) != 1) {
        fprintf(stderr, "403 Forbidden\n");
        printf("WHAT\n");;
    }

    sites[0].id = 0;
    strcpy(sites[0].url, "https://acs.pub.ro/");
    sites[0].description = malloc(MAX_DESC_LEN * sizeof(char));
    if (!sites[0].description) {
        perror("malloc");
        return errno;
    }
    strcpy(sites[0].description, "Computer Science");

    for (int i = 1; i <= sites_number; i++) {
        if (fscanf(f, "%d\n", &sites[i].id) != 1) printf("WHAT\n");;
    
        if (fgets(sites[i].url, MAX_URL_LEN, f) == NULL) printf("WHAT\n");;
        sites[i].url[strcspn(sites[i].url, "\n")] = '\0';
    
        sites[i].description = malloc(MAX_DESC_LEN * sizeof(char));
        if (!sites[i].description) {
            perror("malloc");
            return errno;
        }
    
        if (fgets(sites[i].description, MAX_DESC_LEN, f) == NULL) {
            free(sites[i].description);
            printf("WHAT\n");;
        }
        sites[i].description[strcspn(sites[i].description, "\n")] = '\0';
    }
    

    return sites_number;
}

int open_files(FILE **in, FILE **out) {
    *in = fopen("tema1.in", "r");
    if (!*in) {
        perror("fopen tema1.in");
        return errno;
    }

    *out = fopen("tema1.out", "w");
    if (!*out) {
        perror("fopen tema1.out");
        fclose(*in);
        return errno;
    }

    return 0;
}

// NEW_TAB
void new_tab(browser *br, page *default_page) {
    tab *new_tab = malloc(sizeof(tab));
    new_tab->id = br->next_id++;
    new_tab->curr_page = &default_page[0];
    new_tab->back = st_create(sizeof(page *));
    new_tab->forw = st_create(sizeof(page *));

    dll_node_t *node = malloc(sizeof(dll_node_t));
    node->data = new_tab;

    node->next = br->sen;
    node->prev = br->sen->prev;
    br->sen->prev->next = node;
    br->sen->prev = node;

    br->curr = node;
}

// CLOSE
void close_curr_tab(browser *br, FILE *out) {
    tab *curr_tab = (tab *)br->curr->data;
    if (curr_tab->id == 0) {
        fprintf(out,"403 Forbidden\n");
        return;
    }

    dll_node_t *to_remove = br->curr;
    dll_node_t *left = br->curr->prev;

    to_remove->prev->next = to_remove->next;
    to_remove->next->prev = to_remove->prev;

    br->curr = left; 

    st_free(curr_tab->back);
    st_free(curr_tab->forw);
    free(curr_tab);
    free(to_remove);
}

// OPEN <ID>
void switch_tabs(browser *br, int e) {
    dll_node_t *it = br->sen->next;
    while (it != br->sen) {
        tab *current_tab = (tab*)it->data;
        if (current_tab->id == e) {
            br->curr = it;
            return;
        }
        it = it->next;
    }
    printf("Error: Tab with ID %d not found.\n", e);
}

// NEXT
void next_tab(browser *br)
{
    dll_node_t *temp = br->curr->next;
    if(temp == br->sen)
    {
        temp = temp->next;
    }
    br->curr = temp;
}

// PREV
void prev_tab(browser *br)
{
    dll_node_t *temp = br->curr->prev;
    if(temp == br->sen)
    {
        temp = temp->prev;
    }
    br->curr = temp;
}

void browser_destroy(browser *br) {
    if (!br) return;
    dll_node_t *it = br->sen->next;
    while (it != br->sen) {
        dll_node_t *next = it->next;
        tab *t = it->data;
        st_free(t->back);
        st_free(t->forw);
        free(t);
        free(it);
        it = next;
    }
    free(br->sen);
    free(br);
}

int main(void)
{
    page sites[MAX_SITES];
    FILE *in, *out;
    if (open_files(&in, &out) != 0) {
        printf("WHAT\n");
    }
    int sites_number = read_sites(in, sites);
    browser *browser = browser_create(sites);
    int queries;
    if(!fscanf(in, "%d", &queries))
    {
        fprintf(stderr, "Error reading queries count.\n");
        fclose(in);
        fclose(out);
        printf("WHAT\n");
    }
    fgetc(in);
    for (int i = 0; i < queries; i++) {
        char query[MAX_QUERY_LEN+1];
        if (fgets(query, MAX_QUERY_LEN, in) == NULL) printf("WHAT\n");
        query[strcspn(query, "\n")] = '\0';
        if(!strcmp(query,"NEW_TAB"))
        {
            new_tab(browser,sites);
        }
        else if(!strcmp(query,"CLOSE"))
        {
            close_curr_tab(browser, out);
        }
        else if (strstr(query, "OPEN")) {
            char tab_num[20];
            strcpy(tab_num, query + 5);
            int target_id = atoi(tab_num);
            int tab_exists = 0;
            dll_node_t *it = browser->sen->next;
            while (it != browser->sen) {
                if (((tab*)it->data)->id == target_id) {
                    tab_exists = 1;
                    break;
                }
                it = it->next;
            }
            
            if (tab_exists) {
                switch_tabs(browser, target_id);
            } else {
                fprintf(out, "403 Forbidden\n");
            }
        }
        else if(!strcmp(query,"NEXT"))
        {
            next_tab(browser);
        }
        else if(!strcmp(query,"PREV"))
        {
            prev_tab(browser);
        }
        else if(strstr(query,"PAGE"))
        {
            char tab_num[20];
            strcpy(tab_num, query + 5);
            
            int target_id = atoi(tab_num);
            int page_exists = -1;
            for(int i = 0;i<=sites_number;i++)
            {
                if((sites[i]).id == target_id)
                {
                    page_exists = i;
                    break;
                }
            }
            if (page_exists != -1) {
                open_page(browser, page_exists, sites);
            } else {
                fprintf(out, "403 Forbidden\n");
            }
        }
        else if(!strcmp(query,"BACKWARD"))
        {
            go_down(browser,out);
        }
        else if(!strcmp(query,"FORWARD"))
        {
            go_up(browser,out);
        }
        else if(!strcmp(query,"PRINT"))
        {
            print_all(browser,out);
        }
        else if(strstr(query,"PRINT_HISTORY"))
        {
            char tab_num[20];
            strcpy(tab_num, query + 13);
            
            int target_id = atoi(tab_num);
            int tab_exists = 0;
            dll_node_t *it = browser->sen->next;
            while (it != browser->sen) {
                if (((tab*)it->data)->id == target_id) {
                    tab_exists = 1;
                    break;
                }
                it = it->next;
            }
            
            if (tab_exists) {
                print_history(browser, target_id, out);
            } else {
                fprintf(out, "403 Forbidden\n");
            }
        }
        else
        {
            fprintf(out,"BUG\n");
        }
        
    }

    for(int i = 0;i<=sites_number;i++)
    {
        free(sites[i].description);
    }

    browser_destroy(browser);
    fclose(in);
    fclose(out);

}
