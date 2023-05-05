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
        
        void abovepage(Page above_page);
        bool is_above(Page newpg);

    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id=-1; // unique id for each page
        char content;
        std::vector<Page> above; // 위에 있는 page를 저장할 vector insert 되는 순서대로 push_back된다.


};
    Page::Page(){}    
    Page::Page(int x, int y, int width, int height, int id, int content):x(x),y(y),width(width),height(height), id(id), content((char)content){}
    void Page::abovepage(Page above_page){
        this->above.push_back(above_page);
    }
    bool Page::is_above(Page newpg){
        // check vertices of newpage is pasted over existing page.
        if( x <= newpg.getX() && newpg.getX() < x+width)
            {if(y<=newpg.getY() && newpg.getY()<y+height)
                {
                    return 1;
                }
            }
        if( x <= newpg.getX()+newpg.getwidth() && newpg.getX()+newpg.getwidth() < x+width)
            {if(y<=newpg.getY() && newpg.getY()<y+height)
                {
                    return 1;
                }
            }
        if( x <= newpg.getX() && newpg.getX() < x+width)
            {if(y<=newpg.getY()+newpg.getheight() && newpg.getY()+newpg.getheight()<y+height)
                {
                    return 1;
                }
            }
        if( x <= newpg.getX()+newpg.getwidth() && newpg.getX()+newpg.getwidth() < x+width)
            {if(y<=newpg.getY()+newpg.getheight() && newpg.getY()+newpg.getheight()<y+height)
                {
                    return 1;
                }
            }
        return 0;
    }