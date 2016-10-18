//
// KochFlakeGDI - GDI+ based Koch Snowflake.
//

#include <objidl.h>
#include <gdiplus.h>

#include "Line.hpp"

Line::Line(const Gdiplus::PointF& p1, const Gdiplus::PointF& p2)
    : p1(p1), p2(p2), m_pen(Gdiplus::Color(255, 0, 0, 0))
{
    m_initPen();
}

// -----------------------------------------------------------------------

Line::Line(const Gdiplus::PointF& p1, const Gdiplus::PointF& p2, const Gdiplus::Color& color)
    : p1(p1), p2(p2), m_pen(color)
{
    m_initPen();
}

// -----------------------------------------------------------------------

Line::Line(const Line& line)
    : p1(line.p1), p2(line.p2), m_pen(Gdiplus::Color(255, 0, 0, 0))
{
    Gdiplus::Color color;
    line.getColor(color);
    m_pen.SetColor(color);
    m_initPen();
}

// -----------------------------------------------------------------------

Line::~Line()
{
}

// -----------------------------------------------------------------------

void
Line::setColor(const Gdiplus::Color& color)
{
    m_pen.SetColor(color);
}

// -----------------------------------------------------------------------

void
Line::getColor(Gdiplus::Color& color) const
{
    m_pen.GetColor(&color);
}

// -----------------------------------------------------------------------

void
Line::draw(HDC hdc)
{
    Gdiplus::Graphics graphics(hdc);
    graphics.DrawLine(&m_pen, p1, p2);
}

// -----------------------------------------------------------------------

void
Line::m_initPen()
{
    // Nice, fat width.
    m_pen.SetWidth(10);

    // Round caps, so the lines blend together nicely.
    m_pen.SetStartCap(Gdiplus::LineCapRound);
    m_pen.SetEndCap(Gdiplus::LineCapRound);
}