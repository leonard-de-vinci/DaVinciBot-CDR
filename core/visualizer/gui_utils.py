def mouse_to_table(pos, table_size, irl_size, margins=(0, 0)):
    x, y = pos
    x -= margins[0]
    y -= margins[1]

    return (int(x / table_size[0] * irl_size[0]), int(y / table_size[1] * irl_size[1]))


def table_to_mouse(pos, table_size, irl_size, margins=(0, 0)):
    x, y = pos
    x = x / irl_size[0] * table_size[0]
    y = y / irl_size[1] * table_size[1]

    return (int(x + margins[0]), int(y + margins[1]))
