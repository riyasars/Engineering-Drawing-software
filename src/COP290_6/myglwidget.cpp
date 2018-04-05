// myglwidget.cpp

#include <QtWidgets>
#include <QtOpenGL>

#include "myglwidget.h"

MyGLWidget::MyGLWidget(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    xRot = 0;
    yRot = 0;
    zRot = 0;
    zoomfactor = 1.00;
}

MyGLWidget::~MyGLWidget()
{
}

QSize MyGLWidget::minimumSizeHint() const
{
    return QSize(200, 50);
}

QSize MyGLWidget::sizeHint() const
{
    return QSize(800, 400);
}

static void qNormalizeAngle(int &angle)
{
    while (angle < 0)
        angle += 360 * 16;
    while (angle > 360)
        angle -= 360 * 16;
}

void MyGLWidget::setXRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != xRot) {
        xRot = angle;
        emit xRotationChanged(angle);
        updateGL();
    }
}

void MyGLWidget::setYRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != yRot) {
        yRot = angle;
        emit yRotationChanged(angle);
        updateGL();
    }
}

void MyGLWidget::setZRotation(int angle)
{
    qNormalizeAngle(angle);
    if (angle != zRot) {
        zRot = angle;
        emit zRotationChanged(angle);
        updateGL();
    }
}

void MyGLWidget::setzoom(int zoomval)
{
    zoomfactor = zoomval/100.0f;
    updateGL();
}

void MyGLWidget::setWireframe(){
    wireframe = true;
    updateGL();
}

void MyGLWidget::setFill(){
    wireframe = false;
    updateGL();
}

void MyGLWidget::isometric(){
    xRot = 30;
    yRot = 320;
    zRot = 0;
    updateGL();
}

void MyGLWidget::initializeGL()
{
    qglClearColor(Qt::black);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PROGRAM_POINT_SIZE);
    //glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
    glEnable ( GL_COLOR_MATERIAL );
    glPolygonMode(GL_FRONT, GL_FILL);
    glPolygonMode(GL_BACK, GL_FILL);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    static GLfloat lightPosition[4] = { 0, 0, 10, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

}

void MyGLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
    glRotatef(xRot, 1.0, 0.0, 0.0);
    glRotatef(yRot, 0.0, 1.0, 0.0);
    glRotatef(zRot, 0.0, 0.0, 1.0);
    if (zoomfactor>0.00) {
        glScalef(zoomfactor, zoomfactor, zoomfactor);
    }
    draw(wireframe);
}

void MyGLWidget::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-2, +2, -2, +2, 1.0, 15.0);
#else
    glOrtho(-2, +2, -2, +2, 1.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void MyGLWidget::mousePressEvent(QMouseEvent *event)
{
    lastPos = event->pos();
}

void MyGLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

    if (event->buttons() & Qt::LeftButton) {
        setXRotation((xRot + dy)%360);
        setYRotation((yRot + dx)%360);
    } else if (event->buttons() & Qt::RightButton) {
        setXRotation((xRot + dy)%360);
        setZRotation((zRot + dx)%360);
    }

    lastPos = event->pos();
}
void MyGLWidget::doUpdate(Model3D mod){
    myModel=mod;
    updateGL();
}

void MyGLWidget::draw(bool wireframe)
{
    //glClear();
    //Axes
    glLineWidth(4);

    glBegin(GL_LINES);
        glColor3f( 1.0f, 0.0f, 0.0f );
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(4, 0.0, 0.0);
    glEnd();

    glBegin(GL_LINES);
        glColor3f( 0.0f, 1.0f, 0.0f );
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 4.0, 0.0);
    glEnd();

    glBegin(GL_LINES);
        glColor3f( 0.0f, 0.0f, 1.0f );
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 4.0);
    glEnd();

    if(wireframe==false){
        //3D object
        Point norm;
        glColor3f( 0.8f, 0.8f, 0.8f );
        int i=0;
        while(i<myModel.Planes.size()){
            glBegin(GL_POLYGON);
            norm=myModel.Planes[i].get_normal();
            glNormal3f(norm.x,norm.y,norm.z);
            for(auto iter1=myModel.Planes[i].Bounds.begin();iter1!=myModel.Planes[i].Bounds.end(); iter1++){
                auto iter2=*iter1;
                glVertex3f(iter2->x,iter2->y,iter2->z);
            }
            i++;
            glEnd();
        }
    }

    else{
        glDepthFunc(GL_LEQUAL);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1.0);
        Point norm;
        glColor3f( 0.0f, 0.0f, 0.0f );
        int i=0;
        cout<<"\nbegin solid\n";
        while(i<myModel.Planes.size()){
            glBegin(GL_POLYGON);
            cout<<"\nbegin face\n";
            norm=myModel.Planes[i].get_normal();
            glNormal3f(norm.x,norm.y,norm.z);
            for(auto iter1=myModel.Planes[i].Bounds.begin();iter1!=myModel.Planes[i].Bounds.end(); iter1++){
                auto iter2=*iter1;
                glVertex3f(iter2->x,iter2->y,iter2->z);
                cout<<iter2->x<<" "<<iter2->y<<" "<<iter2->z<<endl;
            }
            i++;
            glEnd();
        }
        glColorMask(0.0, 0.0, 0.0, 0.0);
        glColor4d(0.0, 0.0, 1.0, 1.0);
        i=0;
        while(i<myModel.Edges.size()){
            glBegin(GL_LINES);
            glColor3f( 1.0f, 1.0f, 1.0f );
            glVertex3f(myModel.Edges[i].p1->x,myModel.Edges[i].p1->y,myModel.Edges[i].p1->z);
            glVertex3f(myModel.Edges[i].p2->x,myModel.Edges[i].p2->y,myModel.Edges[i].p2->z);
            i++;
            glEnd();
        }

        glDepthFunc(GL_GREATER);
        glColor4d(0.0, 0.0, 1.0, 1.0); // hidden
        glColorMask(1.0, 1.0, 1.0, 1.0);
        glLineStipple(4, 0xAAAA); // added
        glEnable(GL_LINE_STIPPLE); // added
        glDepthMask(GL_FALSE);
        i=0;
        while(i<myModel.Edges.size()){
            glBegin(GL_LINES);
            glColor3f( 1.0f, 1.0f, 1.0f );
            glVertex3f(myModel.Edges[i].p1->x,myModel.Edges[i].p1->y,myModel.Edges[i].p1->z);
            glVertex3f(myModel.Edges[i].p2->x,myModel.Edges[i].p2->y,myModel.Edges[i].p2->z);
            i++;
            glEnd();
        }

        glDepthMask(GL_TRUE);
        glDisable(GL_LINE_STIPPLE); // added
        glColor4d(1.0, 1.0, 1.0, 1.0); // solid
        glLineWidth(3.0);
        glDepthFunc(GL_LEQUAL);
        i=0;
        while(i<myModel.Edges.size()){
            glBegin(GL_LINES);
            glColor3f( 1.0f, 1.0f, 1.0f );
            glVertex3f(myModel.Edges[i].p1->x,myModel.Edges[i].p1->y,myModel.Edges[i].p1->z);
            glVertex3f(myModel.Edges[i].p2->x,myModel.Edges[i].p2->y,myModel.Edges[i].p2->z);
            i++;
            glEnd();
        }
        if (create){
            glColor3f(1.0, 1.0, 1.0);
            glLineWidth(2.0);
            glBegin(GL_LINES);
            glVertex3f(xCurr+0.05f, yCurr, zCurr);
            glVertex3f(xCurr-0.05f, yCurr, zCurr);
            glEnd();
            glBegin(GL_LINES);
            glVertex3f(xCurr, yCurr+0.05f, zCurr);
            glVertex3f(xCurr, yCurr-0.05f, zCurr);
            glEnd();
            glBegin(GL_LINES);
            glVertex3f(xCurr, yCurr, zCurr+0.05f);
            glVertex3f(xCurr, yCurr, zCurr-0.05f);
            glEnd();
        }

    }



}
