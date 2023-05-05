#include <fstream>

#include "page.h"

using std::endl;
using std::ofstream;

class Board {
    public:
        Board(int num_jobs, int width, int height, ofstream& output_stream);
        ~Board();

        void print_board();
        void print_job(int job_idx, char job_type, int id);

        //job functions
        void insert_page(int x, int y, int width, int height, int id, int content);
        void delete_page(int id);
        void modify_content(int id, char content);
        void modify_position(int id, int x, int y);
        void insert_iteration(int starting_index, int ending_index);

    private:
        int num_jobs, width, height; 
        ofstream& output; 
        char* board;
        Page* pagetrack;

};


Board::Board(int num_jobs, int width, int height, ofstream& output_stream): output(output_stream) {
    this->width = width;
    this->height = height;
    this->num_jobs = num_jobs;

    board = new char[width*height];

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            board[h*width + w] = ' ';
        }
    }
    // initailizing pagetrack pointer, page track can save informaion of inserting pages.
    pagetrack = new Page[num_jobs];
    
}

Board::~Board(){
    delete board;
    
}


void Board::print_board() {
    int h, w;
    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
    
    for (h = 0; h < height; h++) {
        output << "| ";
        for (w = 0; w < width; w++) {
            output << board[h*width + w] << " ";
        }
        output << "| " << endl;
    }

    for (w = 0; w < width+2; w++) output << "- ";
    output << endl;
}

void Board::print_job(int job_idx, char job_type, int id) {
    output << ">> (" << job_idx <<") ";
    switch(job_type) {
        
        case 'i':
            output << "Insert ";
            break;
        case 'd':
            output << "Delete ";
            break;
        case 'm':
            output << "Modify ";
            break;
    }

    output << id << endl;
}
    

void Board::insert_page(int x, int y, int width, int height, int id, int content) {
    //tracking insert_call
    int i = 0;
    while(pagetrack[i].getid() != -1 ){ //insert된 page들을 기록.
        i++;
    }
    pagetrack[i] = Page(x, y, width, height, id, content);



    //changing the board content
    for (int current_x=x; current_x<x+width; current_x++)
    {
        for (int current_y=y; current_y<y+height; current_y++)
        {
            if (board[current_y*this->width + current_x] != ' ')
            {
                for(int p=0; p<i; p++)
                {
                    if (pagetrack[p].getid() > -1)
                    {
                        pagetrack[p].is_above(pagetrack[i]);
                    }
                }
            }
            board[current_y * this->width + current_x] = (char)content;
        }
    }
    print_board();
}

void Board::delete_page(int id) {
    int i = 0;
    while( pagetrack[i].getid() != id){
        i++;
    }
    
    insert_iteration(i, num_jobs);
    print_board();
}

void Board::modify_content(int id, char content) {
    int i = 0;
    while( pagetrack[i].getid() != id){
        i++;
    }
    pagetrack[i].setcontent((int)content);
    insert_iteration(i, num_jobs);
}
void Board::modify_position(int id, int x, int y) {
    int i = 0;
    while( pagetrack[i].getid() != id){
        i++;
    }
    pagetrack[i].setX(pagetrack[i].getX()+x);
    pagetrack[i].setX(pagetrack[i].getY()+y);

    insert_iteration(i, num_jobs);  
}


void Board::insert_iteration(int starting_index, int ending_index){

    for(int i = starting_index; i<ending_index; i++)
    {   
        if(pagetrack[i].getid() == -1 )
        {
            break;
        }
        for (int current_x = pagetrack[i].getX(); current_x < pagetrack[i].getX()+pagetrack[i].getwidth(); current_x++)
        {
        for (int current_y = pagetrack[i].getY(); current_y < pagetrack[i].getY()+pagetrack[i].getheight(); current_y++)
            {
            board[current_y * this->width + current_x] = pagetrack[i].getcontent();
            }
        }
        print_board();
    }
    //reflect pagetrack iteration on board. 
}