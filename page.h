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
        
        
    private:
        int x, y; // position of the page on the board
        int width, height; // width and height of the page 
        int id=-1; // unique id for each page
        char content; 
};
    Page::Page(){}    
    Page::Page(int x, int y, int width, int height, int id, int content):x(x),y(y),width(width),height(height), id(id), content((char)content){}
