def create_dense_connect_graph(start, size, edges):
    end = start + size
    for i in range(start, end):
        for j in range(i + 1, end):
            edges.append((i, j))

def create_star_graph(center, size, edges):
    for satellite in range(center + 1, center + size):
        edges.append((center, satellite))

if __name__ == '__main__':
    edges = []
    size = 50

    # dense connected graph
    create_dense_connect_graph(0, size, edges)

    # star shape (center at 100, satellite in [101..150])
    create_star_graph(100, size, edges)

    # mixture of both
    create_dense_connect_graph(200, size, edges)
    create_star_graph(249, size, edges)

    # output to file
    with open('test_data.txt', 'w') as f:
        for edges in edges:
            f.write('%d %d\n' % edges)
