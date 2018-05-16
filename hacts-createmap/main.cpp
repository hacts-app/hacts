#include <iostream>

#include <QObject>
#include <QFile>
#include <QTextStream>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMap>

#include <QDebug>

struct Node {
    double lat;
    double lon;
};

struct Way {
    QString highwayType;
    QVector<qint64> nodes;
};

static QMap<qint64, Node> nodes;
static QMap<qint64, Way> ways;

static bool hasAllAttributes(QXmlStreamAttributes &attrs, const QStringList &names) {
    foreach(QString name, names) {
        if(! attrs.hasAttribute(name))
            return false;
    }
    return true;
}

static void readNode(QXmlStreamReader &reader) {
    QXmlStreamAttributes attrs = reader.attributes();
    if(! hasAllAttributes(attrs, {"lat", "lon", "id"}))
        return;

    Node node;
    node.lat = attrs.value("lat").toDouble();
    node.lon = attrs.value("lon").toDouble();
    nodes[attrs.value("id").toLongLong()] = node;

    reader.skipCurrentElement();
}

static void readWay(QXmlStreamReader &reader) {
    QXmlStreamAttributes attrs = reader.attributes();
    if(! hasAllAttributes(attrs, {"id"}))
        return;

    Way way;

    bool isDriveableByCar = false;

    while(reader.readNextStartElement()) {
        QXmlStreamAttributes attrs = reader.attributes();

        // ensure the way is a highway
        if(reader.name() == "tag" && attrs.value("k").toString() == "highway") {
            QString highwayType = attrs.value("v").toString();

            // ensure cars can drive here
            if(highwayType != "footway"
                    && highwayType != "cycleway"
                    && highwayType != "bridleway"
                    && highwayType != "path")
            {
                isDriveableByCar = true;
                way.highwayType = highwayType;
            }
        } else if(reader.name() == "nd") {
            way.nodes.append(reader.attributes().value("ref").toLongLong());
        }

        reader.skipCurrentElement();
    }

    if(! isDriveableByCar)
        return;

    ways[attrs.value("id").toLongLong()] = way;
}

static void readMap(QXmlStreamReader &reader) {

    if(! reader.readNextStartElement() || reader.name() != "osm") {
        reader.raiseError("Invalid file");
    }

    while(reader.readNextStartElement()) {


        if(reader.name() == "node") {
            readNode(reader);
        } else if(reader.name() == "way") {
            readWay(reader);
        } else {
            reader.skipCurrentElement();
        }
    }
}

int main(int argc, char *argv[])
{
    QTextStream cerr(stderr);

    argc = 3;

    if(argc != 3) {
        cerr << "Usage: " << argv[0] << " <source map> <output map>\n"
            << "    This program adds lane and other information into OSM maps for\n"
            << "    transport simulation purpouses.\n";
    }

    QFile input("/home/karol/projects/hacts/hacts-createmap/potok.osm");
    if(! input.open(QIODevice::ReadOnly)) {
        cerr << "Failed to open " << argv[1] << ": " << input.errorString() << "\n";
        return 1;
    }

    QXmlStreamReader reader(&input);

    readMap(reader);

    if (reader.hasError()) {
        cerr << "Failed reading input map: " << reader.errorString();
        return 1;
    };

    return 0;
}
