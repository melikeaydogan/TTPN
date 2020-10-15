#include <QtGui>
#include <QtWidgets>
#include <stdlib.h>
#include <string.h>



class BarChart : public QGraphicsItem
{
public:
    enum { Type = UserType + 25 };
    BarChart(int a, int b, int mh, int *c);

    QRectF boundingRect() const;  
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void setColor(QColor newcolor);
    
    int type() const
    { return Type;}

    int index, frame_num, maxh, placeID, *count;
    QColor color;
};
