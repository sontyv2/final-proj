from random import shuffle, randrange
 
def make_maze(w = 16, h = 8):
    vis = [[0] * w + [1] for _ in range(h)] + [[1] * (w + 1)]
    ver = [["100"] * w + ['1'] for _ in range(h)] + [[]]
    hor = [["111"] * w + ['1'] for _ in range(h + 1)]
 
    def walk(x, y):
        vis[y][x] = 1
 
        d = [(x - 1, y), (x, y + 1), (x + 1, y), (x, y - 1)]
        shuffle(d)
        for (xx, yy) in d:
            if vis[yy][xx]: continue
            if xx == x: hor[max(y, yy)][x] = "100"
            if yy == y: ver[y][max(x, xx)] = "000"

            walk(xx, yy)
 
    walk(randrange(w), randrange(h))
 
    s = ""
    for (a, b) in zip(hor, ver):
        s += ''.join(a + ['\n'] + b + ['\n'])
    return s


def write_to_file(text):
    file = open("maze.txt", "w")
    file.write(text)
    file.close()

def to_call(width, height):
    write_to_file(make_maze(width, height))
 
if __name__ == '__main__':
    write_to_file(make_maze())

