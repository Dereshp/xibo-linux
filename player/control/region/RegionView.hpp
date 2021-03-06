#pragma once

#include "control/common/Widget.hpp"

#include <gtkmm/fixed.h>

class RegionView : public Widget
{
public:
    RegionView(int width, int height);

    void addMedia(const std::shared_ptr<Widget>& child, int left, int top);
    void setSize(int width, int height) override;
    void scale(double scaleX, double scaleY) override;

    Gtk::Fixed& get() override;

private:
    void scaleMedia(double scaleX, double scaleY);

private:
    Gtk::Fixed m_handler;
    std::vector<std::shared_ptr<Widget>> m_media;

};
