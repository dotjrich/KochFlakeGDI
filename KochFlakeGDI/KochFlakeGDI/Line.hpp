//
// KochFlakeGDI - GDI+ based Koch Snowflake.
//

#if !defined(_LINE_HPP_)
#define _LINE_HPP_

class Line {
public:
    Gdiplus::Point p1;
    Gdiplus::Point p2;

    Line(const Gdiplus::Point& p1, const Gdiplus::Point& p2);
    Line(const Gdiplus::Point& p1, const Gdiplus::Point& p2, const Gdiplus::Color& color);
    Line(const Line& line);
    ~Line();

    void setColor(const Gdiplus::Color& color);
    void getColor(Gdiplus::Color& color) const;
    void draw(HDC hdc);

private:
    Gdiplus::Pen m_pen;

    void m_initPen();
};

#endif
