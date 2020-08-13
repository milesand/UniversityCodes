points = [(1, 7), (2, 6), (3, 9), (4, 15), (5, 10), (6, 8), (7, 13),
          (8, 19), (10, 17), (11, 6), (12, 20), (13, 1), (14, 13), (15, 17),
          (16, 6), (17, 8), (18, 10), (19, 15), (20, 17), (1, 3), (3, 5),
          (4, 7), (8, 9), (16, 20)]

def distance(point0, point1):
    return (point0[0]-point1[0]) ** 2 + (point0[1]-point1[1]) ** 2

def distance_to_origin(point):
    return point[0]**2 + point[1]**2

def block(depth, point):
    dis = distance(point, (0, 0))
    return dis & ~((4 << depth) - 1)
    
def query(query_point, points):
    return min(zip(map(lambda p: distance(query_point, p), points), points))[1]

def confirm(query_point, result, points):
    result2 = query(query_point, points)
    if result != result2:
        return distance(result, query_point) == distance(result2, query_point)
    return True
