# Библиотека алгоритмов

Небольшие фрагменты кода для копирования в решения на соревнованиях.

## Структура

- [`data_structures/`](data_structures/) — DSU, Fenwick, segment tree, persistent и splay tree, CHT.
- [`geometry/`](geometry/) — точная вычислительная геометрия на целых координатах.
- [`graphs/`](graphs/) — DFS, SCC, 2-SAT, деревья, потоки и паросочетания.
- [`math/`](math/) — арифметика по модулю, комбинаторика, CRT, FFT и NTT.
- [`strings/`](strings/) — KMP, Z-функция и суффиксный массив.
- [`misc/`](misc/) — небольшие заготовки и общие приёмы.
- [`tests/`](tests/) — тесты библиотечных компонентов.

Основная готовая библиотека для вычислений по модулю и NTT: [`math/mod.cpp`](math/mod.cpp).

## Деревья отрезков

- [`data_structures/segtree.cpp`](data_structures/segtree.cpp) — простое редактируемое дерево; по умолчанию сумма и присваивание в точке.
- [`data_structures/lazy_segtree.cpp`](data_structures/lazy_segtree.cpp) — редактируемое lazy-дерево; по умолчанию range add и range sum.
- [`data_structures/segtree_sum.cpp`](data_structures/segtree_sum.cpp) — готовые суммы, точечные обновления и поиск по префиксу.
- [`data_structures/segtree_fast.cpp`](data_structures/segtree_fast.cpp) — минимальная bottom-up версия суммы с хранением в `2*n`.
- [`data_structures/segtree_two_max_gcd.cpp`](data_structures/segtree_two_max_gcd.cpp) — специализированная старая версия с двумя максимумами и НОД остальных значений.

## Графы

Все вершины нумеруются от `0` до `n - 1`. Функции не используют глобальное состояние и не содержат `main`.

- Обходы и пути: [`dfs.cpp`](graphs/dfs.cpp), [`bfs.cpp`](graphs/bfs.cpp), [`dijkstra.cpp`](graphs/dijkstra.cpp).
- Ориентированные графы: [`toposort.cpp`](graphs/toposort.cpp), [`scc.cpp`](graphs/scc.cpp), [`2sat.cpp`](graphs/2sat.cpp).
- Деревья: [`dfs_tree.cpp`](graphs/dfs_tree.cpp), [`lca.cpp`](graphs/lca.cpp), [`centroid.cpp`](graphs/centroid.cpp).
- Неориентированные графы: [`bridges.cpp`](graphs/bridges.cpp), [`mst.cpp`](graphs/mst.cpp).
- Паросочетания: [`bipartite_matching.cpp`](graphs/bipartite_matching.cpp), [`hungarian.cpp`](graphs/hungarian.cpp).
- Потоки: [`maxflow.cpp`](graphs/maxflow.cpp), [`mincost.cpp`](graphs/mincost.cpp), [`mincost_scaling.cpp`](graphs/mincost_scaling.cpp).

`mincost.cpp` — более короткий successive shortest path для обычных ограничений. `mincost_scaling.cpp` — быстрый cost-scaling push-relabel для больших потоков, circulation, lower bounds и заданных балансов вершин.

## Целочисленная геометрия

Основной файл: [`geometry/int_geom.cpp`](geometry/int_geom.cpp). Он не содержит `main`, шаблонов и вещественных вычислений; его можно целиком копировать после `#include <bits/stdc++.h>` и `using namespace std`. Промежуточные произведения считаются в `__int128_t`, а нецелые точки и квадраты расстояний возвращаются точными дробями.

Внутри есть:

- точки, векторные операции, полярная сортировка, ориентация и проверка точки на отрезке;
- точный квадрат расстояния до прямой и отрезка;
- пересечение прямых и отрезков, включая совпадающие прямые и перекрывающиеся отрезки;
- пересечение замкнутых полуплоскостей за `O(n log n)`, включая неограниченные и вырожденные случаи;
- площадь, ориентация и рациональный центроид многоугольника;
- принадлежность треугольнику, произвольному и выпуклому многоугольнику;
- проверка выпуклости и convex hull с настройкой сохранения граничных точек;
- диаметр выпуклого многоугольника за `O(n)` и ближайшая пара точек за `O(n log n)`;
- сумма Минковского выпуклых оболочек;
- число граничных и внутренних целых точек по теореме Пика;
- принадлежность точки окружности, число пересечений прямой с окружностью,
  взаимное расположение двух окружностей и число их пересечений.

`half_plane(a, b, c)` задаёт неравенство `a*x + b*y >= c` (`a` и `b` не равны нулю одновременно).
`half_plane(first, second)` задаёт левую сторону направленной прямой `first -> second`, включая границу; точки должны быть различны.

```cpp
vector<half_plane> planes = {
    {1, 0, 0},    // x >= 0
    {0, 1, 0},    // y >= 0
    {-2, -3, -5} // 2*x + 3*y <= 5
};
auto result = half_plane_intersection(planes);
// result.kind == half_plane_intersection_kind::bounded
// result.vertices: (0, 0), (5/2, 0), (0, 5/3)
```

`result.kind` различает `empty`, `bounded` и `unbounded`. Для `bounded` поле `vertices` содержит вершины против часовой стрелки без повторения первой, два конца отрезка или одну точку. В остальных случаях `vertices` пуст; пустой список ограничений означает всю плоскость (`unbounded`). Алгоритм использует `O(n)` памяти и не добавляет искусственный ограничивающий квадрат. Все промежуточные произведения, в том числе при сравнении дробей, должны помещаться в `__int128_t`.

Старые несвязанные заготовки сохранены в [`legacy/geometry/`](legacy/geometry/), но не считаются частью готовой библиотеки.
