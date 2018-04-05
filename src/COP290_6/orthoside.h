// OrthoSide.h

#ifndef ORTHOSIDE_H
#define ORTHOSIDE_H

#include <QGLWidget>
#include<funcs.h>
class OrthoSide : public QGLWidget
{
    Q_OBJECT
public:
    explicit OrthoSide(QWidget *parent = 0);
    Model3D myModel;
    ~OrthoSide();
signals:

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

private:
    void draw();
};

#endif // OrthoSide.H
