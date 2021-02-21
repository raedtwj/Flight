#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* struct */
typedef struct Node Node;
typedef struct LinkedList LinkedList;
typedef struct Flight Flight;
typedef struct Passenger Passenger;
typedef struct Business Business;
typedef struct Economy Economy;
typedef struct Baggage Baggage;

struct Node
{
    void *data;
    Node *next;
};
struct LinkedList
{
    Node *head;
};
struct Flight
{
    LinkedList *passengers;
    int flightNo;
    char from[86];
    char to[86];
};
struct Passenger
{
    char name[50];
    int seatNo;
    char type[15];
    LinkedList *baggage;
};
struct Economy
{
    Passenger *p;
    char category[15];
};
struct Business
{
    Passenger *p;
    int reward_points;
};
struct Baggage
{
    int id;
    double weight;
};

/* Node*/

Node *create_node(void *data)
{
    Node *n = (Node *)malloc(sizeof(Node));
    n->data = data;
    n->next = NULL;
    return n;
}

/* LinkedList */

LinkedList *create_list()
{
    LinkedList *l = (LinkedList *)malloc(sizeof(LinkedList));
    l->head = NULL;
    return l;
}

void printList(LinkedList *l, void (*printer)(void *)) /* at 2nd arg pass function pointer that take data as arg and printit */
{
    Node *n = l->head;
    while (n != NULL)
    {
        (*printer)(n->data);
        n = n->next;
    }
}
void addToList(LinkedList *l, void *data)
{
    Node *n = create_node(data);
    if (l->head == NULL)
    {
        l->head = n;
        return;
    }
    Node *cur = l->head;
    while (cur->next != NULL)
    {
        cur = cur->next;
    }
    cur->next = n;
}

/* Passenger */

Passenger *create_passenger(char *name, int seat, char *type)
{
    Passenger *p = (Passenger *)malloc(sizeof(Passenger));
    p->baggage = create_list();
    strcpy(p->name, name);
    p->seatNo = seat;
    strcpy(p->type, type);
    return p;
}

char *get_name(Passenger *p)
{
    return p->name;
}
int get_seat(Passenger *p)
{
    return p->seatNo;
}
char *get_ptype(Passenger *p)
{
    return p->type;
}
bool add_baggage(Passenger *p, Baggage *b)
{
    addToList(p->baggage, b);
    return true;
}

double get_total_weight(Passenger *p)
{
    Node *n = p->baggage->head;
    double total = 0.0;
    while (n != NULL)
    {
        Baggage *b = (Baggage *)n->data;
        total += b->weight;
        n = n->next;
    }
    return total;
}

Baggage *get_heaviest_baggage(Passenger *p)
{
    Node *n = p->baggage->head;
    Baggage *heavest;
    Baggage *cur;
    double wightOfHeavest = 0.0;
    while (n != NULL)
    {
        cur = (Baggage *)(n->data);
        if (cur->weight > wightOfHeavest)
        {
            heavest = cur;
            wightOfHeavest = cur->weight;
        }
        n = n->next;
    }
    return heavest;
}

/* Flight */

Flight *create_flight(int no, char *from, char *to)
{
    Flight *f = (Flight *)malloc(sizeof(Flight));
    f->passengers = create_list();
    f->flightNo = no;
    strcpy(f->from, from);
    strcpy(f->to, to);
    return f;
}
bool add_business_passenger(Flight *f, Business *p)
{
    addToList(f->passengers, p);
    return true;
}
bool add_economy_passenger(Flight *f, Economy *p)
{
    addToList(f->passengers, p);
    return true;
}
Passenger *get_passenger(Flight *f, char *type)
{
    Node *n = f->passengers->head;
    Passenger *a;
    int count = 0;
    while (n != NULL)
    {
        a = ((Business *)n->data)->p;
        if (strcmp(a->type, type) == 0)
        {
            count++;
        }
        n = n->next;
    }
    Passenger *p = (Passenger *)malloc(count * sizeof(Passenger));
    count = 0;
    n = f->passengers->head;
    while (n != NULL)
    {
        a = ((Business *)n->data)->p;
        if (strcmp(a->type, type) == 0)
        {
            p[count] = *a;
            count++;
        }
        n = n->next;
    }
    return p;
}
void save_to_file(Flight *f, char *fname)
{
    FILE *file = fopen(fname, "wb");
    if (file == NULL)
    {
        printf("write to file fails :(");
    }
    Node *baggage, *n = f->passengers->head;
    int baggageSize;
    Economy *e;
    Business *b;
    Passenger *p;
    while (n != NULL)
    {
        p = ((Business *)n->data)->p;
        fwrite(p, sizeof(Passenger), 1, file);
        baggage = p->baggage->head;
        baggageSize = 0;
        while (baggage != NULL)
        {
            baggageSize++;
            baggage = baggage->next;
        }
        fwrite(&baggageSize, sizeof(int), 1, file);
        baggage = p->baggage->head;
        while (baggage != NULL)
        {
            fwrite(baggage->data, sizeof(Baggage), 1, file);
            baggage = baggage->next;
        }

        if (strcmp(p->type, "Economy") == 0)
        {
            e = (Economy *)n->data;
            fwrite(e->category, 15, 1, file);
        }
        else if (strcmp(p->type, "Business") == 0)
        {
            b = (Business *)n->data;
            fwrite(&b->reward_points, sizeof(int), 1, file);
        }
        n = n->next;
    }
    fclose(file);
}
void read_from_file(Flight *f, char *fname)
{
    FILE *file = fopen(fname, "rb");
    if (file == NULL)
    {
        printf("read from file fails :(");
    }
    int baggageSize;
    Economy *e;
    Business *b;
    Passenger *p = (Passenger *)malloc(sizeof(Passenger));
    Baggage *bag;
    while (fread(p, sizeof(Passenger), 1, file) > 0) /* read data from file and save it at n */
    {
        p->baggage = create_list();
        fread(&baggageSize, sizeof(int), 1, file);
        for (size_t i = 0; i < baggageSize; i++)
        {
            bag = (Baggage *)malloc(sizeof(Baggage));
            fread(bag, sizeof(Baggage), 1, file);
            addToList(p->baggage, bag);
        }
        if (strcmp(p->type, "Economy") == 0)
        {
            e = (Economy *)malloc(sizeof(Economy));
            e->p = p;
            fread(e->category, 15, 1, file);
            addToList(f->passengers, e);
        }
        else if (strcmp(p->type, "Business") == 0)
        {
            b = (Business *)malloc(sizeof(Business));
            b->p = p;
            fread(&b->reward_points, sizeof(int), 1, file);
            addToList(f->passengers, b);
        }
        p = (Passenger *)malloc(sizeof(Passenger));
    }
    fclose(file);
}

/* Business */

Business *create_business_passenger(char *name, int seat, int reward)
{
    Business *b = (Business *)malloc(sizeof(Business));
    b->p = create_passenger(name, seat, "Business");
    b->reward_points = reward;
    return b;
}

int get_reward(Business *b)
{
    return b->reward_points;
}

/* Economy */

Economy *create_economy_passenger(char *name, int seat, char *cat)
{
    Economy *e = (Economy *)malloc(sizeof(Economy));
    e->p = create_passenger(name, seat, "Economy");
    strcpy(e->category, cat);
    return e;
}

char *get_category(Economy *e)
{
    return e->category;
}

/* Baggage */

Baggage *create_baggage(int id, double w)
{
    Baggage *b = (Baggage *)malloc(sizeof(Baggage));
    b->id = id;
    b->weight = w;
    return b;
}
int get_id(Baggage *b)
{
    return b->id;
}
double get_weight(Baggage *b)
{
    return b->weight;
}

/* printers */

void printBaggage(void *data)
{
    Baggage *b = (Baggage *)data;
    printf("\t\tid : %d, weight : %.2f Kg\n", b->id, b->weight);
}
void printPassenger(void *data)
{
    /* print passenger of type Economy or Business only not of type Passenger */
    Passenger *p = ((Business *)data)->p;
    char *type = p->type;
    printf("\tname : %s, seat number : %d, type : %s", p->name, p->seatNo, p->type);
    if (strcmp(type, "Economy") == 0)
    {
        Economy *e = (Economy *)data;
        printf(", category : %s", e->category);
    }
    else
    {
        Business *b = (Business *)data;
        printf(", reward point : %d", b->reward_points);
    }
    printf("\n\t\tBaggages :-\n");
    printList(p->baggage, &printBaggage);
}
void printFlight(Flight *f)
{
    printf("flight number : %d, from : %s, to : %s\n\tPassengers :-\n", f->flightNo, f->from, f->to);
    printList(f->passengers, &printPassenger);
}
int main()
{
    Flight *f = create_flight(235, "jeddah", "riyadh");

    Economy *pass1 = create_economy_passenger("ali", 25, "catagory");
    add_baggage(pass1->p, create_baggage(12, 13));
    add_economy_passenger(f, pass1);
    Business *pass2 = create_business_passenger("raed", 26, 99);
    add_baggage(pass2->p, create_baggage(13, 17));
    add_baggage(pass2->p, create_baggage(14, 23));
    add_business_passenger(f, pass2);
    printf("heaviest : %.2f\n", get_heaviest_baggage(pass2->p)->weight);
    printf("total weight : %.2f\n", get_total_weight(pass2->p));
    save_to_file(f, "data");
    read_from_file(f, "data");
    printFlight(f);
    return 0;
}