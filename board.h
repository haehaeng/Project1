#include <fstream>
#include "page.h"
#include <vector>

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
        int num_jobs, width, height ;
        ofstream& output; 
        char* board;
        //extra member for implementing job functions
        std::vector<Page> pagetrack; // track of all pages inserted. Saving page size, position, id, index each.
        std::vector<Page> turn_back; // track of pages that sould be posted back.

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
    pagetrack.push_back(Page(x,y,width, height, id, content));

    for(int p=0; p<pagetrack.size()-1; p++)
    {
        if(pagetrack[p].is_above(pagetrack.back())) // 지금까지 insert된 page를 모두 찾아와 insert 하려는 page와 겹치는지 확인
        {
            pagetrack[p].abovepage(id);   // 겹친다면 아래에 있는 page의 above 벡터에 insert된 page id를 push_back()
            pagetrack[p].sort_above();
            
        }    
    }
    //changing the board content
    for (int current_x=x; current_x<x+width; current_x++)
    {
        for (int current_y=y; current_y<y+height; current_y++)
        {   // insert 하려는 board 위의 각 좌표에 대해
            board[current_y * this->width + current_x] = (char)content; // 모두 기록한 뒤 insert 진행    
        }
    }
    print_board();
}

void Board::delete_page(int id) {
    remove_pages(id);
    turn_back.pop_back();
    return_pages();
    //page가 delete 됐으므로 Pagetrack에 들어있는 해당 page가 무용지물이 되게 조치해야 함.
    // 그렇지 않으면 이후 동작에서 출력되어버림.
}

void Board::modify_content(int id, char content) {
    
    remove_pages(id);
    turn_back.back().setcontent(content);
    return_pages();
}

void Board::modify_position(int id, int x, int y) {
    remove_pages(id);
    turn_back.back().setX(x);
    turn_back.back().setY(y);
    return_pages();
    //position 바뀔 때 각 Page class의 above vector 의 update 필요    
    //아예 새로운 insertion이라고 생각하자 다만, 즉 input id를 가지는 page는 pagetrack 내에서 remove된 page를 제외한 stack에서 맨 뒤

    
}

void Board::remove_pages(int id){
    // 함수가 받은 id의 page 위의 page들을 pagetrack vec에서 turn_back vec으로 옮기며 보드를 출력
    // target 위의 모든 page들을 remove -> 여러 개라면  id가 작은 것부터
    // 결국 target page도 옮겨진다.
    
    // input it를 가진 target page를 찾는다.
    int target=0;
    for(; target<pagetrack.size(); target++){
        if (pagetrack[target].getid() == id){
            break;
        }
    }

    // target page 위의 page들의 집합 pagetrack[i].getabove()는 오름차순 sorting이 되어있다.
    // target page 위의 page들을 제거하자.
    while(pagetrack[target].getabove().size() > 0)
    {
        
        for(int i_above = 0; i_above < pagetrack[target].getabove().size(); i_above++)
        {
            if(pagetrack[target].getabove()[i_above] > -1) // id가 0이상이면 (del된 page 및 default id=-2 제외)
            {   
            remove_pages(pagetrack[target].getabove()[i_above]);
            // 위의 모든 page들을 remove_pages
            }
        }
    }
    
    bool existing = 0;
    for(int i = 0; i < turn_back.size(); i++)
    {
        if(turn_back[i].getid() == pagetrack[target].getid())
        {
            existing = 1;
        }
        
    }

    if(!existing) // turn_back에 없다면
    {   
        turn_back.push_back(pagetrack[target]); // 지워진 page를 turn_back vetor의 맨 뒤로 push
        for (int i = 0; i < pagetrack.size();i++) // page가 turn_back page에 저장되면 pagetrack에서 삭제
        {
            if(pagetrack[i].getid() == pagetrack[target].getid())
            {
                pagetrack.erase(pagetrack.begin()+i);
            }
        }
        
        for (int i = 0 ; i< pagetrack.size() ; i++)
        {
            for(int j = 0; j <pagetrack[i].getabove().size() ; j++)
            {
                if(pagetrack[i].getabove()[j] == id)
                {
                    pagetrack[i].delete_above(j);
                }
            }
        }
        // 빈 board를 호출
        for (int h = 0; h < height; h++) {
            for (int w = 0; w < width; w++) {
            board[h*width + w] = ' ';
            }
        }

        //pagetrack update된 후 pagetrack에 저장된 page들 순서대로 싹 다 출력
        for(int i=0; i<pagetrack.size(); i++){
             insert_only(pagetrack[i]);
        }
        print_board();
        }
}


void Board::return_pages(){
    // turn_back vector에 있는 page들을 하나씩 새로 insert_page
    while(!turn_back.empty()) // 복구해야 될 page가 남아있다면
    {
        insert_page(turn_back.back().getX(), turn_back.back().getY(), turn_back.back().getwidth(), turn_back.back().getheight(), turn_back.back().getid(), turn_back.back().getcontent()); // 맨 뒤에서부터 id를 받아와
        turn_back.pop_back(); // 물론 받아온 다음엔 지우고 
        // board에 변경사항이 생겼으므로 board를 출력한다.
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