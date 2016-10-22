//
// KochFlakeGDI - GDI+ based Koch Snowflake.
//

#if !defined(_LINE_HPP_)
#define _LINE_HPP_

class Line {
public:
    Gdiplus::PointF p1;
    Gdiplus::PointF p2;

    Line(const Gdiplus::PointF& p1, const Gdiplus::PointF& p2);
    Line(const Gdiplus::PointF& p1, const Gdiplus::PointF& p2, const Gdiplus::Color& color);
    Line(const Line& line);
    ~Line();

    Line& operator=(Line& rhs);

    void setColor(const Gdiplus::Color& color);
    void getColor(Gdiplus::Color& color) const;
    void draw(HDC hdc);

private:
    Gdiplus::Pen m_pen;

    void m_initPen();
};

#endif
