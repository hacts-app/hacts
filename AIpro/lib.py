import datetime
import time
from typing import Dict
from itertools import *
from math import sin, cos, fabs, sqrt
import shapely.geometry as geometry
from typing import Optional

PI = 3.14159265


def grouper(iterable, n, fillvalue=None):
    # grouper('ABCDEFG', 3, 'x') --> ABC DEF Gxx"
    args = [iter(iterable)] * n
    return zip_longest(*args, fillvalue=fillvalue)


class Node:
    x = 0
    y = 0

    def __init__(self, x, y):
        self.x = x
        self.y = y


class Way:
    id = 0
    line = geometry.LineString()
    NeighboursID = []  # ints

    def __init__(self, ID, points, neighbours):
        self.id = ID
        self.NeighboursID = neighbours

        point1 = geometry.Point(points[0], points[1])
        point2 = geometry.Point(points[2], points[3])
        last = point2

        self.line = geometry.LineString((point1, point2))

        first_points = 2
        for x, y in grouper(points, 2):
            if first_points > 0:
                first_points -= 1
                continue
            point = geometry.Point(x, y)
            new_line = geometry.LineString((last, point))
            self.line = self.line.union(new_line)
            last = point


class Road:
    lines = []  # Ways


def delta_t(bef: float) -> float:
    return (datetime.datetime.now() - bef).total_seconds()


def radToDeg(rad: float) -> float:
    return 180.0 * rad / PI


def degToRad(deg: float) -> float:
    return PI * deg / 180.0


def move_point(point: geometry.Point, angle: float, distance: float) -> geometry.Point:
    return geometry.Point(
        point.x + cos(angle) * distance,
        point.y + sin(angle) * distance)


def radar_line(starting_point: geometry.Point, angle: float, distance: float) -> geometry.LineString:
    distant_point = move_point(starting_point, angle, distance)
    return geometry.LineString((starting_point, distant_point))


def distance(starting_point: geometry.Point, angle: float, way: geometry.LineString, max_distance: float) -> Optional[float]:
    radar = radar_line(starting_point, angle, max_distance)
    intersection_points = radar.intersection(way)
    if intersection_points.is_empty:
        return None
    return starting_point.distance(intersection_points)


def setRoads(path: str) -> {}:
    roads = {}

    def split_ints(a):
        return [int(s) for s in a.split(',') if s != '']

    def split_floats(a):
        return [float(s) for s in a.split(',') if s != '']

    with open(path) as f:
        for line in f:
            fields = line.rstrip('\n').split('\t')
            fields[1] = int(fields[1])

            w = Way(split_ints(fields[0]), split_floats(fields[3]), split_ints(fields[2]))

            road = Road()
            road.lines.append(w)

            roads[fields[1]] = road
    return roads


def moveNode(x: float, y: float, a: float, R: float) -> Node:
    node = Node(x + sin(a * PI / 180.0) * R, y + cos(a * PI / 180.0) * R)
    return node


class Car:
    carId = 0
    velocity = 0  # m/s
    max_velocity = 0
    mass = 0
    max_transfer = 0
    torque = 0
    radius = 0  # Wheel radius
    angle = 0
    wheelAng = 0
    x = 0
    y = 0
    length = 0
    width = 0

    def __init__(self, ID, mass, m_transfer, torque, radius, max_velocity, angle, x, y, length, width):
        self.carId = ID
        self.mass = mass
        self.max_transfer = m_transfer
        self.torque = torque
        self.radius = radius
        self.max_velocity = max_velocity
        self.angle = angle
        self.x = x
        self.y = y
        self.length = length
        self.width = width

    def onGasPush(self, trans, bef):  # trans od 0.00 do 1 to % wcisniecia gazu
        transfer = self.max_transfer * trans

        F = (transfer * self.torque) / self.radius

        acceleration = F / self.mass

        self.velocity += acceleration * delta_t(bef)

        if self.velocity > self.max_velocity:
            self.velocity = self.max_velocity

    def onBrakePush(self, per, bef):
        max_acc = -9.59

        acceleration = max_acc * per

        self.velocity += acceleration * delta_t(bef)

        if self.velocity < 0:
            self.velocity = 0

    def changeWheelAng(self, intensity, bef):
        max_rot = 15

        rot = max_rot * intensity

        self.wheelAng += rot * delta_t(bef)

        if self.wheelAng > 40:
            self.wheelAng = 40
        elif self.wheelAng < -40:
            self.wheelAng = -40

    def radar(self, ways) -> []:
        result = []
        angles = [0, 15, 30, 45, 60, 75, 90, 180, 270, 285, 300, 315, 330, 345]
        minimum = 60

        for k in range(0, 13):
            ang = self.angle + angles[k]

            while ang >= 360:
                ang -= 360

            for i in ways:
                pos = geometry.Point(self.x, self.y)
                tmp = distance(pos, ang, i.line, minimum)
                if tmp is not None:
                    if tmp < minimum:
                        minimum = tmp

            result.append(minimum)
            minimum = 60
        return result

    def onRoad(self, ways) -> bool:
        R = (sqrt(pow(self.length, 2) + pow(self.width, 2))) / 2  # promien okregu opisanego na samochodzie

        alpha = acos(1 - (pow(self.width, 2)) / (2 * pow(R, 2))) * 180.0 / PI  # kat miedzy przekatnymi pojazdu

        an1 = self.angle + alpha / 2  # katy przekatnych
        an2 = self.angle - alpha / 2

        pos = geometry.Point(self.x, self.y)

        carLine = geometry.LineString((move_point(pos, an1, R), move_point(pos, an2, R),
                                       move_point(pos, an1, -R), move_point(pos, an2, -R), move_point(pos, an1, R)))
        for way in ways:
            inter_pt = carLine.intersection(way)
            if inter_pt.is_empty:
                return False
        return True

    def givePos(self):
        print("movecar " + str(self.carId) + " " + str(self.x) + " " + str(self.y) + " " + str(self.angle))

    def changePos(self, bef):
        self.angle += radToDeg((2 * self.velocity * sin(self.wheelAng * PI / 180.0)) / (self.length - 0.6)) * delta_t(bef)

        while self.angle >= 360:
            self.angle -= 360

        while self.angle < 0:
            self.angle += 360

        self.x += self.velocity * cos(self.angle * PI / 180.0) * delta_t(bef)

        self.y += self.velocity * sin(self.angle * PI / 180.0) * delta_t(bef)