#include <QColor>
#include <QApplication>

#include <cg/visualization/viewer_adapter.h>
#include <cg/visualization/draw_util.h>

#include <cg/triangulation/delaunay.h>
#include <cg/triangulation/predicat.h>

using cg::point_2f;
using cg::triangle_2;

struct delaunay_viewer : cg::visualization::viewer_adapter
{
    delaunay_viewer()
    { }
 
    bool check(const std::vector<cg::triangle_2> & ts) const
    {
        for (triangle_2 tr : ts) {
            for (triangle_2 p_tr : ts) {
                for (int i = 0; i < 3; i++) {
                    cg::point_2 p = p_tr[i];
                    if (tr[0] != p && tr[1] != p &&
                         tr[2] != p && cg::in_circle(tr[0], tr[1], tr[2], p)) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void draw(cg::visualization::drawer_type & drawer) const
    {
        for (triangle_2 tr : trs) {
            drawer.set_color(Qt::red);
            cg::visualization::draw(drawer, tr);

            drawer.set_color(Qt::yellow);
            cg::visualization::draw(drawer, tr.circumcenter(), tr.circumradius());
        }
    }
 
    void print(cg::visualization::printer_type & p) const
    {
        p.corner_stream() << "rigth click: add point" << cg::visualization::endl;
        p.corner_stream() << (check(trs) ? "Ok" : "Fail") << cg::visualization::endl;
    }
 
    bool on_release(const point_2f & p)
    {
       triangulation.add_point(p);
       trs = triangulation.get_triangulation();
       return true;
    }

    bool on_double_click(const cg::point_2f &)
    {
        return true;
    }

private:
    std::vector<triangle_2> trs;
    cg::delaunay_triangulation<double> triangulation;
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   delaunay_viewer viewer;
   cg::visualization::run_viewer(&viewer, "delaunay_viewer");
}
