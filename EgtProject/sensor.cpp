#include <egt/app.h>
#include <egt/button.h>
#include <egt/combo.h>
#include <egt/grid.h>
#include <egt/shapes.h>
#include <egt/sideboard.h>
#include <egt/sizer.h>
#include <egt/virtualkeyboard.h>
#include <egt/window.h>
extern "C"
{
#include<i2c/smbus.h>
#include <linux/i2c-dev.h>
}

enum TemperatureUnit
{
    Celsius,
    Fahrenheit,
    Kelvin
};

template<>
const std::pair<TemperatureUnit, char const*> egt::detail::EnumStrings<TemperatureUnit>::data[] =
{
    {TemperatureUnit::Celsius, " °C"},
    {TemperatureUnit::Fahrenheit, " °F"},
    {TemperatureUnit::Kelvin, " K"},
};

double binary_value_to_double(__s16 value)
{
    __s16 res = value;
    int mask;
    __s16 res1;
    __s16 res2;
    double val_double;

    mask = 65408;
    res1 = (res << 8) & 0xff00;
    res2 = (res >> 8) & 0x00ff;
    res = res1 | res2;
    res = res & mask;
    val_double = res / std::pow(2, 8);

    return val_double;
}

double temperature_value_from_sensor()
{
    char const* filename = "/dev/i2c-1";
    int addr = 0x48;
    int file;

    if ((file = open(filename, O_RDWR)) < 0)
    {
        throw std::invalid_argument("Error trying to open the bus");
    }

    if (ioctl(file, I2C_SLAVE, addr) < 0)
    {
        throw std::invalid_argument("Error trying to read the file");
        exit(1);
    }

    __u8 address = 0x00;

    __s16 res = (__s16) 0x80ff;
    res = i2c_smbus_read_word_data(file, address);

    return binary_value_to_double(res);
}

#define LIL_RATIO 4./5
#define BIG_RATIO 9./10

#define TEMPERATURE_RANGE 60

class Thermometer : public egt::Frame
{

    static constexpr double default_low_temperature = 0;
    static constexpr double default_high_temperature = 30;

    const double ratio_text = 1. / 6;

public:

    Thermometer(egt::Frame& parent, egt::Circle circle, TemperatureUnit uom = TemperatureUnit::Celsius, double low_temperature = default_low_temperature, double high_temperature = default_high_temperature, double gauge_angle = 3 * M_PI / 2)
        : Frame(parent, circle.rect()),
          m_circle(circle),
          m_inside_circle(m_circle.center(), m_circle.radius() - 1),
          m_temperature_unit(uom),
          m_low_temperature(low_temperature),
          m_high_temperature(high_temperature),
          m_gauge_angle(gauge_angle),
          m_label_displayer(*this),
          m_fill_color(egt::Color(100, 100, 100)),
          m_stroke_color(0, 0, 0),
          m_low_temperature_color(egt::Palette::darkblue),
          m_high_temperature_color(egt::Palette::darkred)

    {
        srand(time(0));

        temperature_celsius_fct_temperature_value_and_unit();
        set_high_low_temperature_boundaries_celsius();
        m_temperature_celsius = temperature_value_from_sensor();
        temperature_fct_temperature_celsius_value_and_unit();
        color_fct_of_temperature();
        m_label_displayer.text(double_to_string(m_temperature) + egt::detail::enum_to_string(m_temperature_unit));
        m_label_displayer.align(egt::AlignFlag::center);
        m_label_displayer.font(egt::Font(m_circle.radius() * ratio_text));

        this->on_event([this](egt::Event & event)
        {
            if (event.id() == egt::EventId::pointer_click)
            {
                m_temperature_celsius = temperature_value_from_sensor();
                temperature_fct_temperature_celsius_value_and_unit();
                color_fct_of_temperature();
                m_label_displayer.text(double_to_string(m_temperature) + egt::detail::enum_to_string(m_temperature_unit));
                damage();
            }
        });
    }

    void draw(egt::Painter& painter, const egt::Rect& rect) override;

    void label_displayer(egt::Label& label)
    {
        m_label_displayer = std::move(label);
    }

    egt::Label& label_displayer()
    {
        return m_label_displayer;
    }

    void temperature_unit(TemperatureUnit unit)
    {
        m_temperature_unit = unit;
    }

    TemperatureUnit temperature_unit() const
    {
        return m_temperature_unit;
    }

    void gauge_angle(double angle)
    {
        m_gauge_angle = angle;
    }

    double gauge_angle() const
    {
        return m_gauge_angle;
    }

    void temperature_celsius(double temperature_celsius)
    {
        m_temperature_celsius = temperature_celsius;
    }

    double temperature_celsius() const
    {
        return m_temperature_celsius;
    }

    void temperature(double temperature)
    {
        m_temperature = temperature;
    }

    double temperature() const
    {
        return m_temperature;
    }

    void low_temperature_celsius(double low_temperature_celsius)
    {
        m_low_temperature_celsius = low_temperature_celsius;
    }

    double low_temperature_celsius() const
    {
        return m_low_temperature_celsius;
    }

    void low_temperature(double low_temperature)
    {
        m_low_temperature = low_temperature;
    }

    double low_temperature() const
    {
        return m_low_temperature;
    }

    void high_temperature_celsius(double high_temperature_celsius)
    {
        m_high_temperature_celsius = high_temperature_celsius;
    }

    double high_temperature_celsius() const
    {
        return m_high_temperature_celsius;
    }

    void high_temperature(double high_temperature)
    {
        m_high_temperature = high_temperature;
    }

    double high_temperature() const
    {
        return m_high_temperature;
    }

    void fill_color(egt::Color color)
    {
        m_fill_color = color;
    }

    egt::Color fill_color()
    {
        return m_fill_color;
    }

    void stroke_color(egt::Color color)
    {
        m_stroke_color = color;
    }

    egt::Color stroke_color()
    {
        return m_stroke_color;
    }

    void low_temperature_color(egt::Color color)
    {
        m_low_temperature_color = color;
    }

    egt::Color low_temperature_color()
    {
        return m_low_temperature_color;
    }

    void high_temperature_color(egt::Color color)
    {
        m_high_temperature_color = color;
    }

    egt::Color high_temperature_color()
    {
        return m_high_temperature_color;
    }

    std::string double_to_string(double double_value)
    {
        std::ostringstream string_value;
        string_value << std::fixed << std::setprecision(1) << double_value;
        return string_value.str();
    }

    void temperature_fct_temperature_celsius_value_and_unit()
    {
        switch (m_temperature_unit)
        {
        case Celsius:
            m_temperature = m_temperature_celsius;
            m_low_temperature = m_low_temperature_celsius;
            m_high_temperature = m_high_temperature_celsius;
            m_low_temperature_boundary = m_low_temperature_boundary_celsius;
            m_high_temperature_boundary = m_high_temperature_boundary_celsius;
            break;
        case Fahrenheit:
            m_temperature = m_temperature_celsius * 9 / 5 + 32;
            m_low_temperature = m_low_temperature_celsius * 9 / 5 + 32;
            m_high_temperature = m_high_temperature_celsius * 9 / 5 + 32;
            m_low_temperature_boundary = m_low_temperature_boundary_celsius * 9 / 5 + 32;
            m_high_temperature_boundary = m_high_temperature_boundary_celsius * 9 / 5 + 32;
            break;
        case Kelvin:
            m_temperature = m_temperature_celsius + 273.15;
            m_low_temperature = m_low_temperature_celsius + 273.15;
            m_high_temperature = m_high_temperature_celsius + 273.15;
            m_low_temperature_boundary = m_low_temperature_boundary_celsius + 273.15;
            m_high_temperature_boundary = m_high_temperature_boundary_celsius + 273.15;
            break;
        default:
            break;
        }
    }

protected:

    egt::Circle m_circle;
    egt::Circle m_inside_circle;

    TemperatureUnit m_temperature_unit;

    double m_temperature_celsius;
    double m_temperature;

    double m_low_temperature_celsius;
    double m_low_temperature;

    double m_high_temperature_celsius;
    double m_high_temperature;

    double m_gauge_angle;

    double m_low_temperature_boundary_celsius;
    double m_low_temperature_boundary;

    double m_high_temperature_boundary_celsius;
    double m_high_temperature_boundary;

    egt::Label m_label_displayer;

    egt::Color m_fill_color;
    egt::Color m_stroke_color;
    egt::Color m_low_temperature_color;
    egt::Color m_high_temperature_color;

    void temperature_celsius_fct_temperature_value_and_unit()
    {
        switch (m_temperature_unit)
        {
        case Celsius:
            m_temperature_celsius = m_temperature;
            m_low_temperature_celsius = m_low_temperature;
            m_high_temperature_celsius = m_high_temperature;
            m_low_temperature_boundary_celsius = m_low_temperature_boundary;
            m_high_temperature_boundary_celsius = m_high_temperature_boundary;
            break;
        case Fahrenheit:
            m_temperature_celsius = (m_temperature - 32) * 5 / 9;
            m_low_temperature_celsius = (m_low_temperature - 32) * 5 / 9;
            m_high_temperature_celsius = (m_high_temperature - 32) * 5 / 9;
            m_low_temperature_boundary_celsius = (m_low_temperature_boundary - 32) * 5 / 9;
            m_high_temperature_boundary_celsius = (m_high_temperature_boundary - 32) * 5 / 9;
            break;
        case Kelvin:
            m_temperature_celsius = m_temperature - 273.15;
            m_low_temperature_celsius = m_low_temperature - 273.15;
            m_high_temperature_celsius = m_high_temperature - 273.15;
            m_low_temperature_boundary_celsius = m_low_temperature_boundary - 273.15;
            m_high_temperature_boundary_celsius = m_high_temperature_boundary - 273.15;
            break;
        default:
            break;
        }
    }

    void color_fct_of_temperature()
    {
        if (m_temperature_celsius >= m_high_temperature_celsius)
        {
            m_label_displayer.color(egt::Palette::ColorId::label_text, m_high_temperature_color);
        }
        else if (m_temperature_celsius <= m_low_temperature_celsius)
        {
            m_label_displayer.color(egt::Palette::ColorId::label_text, m_low_temperature_color);
        }
        else
        {
            m_label_displayer.color(egt::Palette::ColorId::label_text, egt::Palette::black);
        }
    }

    void set_high_low_temperature_boundaries_celsius()
    {
        m_low_temperature_boundary_celsius = -10;
        m_high_temperature_boundary_celsius = 50;
    }

    double temperature_span(double low_boundary, double high_boundary)
    {
        return high_boundary - low_boundary;
    }

    double temperature_ratio()
    {
        return temperature_span(m_low_temperature_boundary_celsius, m_temperature_celsius) / temperature_span(m_low_temperature_boundary_celsius, m_high_temperature_boundary_celsius);
    }

    double low_temperature_ratio()
    {
        return temperature_span(m_low_temperature_boundary_celsius, m_low_temperature_celsius) / temperature_span(m_low_temperature_boundary_celsius, m_high_temperature_boundary_celsius);
    }

    double high_temperature_ratio()
    {
        return temperature_span(m_high_temperature_celsius, m_high_temperature_boundary_celsius) / temperature_span(m_low_temperature_boundary_celsius, m_high_temperature_boundary_celsius);
    }

    double low_temperature_circle_radius()
    {
        double low_temperature_angle_size = low_temperature_ratio() * m_gauge_angle;
        return std::sqrt(std::pow((1 - std::cos(low_temperature_angle_size)), 2) + std::pow(std::sin(low_temperature_angle_size), 2));
    }

    double high_temperature_circle_radius()
    {
        double high_temperature_angle_size = high_temperature_ratio() * m_gauge_angle;
        return std::sqrt(std::pow((1 + std::cos(high_temperature_angle_size)), 2) + std::pow(std::sin(high_temperature_angle_size), 2));
    }

};

void Thermometer::draw(egt::Painter& painter, const egt::Rect& rect)
{

	//auto cr = painter.context().get();
    //draw circle
    /**
     * Draw gradient in three steps:
     * - Colorizing the background of the gauge in white
     * - Colorizing the low temperature part with a gradient going from blue to white, from left to right
     * - Colorizing the high temperature part with a gradient going from red to white, from right to left
     */
    m_circle.center(egt::Point(center().x(), center().y()));
    m_inside_circle.center(m_circle.center());

    painter.set(m_fill_color);
    painter.draw(m_inside_circle);
    painter.fill();

    painter.set(m_stroke_color);
    painter.draw(m_inside_circle);
    painter.stroke();

    //draw gauge
    double start_angle = M_PI / 2 + ((2 * M_PI - m_gauge_angle) / 2);
    double end_angle = start_angle + m_gauge_angle;

    int lil_radius = m_inside_circle.radius() * LIL_RATIO;
    int big_radius = m_inside_circle.radius() * BIG_RATIO;

    egt::Point low_point(m_circle.center().x() + std::cos(start_angle + low_temperature_ratio() * m_gauge_angle) * lil_radius, m_circle.center().y() + std::sin(start_angle + low_temperature_ratio() * m_gauge_angle) * lil_radius);
    egt::Point high_point(m_circle.center().x() + std::cos(end_angle - high_temperature_ratio() * m_gauge_angle) * lil_radius, m_circle.center().y() + std::sin(end_angle - (high_temperature_ratio() * m_gauge_angle)) * lil_radius);

    egt::Point pt_ll(m_circle.center().x() + cos(start_angle) * big_radius, m_circle.center().y() + sin(start_angle) * big_radius);
    egt::Point pt_rr(m_circle.center().x() + cos(end_angle) * big_radius, m_circle.center().y() + sin(end_angle) * big_radius);

    double white_color_angle = m_gauge_angle - (low_temperature_ratio() + high_temperature_ratio()) * m_gauge_angle;
    double in_between_low_and_high_angle(start_angle + low_temperature_ratio() * m_gauge_angle + white_color_angle / 2);
    double in_between_low_and_high_cos = std::cos(in_between_low_and_high_angle);
    double in_between_low_and_high_sin = std::sin(in_between_low_and_high_angle);
    egt::Point in_between_low_and_high_point(m_circle.center().x() + in_between_low_and_high_cos * lil_radius, m_circle.center().y() + in_between_low_and_high_sin * lil_radius);

    double m_temperature_angle = start_angle + temperature_ratio() * m_gauge_angle;
    double m_temperature_cos = std::cos(m_temperature_angle);
    double m_temperature_sin = std::sin(m_temperature_angle);
    egt::Point m_temperature_point(m_circle.center().x() + m_temperature_cos * lil_radius, m_circle.center().y() + m_temperature_sin * lil_radius);

    //setting the color to white
    cairo_set_source_rgb(cr, 255, 255, 255);

    //drawing the gauge shape
    cairo_move_to(cr, pt_ll.x(), pt_ll.y());
    cairo_arc(cr, m_circle.center().x(), m_circle.center().y(), lil_radius, start_angle, std::min(end_angle, m_temperature_angle));
    cairo_arc_negative(cr, m_circle.center().x(), m_circle.center().y(), big_radius, std::min(end_angle, m_temperature_angle), start_angle);

    //filling the drawn shape with the setted color
    cairo_fill(cr);

    //setting color with m_stroke_color
    cairo_set_source_rgb(cr, m_stroke_color.redf(), m_stroke_color.greenf(), m_stroke_color.bluef());

    //drawing the gauge shape
    cairo_move_to(cr, pt_ll.x(), pt_ll.y());
    cairo_arc(cr, m_circle.center().x(), m_circle.center().y(), lil_radius, start_angle, std::min(end_angle, m_temperature_angle));
    cairo_arc_negative(cr, m_circle.center().x(), m_circle.center().y(), big_radius, std::min(end_angle, m_temperature_angle), start_angle);

    //drawing strokes with the setted color
    cairo_stroke(cr);

    //draw gradient
    cairo_pattern_t* rad_grad;

    //blue gradient
    //drawing the gauge shape
    cairo_move_to(cr, pt_ll.x(), pt_ll.y());
    cairo_arc(cr, m_circle.center().x(), m_circle.center().y(), lil_radius, start_angle, std::min(in_between_low_and_high_angle, m_temperature_angle));
    cairo_arc_negative(cr, m_circle.center().x(), m_circle.center().y(), big_radius, std::min(in_between_low_and_high_angle, m_temperature_angle), start_angle);

    //defining circles center and radiuses
    rad_grad = cairo_pattern_create_radial(pt_ll.x(), pt_ll.y(), pt_ll.distance_to(low_point) * 5 / 6, pt_ll.x(), pt_ll.y(), pt_ll.distance_to(low_point) * 7 / 6);

    //defining each color proportion
    //low temperature color
    cairo_pattern_add_color_stop_rgba(rad_grad, 0.50,  m_low_temperature_color.redf(), m_low_temperature_color.greenf(), m_low_temperature_color.bluef(), 1);
    //white
    cairo_pattern_add_color_stop_rgba(rad_grad, 1,  1, 1, 1, 0);

    //setting the color with the gradient's one
    cairo_set_source(cr, rad_grad);
    //filling the drawn shape with the setted color
    cairo_fill(cr);

    //red gradient
    //drawing the gauge shape
    cairo_move_to(cr, in_between_low_and_high_point.x(), in_between_low_and_high_point.y());
    cairo_arc(cr, m_circle.center().x(), m_circle.center().y(), lil_radius, in_between_low_and_high_angle, std::max(in_between_low_and_high_angle, m_temperature_angle));
    cairo_arc_negative(cr, m_circle.center().x(), m_circle.center().y(), big_radius, std::max(in_between_low_and_high_angle, m_temperature_angle), in_between_low_and_high_angle);

    //defining circles center and radiuses
    rad_grad = cairo_pattern_create_radial(pt_rr.x(), pt_rr.y(), pt_rr.distance_to(high_point) * 5 / 6, pt_rr.x(), pt_rr.y(), pt_rr.distance_to(high_point) * 7 / 6);

    //defining each color proportion
    //high temperature color
    cairo_pattern_add_color_stop_rgba(rad_grad, 0.50,  m_high_temperature_color.redf(), m_high_temperature_color.greenf(), m_high_temperature_color.bluef(), 1);
    //white
    cairo_pattern_add_color_stop_rgba(rad_grad, 1,  1, 1, 1, 0);

    //setting the color with the gradient's one
    cairo_set_source(cr, rad_grad);
    //filling the drawn shape with the setted color
    cairo_fill(cr);

    egt::Frame::draw(painter, rect);
}

void settings_scrolledview_align_fct_sidebord_pos(egt::ScrolledView& settings_scrolledview, egt::SideBoard& sideboard)
{
    switch (sideboard.position())
    {
    case egt::SideBoard::PositionFlag::top :
        settings_scrolledview.align().clear();
        settings_scrolledview.height(400);
        settings_scrolledview.align(egt::AlignFlags(egt::AlignFlag::top | egt::AlignFlag::expand_horizontal));
        break;
    case egt::SideBoard::PositionFlag::right :
        settings_scrolledview.align().clear();
        settings_scrolledview.width(400);
        settings_scrolledview.align(egt::AlignFlags(egt::AlignFlag::right | egt::AlignFlag::expand_vertical));
        break;
    case egt::SideBoard::PositionFlag::bottom :
        settings_scrolledview.align().clear();
        settings_scrolledview.height(400);
        settings_scrolledview.align(egt::AlignFlags(egt::AlignFlag::bottom | egt::AlignFlag::expand_horizontal));
        break;
    case egt::SideBoard::PositionFlag::left :
        settings_scrolledview.align().clear();
        settings_scrolledview.width(400);
        settings_scrolledview.align(egt::AlignFlags(egt::AlignFlag::left | egt::AlignFlag::expand_vertical));
        break;
    }
}

int main()
{
    egt::Application app;
    egt::TopWindow window;

    int middle_width_window = window.width() / 2;
    int middle_height_window = window.height() / 2;

    window.color(egt::Palette::ColorId::bg, egt::Palette::cadetblue);

    Thermometer thermo(window, egt::Circle(egt::Point(middle_width_window, middle_height_window), 140), TemperatureUnit::Fahrenheit, 32, 86);
    egt::align(thermo, egt::AlignFlag::left);

    egt::CustomizableSideBoard sideboard(egt::SideBoard::PositionFlag::right,
                                         std::chrono::milliseconds(500),
                                         egt::easing_linear, std::chrono::milliseconds(500),
                                         egt::easing_linear,
                                         egt::WindowHint::automatic,
                                         middle_width_window);

    egt::ScrolledView settings_scrolledview(sideboard);
    settings_scrolledview_align_fct_sidebord_pos(settings_scrolledview, sideboard);

    egt::VerticalBoxSizer settings_panel(settings_scrolledview);
    settings_panel.align(egt::AlignFlag::center_horizontal);

    auto default_keyboard = std::make_shared<egt::VirtualKeyboard>();
    egt::PopupVirtualKeyboard popup_keyboard {default_keyboard};

    egt::Label setting_title(settings_panel, "Settings", egt::Rect(egt::Size(middle_width_window, 0)), egt::AlignFlag::center);
    setting_title.font(egt::Font(egt::Font::Size(25), egt::Font::Weight::bold));

    egt::Label live_settings_title(settings_panel, "Live settings", egt::Rect(egt::Size(0, 0)), egt::AlignFlag::center);
    live_settings_title.font(egt::Font(egt::Font::Size(18), egt::Font::Weight::bold));

    egt::HorizontalBoxSizer panel_position_sizer(settings_panel);
    egt::expand_horizontal(panel_position_sizer);
    egt::Label panel_position_label(panel_position_sizer, "Panel position: ");
    egt::expand_horizontal(panel_position_label);
    egt::StaticGrid position_grid(panel_position_sizer, egt::Rect(egt::Size(240, 90)), egt::StaticGrid::GridSize(4, 3));
    position_grid.align(egt::AlignFlag::center);
    egt::RectangleWidget left_position_rect(egt::Rect(egt::Size(80, 30)));
    left_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
    left_position_rect.border(2);
    left_position_rect.color(egt::Palette::ColorId::border, egt::Palette::black);
    egt::RectangleWidget top_position_rect(egt::Rect(egt::Size(80, 30)));
    top_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
    top_position_rect.border(2);
    top_position_rect.color(egt::Palette::ColorId::border, egt::Palette::black);
    egt::RectangleWidget right_position_rect(egt::Rect(egt::Size(80, 30)));
    right_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
    right_position_rect.border(2);
    right_position_rect.color(egt::Palette::ColorId::border, egt::Palette::black);
    egt::RectangleWidget bottom_position_rect(egt::Rect(egt::Size(80, 30)));
    bottom_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
    bottom_position_rect.border(2);
    bottom_position_rect.color(egt::Palette::ColorId::border, egt::Palette::black);

    position_grid.add(left_position_rect, egt::StaticGrid::GridPoint(0, 1));
    position_grid.add(top_position_rect, egt::StaticGrid::GridPoint(1, 0));
    position_grid.add(right_position_rect, egt::StaticGrid::GridPoint(2, 1));
    position_grid.add(bottom_position_rect, egt::StaticGrid::GridPoint(1, 2));

    switch (sideboard.position())
    {
    case egt::SideBoard::PositionFlag::left:
        left_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
        break;
    case egt::SideBoard::PositionFlag::top:
        top_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
        break;
    case egt::SideBoard::PositionFlag::right:
        right_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
        break;
    case egt::SideBoard::PositionFlag::bottom:
        bottom_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
        break;
    default:
        break;
    }

    left_position_rect.on_event([&settings_scrolledview, &sideboard, &left_position_rect, &top_position_rect, &right_position_rect, &bottom_position_rect](egt::Event & event)
    {
        if (event.id() == egt::EventId::pointer_click)
        {
            sideboard.position(egt::SideBoard::PositionFlag::left);
            settings_scrolledview_align_fct_sidebord_pos(settings_scrolledview, sideboard);
            left_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
            top_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            right_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            bottom_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
        }
    });

    top_position_rect.on_event([&settings_scrolledview, &sideboard, &left_position_rect, &top_position_rect, &right_position_rect, &bottom_position_rect](egt::Event & event)
    {
        if (event.id() == egt::EventId::pointer_click)
        {
            sideboard.position(egt::SideBoard::PositionFlag::top);
            settings_scrolledview_align_fct_sidebord_pos(settings_scrolledview, sideboard);
            left_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            top_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
            right_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            bottom_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
        }
    });

    right_position_rect.on_event([&settings_scrolledview, &sideboard, &left_position_rect, &top_position_rect, &right_position_rect, &bottom_position_rect](egt::Event & event)
    {
        if (event.id() == egt::EventId::pointer_click)
        {
            sideboard.position(egt::SideBoard::PositionFlag::right);
            settings_scrolledview_align_fct_sidebord_pos(settings_scrolledview, sideboard);
            left_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            top_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            right_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
            bottom_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
        }
    });

    bottom_position_rect.on_event([&settings_scrolledview, &sideboard, &left_position_rect, &top_position_rect, &right_position_rect, &bottom_position_rect](egt::Event & event)
    {
        if (event.id() == egt::EventId::pointer_click)
        {
            sideboard.position(egt::SideBoard::PositionFlag::bottom);
            settings_scrolledview_align_fct_sidebord_pos(settings_scrolledview, sideboard);
            left_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            top_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            right_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            bottom_position_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
        }
    });

    egt::HorizontalBoxSizer alignment_boxsizer(settings_panel);
    alignment_boxsizer.align(egt::AlignFlag::left);
    egt::Label alignment_label(alignment_boxsizer, "Circle alignment: ");
    egt::RectangleWidget left_alignment_rect(alignment_boxsizer, egt::Rect(egt::Size(80, 30)));
    left_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
    left_alignment_rect.border(2);
    left_alignment_rect.color(egt::Palette::ColorId::border, egt::Palette::black);
    egt::RectangleWidget center_alignment_rect(alignment_boxsizer, egt::Rect(egt::Size(80, 30)));
    center_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
    center_alignment_rect.border(2);
    center_alignment_rect.color(egt::Palette::ColorId::border, egt::Palette::black);
    egt::RectangleWidget right_alignment_rect(alignment_boxsizer, egt::Rect(egt::Size(80, 30)));
    right_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
    right_alignment_rect.border(2);
    right_alignment_rect.color(egt::Palette::ColorId::border, egt::Palette::black);

    switch (thermo.align().raw())
    {
    case (unsigned int) egt::AlignFlag::left :
        left_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
        break;
    case (unsigned int) egt::AlignFlag::center:
        center_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
        break;
    case (unsigned int) egt::AlignFlag::right:
        right_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
        break;
    default:
        break;
    }

    left_alignment_rect.on_event([&thermo, &left_alignment_rect, &center_alignment_rect, &right_alignment_rect, &sideboard](egt::Event & event)
    {
        if (event.id() == egt::EventId::pointer_click)
        {
            thermo.align(egt::AlignFlag::left);
            left_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
            center_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            right_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
        }
    });

    center_alignment_rect.on_event([&thermo, &left_alignment_rect, &center_alignment_rect, &right_alignment_rect](egt::Event & event)
    {
        if (event.id() == egt::EventId::pointer_click)
        {
            thermo.align(egt::AlignFlag::center);
            left_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            center_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
            right_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
        }
    });

    right_alignment_rect.on_event([&thermo, &left_alignment_rect, &center_alignment_rect, &right_alignment_rect](egt::Event & event)
    {
        if (event.id() == egt::EventId::pointer_click)
        {
            thermo.align(egt::AlignFlag::right);
            left_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            center_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::white);
            right_alignment_rect.color(egt::Palette::ColorId::button_bg, egt::Palette::orangered);
        }
    });

    egt::Label applicable_settings_title(settings_panel, "Applicable settings", egt::Rect(egt::Size(0, 0)), egt::AlignFlag::center);
    applicable_settings_title.font(egt::Font(egt::Font::Size(18), egt::Font::Weight::bold));

    egt::HorizontalBoxSizer fill_color_boxsizer(settings_panel);
    fill_color_boxsizer.align(egt::AlignFlag::left);
    egt::Label fill_color_label(fill_color_boxsizer, "RGB fill color: ");
    egt::TextBox fill_red_color_textbox(fill_color_boxsizer, std::to_string(thermo.fill_color().red()), egt::Rect(egt::Size(45, 30)));
    egt::TextBox fill_green_color_textbox(fill_color_boxsizer, std::to_string(thermo.fill_color().green()), egt::Rect(egt::Size(45, 30)));
    egt::TextBox fill_blue_color_textbox(fill_color_boxsizer, std::to_string(thermo.fill_color().blue()), egt::Rect(egt::Size(45, 30)));

    egt::HorizontalBoxSizer stroke_color_boxsizer(settings_panel);
    stroke_color_boxsizer.align(egt::AlignFlag::left);
    egt::Label stroke_color_label(stroke_color_boxsizer, "RGB strokes color: ");
    egt::TextBox stroke_red_color_textbox(stroke_color_boxsizer, std::to_string(thermo.stroke_color().red()), egt::Rect(egt::Size(45, 30)));
    egt::TextBox stroke_green_color_textbox(stroke_color_boxsizer, std::to_string(thermo.stroke_color().green()), egt::Rect(egt::Size(45, 30)));
    egt::TextBox stroke_blue_color_textbox(stroke_color_boxsizer, std::to_string(thermo.stroke_color().blue()), egt::Rect(egt::Size(45, 30)));

    egt::HorizontalBoxSizer low_color_boxsizer(settings_panel);
    low_color_boxsizer.align(egt::AlignFlag::left);
    egt::Label low_color_label(low_color_boxsizer, "RGB low color: ");
    egt::TextBox low_red_color_textbox(low_color_boxsizer, std::to_string(thermo.low_temperature_color().red()), egt::Rect(egt::Size(45, 30)));
    egt::TextBox low_green_color_textbox(low_color_boxsizer, std::to_string(thermo.low_temperature_color().green()), egt::Rect(egt::Size(45, 30)));
    egt::TextBox low_blue_color_textbox(low_color_boxsizer, std::to_string(thermo.low_temperature_color().blue()), egt::Rect(egt::Size(45, 30)));

    egt::HorizontalBoxSizer high_color_boxsizer(settings_panel);
    high_color_boxsizer.align(egt::AlignFlag::left);
    egt::Label high_color_label(high_color_boxsizer, "RGB high color: ");
    egt::TextBox high_red_color_textbox(high_color_boxsizer, std::to_string(thermo.high_temperature_color().red()), egt::Rect(egt::Size(45, 30)));
    egt::TextBox high_green_color_textbox(high_color_boxsizer, std::to_string(thermo.high_temperature_color().green()), egt::Rect(egt::Size(45, 30)));
    egt::TextBox high_blue_color_textbox(high_color_boxsizer, std::to_string(thermo.high_temperature_color().blue()), egt::Rect(egt::Size(45, 30)));

    std::pair <std::string, double> angles[]
    {
        std::make_pair("2PI", 2 * M_PI),
        std::make_pair("3/2PI", 3 * M_PI / 2),
        std::make_pair("PI", M_PI),
        std::make_pair("PI/2", M_PI / 2)
    };

    egt::ComboBox::ItemArray angle_array;
    for (const auto& i : angles)
    {
        angle_array.push_back(std::make_shared<egt::StringItem>(i.first));
    }


    egt::HorizontalBoxSizer angle_boxsizer(settings_panel);
    angle_boxsizer.align(egt::AlignFlag::left);
    egt::Label angle_label(angle_boxsizer, "Angle value: ");
    egt::ComboBox angle_combo(angle_boxsizer, angle_array, egt::Rect(egt::Size(0, 0)));
    angle_combo.autoresize(false);
    angle_combo.width(150);
    angle_combo.height(35);

    for (int i = 0; i < (int)(sizeof(angles) / sizeof(angles[0])); i++)
    {
        if (std::abs(thermo.gauge_angle() - angles[i].second) < 0.1)
        {
            angle_combo.selected(i);
        }
    }

    egt::ComboBox::ItemArray unit_array;
    for (const auto& i : egt::detail::EnumStrings<TemperatureUnit>::data)
    {
        unit_array.push_back(std::make_shared<egt::StringItem>(i.second));
    }

    egt::HorizontalBoxSizer unit_boxsizer(settings_panel);
    unit_boxsizer.align(egt::AlignFlag::left);
    egt::Label unit_label(unit_boxsizer, "Temperature unit: ");
    egt::ComboBox unit_combo(unit_boxsizer, unit_array, egt::Rect(egt::Size(0, 0)));
    unit_combo.autoresize(false);
    unit_combo.width(150);
    unit_combo.height(35);

    for (int i = 0; i < (int) settings_panel.count_children(); i++)
    {
        settings_panel.child_at(i)->margin(3);
    }

    switch (thermo.temperature_unit())
    {
    case 0:
        unit_combo.selected(0);
        break;
    case 1:
        unit_combo.selected(1);
        break;
    case 2:
        unit_combo.selected(2);
        break;
    default:
        break;
    }

    egt::Button apply_button(settings_panel, "Apply", egt::Rect(egt::Size(200, 45)));
    apply_button.autoresize(false);

    apply_button.on_click([&thermo,
                           &fill_red_color_textbox,
                           &fill_green_color_textbox,
                           &fill_blue_color_textbox,
                           &stroke_red_color_textbox,
                           &stroke_green_color_textbox,
                           &stroke_blue_color_textbox,
                           &low_red_color_textbox,
                           &low_green_color_textbox,
                           &low_blue_color_textbox,
                           &high_red_color_textbox,
                           &high_green_color_textbox,
                           &high_blue_color_textbox,
                           &angle_combo,
                           &angles,
                           &unit_combo](egt::Event&)
    {
        thermo.fill_color(egt::Color(std::stof(fill_red_color_textbox.text()), std::stof(fill_green_color_textbox.text()), std::stof(fill_blue_color_textbox.text())));
        thermo.stroke_color(egt::Color(std::stof(stroke_red_color_textbox.text()), std::stof(stroke_green_color_textbox.text()), std::stof(stroke_blue_color_textbox.text())));
        thermo.low_temperature_color(egt::Color(std::stof(low_red_color_textbox.text()), std::stof(low_green_color_textbox.text()), std::stof(low_blue_color_textbox.text())));
        thermo.high_temperature_color(egt::Color(std::stof(high_red_color_textbox.text()), std::stof(high_green_color_textbox.text()), std::stof(high_blue_color_textbox.text())));

        switch (angle_combo.selected())
        {
        case 0:
            thermo.gauge_angle(angles[0].second);
            break;
        case 1:
            thermo.gauge_angle(angles[1].second);
            break;
        case 2:
            thermo.gauge_angle(angles[2].second);
            break;
        case 3:
            thermo.gauge_angle(angles[3].second);
            break;
        }

        switch (unit_combo.selected())
        {
        case 0:
            thermo.temperature_unit(TemperatureUnit::Celsius);
            break;
        case 1:
            thermo.temperature_unit(TemperatureUnit::Fahrenheit);
            break;
        case 2:
            thermo.temperature_unit(TemperatureUnit::Kelvin);
            break;
        }

        thermo.temperature_fct_temperature_celsius_value_and_unit();
        thermo.label_displayer().text(thermo.double_to_string(thermo.temperature()) + egt::detail::enum_to_string(thermo.temperature_unit()));
        thermo.damage();
    });

    window.add(sideboard);
    window.add(popup_keyboard);
    sideboard.show();

    window.show();
    return app.run();
}

