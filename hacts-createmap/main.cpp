#include <QObject>
#include <QSet>
#include <QMap>
#include <QMultiMap>
#include <QtMath>
#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QGeoCoordinate>
#include <QDebug>

#define SECTOR_WIDTH 30 // meters
#define SECTOR_HEIGHT 30 // meters

enum class OneWay { No, Yes, YesReverse };

struct Node {
    double x;
    double y;
};

struct Way {
    QString highwayType;
    OneWay oneWay = OneWay::No;
    QVector<qint64> nodes;
};

struct OutputWay {
    qint64 highwayId;
    QVector<qint64> nodes;
    QSet<qint64> sectors;

    OutputWay(qint64 hId) : highwayId(hId) {}
};

static QMap<qint64, Node> highwayNodes;
static QMap<qint64, Node> borderNodes;
static QMap<qint64, Way> ways;
static QMultiMap<qint64, OutputWay*> sectors;
static QVector<OutputWay*> outputWays;

static struct {
   double minlat = 0, minlon = 0, maxlat = 0, maxlon = 0;
   double width = 0, height = 0; // in meters

   // how many sectors does the map have
   int sectors_horizontal = 0, sectors_vertical = 0;
} bounds;

static qint64 getSector(const Node &node) {
    return qFloor(bounds.height - node.x) % SECTOR_WIDTH + (qFloor(node.y) % SECTOR_HEIGHT) * bounds.sectors_horizontal;
}

// converts coordinates from geographical to cartesian when importing OSM data
static Node latLonToCartesian(double lat, double lon) {
    const double latwidth = bounds.maxlat - bounds.minlat;
    const double lonheight = bounds.maxlon - bounds.minlon;

    Node node;
    node.x = (lat - bounds.minlat) * bounds.width / latwidth;
    node.y = (lon - bounds.minlon) * bounds.height / lonheight;
    return node;
}

static bool hasAllAttributes(QXmlStreamAttributes &attrs, const QStringList &names) {
    foreach(QString name, names) {
        if(! attrs.hasAttribute(name))
            return false;
    }
    return true;
}


// Returns an angle ABC
//
// https://stackoverflow.com/a/3487062/3105260
static double angleBetweenNodes(const Node &a, const Node &b, const Node &c) {
    Node ab { b.x - a.x, b.y - a.y };
    Node cb { b.x - c.x, b.y - c.y };

    double dot = (ab.x * cb.x + ab.y * cb.y);
    double cross = (ab.x * cb.y - ab.y * cb.x);

    return qAtan2(cross, dot);
}

static Node moveNode(Node node, double angle, double distance) {
    return Node { node.x + qSin(angle) * distance, node.y + qCos(angle) * distance };
}

static Node nodeHalfwayAcross(Node a, Node b) {
    return Node { (a.x + b.x) / 2, (a.y + b.y) / 2 };
}

// Read and process <node id=".." lat=".." lon=".." />
static void readNode(QXmlStreamReader &reader) {
    QXmlStreamAttributes attrs = reader.attributes();
    if(! hasAllAttributes(attrs, {"lat", "lon", "id"}))
        return;

    double lat = attrs.value("lat").toDouble();
    double lon = attrs.value("lon").toDouble();

    highwayNodes[attrs.value("id").toLongLong()] = latLonToCartesian(lat, lon);

    reader.skipCurrentElement();
}

// Read and process highways. Example:
// <way id="..">
//     <nd ref=".." />
//     <nd ref=".." />
//     <tag k="highway" v="..">
// </way>
static void readWay(QXmlStreamReader &reader) {
    QXmlStreamAttributes attrs = reader.attributes();
    if(! hasAllAttributes(attrs, {"id"}))
        return;

    Way way;

    bool isDriveableByCar = false;

    while(reader.readNextStartElement()) {
        QXmlStreamAttributes attrs = reader.attributes();

        // parse <tag k="highway" v=".." /> and ensure the way is a highway
        if(reader.name() == "tag" && attrs.value("k").toString() == "highway") {
            QString highwayType = attrs.value("v").toString();

            // ensure cars can actually drive here
            if(highwayType != "footway"
                    && highwayType != "cycleway"
                    && highwayType != "bridleway"
                    && highwayType != "pedestrian"
                    && highwayType != "steps"
                    && highwayType != "path")
            {
                isDriveableByCar = true;
                way.highwayType = highwayType;
            }
        // parse <nd ref=".." />
        } else if(reader.name() == "nd") {
            qint64 nodeId = reader.attributes().value("ref").toLongLong();
            way.nodes.append(nodeId);
        }

        reader.skipCurrentElement();
    }

    if(! isDriveableByCar)
        return;

    ways[attrs.value("id").toLongLong()] = way;
}

// Read and process <bounds minlat=".." maxlat=".." maxlon=".." minlon=".." />
static void readBounds(QXmlStreamReader &reader) {
    const QXmlStreamAttributes attrs = reader.attributes();

    bounds.maxlat = attrs.value("maxlat").toDouble();
    bounds.minlat = attrs.value("minlat").toDouble();
    bounds.maxlon = attrs.value("maxlon").toDouble();
    bounds.minlon = attrs.value("minlon").toDouble();

    QGeoCoordinate topLeft(bounds.minlat, bounds.minlon);
    QGeoCoordinate topRight(bounds.maxlat, bounds.minlon);
    QGeoCoordinate bottomLeft(bounds.maxlat, bounds.minlon);

    bounds.width = topLeft.distanceTo(topRight);
    bounds.height = topLeft.distanceTo(bottomLeft);

    bounds.sectors_horizontal = bounds.width / SECTOR_WIDTH + 1;
    bounds.sectors_vertical = bounds.height / SECTOR_HEIGHT + 1;

    reader.skipCurrentElement(); // <bounds /> never has anything inside
}

static void readMap(QXmlStreamReader &reader) {

    // ensure that the file starts with <osm ...> and enter this tag
    if(! reader.readNextStartElement() || reader.name() != "osm") {
        reader.raiseError("Invalid file");
    }

    // read every tag inside <osm ...>, and look for <node> and <way> tags
    while(reader.readNextStartElement()) {

        if(reader.name() == "node") {
            readNode(reader);
        } else if(reader.name() == "way") {
            readWay(reader);
        } else if(reader.name() == "bounds") {
            readBounds(reader);
        } else {
            reader.skipCurrentElement();
        }
    }
}

static void removeNodesNotPresentInWays() {
    QSet<qint64> nodesInAHighway;

    for(const Way &way : ways) {
        for(qint64 nodeId : way.nodes) {
            nodesInAHighway.insert(nodeId);
        }
    }

    // filter nodes so that only ones present in nodesInAHighway stay
    for (auto it = highwayNodes.begin(); it != highwayNodes.end(); ) {
        qint64 nodeId = it.key();
        if(nodesInAHighway.contains(nodeId)) {
            ++it;
        } else {
            it = highwayNodes.erase(it);
        }
    }
}

static void addBorderNode(OutputWay *outputWay, Node node) {
    if(qIsNaN(node.x) || qIsNaN(node.y)) // TODO: why is this NaN sometimes?
        return;

    static qint64 nodeId = -1;
    borderNodes[nodeId] = node;

    qint64 sectorId = getSector(node);
    outputWay->nodes.append(nodeId);
    outputWay->sectors |= sectorId;
    if(! sectors.contains(sectorId, outputWay))
        sectors.insert(sectorId, outputWay);

    nodeId -= 1;
}

//static void addBorderNodes(Node node, double angle, OutputWay *left, OutputWay *right) {

//    // make the angle be for a line perpendicular to |AB|
//    angle += M_PI/2.0;

//    addBorderNode(left, moveNode(node, angle, roadWidth));
//    addBorderNode(right, moveNode(node, angle, -roadWidth));
//}

static void addBorderToTwoNodeWay(Node *prev, Node a, Node b, Node *next, OutputWay *left, OutputWay *right) {
    double roadWidth = 4; // meters

    double roadFragmentCartesianAngle = qAtan2(a.x - b.x, a.y - b.y);
    double perpendicularAngle = roadFragmentCartesianAngle + M_PI/2.0;

    double angleOnStart = M_PI;
    double angleOnEnd = M_PI;

    if(prev)
        angleOnStart = angleBetweenNodes(*prev, a, b);
    if(next)
        angleOnEnd = angleBetweenNodes(a, b, *next);

    double startDisplacement = qSin(qAbs(angleOnStart)) * -4;
    double endDisplacement = qSin(qAbs(angleOnEnd)) * -4;

    Node backedUpA = moveNode(a, roadFragmentCartesianAngle, startDisplacement);
    Node backedUpB = moveNode(b, roadFragmentCartesianAngle + M_PI, endDisplacement);
    Node halfway = nodeHalfwayAcross(a, b);

    bool startTurnsRight = angleOnStart < 0;
    bool endTurnsRight = angleOnEnd < 0;

    addBorderNode(right, moveNode(startTurnsRight ? backedUpA : a, perpendicularAngle, roadWidth));
    addBorderNode(right, moveNode(halfway, perpendicularAngle, roadWidth));
    addBorderNode(right, moveNode(endTurnsRight ? backedUpB : b, perpendicularAngle, roadWidth));

    addBorderNode(left, moveNode(!startTurnsRight ? backedUpA : a, perpendicularAngle, -roadWidth));
    addBorderNode(left, moveNode(halfway, perpendicularAngle, -roadWidth));
    addBorderNode(left, moveNode(!endTurnsRight ? backedUpB : b, perpendicularAngle, -roadWidth));
}

static void addNodesToHighway() {
    for(qint64 wayId : ways.keys()) {
        const Way &way = ways[wayId];

        OutputWay *left = new OutputWay(wayId);
        OutputWay *right = new OutputWay(wayId);
        outputWays << left << right;

        // iterate for every element but first
        for(int i = 1; i < way.nodes.length(); i++) {
            Node firstNode = highwayNodes.value(way.nodes.value(i - 1));
            Node secondNode = highwayNodes.value(way.nodes.value(i));

            Node *previousNode = nullptr;
            if(i != 1)
                previousNode = &highwayNodes[way.nodes.value(i - 2)];

            Node *nextNode = nullptr;
            if(i != way.nodes.length() - 1)
                nextNode = &highwayNodes[way.nodes.value(i + 1)];

            addBorderToTwoNodeWay(previousNode, firstNode, secondNode, nextNode, left, right);
        }
    }
}

static void outputDataOSM(QFile &file) {
    QTextStream out(&file);

    // Coordinates need higher precision than the default 6
    out.setRealNumberPrecision(12);

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    out << "<osm version=\"0.6\" generator=\"hacts-createmap\">\n";

    for(auto key : highwayNodes.keys()) {
        Node node = highwayNodes[key];
        node.x /= bounds.width; node.x += 19;
        node.y /= bounds.height; node.y += 50;
        out << "  <node id=\"" << key << "\" lat=\"" << node.x << "\" lon=\"" << node.y << "\" version=\"1\" />\n";
    }

    for(auto key : borderNodes.keys()) {
        Node node = borderNodes[key];
        node.x /= bounds.width; node.x += 19;
        node.y /= bounds.height; node.y += 50;
        out << "  <node id=\"" << key << "\" lat=\"" << node.x << "\" lon=\"" << node.y << "\" version=\"1\" />\n";
    }

    for(auto key : ways.keys()) {
        const Way &way = ways[key];
        out << "  <way id=\"" << key << "\" version=\"1\">\n";
        out << "    <tag k=\"highway\" v=\"" << way.highwayType << "\" />\n";
        for(auto nodeId : way.nodes)
            out << "    <nd ref=\"" << nodeId << "\" />\n";
        out << "  </way>\n";
    }

    qint64 wayId = -100000000;
    for(OutputWay *way : outputWays) {
        out << "  <way id=\"" << wayId++ << "\" version=\"1\">\n";
        out << "    <tag k=\"highway\" v=\"track\" />\n";
        for(auto nodeId : way->nodes)
            out << "    <nd ref=\"" << nodeId << "\" />\n";
        out << "  </way>\n";
    }

    out << "</osm>\n";
}

static void outputDataHACTS(QFile &file) {
    QTextStream out(&file);
    out.setRealNumberPrecision(12);

//    out << "hacts map file v1\n";
//    out << "width_meters=" << bounds.width << "\n";
//    out << "height_meters=" << bounds.height << "\n";
//    out << "width_sectors=" << bounds.sectors_horizontal << "\n";
//    out << "height_sectors=" << bounds.sectors_vertical << "\n";
//    out << "\n";
    for(qint64 outputWayId = 0; outputWayId < outputWays.size(); ++outputWayId) {
        OutputWay const * const way = outputWays[outputWayId];

        if(way->nodes.empty())
            continue;

        out << outputWayId << '\t';
        out << way->highwayId << '\t';

        out << '\t'; // no neighbours

        bool first = true;
        for(qint64 nodeId : way->nodes) {
            if(!first)
                out << ',';

            out << borderNodes.value(nodeId).x;
            out << ',';
            out << borderNodes.value(nodeId).y;

            first = false;
        }
        out << '\n';
    }

    // output ways
}

int main(int argc, char *argv[]) {
    QTextStream cerr(stderr);

    if(argc != 3) {
        cerr << "Usage " << argv[0] << " <source map> <output map>\n"
               "    This program adds lane and other information into OSM maps for\n"
               "    transport simulation purpouses.\n"
               "\n"
               "    When <source map> or <output map> are \"-\", stdin and stdout are used \n"
               "    respectively.\n";
    }


    QFile input;
    if(QString("-") == argv[1]) {
        input.open(stdin, QIODevice::ReadOnly);
    } else {
        input.setFileName(argv[1]);
        if(! input.open(QIODevice::ReadOnly)) {
            cerr << "Failed to open " << argv[1] << ": " << input.errorString() << "\n";
            return 1;
        }
    }

    QXmlStreamReader reader(&input);

    readMap(reader);
    if (reader.hasError()) {
        cerr << "Failed reading input map: " << reader.errorString();
        return 1;
    };

    removeNodesNotPresentInWays();

    addNodesToHighway();


    QFile output;
    if(QString("-") == argv[2]) {
        output.open(stdout, QIODevice::WriteOnly);
    } else {
        output.setFileName(argv[2]);
        if(! output.open(QIODevice::WriteOnly)) {
            cerr << "Failed to open " << argv[2] << " for writing: " << output.errorString() << "\n";
            return 1;
        }
    }
    //outputDataOSM(output);
    outputDataHACTS(output);

    if(output.error() != QFile::NoError) {
        cerr << "Failed writing to " << argv[2] << ": " << output.errorString() << "\n";
        return 1;
    }

    return 0;
}
