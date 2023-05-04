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
        void get_board(int index);
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
    /* 
    Inserting page means as like building up "ingredients" that we can modify of delete.
    So when pages are inserted, tracking might have to follow.
    We should check overlapping over other pages to implement modify & delete.
    How could I check? 
    way1 : save the board whenever functions executed.
        detail :
        1. [FOR REEXECUTIONS OF INSERTING]
            When I execute functions, preserve the sequence of function calls. -> need command array or stack which is member of Board::
            at least we need order of pages inserted. 
                 -> Whenever insert function called, we will preserve the page id sequentially.
                is it okay to only save sequence? Can I exectue the functions again only by page id?
                We should track not only the page id, but position, content, width, height. Luckily all of'em are int type.
                So let's put them in one int array[6] when every inserting occurs.
                    int**pagetrack = new int*[toatal command counts]
                    pagetrack[i][0], page[i][1] -> position
                    pagetrack[i][2], page[i][3] - width, height
                    pagetrack[i][4] -> page id
                    pagetrack[i][5] -> page content
                We will access page by page id. So when we try to find page, use for loop. i= 0~num_jobs, if pagetrack[i][4] == finding id.

        2. [JUST TO GO BACK]
        Before I insert new page, get track of current board 
            / -> need **c_board = new char*[total command counts], at c_board[N], the Nth current board saved
            / _ Array length equals total command counts.
            /So, get track of the board by saving the current board on board[command order]
            /When modify or delete function is called, we'll go back to the board of the page discussed. And execute the function.
            /And then insert pages after that in turn again.
            /So, we should get track of Functions. At least, we should put attention on 'INSERT'.

            <Revision>
            Should we have to track????
            We can just insert all the things again before the discussed page.
            find the value i by searching issued id.

            And insert pages til the page.

        
        3. Abstraction of opertaions _ modifying and deletion.
            - modifying postion. 
            go back to the page issued. (board before the page inserted.) -> modifying pagetrack[i][0], pagetracj[i][1] into proper value.
            -> iteration of inserting in turn.
            - modifying content.
            go back to the page issued. (board before the page inserted.) -> modify pagetrack[i][5] -> iteration of inserting in turn.
            - delete page.
            go back to the page issued. (board before the page inserted.) -> delete the track. -> iteration of inserting in turn.

            Let's implement
            1. int** pagetrack -> How about vector? not array. Vector whose content is vector.
                - Board member
                - int**pagetrack = new int*[num_jobs]
                    pagetrack[i][0], page[i][1] -> position
                    pagetrack[i][2], page[i][3] - width, height
                    pagetrack[i][4] -> page id
                    pagetrack[i][5] -> page content
                - when inserting something put inserting informatino into array.
                    use while iteration.
                    int i = 0
                    while(pagetrack[i] != null)
                    {i++;}
                    pagetrack[i] = 에 저장.
            2. inserting pages by pagetrack[][] informations _--> do with function.
            
                void insert_iteration(int start_index, int end_index){
                    starting_index부터 end_index 미만까지
                    pagetrack에 저장된 정보대로 insert
                    tracking이 아닌 board update만이 목적
                }
            
            3. deletion and modifying some alterations.



    way2 : tracking the board by saving all history of board parts.
        detail:
        define **history which save the board history
        but,, it is 
        inefficient!*/
     

void Board::insert_page(int x, int y, int width, int height, int id, int content) {
    //tracking insert_call
    int i = 0;
    while(pagetrack[i].getid() > 0 ){
        i++;
    }
    pagetrack[i] = Page(x, y, width, height, id, content);

    //changing the board content
    for (int current_x=x; current_x<x+width; current_x++)
    {
        for (int current_y=y; current_y<y+height; current_y++)
        {
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
    get_board(i);
    insert_iteration(i, num_jobs);
    print_board();
}

void Board::modify_content(int id, char content) {
    int i = 0;
    while( pagetrack[i].getid() != id){
        i++;
    }
    pagetrack[i].setcontent((int)content);
    get_board(i);
    insert_iteration(i, num_jobs);
}
void Board::modify_position(int id, int x, int y) {
    int i = 0;
    while( pagetrack[i].getid() != id){
        i++;
    }
    pagetrack[i].setX(pagetrack[i].getX()+x);
    pagetrack[i].setX(pagetrack[i].getY()+y);
    get_board(i);
    insert_iteration(i, num_jobs);  
}

void Board::get_board(int index){

    for(int i = 0; i< index; i++)
    {   
        if(pagetrack[i].getid() == -1 )
        {
            break;
        }
        for (int current_x = pagetrack[i].getX(); current_x < pagetrack[i].getX()+pagetrack[i].getwidth(); current_x++)
        {
        for (int current_y = pagetrack[i].getY(); current_y < pagetrack[i].getY()+pagetrack[i].getheight(); current_y++)
            {
            board[current_y * this->height + current_x] = pagetrack[i].getcontent();
            }
        }
    }
    print_board();
    //reflect pagetrack iteration on board. 
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
            board[current_y * this->height + current_x] = pagetrack[i].getcontent();
            }
        }
        print_board();
    }
    //reflect pagetrack iteration on board. 
}