#include <bits/stdc++.h>

using namespace std;

#ifndef GEOMETRY_NO_INT_MACROS
#define int int64_t
#define long int64_t
#endif

#include "../geometry/int_geom.cpp"

void test_arithmetic() {
    assert(to_string128(i128(1234567890123456789LL) * 100) ==
           "123456789012345678900");
    assert(to_string128(-i128(17)) == "-17");
    assert(fraction(6, -8) == fraction(-3, 4));
    assert(fraction(0, -42) == fraction(0, 1));

    point a{2, 3};
    point b{-1, 5};
    assert(dot(a, b) == 13);
    assert(cross(a, b) == 13);
    assert(cross(point{0, 0}, point{2, 0}, point{0, 3}) == 6);
    assert(orientation({0, 0}, {2, 0}, {1, 1}) == 1);
    assert(orientation({0, 0}, {2, 0}, {1, -1}) == -1);
    assert(on_segment({1, 0}, {0, 0}, {2, 0}));
    assert(!on_segment({3, 0}, {0, 0}, {2, 0}));
    assert(distance_squared({-2, 1}, {1, 5}) == 25);
    assert(distance_to_line_squared({1, 2}, {0, 0}, {2, 0}) == fraction(4));
    assert(distance_to_segment_squared({3, 1}, {0, 0}, {2, 0}) == fraction(2));
    assert(distance_to_segment_squared({1, 1}, {0, 0}, {2, 2}) == fraction(0));
}

void test_intersections() {
    auto crossing = line_intersection({0, 0}, {2, 2}, {0, 2}, {2, 0});
    assert(crossing.kind == line_intersection_kind::point);
    assert((crossing.intersection == rational_point{{1, 1}, {1, 1}}));

    auto rational = line_intersection({0, 0}, {3, 0}, {1, -1}, {2, 2});
    assert(rational.kind == line_intersection_kind::point);
    assert((rational.intersection == rational_point{{4, 3}, {0, 1}}));
    assert(line_intersection({0, 0}, {1, 0}, {0, 1}, {1, 1}).kind ==
           line_intersection_kind::none);
    assert(line_intersection({0, 0}, {1, 0}, {2, 0}, {3, 0}).kind ==
           line_intersection_kind::coincident);

    auto overlap = segment_intersection({0, 0}, {5, 0}, {2, 0}, {7, 0});
    assert(overlap.kind == segment_intersection_kind::overlap);
    assert(overlap.overlap_start == point({2, 0}));
    assert(overlap.overlap_end == point({5, 0}));

    auto endpoint = segment_intersection({0, 0}, {2, 0}, {2, 0}, {2, 3});
    assert(endpoint.kind == segment_intersection_kind::point);
    assert(endpoint.intersection == as_rational({2, 0}));

    auto middle = segment_intersection({0, 0}, {3, 0}, {1, -1}, {2, 2});
    assert(middle.kind == segment_intersection_kind::point);
    assert((middle.intersection == rational_point{{4, 3}, {0, 1}}));
    assert(!segments_intersect({0, 0}, {1, 0}, {2, -1}, {2, 1}));
    assert(segments_intersect({1, 1}, {1, 1}, {0, 0}, {2, 2}));
}

void test_polygons() {
    vector<point> square = {{0, 0}, {4, 0}, {4, 4}, {0, 4}};
    assert(signed_double_area(square) == 32);
    assert(polygon_orientation(square) == 1);
    assert(point_in_polygon(square, {2, 2}) == point_location::inside);
    assert(point_in_polygon(square, {4, 2}) == point_location::boundary);
    assert(point_in_polygon(square, {5, 2}) == point_location::outside);
    assert(point_in_triangle({1, 1}, {0, 0}, {4, 0}, {0, 4}) ==
           point_location::inside);
    assert(point_in_triangle({2, 2}, {0, 0}, {4, 0}, {0, 4}) ==
           point_location::boundary);
    assert(point_in_triangle({5, 0}, {0, 0}, {2, 0}, {4, 0}) ==
           point_location::outside);
    assert(point_in_convex_polygon(square, {2, 2}) == point_location::inside);
    assert(point_in_convex_polygon(square, {0, 3}) == point_location::boundary);
    assert(point_in_convex_polygon(square, {-1, 3}) == point_location::outside);

    vector<point> concave = {{0, 0}, {4, 0}, {4, 4}, {2, 2}, {0, 4}};
    assert(is_convex_polygon(square));
    assert(!is_convex_polygon(concave));
    assert(point_in_polygon(concave, {1, 1}) == point_location::inside);
    assert(point_in_polygon(concave, {2, 3}) == point_location::outside);
    assert(point_in_polygon(concave, {3, 3}) == point_location::boundary);

    vector<point> cloud = {{0, 0}, {2, 0}, {2, 2}, {0, 2}, {1, 0},
                           {1, 1}, {2, 1}, {0, 0}};
    assert(convex_hull(cloud) ==
           vector<point>({{0, 0}, {2, 0}, {2, 2}, {0, 2}}));
    assert(convex_hull(cloud, true) ==
           vector<point>({{0, 0}, {1, 0}, {2, 0}, {2, 1}, {2, 2}, {0, 2}}));
    assert(convex_hull({{0, 0}, {2, 0}, {1, 0}}, true) ==
           vector<point>({{0, 0}, {1, 0}, {2, 0}}));

    vector<point> lattice_square = {{0, 0}, {2, 0}, {2, 2}, {0, 2}};
    assert(boundary_lattice_points(lattice_square) == 8);
    assert(interior_lattice_points(lattice_square) == 1);
    assert((polygon_centroid(lattice_square) ==
            rational_point{{1, 1}, {1, 1}}));
}

i128 brute_diameter(const vector<point> &points) {
    i128 answer = 0;
    for (point first : points) {
        for (point second : points) {
            answer = max(answer, distance_squared(first, second));
        }
    }
    return answer;
}

i128 brute_closest(const vector<point> &points) {
    if (points.size() < 2) return -1;
    i128 answer = i128(1) << 126;
    for (int i = 0; i < static_cast<int>(points.size()); ++i) {
        for (int j = i + 1; j < static_cast<int>(points.size()); ++j) {
            answer = min(answer, distance_squared(points[i], points[j]));
        }
    }
    return answer;
}

void test_optimized_algorithms(mt19937_64 &rng) {
    for (int iteration = 0; iteration < 3000; ++iteration) {
        int size = rng() % 20;
        vector<point> points(size);
        for (point &value : points) {
            value.x = int(rng() % 21) - 10;
            value.y = int(rng() % 21) - 10;
        }
        vector<point> hull = convex_hull(points);
        assert(convex_diameter_squared(hull) == brute_diameter(points));
        assert(closest_pair_squared(points) == brute_closest(points));
        if (hull.size() >= 3) assert(is_convex_polygon(hull));
        for (int probe = 0; probe < 10; ++probe) {
            point value{int(rng() % 31) - 15, int(rng() % 31) - 15};
            assert(point_in_convex_polygon(hull, value) ==
                   point_in_polygon(hull, value));
        }
    }
}

void test_minkowski(mt19937_64 &rng) {
    for (int iteration = 0; iteration < 3000; ++iteration) {
        vector<point> first(rng() % 9);
        vector<point> second(rng() % 9);
        for (point &value : first) {
            value.x = int(rng() % 11) - 5;
            value.y = int(rng() % 11) - 5;
        }
        for (point &value : second) {
            value.x = int(rng() % 11) - 5;
            value.y = int(rng() % 11) - 5;
        }

        vector<point> all_sums;
        for (point left : first) {
            for (point right : second) all_sums.push_back(left + right);
        }
        vector<point> expected = convex_hull(all_sums);
        vector<point> actual = minkowski_sum(first, second);
        assert(actual == expected);
    }
}

void test_circles() {
    circle base{{0, 0}, 5};
    assert(point_in_circle(base, {0, 0}) == point_location::inside);
    assert(point_in_circle(base, {3, 4}) == point_location::boundary);
    assert(point_in_circle(base, {6, 0}) == point_location::outside);
    assert(line_circle_intersection_count({-10, 0}, {10, 0}, base) == 2);
    assert(line_circle_intersection_count({-10, 5}, {10, 5}, base) == 1);
    assert(line_circle_intersection_count({-10, 6}, {10, 6}, base) == 0);
    assert(relation(base, {{11, 0}, 5}) == circle_relation::separate);
    assert(relation(base, {{10, 0}, 5}) == circle_relation::externally_tangent);
    assert(relation(base, {{6, 0}, 5}) == circle_relation::intersecting);
    assert(relation(base, {{3, 0}, 2}) == circle_relation::internally_tangent);
    assert(relation(base, {{1, 0}, 2}) == circle_relation::contained);
    assert(relation(base, {{0, 0}, 5}) == circle_relation::identical);
    assert(circle_intersection_count(base, {{6, 0}, 5}) == 2);
    assert(circle_intersection_count(base, base) == -1);
    assert(circle_intersection_count({{0, 0}, 0}, {{0, 0}, 0}) == 1);
}

bool rational_point_less(const rational_point &first, const rational_point &second) {
    if (!(first.x == second.x)) {
        return first.x.numerator * second.x.denominator <
               second.x.numerator * first.x.denominator;
    }
    return first.y.numerator * second.y.denominator <
           second.y.numerator * first.y.denominator;
}

// Independent cubic oracle: enumerate boundary intersections and boundary
// projections of the origin, then check each candidate against every plane.
half_plane_intersection_result brute_half_planes(const vector<half_plane> &planes) {
    bool feasible = planes.empty();
    vector<rational_point> vertices;
    auto accepts = [&](i128 x, i128 y, i128 denominator) {
        if (denominator < 0) {
            x = -x;
            y = -y;
            denominator = -denominator;
        }
        for (const half_plane &plane : planes) {
            if (plane.a * x + plane.b * y < plane.c * denominator) return false;
        }
        return true;
    };
    feasible |= accepts(0, 0, 1);
    for (const half_plane &plane : planes) {
        feasible |= accepts(plane.a * plane.c, plane.b * plane.c,
                            plane.a * plane.a + plane.b * plane.b);
        for (const half_plane &other : planes) {
            i128 denominator = plane.a * other.b - other.a * plane.b;
            if (denominator == 0) continue;
            i128 x = plane.c * other.b - other.c * plane.b;
            i128 y = plane.a * other.c - other.a * plane.c;
            if (accepts(x, y, denominator)) {
                feasible = true;
                vertices.push_back({{x, denominator}, {y, denominator}});
            }
        }
    }
    if (!feasible) return {};
    bool unbounded = planes.empty();
    // A nontrivial recession cone contains a direction parallel to at least
    // one boundary (unless there are no constraints).
    for (const half_plane &plane : planes) {
        for (int direction : {-1, 1}) {
            i128 dx = plane.b * direction, dy = -plane.a * direction;
            bool valid = true;
            for (const half_plane &other : planes) {
                valid &= other.a * dx + other.b * dy >= 0;
            }
            unbounded |= valid;
        }
    }
    if (unbounded) return {half_plane_intersection_kind::unbounded, {}};
    sort(vertices.begin(), vertices.end(), rational_point_less);
    vertices.erase(unique(vertices.begin(), vertices.end()), vertices.end());
    return {half_plane_intersection_kind::bounded, vertices};
}

void check_half_planes(const vector<half_plane> &planes) {
    auto expected = brute_half_planes(planes);
    auto actual = half_plane_intersection(planes);
    assert(actual.kind == expected.kind);
    if (actual.vertices.size() >= 3) {
        // Every consecutive triple must turn counterclockwise.
        for (size_t i = 0; i < actual.vertices.size(); ++i) {
            const auto &a = actual.vertices[i];
            const auto &b = actual.vertices[(i + 1) % actual.vertices.size()];
            const auto &c = actual.vertices[(i + 2) % actual.vertices.size()];
            fraction dx1(b.x.numerator * a.x.denominator - a.x.numerator * b.x.denominator,
                         b.x.denominator * a.x.denominator);
            fraction dy1(b.y.numerator * a.y.denominator - a.y.numerator * b.y.denominator,
                         b.y.denominator * a.y.denominator);
            fraction dx2(c.x.numerator * a.x.denominator - a.x.numerator * c.x.denominator,
                         c.x.denominator * a.x.denominator);
            fraction dy2(c.y.numerator * a.y.denominator - a.y.numerator * c.y.denominator,
                         c.y.denominator * a.y.denominator);
            assert(dx1.numerator * dy2.numerator * dy1.denominator * dx2.denominator >
                   dy1.numerator * dx2.numerator * dx1.denominator * dy2.denominator);
        }
    }
    sort(actual.vertices.begin(), actual.vertices.end(), rational_point_less);
    assert(actual.vertices == expected.vertices);
}

void test_half_planes(mt19937_64 &rng) {
    using kind = half_plane_intersection_kind;
    assert(half_plane_intersection({}).kind == kind::unbounded);
    vector<vector<half_plane>> cases = {
        {{1, 0, 2}},                                      // half-plane
        {{1, 0, 2}, {-1, 0, -5}},                        // strip
        {{1, 0, 2}, {-1, 0, -2}},                        // vertical line
        {{1, 0, 2}, {-1, 0, -1}},                        // contradiction
        {{1, 0, 0}, {0, 1, 0}},                         // wedge
        {{1, 0, 0}, {0, 1, 0}, {-1, -1, 1}},            // empty, no parallels
        {{1, 0, 0}, {0, 1, 0}, {-1, -1, 0}},            // point
        {{1, -1, 0}, {-1, 1, 0}, {1, 0, 0}},            // diagonal ray
        {{1, -1, 0}, {-1, 1, 0}, {1, 0, 0}, {-1, 0, -3}}, // diagonal segment
        {{0, 1, 2}, {0, -1, -2}},                       // horizontal line
        {{0, 1, 2}, {0, -1, -1}},                       // horizontal contradiction
        {{1, 0, 2}, {-1, 0, -2}, {0, 1, 0}, {0, -1, -3}}, // vertical segment
        {{0, 1, 0}, {0, -1, 0}, {1, 0, 0}, {-1, 0, -3}}, // horizontal segment
        {{0, 2, 1}, {2, 0, 1}, {-2, -2, -3}},           // rational triangle
        {{1, 0, 0}, {1, 0, 1}, {2, 0, 2}, {-1, 0, -3},
         {0, 1, 0}, {0, -1, -4}},                       // duplicates/redundant
        {{1, 0, 1000000000}, {-1, 0, -1000000001},
         {0, 1, 1000000000}, {0, -1, -1000000001}}
    };
    for (auto planes : cases) {
        for (int repeat = 0; repeat < 10; ++repeat) {
            shuffle(planes.begin(), planes.end(), rng);
            check_half_planes(planes);
        }
    }
    vector<point> polygon = {{0, 0}, {3, 0}, {3, 2}, {0, 2}};
    vector<half_plane> square;
    for (size_t i = 0; i < polygon.size(); ++i) {
        square.emplace_back(polygon[i], polygon[(i + 1) % polygon.size()]);
    }
    check_half_planes(square);
    assert(half_plane_intersection(square).vertices.size() == 4);

    for (int iteration = 0; iteration < 15000; ++iteration) {
        vector<half_plane> planes;
        int n = rng() % 14;
        for (int i = 0; i < n; ++i) {
            int64_t a = int64_t(rng() % 11) - 5;
            int64_t b = int64_t(rng() % 11) - 5;
            if (a == 0 && b == 0) a = 1;
            planes.emplace_back(a, b, int64_t(rng() % 21) - 10);
        }
        check_half_planes(planes);
    }
    // Many active envelope pieces: y >= 2*t*x - t*t, closed by y <= limit^2.
    const int64_t limit = 10000;
    vector<half_plane> many;
    for (int64_t t = -limit; t <= limit; ++t) many.emplace_back(-2 * t, 1, -t * t);
    many.emplace_back(0, -1, -limit * limit);
    auto result = half_plane_intersection(many);
    assert(result.kind == kind::bounded);
    assert(result.vertices.size() == size_t(2 * limit + 2));
}

signed main() {
    mt19937_64 rng(123456789);
    test_arithmetic();
    test_intersections();
    test_polygons();
    test_optimized_algorithms(rng);
    test_minkowski(rng);
    test_circles();
    test_half_planes(rng);
    cout << "geometry tests passed\n";
}
