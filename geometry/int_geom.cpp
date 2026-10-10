// Exact 2D geometry for integer coordinates.
// Products and determinants use __int128_t. Input coordinates and results of
// point addition/subtraction must fit into int64_t; exact intermediate values
// must fit into __int128_t.

// ---------- Exact arithmetic ----------

using i128 = __int128_t;

i128 abs128(i128 value) {
    return value < 0 ? -value : value;
}

i128 gcd128(i128 first, i128 second) {
    first = abs128(first);
    second = abs128(second);
    while (second != 0) {
        i128 remainder = first % second;
        first = second;
        second = remainder;
    }
    return first;
}

string to_string128(i128 value) {
    if (value == 0) return "0";
    bool negative = value < 0;
    __uint128_t magnitude = negative
        ? __uint128_t(-(value + 1)) + 1
        : __uint128_t(value);
    string result;
    while (magnitude > 0) {
        result.push_back(char('0' + magnitude % 10));
        magnitude /= 10;
    }
    if (negative) result.push_back('-');
    reverse(result.begin(), result.end());
    return result;
}

struct fraction {
    i128 numerator = 0;
    i128 denominator = 1;

    fraction(i128 numerator_value = 0, i128 denominator_value = 1)
        : numerator(numerator_value), denominator(denominator_value) {
        assert(this->denominator != 0);
        if (this->denominator < 0) {
            this->numerator = -this->numerator;
            this->denominator = -this->denominator;
        }
        i128 divisor = gcd128(this->numerator, this->denominator);
        this->numerator /= divisor;
        this->denominator /= divisor;
    }

    friend bool operator==(const fraction &left, const fraction &right) {
        return left.numerator == right.numerator &&
               left.denominator == right.denominator;
    }
};

// ---------- Points and vectors ----------

struct point {
    int64_t x = 0;
    int64_t y = 0;

    point operator+() const { return *this; }
    point operator-() const { return {-x, -y}; }
    point operator+(point other) const { return {x + other.x, y + other.y}; }
    point operator-(point other) const { return {x - other.x, y - other.y}; }
    point operator*(int64_t multiplier) const {
        return {x * multiplier, y * multiplier};
    }

    point &operator+=(point other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    friend bool operator==(point left, point right) {
        return left.x == right.x && left.y == right.y;
    }
    friend bool operator!=(point left, point right) { return !(left == right); }
    friend bool operator<(point left, point right) {
        return left.x < right.x || (left.x == right.x && left.y < right.y);
    }
};

struct rational_point {
    fraction x;
    fraction y;

    friend bool operator==(const rational_point &left, const rational_point &right) {
        return left.x == right.x && left.y == right.y;
    }
};

rational_point as_rational(point value) {
    return {{value.x, 1}, {value.y, 1}};
}

i128 dot(point first, point second) {
    return i128(first.x) * second.x + i128(first.y) * second.y;
}

i128 dot(point origin, point first, point second) {
    i128 first_x = i128(first.x) - origin.x;
    i128 first_y = i128(first.y) - origin.y;
    i128 second_x = i128(second.x) - origin.x;
    i128 second_y = i128(second.y) - origin.y;
    return first_x * second_x + first_y * second_y;
}

i128 cross(point first, point second) {
    return i128(first.x) * second.y - i128(first.y) * second.x;
}

i128 cross(point origin, point first, point second) {
    i128 first_x = i128(first.x) - origin.x;
    i128 first_y = i128(first.y) - origin.y;
    i128 second_x = i128(second.x) - origin.x;
    i128 second_y = i128(second.y) - origin.y;
    return first_x * second_y - first_y * second_x;
}

i128 distance_squared(point first, point second) {
    i128 dx = i128(first.x) - second.x;
    i128 dy = i128(first.y) - second.y;
    return dx * dx + dy * dy;
}

int sign(i128 value) {
    return (value > 0) - (value < 0);
}

int orientation(point first, point second, point third) {
    return sign(cross(first, second, third));
}

bool on_segment(point value, point first, point second) {
    if (orientation(first, second, value) != 0) return false;
    return min(first.x, second.x) <= value.x && value.x <= max(first.x, second.x) &&
           min(first.y, second.y) <= value.y && value.y <= max(first.y, second.y);
}

fraction distance_to_line_squared(point value, point first, point second) {
    assert(first != second);
    i128 product = cross(first, second, value);
    return {product * product, distance_squared(first, second)};
}

fraction distance_to_segment_squared(point value, point first, point second) {
    if (first == second) return {distance_squared(value, first), 1};
    if (dot(first, value, second) <= 0) {
        return {distance_squared(value, first), 1};
    }
    if (dot(second, value, first) <= 0) {
        return {distance_squared(value, second), 1};
    }
    return distance_to_line_squared(value, first, second);
}

int polar_half(point vector) {
    return vector.y < 0 || (vector.y == 0 && vector.x < 0);
}

bool polar_less(point first, point second) {
    if (polar_half(first) != polar_half(second)) {
        return polar_half(first) < polar_half(second);
    }
    i128 product = cross(first, second);
    if (product != 0) return product > 0;
    return distance_squared({0, 0}, first) < distance_squared({0, 0}, second);
}

// ---------- Lines and segments ----------

enum class line_intersection_kind {
    none,
    point,
    coincident
};

struct line_intersection_result {
    line_intersection_kind kind = line_intersection_kind::none;
    rational_point intersection;
};

line_intersection_result line_intersection(
    point first_a,
    point first_b,
    point second_a,
    point second_b
) {
    assert(first_a != first_b && second_a != second_b);
    i128 first_dx = i128(first_b.x) - first_a.x;
    i128 first_dy = i128(first_b.y) - first_a.y;
    i128 second_dx = i128(second_b.x) - second_a.x;
    i128 second_dy = i128(second_b.y) - second_a.y;
    i128 denominator = first_dx * second_dy - first_dy * second_dx;
    if (denominator == 0) {
        if (orientation(first_a, first_b, second_a) == 0) {
            return {line_intersection_kind::coincident, {}};
        }
        return {line_intersection_kind::none, {}};
    }

    i128 offset_x = i128(second_a.x) - first_a.x;
    i128 offset_y = i128(second_a.y) - first_a.y;
    i128 parameter = offset_x * second_dy - offset_y * second_dx;
    i128 x_numerator = i128(first_a.x) * denominator + first_dx * parameter;
    i128 y_numerator = i128(first_a.y) * denominator + first_dy * parameter;
    return {
        line_intersection_kind::point,
        {{x_numerator, denominator}, {y_numerator, denominator}}
    };
}

enum class segment_intersection_kind {
    none,
    point,
    overlap
};

struct segment_intersection_result {
    segment_intersection_kind kind = segment_intersection_kind::none;
    rational_point intersection;
    point overlap_start;
    point overlap_end;
};

segment_intersection_result segment_intersection(
    point first_a,
    point first_b,
    point second_a,
    point second_b
) {
    if (first_a == first_b) {
        if (on_segment(first_a, second_a, second_b)) {
            return {segment_intersection_kind::point, as_rational(first_a), {}, {}};
        }
        return {};
    }
    if (second_a == second_b) {
        if (on_segment(second_a, first_a, first_b)) {
            return {segment_intersection_kind::point, as_rational(second_a), {}, {}};
        }
        return {};
    }

    int first_side_a = orientation(first_a, first_b, second_a);
    int first_side_b = orientation(first_a, first_b, second_b);
    int second_side_a = orientation(second_a, second_b, first_a);
    int second_side_b = orientation(second_a, second_b, first_b);

    if (first_side_a == 0 && first_side_b == 0) {
        if (first_b < first_a) swap(first_a, first_b);
        if (second_b < second_a) swap(second_a, second_b);
        point left = max(first_a, second_a);
        point right = min(first_b, second_b);
        if (right < left) return {};
        if (left == right) {
            return {segment_intersection_kind::point, as_rational(left), {}, {}};
        }
        return {segment_intersection_kind::overlap, {}, left, right};
    }

    if (first_side_a * first_side_b > 0 || second_side_a * second_side_b > 0) {
        return {};
    }
    line_intersection_result result = line_intersection(
        first_a,
        first_b,
        second_a,
        second_b
    );
    assert(result.kind == line_intersection_kind::point);
    return {segment_intersection_kind::point, result.intersection, {}, {}};
}

bool segments_intersect(point first_a, point first_b, point second_a, point second_b) {
    return segment_intersection(first_a, first_b, second_a, second_b).kind !=
           segment_intersection_kind::none;
}

// ---------- Half-planes ----------

// Closed half-plane a*x + b*y >= c. For two points, the allowed side is
// to the left of the directed line first -> second (including the line).
struct half_plane {
    i128 a, b, c;

    half_plane(int64_t a_value, int64_t b_value, int64_t c_value)
        : a(a_value), b(b_value), c(c_value) {
        assert(a != 0 || b != 0);
    }

    half_plane(point first, point second)
        : a(i128(first.y) - second.y),
          b(i128(second.x) - first.x),
          c(a * first.x + b * first.y) {
        assert(first != second);
    }
};

enum class half_plane_intersection_kind {
    empty,
    bounded,
    unbounded
};

struct half_plane_intersection_result {
    half_plane_intersection_kind kind = half_plane_intersection_kind::empty;
    // Only populated for bounded intersections: CCW polygon without a
    // repeated first vertex, two segment endpoints, or a single point.
    vector<rational_point> vertices;
};

// O(n log n) time, O(n) memory. No bounding box or floating-point arithmetic.
// Empty input means the whole plane. All intermediate products (including
// comparisons of fractions) must fit into i128; full-range int64 is not safe.
half_plane_intersection_result half_plane_intersection(
    const vector<half_plane> &planes
) {
    auto less = [](const fraction &first, const fraction &second) {
        return first.numerator * second.denominator <
               second.numerator * first.denominator;
    };
    auto tighten_left = [&](optional<fraction> &bound, const fraction &value) {
        if (!bound || less(*bound, value)) bound = value;
    };
    auto tighten_right = [&](optional<fraction> &bound, const fraction &value) {
        if (!bound || less(value, *bound)) bound = value;
    };

    // y = (k*x + m)/d, d > 0. Build maxima of the lower bounds and of the
    // negated upper bounds. Their sum must be <= 0.
    struct boundary {
        i128 k, m, d;
        optional<fraction> start; // nullopt means -infinity
    };
    vector<boundary> lower, negative_upper;
    optional<fraction> left, right;
    for (const half_plane &plane : planes) {
        if (plane.b > 0) lower.push_back({-plane.a, plane.c, plane.b, {}});
        else if (plane.b < 0) {
            negative_upper.push_back({-plane.a, plane.c, -plane.b, {}});
        } else if (plane.a > 0) tighten_left(left, {plane.c, plane.a});
        else tighten_right(right, {plane.c, plane.a});
    }
    if (left && right && less(*right, *left)) return {};

    auto envelope = [&](vector<boundary> lines) {
        sort(lines.begin(), lines.end(), [](const boundary &first, const boundary &second) {
            i128 slope = first.k * second.d - second.k * first.d;
            if (slope != 0) return slope < 0;
            return first.m * second.d > second.m * first.d;
        });
        vector<boundary> result;
        for (boundary current : lines) {
            if (!result.empty() &&
                result.back().k * current.d == current.k * result.back().d) continue;
            while (!result.empty()) {
                const boundary &previous = result.back();
                current.start = fraction(
                    previous.m * current.d - current.m * previous.d,
                    current.k * previous.d - previous.k * current.d
                );
                if (!previous.start || less(*previous.start, *current.start)) break;
                result.pop_back();
            }
            if (result.empty()) current.start.reset();
            result.push_back(current);
        }
        return result;
    };
    lower = envelope(move(lower));
    negative_upper = envelope(move(negative_upper));
    if (lower.empty() || negative_upper.empty()) {
        return {half_plane_intersection_kind::unbounded, {}};
    }

    // Merge envelope breakpoints. On each interval feasibility is a single
    // linear inequality in x. The union of feasible intervals is an interval.
    size_t first = 0, second = 0;
    optional<fraction> interval_start, feasible_left, feasible_right;
    bool found = false;
    while (true) {
        optional<fraction> interval_end;
        if (first + 1 < lower.size()) interval_end = lower[first + 1].start;
        if (second + 1 < negative_upper.size()) {
            tighten_right(interval_end, *negative_upper[second + 1].start);
        }
        optional<fraction> current_left = left, current_right = right;
        if (interval_start) tighten_left(current_left, *interval_start);
        if (interval_end) tighten_right(current_right, *interval_end);
        const boundary &lo = lower[first], &hi = negative_upper[second];
        i128 k = lo.k * hi.d + hi.k * lo.d;
        i128 m = lo.m * hi.d + hi.m * lo.d;
        if (k > 0) tighten_right(current_right, {-m, k});
        if (k < 0) tighten_left(current_left, {-m, k});
        bool valid = (k != 0 || m <= 0) &&
                     !(current_left && current_right && less(*current_right, *current_left));
        if (valid) {
            if (!found) feasible_left = current_left;
            feasible_right = current_right;
            found = true;
        }
        if (!interval_end) break;
        if (first + 1 < lower.size() && *lower[first + 1].start == *interval_end) ++first;
        if (second + 1 < negative_upper.size() &&
            *negative_upper[second + 1].start == *interval_end) ++second;
        interval_start = interval_end;
    }
    if (!found) return {};
    if (!feasible_left || !feasible_right) {
        return {half_plane_intersection_kind::unbounded, {}};
    }

    auto chain = [&](const vector<boundary> &lines, bool negate) {
        vector<rational_point> result;
        auto append = [&](const boundary &line, const fraction &x) {
            i128 y = line.k * x.numerator + line.m * x.denominator;
            result.push_back({x, {negate ? -y : y, line.d * x.denominator}});
        };
        size_t index = 0;
        while (index + 1 < lines.size() && !less(*feasible_left, *lines[index + 1].start)) {
            ++index;
        }
        append(lines[index], *feasible_left);
        while (index + 1 < lines.size() && less(*lines[index + 1].start, *feasible_right)) {
            ++index;
            append(lines[index], *lines[index].start);
        }
        append(lines[index], *feasible_right);
        return result;
    };
    vector<rational_point> vertices = chain(lower, false);
    vector<rational_point> upper = chain(negative_upper, true);
    vertices.insert(vertices.end(), upper.rbegin(), upper.rend());
    vertices.erase(unique(vertices.begin(), vertices.end()), vertices.end());
    if (vertices.size() > 1 && vertices.front() == vertices.back()) vertices.pop_back();
    return {half_plane_intersection_kind::bounded, move(vertices)};
}

// ---------- Polygons ----------

i128 signed_double_area(const vector<point> &polygon) {
    i128 result = 0;
    for (int i = 0; i < static_cast<int>(polygon.size()); ++i) {
        result += cross(polygon[i], polygon[(i + 1) % polygon.size()]);
    }
    return result;
}

int polygon_orientation(const vector<point> &polygon) {
    return sign(signed_double_area(polygon));
}

enum class point_location {
    outside,
    boundary,
    inside
};

point_location point_in_triangle(point value, point first, point second, point third) {
    if (orientation(first, second, third) == 0) {
        return on_segment(value, first, second) ||
               on_segment(value, second, third) ||
               on_segment(value, third, first)
            ? point_location::boundary
            : point_location::outside;
    }
    int first_side = orientation(first, second, value);
    int second_side = orientation(second, third, value);
    int third_side = orientation(third, first, value);
    bool has_positive = first_side > 0 || second_side > 0 || third_side > 0;
    bool has_negative = first_side < 0 || second_side < 0 || third_side < 0;
    if (has_positive && has_negative) return point_location::outside;
    if (first_side == 0 || second_side == 0 || third_side == 0) {
        return point_location::boundary;
    }
    return point_location::inside;
}

point_location point_in_polygon(const vector<point> &polygon, point value) {
    int winding = 0;
    for (int i = 0; i < static_cast<int>(polygon.size()); ++i) {
        point first = polygon[i];
        point second = polygon[(i + 1) % polygon.size()];
        if (on_segment(value, first, second)) return point_location::boundary;
        if (first.y <= value.y && value.y < second.y &&
            orientation(first, second, value) > 0) ++winding;
        if (second.y <= value.y && value.y < first.y &&
            orientation(first, second, value) < 0) --winding;
    }
    return winding == 0 ? point_location::outside : point_location::inside;
}

// The polygon is assumed to be simple. Collinear consecutive edges are
// allowed. A completely degenerate polygon is not considered convex.
bool is_convex_polygon(const vector<point> &polygon) {
    int n = static_cast<int>(polygon.size());
    if (n < 3) return false;
    int direction = 0;
    for (int i = 0; i < n; ++i) {
        int turn = orientation(
            polygon[i],
            polygon[(i + 1) % n],
            polygon[(i + 2) % n]
        );
        if (turn == 0) continue;
        if (direction != 0 && turn != direction) return false;
        direction = turn;
    }
    return direction != 0;
}

// The polygon must have non-zero area. Its centroid is returned exactly.
rational_point polygon_centroid(const vector<point> &polygon) {
    i128 area = signed_double_area(polygon);
    assert(area != 0);
    i128 x_numerator = 0;
    i128 y_numerator = 0;
    for (int i = 0; i < static_cast<int>(polygon.size()); ++i) {
        point next = polygon[(i + 1) % polygon.size()];
        i128 product = cross(polygon[i], next);
        x_numerator += (i128(polygon[i].x) + next.x) * product;
        y_numerator += (i128(polygon[i].y) + next.y) * product;
    }
    return {{x_numerator, 3 * area}, {y_numerator, 3 * area}};
}

// ---------- Convex geometry ----------

vector<point> convex_hull(vector<point> points, bool keep_collinear = false) {
    sort(points.begin(), points.end());
    points.erase(unique(points.begin(), points.end()), points.end());
    if (points.size() <= 1) return points;

    bool all_collinear = true;
    for (int i = 2; i < static_cast<int>(points.size()); ++i) {
        if (orientation(points[0], points[1], points[i]) != 0) {
            all_collinear = false;
            break;
        }
    }
    if (all_collinear) {
        if (keep_collinear) return points;
        return {points.front(), points.back()};
    }

    vector<point> lower;
    vector<point> upper;
    for (point value : points) {
        while (lower.size() >= 2) {
            int turn = orientation(lower[lower.size() - 2], lower.back(), value);
            if (turn > 0 || (keep_collinear && turn == 0)) break;
            lower.pop_back();
        }
        lower.push_back(value);
    }
    for (int i = static_cast<int>(points.size()) - 1; i >= 0; --i) {
        point value = points[i];
        while (upper.size() >= 2) {
            int turn = orientation(upper[upper.size() - 2], upper.back(), value);
            if (turn > 0 || (keep_collinear && turn == 0)) break;
            upper.pop_back();
        }
        upper.push_back(value);
    }
    lower.pop_back();
    upper.pop_back();
    lower.insert(lower.end(), upper.begin(), upper.end());
    return lower;
}

// The polygon must be strictly convex, counter-clockwise and without a
// repeated first vertex. Runs in O(log n).
point_location point_in_convex_polygon(const vector<point> &polygon, point value) {
    int n = static_cast<int>(polygon.size());
    if (n == 0) return point_location::outside;
    if (n == 1) return value == polygon[0] ? point_location::boundary
                                          : point_location::outside;
    if (n == 2) return on_segment(value, polygon[0], polygon[1])
        ? point_location::boundary
        : point_location::outside;

    int left_side = orientation(polygon[0], polygon[1], value);
    int right_side = orientation(polygon[0], polygon[n - 1], value);
    if (left_side < 0 || right_side > 0) return point_location::outside;
    if (left_side == 0) return on_segment(value, polygon[0], polygon[1])
        ? point_location::boundary
        : point_location::outside;
    if (right_side == 0) return on_segment(value, polygon[0], polygon[n - 1])
        ? point_location::boundary
        : point_location::outside;

    int left = 1;
    int right = n - 1;
    while (right > left + 1) {
        int middle = (left + right) / 2;
        if (orientation(polygon[0], polygon[middle], value) >= 0) left = middle;
        else right = middle;
    }
    int side = orientation(polygon[left], polygon[right], value);
    if (side < 0) return point_location::outside;
    if (side == 0) return point_location::boundary;
    return point_location::inside;
}

// The polygon must be convex, counter-clockwise and without three consecutive
// collinear vertices (convex_hull returns exactly this form by default).
i128 convex_diameter_squared(const vector<point> &polygon) {
    int n = static_cast<int>(polygon.size());
    if (n <= 1) return 0;
    if (n == 2) return distance_squared(polygon[0], polygon[1]);

    i128 result = 0;
    int opposite = 1;
    for (int i = 0; i < n; ++i) {
        int next = (i + 1) % n;
        while (abs128(cross(polygon[i], polygon[next], polygon[(opposite + 1) % n])) >
               abs128(cross(polygon[i], polygon[next], polygon[opposite]))) {
            opposite = (opposite + 1) % n;
        }
        result = max(result, distance_squared(polygon[i], polygon[opposite]));
        result = max(result, distance_squared(polygon[next], polygon[opposite]));
    }
    return result;
}

// ---------- Closest pair ----------

// Returns -1 when fewer than two points are given.
i128 closest_pair_squared(vector<point> points) {
    int n = static_cast<int>(points.size());
    if (n < 2) return -1;
    sort(points.begin(), points.end());
    vector<point> buffer(n);
    const i128 infinity = i128((__uint128_t(1) << 127) - 1);

    function<i128(int, int)> solve = [&](int left, int right) -> i128 {
        if (right - left <= 3) {
            i128 result = infinity;
            for (int i = left; i < right; ++i) {
                for (int j = i + 1; j < right; ++j) {
                    result = min(result, distance_squared(points[i], points[j]));
                }
            }
            sort(points.begin() + left, points.begin() + right,
                 [](point first, point second) {
                     return first.y < second.y ||
                            (first.y == second.y && first.x < second.x);
                 });
            return result;
        }

        int middle = (left + right) / 2;
        int64_t middle_x = points[middle].x;
        i128 result = min(solve(left, middle), solve(middle, right));
        merge(points.begin() + left, points.begin() + middle,
              points.begin() + middle, points.begin() + right,
              buffer.begin(), [](point first, point second) {
                  return first.y < second.y ||
                         (first.y == second.y && first.x < second.x);
              });
        copy(buffer.begin(), buffer.begin() + (right - left), points.begin() + left);

        vector<point> strip;
        for (int i = left; i < right; ++i) {
            i128 dx = i128(points[i].x) - middle_x;
            if (dx * dx >= result) continue;
            for (int j = static_cast<int>(strip.size()) - 1; j >= 0; --j) {
                i128 dy = i128(points[i].y) - strip[j].y;
                if (dy * dy >= result) break;
                result = min(result, distance_squared(points[i], strip[j]));
            }
            strip.push_back(points[i]);
        }
        return result;
    };
    return solve(0, n);
}

// ---------- Minkowski sum ----------

void rotate_to_lowest(vector<point> &polygon) {
    if (polygon.empty()) return;
    int best = 0;
    for (int i = 1; i < static_cast<int>(polygon.size()); ++i) {
        if (polygon[i].y < polygon[best].y ||
            (polygon[i].y == polygon[best].y && polygon[i].x < polygon[best].x)) {
            best = i;
        }
    }
    rotate(polygon.begin(), polygon.begin() + best, polygon.end());
}

vector<point> minkowski_sum(vector<point> first, vector<point> second) {
    first = convex_hull(first);
    second = convex_hull(second);
    if (first.empty() || second.empty()) return {};
    if (first.size() == 1) {
        for (point &value : second) value += first[0];
        return second;
    }
    if (second.size() == 1) {
        for (point &value : first) value += second[0];
        return first;
    }

    rotate_to_lowest(first);
    rotate_to_lowest(second);
    int first_index = 0;
    int second_index = 0;
    point current = first[0] + second[0];
    vector<point> result = {current};

    while (first_index < static_cast<int>(first.size()) ||
           second_index < static_cast<int>(second.size())) {
        point first_edge;
        point second_edge;
        if (first_index < static_cast<int>(first.size())) {
            first_edge = first[(first_index + 1) % first.size()] - first[first_index];
        }
        if (second_index < static_cast<int>(second.size())) {
            second_edge = second[(second_index + 1) % second.size()] - second[second_index];
        }

        if (second_index == static_cast<int>(second.size())) {
            current += first_edge;
            ++first_index;
        } else if (first_index == static_cast<int>(first.size())) {
            current += second_edge;
            ++second_index;
        } else {
            i128 product = cross(first_edge, second_edge);
            if (product >= 0) {
                current += first_edge;
                ++first_index;
            }
            if (product <= 0) {
                current += second_edge;
                ++second_index;
            }
        }
        result.push_back(current);
    }
    result.pop_back();
    return convex_hull(result);
}

// ---------- Lattice polygons ----------

i128 boundary_lattice_points(const vector<point> &polygon) {
    i128 result = 0;
    for (int i = 0; i < static_cast<int>(polygon.size()); ++i) {
        point next = polygon[(i + 1) % polygon.size()];
        result += gcd128(
            i128(polygon[i].x) - next.x,
            i128(polygon[i].y) - next.y
        );
    }
    return result;
}

i128 interior_lattice_points(const vector<point> &polygon) {
    if (polygon.size() < 3) return 0;
    return (abs128(signed_double_area(polygon)) - boundary_lattice_points(polygon) + 2) / 2;
}

// ---------- Circles ----------

struct circle {
    point center;
    int64_t radius;
};

point_location point_in_circle(circle value, point tested) {
    assert(value.radius >= 0);
    i128 difference = distance_squared(value.center, tested) -
                      i128(value.radius) * value.radius;
    if (difference < 0) return point_location::inside;
    if (difference == 0) return point_location::boundary;
    return point_location::outside;
}

int line_circle_intersection_count(point first, point second, circle value) {
    assert(first != second && value.radius >= 0);
    i128 product = cross(first, second, value.center);
    i128 left = product * product;
    i128 right = i128(value.radius) * value.radius *
                 distance_squared(first, second);
    if (left > right) return 0;
    if (left == right) return 1;
    return 2;
}

enum class circle_relation {
    separate,
    externally_tangent,
    intersecting,
    internally_tangent,
    contained,
    identical
};

circle_relation relation(circle first, circle second) {
    assert(first.radius >= 0 && second.radius >= 0);
    i128 centers = distance_squared(first.center, second.center);
    if (centers == 0 && first.radius == second.radius) return circle_relation::identical;
    i128 sum = i128(first.radius) + second.radius;
    i128 difference = abs128(i128(first.radius) - second.radius);
    if (centers > sum * sum) return circle_relation::separate;
    if (centers == sum * sum) return circle_relation::externally_tangent;
    if (centers < difference * difference) return circle_relation::contained;
    if (centers == difference * difference) return circle_relation::internally_tangent;
    return circle_relation::intersecting;
}

// -1 means infinitely many intersection points.
int circle_intersection_count(circle first, circle second) {
    circle_relation value = relation(first, second);
    if (value == circle_relation::identical) return first.radius == 0 ? 1 : -1;
    if (value == circle_relation::externally_tangent ||
        value == circle_relation::internally_tangent) return 1;
    if (value == circle_relation::intersecting) return 2;
    return 0;
}
