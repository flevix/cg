#include <QColor>
#include <QApplication>

#include <cg/visualization/viewer_adapter.h>
#include <cg/visualization/draw_util.h>

#include <cg/triangulation/delaunay.h>

using cg::point_2f;

struct delaunay_viewer : cg::visualization::viewer_adapter
{
    delaunay_viewer()
    { }
 
    void draw(cg::visualization::drawer_type & drawer) const
    {
        drawer.set_color(Qt::red);

        for (size_t i = 1; i < pts.size(); i++) {
            drawer.draw_line(pts[i - 1], pts[i]);
        }
    }
 
    void print(cg::visualization::printer_type & p) const
    {
        p.corner_stream() << "rigth click: add point" << cg::visualization::endl;
        p.corner_stream() << "double click: clear" << cg::visualization::endl;
    }
 
    bool on_release(const point_2f & p)
    {
       pts.push_back(p);
       return pts.size() > 1;
    }

    bool on_double_click(const cg::point_2f &)
    {
        pts.clear();
        return true;
    }

private:
    std::vector<point_2f> pts;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   delaunay_viewer viewer;
   cg::visualization::run_viewer(&viewer, "delaunay_viewer");
}
