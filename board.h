#include <fstream>
#include "page.h"
#include <vector>
#include <algorithm>

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
        void remove_pages(int id); // argument: page wanted. Get rid of pages above discussed page.
        void insert_only(Page inserted);

    private:
        int num_jobs, width, height, insert_count=0 ;
        ofstream& output; 
        char* board;
        Page* pagetrack;
        std::vector<int> turn_back;

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
    while(pagetrack[insert_count].getid() != -1 ){ //insert된 page들을 기록.
        insert_count++;
    }
    pagetrack[insert_count-1] = Page(x, y, width, height, id, content);
    pagetrack[insert_count-1].setboard(board); //insert 전 상태의 board를 기록.

    //changing the board content
    for (int current_x=x; current_x<x+width; current_x++)
    {
        for (int current_y=y; current_y<y+height; current_y++)
        {   // insert 하려는 board 위의 각 좌표에 대해

            if (board[current_y*this->width + current_x] != ' ') // content를 update하기 전에 이미 값이 있었다면
            {
                for(int p=0; p<insert_count; p++)
                {
                    if(pagetrack[p].is_above(pagetrack[insert_count])) // 지금까지 insert된 page를 모두 찾아와 insert 하려는 page와 겹치는지 확인
                    {
                        pagetrack[p].abovepage(pagetrack[insert_count].getid());   // 겹친다면 아래에 있는 page의 above 벡터에 insert된 page id를 push_back()
                        sort(pagetrack[p].getabove().begin(), pagetrack[p].getabove().end());
                    }
                    
                }
            }
            board[current_y * this->width + current_x] = (char)content; // 모두 기록한 뒤 insert 진행
            
        }
    }
    print_board();
}

void Board::delete_page(int id) {
    int i = 0;
    while( pagetrack[i].getid() != id){
        i++;
    }
    
    print_board();
}

void Board::modify_content(int id, char content) {
    int i = 0;
    while( pagetrack[i].getid() != id){
        i++;
    }
    pagetrack[i].setcontent((int)content);

}

void Board::modify_position(int id, int x, int y) {
    int i = 0;
    while( pagetrack[i].getid() != id){
        i++;
    }
    pagetrack[i].setX(pagetrack[i].getX()+x);
    pagetrack[i].setX(pagetrack[i].getY()+y);

    
}

void Board::remove_pages(int id){
    // 함수가 받은 id의 page 위의 page들을 한 장씩 제거하고 보드를 출력
    // target 위의 모든 page들을 remove -> 여러 개라면  id가 작은 것부터
    // 결국 target page도 지워진다.
    // input it를 가진 target page를 찾는다.
    int target=0;
    for(; target<insert_count; target++){
        if (pagetrack[target].getid() == id){
            break;
        }
    }

    // target page 위의 page들의 집합 pagetrack[i].getabove()는 오름차순 sorting이 되어있다.
    // target page 위의 page들을 제거하자.
    if(!pagetrack[target].getabove().empty())
    {
        std::vector<int>::iterator it;
        for(it = pagetrack[target].getabove().begin(); it < pagetrack[target].getabove().end(); it++)
        {
            remove_pages(*it);
            // 위의 모든 page들을 remove_pages
            // above page 위에 또 above page가 있다면 중복이 발생할 수 있다!
        }
    }

    // 해당 page 위의 page가 아니지만, 해당 page가 insert 된 후에 insert된 경우
    auto iter = find(turn_back.begin(), turn_back.end(), id);
    if(iter == turn_back.end())
    {   
        turn_back.push_back(id); // 지워진 page의 id를 turn_back vetor의 맨 뒤로 push
        board = pagetrack[0].getboard(); // 빈 board 호출
        for(int i=0; i<insert_count; i++){
            iter = find(turn_back.begin(), turn_back.end(),pagetrack[i].getid());
            if(iter == turn_back.end()){
                 insert_only(pagetrack[i]);
            }
        }
        print_board();
    }
}


void Board::insert_only(Page inserted){

    for (int current_x = inserted.getX(); current_x <inserted.getX()+inserted.getwidth(); current_x++)
    {
        for (int current_y = inserted.getY(); current_y < inserted.getY()+inserted.getheight(); current_y++)
        {
        board[current_y * this->width + current_x] = inserted.getcontent();
        }
    }    //just print Page on board. 
}