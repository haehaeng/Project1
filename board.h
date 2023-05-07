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
        
        //extra functions for implementing job functinons
        void remove_pages(int id); // argument: page wanted. Get rid of pages above discussed page.
        void return_pages();
        void insert_only(Page inserted); // reflect page(input) onto board. (Not printing board.)
    private:
        int num_jobs, width, height, insert_count ;
        ofstream& output; 
        char* board;
        //extra member for implementing job functions
        Page* pagetrack; // track of all pages inserted. Saving page size, position, id, index each.
        std::vector<int> turn_back; // track of pages that sould be posted back.

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
    insert_count = 1;
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

    while(pagetrack[insert_count-1].getid() != -1 ){ //insert된 page들을 기록.
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
    remove_pages(id);
    int target=0;
    for(; target<insert_count; target++){
        if (pagetrack[target].getid() == id){
            break;
        }
    }
    pagetrack[target] = Page();
    return_pages();
    //page가 delete 됐으므로 Pagetrack에 들어있는 해당 page가 무용지물이 되게 조치해야 함.
    // 그렇지 않으면 이후 동작에서 출력되어버림.
}

void Board::modify_content(int id, char content) {
    remove_pages(id);
    int i = 0;
    while( pagetrack[i].getid() != id){
        i++;
    }
    pagetrack[i].setcontent((int)content);
    insert_only(pagetrack[i]);
    print_board();
    return_pages();
}

void Board::modify_position(int id, int x, int y) {
    remove_pages(id);
    int i = 0;
    while( pagetrack[i].getid() != id){
        i++;
    }
    pagetrack[i].setX(x);
    pagetrack[i].setY(y);
    insert_only(pagetrack[i]);
    print_board();
    return_pages();
    //position 바뀔 때 각 Page class의 above vector 의 update 필요    
    
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

    
    auto iter = find(turn_back.begin(), turn_back.end(), id);
    if(iter == turn_back.end())
    {   
        turn_back.push_back(id); // 지워진 page의 id를 turn_back vetor의 맨 뒤로 push
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
            board[h*width + w] = ' ';
            }
        } // 빈 board 호출
        for(int i=0; i<insert_count; i++){
            iter = find(turn_back.begin(), turn_back.end(),pagetrack[i].getid());
            if(iter == turn_back.end()&&pagetrack[i].getid()!=-1){
                 insert_only(pagetrack[i]);
            }
        }
        print_board();
    }
}

void Board::return_pages(){
    int id;
    turn_back.pop_back(); // return_pages()함수가 call될 때 turn_back vector의 맨 뒷 값은 지워져야 할 page이므로 먼저 지운다.
    while(!turn_back.empty()) // 복구해야 될 page가 남아있다면
    {
        id = turn_back.back(); // 맨 뒤에서부터 id를 받아와
        turn_back.pop_back(); // 물론 받아온 다음엔 지우고
        { // target id를 가진 page를 pagetrack 내에서 찾는다. output -> index int.
        int target = 0;
            for(; target <insert_count; target++){
                if (pagetrack[target].getid() == id){
                break;
                }
            }
        insert_only(pagetrack[target]); // 찾아낸 page를 board에 출력한다.
        print_board();
        // board에 변경사항이 생겼으므로 board를 출력한다.
    }
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