#include <vector>
#include <math.h>
#include <algorithm>

class Page {
    public:
        Page();
        Page(int x, int y, int width, int height, int id, int content);
        
        void setX(int x){this->x = x;}
        int getX(){return x;}
        void setY(int y){this->y = y;}
        int getY(){return y;}
        void setwidth(int width){this->width = width;}
        int getwidth(){return width;}
        void setheight(int height){this->height = height;}
        int getheight(){return height;}
        void setid(int id){this->id = id;}
        int getid(){return id;}
        void setcontent(int content){this->content = content;}
        char getcontent(){return content;}
        std::vector<int> getabove(){return above;}

        void abovepage(int above_page);
        bool is_above(Page newpg);
        void sort_above();
        void empty_above();
        void delete_above(int index);
    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id=-2; // unique id for each page
        char content;
        std::vector<int> above; // 위에 있는 page id를 저장할 vector insert 되는 순서대로 push_back된다.
};
    Page::Page(){}    
    Page::Page(int x, int y, int width, int height, int id, int content):x(x),y(y),width(width),height(height), id(id), content((char)content){}
    void Page::abovepage(int above_page){
        this->above.push_back(above_page);
    }
    void Page::empty_above()
    {
        above.clear();
    }

    bool Page::is_above(Page newpg){
    
        if(fabs(newpg.getX()+newpg.getwidth()/2 - x - width/2) < (newpg.getwidth()+width)/2)
        {
            if(fabs(newpg.getY()+newpg.getheight()/2 - y - height/2) < (newpg.getheight()+height)/2)
                {
                    return 1;
                }
        }
        return 0;
        
    }

    void Page::sort_above(){
        sort(above.begin(), above.end());
    }
    
    void Page::delete_above(int index)
    {
        above.erase(above.begin()+index);
    }