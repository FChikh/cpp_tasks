#include <iostream>
#include <vector>
#include <string>
#include <cmath>

const double ACC = 1e-6;

class Line;

bool isEqual(const double &a, const double &b) {
    return abs(b - a) < ACC;
}

bool isLess(const double &a, const double &b) {
    return b - a > ACC;
}

bool isGreater(const double &a, const double &b) {
    return a - b > ACC;
}

bool isLessEqual(const double &a, const double &b) {
    return isLess(a, b) || isEqual(a, b);
}

bool isGreaterEqual(const double &a, const double &b) {
    return isGreater(a, b) || isEqual(a, b);
}

struct Point {
    double x, y;

    Point(const double &x, const double &y) : x(x), y(y) {};

    Point() : x(0), y(0) {};

    bool operator==(const Point &a) const {
        return (isEqual(x, a.x) && isEqual(y, a.y));
    }

    bool operator!=(const Point &a) const {
        return !(*this == a);
    }

    double distance(const Point &a) const {
        return sqrt(((x - a.x) * (x - a.x) + (y - a.y) * (y - a.y)));
    }

    void rotate(const Point &center, const double &angle) {
        double s = sin(M_PI * angle / 180);
        double c = cos(M_PI * angle / 180);
        x -= center.x;
        y -= center.y;
        double x_new = x * c - y * s;
        double y_new = x * s + y * c;
        x = center.x + x_new;
        y = center.y + y_new;
    }

    void reflex(const Point &center) {
        x = 2 * center.x - x;
        y = 2 * center.y - y;
    }

    void reflex(const Line &axis);

    void scale(const Point &center, const double &coefficient) {
        x = center.x + (x - center.x) * coefficient;
        y = center.y + (y - center.y) * coefficient;
    }

    Point operator-(const Point &a) const {
        return Point(x - a.x, y - a.y);
    }
};

double sca(const Point &a, const Point &b) {
    return a.x * b.x + a.y * b.y;
}

double vec(const Point &a, const Point &b) {
    return a.x * b.y - a.y * b.x;
}

class Line {
private:
    double a, b, c;
public:
    Line() = default;

    ~Line() = default;

    Line(const double &a, const double &b, const double &c) : a(a), b(b), c(c) {

    }

    Line(const Point &n, const Point &m) {
        a = n.y - m.y;
        b = m.x - n.x;
        c = n.x * m.y - n.y * m.x;
    }

    Line(const double &k, const double &l) {
        a = k;
        b = -1;
        c = l;
    }

    Line(const Point &n, const double &k) {
        a = k;
        b = -1;
        c = n.y - a * n.x;
    }

    bool operator==(const Line &x) const {
        if (a == 0) {
            if (x.a == 0) {
                if (c == 0) {
                    return (x.b == b && x.c == c);
                }
                return (x.b / b == x.c / c);
            } else {
                return false;
            }
        }
        if (b == 0) {
            if (x.b == 0) {
                if (c == 0) {
                    return (x.a == a && x.c == c);
                }
                return (x.a / a == x.c / c);
            } else {
                return false;
            }
        }
        if (c == 0) {
            return (x.a / a == x.b / b);
        }
        return (isEqual(x.a / a, x.b / b) && isEqual(x.a / a, x.c / c));
    }

    bool operator!=(const Line &x) const {
        return !(*this == x);
    }

    std::pair<Line, Line> parallel(const double &len) const {
        return {Line(a, b, c - len * sqrt(a * a + b * b)), Line(a, b, c + len * sqrt(a * a + b * b))};
    }

    Line normal_line(const Point p) const {
        return Line(-b, a, -a * p.y + b * p.x);
    }

    std::pair<double, double> normal() const {
        return {a, b};
    }

    Point intersection(const Line &x) const {
        double det = a * x.b - b * x.a;
        if (det == 0) {
            return {1e16, 1e16};
        } else {
            double det_x = -c * x.b + b * x.c;
            double det_y = -a * x.c + x.a * c;
            return {det_x / det, det_y / det};
        }
    }
};

void Point::reflex(const Line &axis) {
    Line tmp = axis.normal_line(*this);
    Point o = axis.intersection(tmp);
    x = 2 * o.x - x;
    y = 2 * o.y - y;
}

class Shape {
public:
    Shape() = default;

    virtual double perimeter() const = 0;

    virtual double area() const = 0;

    virtual bool operator==([[maybe_unused]] const Shape &x) const = 0;

    virtual bool operator!=([[maybe_unused]] const Shape &x) const = 0;

    virtual bool isCongruentTo([[maybe_unused]] const Shape &x) const = 0;

    virtual bool isSimilarTo([[maybe_unused]] const Shape &x) const = 0;

    virtual bool containsPoint(const Point &point) const = 0;

    virtual void rotate(const Point &center, const double &angle) = 0;

    virtual void reflex(const Point &center) = 0;

    virtual void reflex(const Line &axis) = 0;

    virtual void scale(const Point &center, const double &coefficient) = 0;

    virtual ~Shape() = default;
};

class Polygon : public Shape {
protected:
    std::vector<Point> vertices;
    size_t cnt;
public:

    Polygon(const std::initializer_list<Point> &new_nodes) {
        vertices.assign(new_nodes.begin(), new_nodes.end());
        cnt = vertices.size();
    }

    Polygon(const std::vector<Point> &new_nodes) {
        vertices.assign(new_nodes.begin(), new_nodes.end());
        cnt = vertices.size();
    }

    bool operator==([[maybe_unused]] const Shape &x) const override {
        auto *tmp = dynamic_cast<const Polygon *>(&x);
        if (tmp == nullptr) {
            return false;
        } else {
            return (*this == *tmp);
        }
    }

    bool operator!=([[maybe_unused]] const Shape &x) const override {
        return !(*this == x);
    }

    bool isCongruentTo([[maybe_unused]] const Shape &x) const override {
        auto *tmp = dynamic_cast<const Polygon *>(&x);
        if (tmp == nullptr) {
            return false;
        } else {
            return (this->isCongruentTo(*tmp));
        }
    }

    bool isSimilarTo([[maybe_unused]] const Shape &x) const override {
        auto *tmp = dynamic_cast<const Polygon *>(&x);
        if (tmp == nullptr) {
            return false;
        } else {
            return (this->isSimilarTo(*tmp));
        }
    }

    double perimeter() const override {
        double ans = 0;
        for (size_t i = 1; i < cnt; ++i) {
            ans += vertices[i].distance(vertices[i - 1]);
        }
        ans += vertices[0].distance(vertices[cnt - 1]);
        return ans;
    }

    double area() const override {
        double ans = 0;
        for (size_t i = 1; i < cnt; ++i) {
            ans += vertices[i].y * vertices[i - 1].x - vertices[i].x * vertices[i - 1].y;
        }
        ans += vertices[0].y * vertices[cnt - 1].x - vertices[0].x * vertices[cnt - 1].y;
        ans = abs(ans) / 2.0;
        return ans;
    }

    std::pair<std::vector<double>, std::vector<double>> getPolygonParams() const {
        std::vector<double> len(cnt, 0), a(cnt, 0);
        double sc1, vc1;
        for (size_t i = 1; i < cnt - 1; ++i) {
            len[i] = vertices[i].distance(vertices[i - 1]);
            vc1 = vec(vertices[i + 1] - vertices[i], vertices[i - 1] - vertices[i]);
            sc1 = sca(vertices[i + 1] - vertices[i], vertices[i - 1] - vertices[i]);
            a[i] = std::atan2(vc1, sc1);
            if (a[i] < 0) {
                a[i] = 2 * M_PI + a[i];
            }
        }
        len[0] = vertices[0].distance(vertices[cnt - 1]);
        len[cnt - 1] = vertices[cnt - 1].distance(vertices[cnt - 2]);
        vc1 = vec(vertices[1] - vertices[0], vertices[cnt - 1] - vertices[0]);
        sc1 = sca(vertices[1] - vertices[0], vertices[cnt - 1] - vertices[0]);
        a[0] = std::atan2(vc1, sc1);
        if (a[0] < 0) {
            a[0] = 2 * M_PI + a[0];
        }
        vc1 = vec(vertices[0] - vertices[cnt - 1], vertices[cnt - 2] - vertices[cnt - 1]);
        sc1 = sca(vertices[0] - vertices[cnt - 1], vertices[cnt - 2] - vertices[cnt - 1]);
        a[cnt - 1] = std::atan2(vc1, sc1);
        if (a[cnt - 1] < 0) {
            a[cnt - 1] = 2 * M_PI + a[cnt - 1];
        }
        return {len, a};
    }

    std::vector<Point> getVertices() const {
        return vertices;
    }

    bool operator==(const Polygon &x) const {
        if (cnt != x.cnt) {
            return false;
        }
        size_t st = cnt;
        for (size_t i = 0; i < cnt; ++i) {
            if (vertices[i] == x.vertices[0]) {
                st = i;
                break;
            }
        }
        if (st == cnt) {
            return false;
        }
        if (vertices[(st + 1) % cnt] == x.vertices[1]) {
            for (size_t i = 1; i < cnt; ++i) {
                if (vertices[(st + i) % cnt] != x.vertices[i]) {
                    return false;
                }
            }
        } else {
            for (size_t i = 1; i < cnt; ++i) {
                if (vertices[(cnt + (st - i)) % cnt] != x.vertices[i]) {
                    return false;
                }
            }
        }
        return true;
    }

    bool operator!=(const Polygon &x) const {
        return !(*this == x);
    }

    bool isCongruentTo(const Polygon &x) const {
        if (cnt != x.cnt) {
            return false;
        }
        std::pair<std::vector<double>, std::vector<double>> params1 = getPolygonParams(), params2 = x.getPolygonParams();
        std::vector<double> len1 = params1.first, len2 = params2.first;
        std::vector<double> a1 = params1.second, a2 = params2.second;
        double sum = 0;
        for (size_t i = 0; i < cnt; ++i) {
            for (size_t j = 0; j < cnt; ++j) {
                if ((isEqual(a1[j], a2[(i + j) % cnt]) ||
                     isEqual(a1[j] + a2[(i + j) % cnt], 2 * M_PI)) && isEqual(len1[j], len2[(i + j) % cnt])) {
                    ++sum;
                } else {
                    break;
                }
            }
            if (sum == cnt) {

                return true;
            } else {
                sum = 0;
            }
        }
        for (size_t i = 0; i < cnt; ++i) {
            for (size_t j = 0; j < cnt; ++j) {
                if ((isEqual(a1[j], a2[(cnt + (i - j)) % cnt]) ||
                     isEqual(a1[j] + a2[(cnt + (i - j)) % cnt], 2 * M_PI)) &&
                    isEqual(len1[j], len2[(cnt + (i - j) + 1) % cnt])) {
                    ++sum;
                } else {
                    break;
                }
            }
            if (sum == cnt) {
                return true;
            } else {
                sum = 0;
            }
        }
        return false;
    }

    bool isSimilarTo(const Polygon &x) const {
        if (cnt != x.cnt) {
            return false;
        }
        std::pair<std::vector<double>, std::vector<double>> params1 = getPolygonParams(), params2 = x.getPolygonParams();
        std::vector<double> a1 = params1.second, a2 = params2.second;
        double sum = 0;
        for (size_t i = 0; i < cnt; ++i) {
            for (size_t j = 0; j < cnt; ++j) {
                if (isEqual(a1[j], a2[(i + j) % cnt]) || isEqual(a1[j] + a2[(i + j) % cnt], 2 * M_PI)) {
                    ++sum;
                } else {
                    break;
                }
            }
            if (sum == cnt) {
                return true;
            } else {
                sum = 0;
            }
        }
        for (size_t i = 0; i < cnt; ++i) {
            for (size_t j = 0; j < cnt; ++j) {
                if (isEqual(a1[j], a2[(cnt + (i - j)) % cnt]) ||
                    isEqual(a1[j] + a2[(cnt + (i - j)) % cnt], 2 * M_PI)) {
                    ++sum;
                } else {
                    break;
                }
            }
            if (sum == cnt) {
                return true;
            } else {
                sum = 0;
            }
        }
        return false;
    }

    bool containsPoint(const Point &point) const override {
        double sum = 0;
        for (size_t i = 1; i < cnt; ++i) {
            sum += std::atan2(vec(vertices[i] - point, vertices[i - 1] - point),
                              sca(vertices[i] - point, vertices[i - 1] - point));
        }
        sum += std::atan2(vec(vertices[0] - point, vertices[cnt - 1] - point),
                          sca(vertices[0] - point, vertices[cnt - 1] - point));
        return !isEqual(sum, 0);
    }


    bool isConvex() const {
        size_t cnt = 0;
        double q1 = vec(vertices[2] - vertices[0], vertices[1] - vertices[0]), q2;
        for (size_t i = 3; i < cnt; ++i) {
            q2 = vec(vertices[i] - vertices[i - 2], vertices[i - 1] - vertices[i - 2]);
            if (q1 / q2 <= 0) {
                cnt++;
            }
            q1 = q2;
        }
        q2 = vec(vertices[0] - vertices[cnt - 2], vertices[cnt - 1] - vertices[cnt - 2]);
        if (q2 == 0 || q1 / q2 <= 0) {
            cnt++;
        }
        if ((cnt == 3 && q1 == 0) || cnt > 0) {
            return false;
        } else {
            return true;
        }
    }

    void rotate(const Point &center, const double &angle) override {
        for (size_t i = 0; i < cnt; ++i) {
            vertices[i].rotate(center, angle);
        }
    }

    void reflex(const Point &center) override {
        for (size_t i = 0; i < cnt; ++i) {
            vertices[i].reflex(center);
        }
    }

    void reflex(const Line &axis) override {
        for (size_t i = 0; i < cnt; ++i) {
            vertices[i].reflex(axis);
        }

    }

    void scale(const Point &center, const double &coefficient) override {
        for (size_t i = 0; i < cnt; ++i) {
            vertices[i].scale(center, coefficient);
        }
    }

};

class Ellipse : public Shape {
protected:
    Point f1, f2;
    double dist;
    double a, b, c, e;
public:
    Ellipse() = default;

    Ellipse(Point _f1, Point _f2, double _radius) : f1(_f1), f2(_f2), dist(_radius) {
        c = f1.distance(f2) / 2;
        a = dist / 2;
        e = c / a;
        b = a * sqrt(1 - e * e);
    }

    bool operator==([[maybe_unused]] const Shape &x) const override {
        auto *tmp = dynamic_cast<const Ellipse *>(&x);
        if (tmp == nullptr) {
            return false;
        } else {
            return (*this == *tmp);
        }

    }

    bool operator!=([[maybe_unused]] const Shape &x) const override {
        return !(*this == x);
    }

    bool isCongruentTo([[maybe_unused]] const Shape &x) const override {
        auto *tmp = dynamic_cast<const Ellipse *>(&x);
        if (tmp == nullptr) {
            return false;
        } else {
            return (this->isCongruentTo(*tmp));
        }
    }

    bool isSimilarTo([[maybe_unused]] const Shape &x) const override {
        auto *tmp = dynamic_cast<const Ellipse *>(&x);
        if (tmp == nullptr) {
            return false;
        } else {
            return (this->isSimilarTo(*tmp));
        }
    }

    std::pair<Point, Point> focuses() const {
        return {f1, f2};
    }

    std::pair<Line, Line> directrices() const {
        Point center = Point((f1.x + f2.x) / 2, (f1.y + f2.y) / 2);
        Line major = Line(f1, f2);
        Line minor = major.normal_line(center);
        return minor.parallel(a / e);
    }

    double eccentricity() const {
        return e;
    }

    virtual Point center() const {
        return Point((f1.x + f2.x) / 2, (f1.y + f2.y) / 2);
    }

    double perimeter() const override {
        return M_PI * (3 * (a + b) - sqrt((3 * a + b) * (3 * b + a)));
    }

    double area() const override {
        return M_PI * a * b;
    }

    bool operator==(const Ellipse &x) const {
        return ((f1 == x.f1) && (f2 == x.f2) && isEqual(dist, x.dist));
    };

    bool operator!=(const Ellipse &x) const {
        return !(*this == x);
    };

    bool isCongruentTo(const Ellipse &x) const {
        return isEqual(a, x.a) && isEqual(b, x.b);
    };

    bool isSimilarTo(const Ellipse &x) const {
        return isEqual(a * x.b - b * x.a, 0);
    };

    bool containsPoint(const Point &point) const override {
        std::cerr << f1.x << ' ' << f1.y << ' ' << f2.x << ' ' << f2.y << ' ' << e << '\n';
        return isLessEqual(f1.distance(point) + f2.distance(point), dist);
    };

    void rotate(const Point &center, const double &angle) override {
        f1.rotate(center, angle);
        f2.rotate(center, angle);
    }

    void reflex(const Point &center) override {
        f1.reflex(center);
        f2.reflex(center);
    }

    void reflex(const Line &axis) override {
        f1.reflex(axis);
        f2.reflex(axis);
    }

    void scale(const Point &center, const double &coefficient) override {
        f1.scale(center, coefficient);
        f2.scale(center, coefficient);
        dist *= coefficient;
        c = f1.distance(f2) / 2;
        a = dist / 2;
        e = c / a;
        b = a * sqrt(1 - e * e);
    }

};

class Circle : public Ellipse {
private:
    double r;
    Point cent;

public:
    Circle() = default;

    Circle(Point center, double radius) : Ellipse(center, center, 2 * radius) {
        cent = center, r = radius;
    }


    double radius() const {
        return r;
    }

    Point center() const override {
        return cent;
    }

    double perimeter() const override {
        return 2 * M_PI * r;
    }

    double area() const override {
        return M_PI * r * r;
    }

    bool containsPoint(const Point &point) const override {
        return isLess(cent.distance(point), r);
    }

    void rotate(const Point &center, const double &angle) override {
        Ellipse::rotate(center, angle);
        cent.rotate(center, angle);
    }

    void reflex(const Point &center) override {
        Ellipse::reflex(center);
        cent.reflex(center);
    }

    void reflex(const Line &axis) override {
        Ellipse::reflex(axis);
        cent.reflex(axis);
    }

    void scale(const Point &center, const double &coefficient) override {
        Ellipse::scale(center, coefficient);
        cent.scale(center, coefficient);
        r *= coefficient;
    }

};

class Rectangle : public Polygon {
protected:
    Point c;
public:
    Rectangle() = default;

    Rectangle(const Point &a, const Point &b, const double &ratio) : Polygon({a, b, b, b}) {
        c = Point((a.x + b.x) / 2, (a.y + b.y) / 2);
        double alpha = 180 * atan(ratio) / M_PI;
        if (ratio <= 1) {
            vertices[1].rotate(c, -2 * alpha);
            vertices[3].rotate(c, 180 - 2 * alpha);
        } else {
            vertices[1].rotate(c, -180 + 2 * alpha);
            vertices[3].rotate(c, 2 * alpha);
        }
    }
    Point center() const {
        return c;
    }

    std::pair<Line, Line> diagonals() const {
        return {Line(vertices[0], vertices[2]), Line(vertices[1], vertices[3])};
    }
    void scale(const Point &center, const double &coefficient) override {
        Polygon::scale(center, coefficient);
        c = Point((vertices[0].x + vertices[2].x) / 2, (vertices[0].y + vertices[2].y) / 2);
    }
};

class Square : public Rectangle {
private:
    double len;
public:
    Square() = default;

    Square(const Point &a, const Point &b) : Rectangle(a, b, 1) {
        len = vertices[0].distance(vertices[1]);
    }

    Circle circumscribedCircle() const {
        return Circle(c, len);
    }

    Circle inscribedCircle() const {
        return Circle(c, len * sqrt(2));
    }

    void scale(const Point &center, const double &coefficient) override {
        Polygon::scale(center, coefficient);
        len = vertices[0].distance(vertices[1]);
    }
};

class Triangle : public Polygon {
private:
    double len12, len23, len31;
public:
    Triangle() = default;

    Triangle(const Point &x, const Point &y, const Point &z) : Polygon({x, y, z}) {
        len12 = vertices[0].distance(vertices[1]);
        len23 = vertices[1].distance(vertices[2]);
        len31 = vertices[2].distance(vertices[0]);
    }


    Circle circumscribedCircle() const {
        Point p1 = vertices[0], p2 = vertices[1], p3 = vertices[2];
        double x = (sca(p1, p1) * (p2.y - p3.y) + sca(p2, p2) * (p3.y - p1.y) + sca(p3, p3) * (p1.y - p2.y));
        double y = (sca(p1, p1) * (p3.x - p2.x) + sca(p2, p2) * (p1.x - p3.x) + sca(p3, p3) * (p2.x - p1.x));
        double d = 2 * (p1.x * (p2.y - p3.y) + p2.x * (p3.y - p1.y) + p3.x * (p1.y - p2.y));
        double r = (len12 * len23 * len31) / (4 * area());
        x /= d;
        y /= d;
        Circle tmp = Circle(Point(x, y), r);
        return tmp;
    }

    Circle inscribedCircle() const {
        Point p1 = vertices[0], p2 = vertices[1], p3 = vertices[2];
        double x = (len12 * p3.x + len23 * p1.x + len31 * p2.x) / (len12 + len23 + len31);
        double y = (len12 * p3.y + len23 * p1.y + len31 * p2.y) / (len12 + len23 + len31);
        double r = 2 * area() / (len12 + len23 + len31);
        return Circle(Point(x, y), r);
    }

    Point centroid() const {
        return Point((vertices[0].x + vertices[1].x + vertices[2].x) / 3,
                     (vertices[0].y + vertices[1].y + vertices[2].y) / 3);
    }

    Point orthocenter() const {
        Point p1 = vertices[0], p2 = vertices[1], p3 = vertices[2];
        Point o = circumscribedCircle().center();
        Point a = Point(p1.x - o.x, p1.y - o.y);
        Point b = Point(p2.x - o.x, p2.y - o.y);
        Point c = Point(p3.x - o.x, p3.y - o.y);
        return Point(o.x + a.x + b.x + c.x, o.y + a.y + b.y + c.y);
    }

    Line EulerLine() const {
        return Line(orthocenter(), circumscribedCircle().center());
    }

    Circle ninePointsCircle() const {
        Circle o = circumscribedCircle();
        Point h = orthocenter();
        return Circle(Point((o.center().x + h.x) / 2, (o.center().y + h.y) / 2), o.radius() / 2);
    }

    void scale(const Point &center, const double &coefficient) override {
        Polygon::scale(center, coefficient);
        len12 = vertices[0].distance(vertices[1]);
        len23 = vertices[1].distance(vertices[2]);
        len31 = vertices[2].distance(vertices[0]);
    }
};