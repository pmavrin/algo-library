const double eps = 1e-9;

struct point {
  double x, y;

  point( double _x = 0, double _y = 0 ) : x(_x), y(_y) { }

  inline point operator - () const { return point(-x, -y); }

  inline point operator + ( const point &p ) const { return point(x + p.x, y + p.y); }
  inline point operator - ( const point &p ) const { return point(x - p.x, y - p.y); }
  inline point operator * ( const double &k ) const { return point(x * k, y * k); }
  inline point operator / ( const double &k ) const { return point(x / k, y / k); }

  inline double operator * ( const point &p ) const { return x * p.y - y * p.x; }
  inline double operator ^ ( const point &p ) const { return x * p.x + y * p.y; }

  inline bool operator == ( const point &p ) const { return fabs(x - p.x) + fabs(y - p.y) < eps; }
  inline bool operator != ( const point &p ) const { return fabs(x - p.x) + fabs(y - p.y) > eps; }
  inline bool operator < ( const point &p ) const  { return fabs(x - p.x) > eps ? x < p.x : y + eps < p.y; }

  inline void read() { assert(scanf("%lf%lf", &x, &y) == 2); }

  inline double d2() const { return x * x + y * y; }
  inline double d() const { return sqrt(d2()); }

  inline point norm() const { return *this / d(); }
  inline point ort() const { return point(-y, x); }
  inline double ang() const { return atan2(y, x); }

  point rot( double a ) {
    double co = cos(a), si = sin(a);
    return point(x * co - y * si, x * si + y * co);
  }

  static const int prec = 20;
  void out() { printf("%.*f %.*f ", prec, (double)x, prec, (double)y); }
  void outln() { out(), puts(""); }
};

string str( point p ) {
  static char buf[99];
  sprintf(buf, "(%.*f,%.*f)", point::prec, (double)p.x, point::prec, (double)p.y);
  return string(buf);
}

inline point pointByAngle( double a ) { return point(cos(a), sin(a)); }

struct line {
  double a, b, c;
  line() { }
  line( point p, point q ) {
    point no = (q - p).ort().norm();
    a = no.x;
    b = no.y;
    c = -(no ^ p);
  }
  line( point no, double C ) {
    double d = no.d();
    a = no.x / d, b = no.y / d, c = C / d;
  }
  double d( const point &p ) const { return a * p.x + b * p.y + c; }
  point no() const { return point(a, b); }
};

struct circle {
  point p;
  double r;
};

double sqr(double x) {
    return x * x;
}

vector<point> line_line( const line &l, const line &m ) {
  vector<point> v;
  double t = m.a * l.b - l.a * m.b;
  if (fabs(t) > eps)
    v.push_back(point(-(m.c * l.b - l.c * m.b) / t, (m.c * l.a - l.c * m.a) / t));
  return v;
}

vector<point> circle_line( const circle &c, const line &l ) {
  vector<point> v;
  double d = l.d(c.p);
  if (fabs(d) > c.r + eps)
    return v;
  if (fabs(fabs(d) / c.r - 1) < eps)
    v.push_back(c.p - l.no() * d);
  else {
    double s = sqrt(fabs(sqr(c.r) - sqr(d)));
    v.push_back(c.p - l.no() * d + l.no().ort() * s);
    v.push_back(c.p - l.no() * d - l.no().ort() * s);
  }
  return v;
}

void circle_circle( const circle &a, const circle &b ) {
  circle_line(a, line((b.p - a.p) * 2, a.p.d2() - b.p.d2() + sqr(b.r) - sqr(a.r)));
}

double dist2( point p, point a, point b ) {
  if (((p - a) ^ (b - a)) < 0) return (p - a).d2();
  if (((p - b) ^ (a - b)) < 0) return (p - b).d2();
  return fabs((p - a) * (b - a)) / (b - a).d2();
}

line serper(point a, point b) {
    point m = (a + b) * .5;
    point no = (b - a).norm();
    line res;
    res.a = no.x;
    res.b = no.y;
    res.c = -(no ^ m);
    return res;
}

pair<point, double> min_circle_cover_2(vector<point> points, point a, point b, int n) {
    point o = (a + b) * .5;
    double r = (a - o).d();
    // cout << a.x << "," << a.y << " " << b.x << "," << b.y << "\n";
    // cout << o.x << "," << o.y << " " << r << "\n";
    for (int i = 0; i < n; i++) {
        auto c = points[i];
        if ((c - o).d() > r + eps) {
            line l1 = serper(a, b);
            line l2 = serper(a, c);
            auto oo = line_line(l1, l2);
            if (!oo.empty()) {
                o = oo[0];
            } else {
                double x1 = min(a.x, b.x);
                x1 = min(x1, c.x);
                double x2 = max(a.x, b.x);
                x2 = max(x2, c.x);
                double y1 = min(a.y, b.y);
                y1 = min(y1, c.y);
                double y2 = max(a.y, b.y);
                y2 = max(y2, c.y);
                o.x = (x1 + x2) / 2;
                o.y = (y1 + y2) / 2;
            }
            r = (a - o).d();
            // cout << c.x << "," << c.y << "\n";
            // cout << o.x << "," << o.y << " " << r << "\n";
        }
    }
    return {o, r};
}

pair<point, double> min_circle_cover_1(vector<point> points, point a, int n) {
    point o = a;
    double r = 0;
    for (int i = 0; i < n; i++) {
        auto p = points[i];
        if ((p - o).d() > r + eps) {
            auto [oo, rr] = min_circle_cover_2(points, a, p, i + 1);
            o = oo;
            r = rr;
        }
    }
    return {o, r};
}

pair<point, double> min_circle_cover(vector<point> points) {
    random_device rd;
    mt19937 g(rd());
    shuffle(points.begin(), points.end(), g);

    point o = points[0];
    double r = 0;
    for (int i = 0; i < points.size(); i++) {
        auto p = points[i];
        if ((p - o).d() > r + eps) {
            auto [oo, rr] = min_circle_cover_1(points, p, i + 1);
            o = oo;
            r = rr;
        }
    }
    return {o, r};
}
