from lib import *

path = "data.txt"

roads: Dict[int, Road] = setRoads(path)

golf3 = Car(0, 1540, 350, 3.23, 0.315, 54, 0, 0, 0, 4.02, 1.7)

test_time = 0.05


def __main__():
    bef = datetime.datetime.now()

    while golf3.velocity < 30:
        time.sleep(test_time)

        golf3.onGasPush(1, bef)

        golf3.changePos(bef)
        radar = golf3.radar(roads[13].lines)
        for x in radar:
            if fabs(x) < 1:
                return None
        golf3.givePos()

        bef = datetime.datetime.now()


if __name__ == '__main__':
    __main__()