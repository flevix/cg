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
    { }
 
    void print(cg::visualization::printer_type & p) const
    {
        p.corner_stream() << "Delaunay triangulation" << cg::visualization::endl;
    }
 
    bool on_release(const point_2f & p)
    {
       return true;
    }
};

int main(int argc, char ** argv)
{
   QApplication app(argc, argv);
   delaunay_viewer viewer;
   cg::visualization::run_viewer(&viewer, "delaunay_viewer");
}
