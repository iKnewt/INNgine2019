#include "innpch.h"
#include "vertex.h"

Vertex::Vertex() { }

Vertex::Vertex(float x, float y, float z, float r, float g, float b)
{
    XYZ_.setX(x);
    XYZ_.setY(y);
    XYZ_.setZ(z);

    normal_.setX(r);
    normal_.setY(g);
    normal_.setZ(b);
}

Vertex::Vertex(gsl::Vector3D a, gsl::Vector3D b , gsl::Vector2D c)
{
    XYZ_ = a;
    normal_ = b;
    ST_ = c;
}

Vertex::~Vertex() { }

void Vertex::set_xyz(GLfloat *xyz)
{
    XYZ_.setX(xyz[0]);
    XYZ_.setY(xyz[1]);
    XYZ_.setZ(xyz[2]);
}

void Vertex::set_xyz(GLfloat x, GLfloat y, GLfloat z)
{
    XYZ_.setX(x);
    XYZ_.setY(y);
    XYZ_.setZ(z);
}

void Vertex::set_xyz(gsl::Vector3D xyz_in)
{
    XYZ_ = xyz_in;
}

void Vertex::set_rgb(GLfloat *rgb)
{
    normal_.setX(rgb[0]);
    normal_.setY(rgb[1]);
    normal_.setZ(rgb[2]);
}

void Vertex::set_rgb(GLfloat r, GLfloat g, GLfloat b)
{
    normal_.setX(r);
    normal_.setY(g);
    normal_.setZ(b);
}

void Vertex::set_normal(GLfloat *normal)
{
    normal_.setX(normal[0]);
    normal_.setY(normal[1]);
    normal_.setZ(normal[2]);
}

void Vertex::set_normal(GLfloat x, GLfloat y, GLfloat z)
{
    normal_.setX(x); normal_.setY(y); normal_.setZ(z);
}

void Vertex::set_normal(gsl::Vector3D normal_in)
{
    normal_ = normal_in;
}

void Vertex::set_st(GLfloat *st)
{
    ST_.setX(st[0]);
    ST_.setY(st[1]);
}

void Vertex::set_st(GLfloat s, GLfloat t)
{
    ST_.setX(s); ST_.setY(t);
}

void Vertex::set_uv(GLfloat u, GLfloat v)
{
    ST_.setX(u);
    ST_.setY(v);
}

std::ostream& operator<< (std::ostream& os, const Vertex& v) {
    os << std::fixed;
    os << "(" << v.XYZ_.getX() << ", " << v.XYZ_.getY() << ", " << v.XYZ_.getZ() << ") ";
    os << "(" << v.normal_.getX() << ", " << v.normal_.getY() << ", " << v.normal_.getZ() << ") ";
    os << "(" << v.ST_.getX() << ", " << v.ST_.getY() << ") ";
    return os;
}

std::istream& operator>> (std::istream& is, Vertex& v) {
    // needs 4 temps to get commas and parenthesis
    char temp, temp2, temp3, temp4;
    is >> temp >> v.XYZ_.x >> temp2 >> v.XYZ_.y >> temp3 >> v.XYZ_.z >> temp4;
    is >> temp >> v.normal_.x >> temp2 >> v.normal_.y >> temp3 >> v.normal_.z >> temp4;
    is >> temp >> v.ST_.x >> temp2 >> v.ST_.y >> temp3;
    return is;
}
