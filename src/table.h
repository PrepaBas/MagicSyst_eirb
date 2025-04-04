#ifndef table_h
#define table_h


struct position {
    float x;
    float y;
    float theta;
};


struct table {
    struct position bl;
    struct position tl;
    struct position br;
    struct position tr;
    struct position extreme;
};


table table_coupe = {{650, 670}, {650, 1300}, {2340, 670}, {2340, 1300}, {3000, 2000}};

#endif