#include "Widget.hpp"

Widget::Widget(Gtk::Widget& widget) :
    m_widget(widget)
{
}

void Widget::show()
{
    m_widget.show();
    m_shown.emit();
}

void Widget::showAll()
{
    m_widget.show();
    m_shown.emit();
}

void Widget::hide()
{
    m_widget.hide();
}

bool Widget::isShown() const
{
    return m_widget.is_visible();
}

void Widget::scale(double scaleX, double scaleY)
{
    setSize(static_cast<int>(width() * scaleX),
            static_cast<int>(height() * scaleY));
}

void Widget::setSize(int width, int height)
{
    m_widget.set_size_request(width, height);
}

int Widget::width() const
{
    int width, _;
    m_widget.get_size_request(width, _);
    return width;
}

int Widget::height() const
{
    int _, height;
    m_widget.get_size_request(_, height);
    return height;
}

SignalShown Widget::shown()
{
    return m_shown;
}

